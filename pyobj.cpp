#include "pyobj.h"

namespace pynodes {

pyobj::~pyobj() {
  Py_XDECREF(m_pyobj);
}

PyObject* const pyobj::getPyObject() const {
  return m_pyobj;
}

void pyobj::setPyObject(PyObject* obj) {
  m_pyobj = obj;
}

void pyobj::invalidatePyObject() {
  Py_XDECREF(m_pyobj);
  m_pyobj=NULL;
}

}//namespace
