#include <iostream>
#include <fstream>
#include <filesystem>

#include "pyexecutable.h"

namespace pynodes {
  
  void pyExecutable::setScript(const char* script) {
    m_needToReloadPythonScript = true;
    m_script = std::string(script);
  }
  
  const char* pyExecutable::getScript() const {
    return m_script.c_str();
  }
  
  
  PyObject* pyExecutable::safeImportModule(const char* modname) const {

    std::string filename = "scripts/"+std::string(modname)+".py";

    bool fileExists = true;
    if(!std::filesystem::exists(std::filesystem::path(filename.c_str()))) {
      fileExists=false;
      PyRun_SimpleString("import importlib\nimportlib.machinery.PathFinder.invalidate_caches()");
    }

    if(!fileExists || m_needToReloadPythonScript) {
      std::ofstream of(filename.c_str());
      of << getScript();
      of.close();
    }

    PyObject* pModule = NULL;
    PyObject* _pModule = PyImport_ImportModule(modname);

    if(m_needToReloadPythonScript) {
      if(_pModule == NULL) {
        if (PyErr_Occurred())
          PyErr_Print();
        std::cerr << "cannot load script " << modname << '\n';
        return pModule;
      }
      pModule = PyImport_ReloadModule(_pModule);
      Py_XDECREF(_pModule);
      m_needToReloadPythonScript = false;
    } else
      pModule = _pModule;
    return pModule;
  }

  bool pyExecutable::initPyExecutables() {
    auto scripts_path = std::filesystem::current_path() / "scripts";
    std::filesystem::create_directory(scripts_path);
   
    Py_Initialize();
    PyRun_SimpleString(("import sys\nsys.path.append('"+scripts_path.string()+"')\n").c_str());
    return true;
  }
  
} //namespace
