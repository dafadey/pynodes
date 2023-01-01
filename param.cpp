#include <iostream>
#include <fstream>
#include <filesystem>

#include "param.h"
#include "graph.h"

namespace pynodes {

param::param(const graph* g) : base_item(g->getNewUniqueId()) {}

param::param(const graph* g, const char* lbl) : base_item(g->getNewUniqueId()) {
  setLabel(lbl);
}

param::~param() {}

void param::setUnapplied() {
  m_unapplied = true;
}

void param::setApplied() {
  m_unapplied = false;
}

bool param::applied() const {
  return !m_unapplied;
}

void param::setValue(const char* val) {
  if(m_value!=std::string(val)) {
    std::string script{};
    script = "def initparam():\n\treturn " + std::string(val);
    setScript(script.c_str());
  }
  m_value=std::string(val);
}

const char* param::getValue() const {
  return m_value.c_str();
}

bool param::run() {

  std::string modname = "param_"+std::string(getLabel())+"_"+std::to_string(id());
  
  auto pModule = safeImportModule(modname.c_str());
  
  if (pModule != NULL) {
    PyObject* pFunc = PyObject_GetAttrString(pModule, "initparam");
    if (pFunc && PyCallable_Check(pFunc)) {
      Py_XDECREF(m_pyobj);
      m_pyobj = PyObject_CallNoArgs(pFunc);
      if (m_pyobj == NULL) {
        if (PyErr_Occurred())
          PyErr_Print();
        std::cerr << "call of 'initparam' for param " << getLabel() << " failed\n";
      }
      Py_XDECREF(pFunc);
    } else {
      if (PyErr_Occurred())
        PyErr_Print();
      std::cerr << "cannot find function 'initparam' in script of param " << getLabel() << '\n';
    }
    Py_DECREF(pModule);
  }
  else {
    if (PyErr_Occurred())
      PyErr_Print();
    std::cerr << "cannot load script for param " << getLabel() << '\n';
  }
  return true;
}

}//namespace
