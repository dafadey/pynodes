#pragma once
#include <Python.h>

namespace pynodes {

class pyobj {
protected:
  PyObject* m_pyobj{nullptr};

public:
  virtual ~pyobj();
  PyObject* const getPyObject() const;
  void setPyObject(PyObject* obj);
  void invalidatePyObject();
};

}//namespace
