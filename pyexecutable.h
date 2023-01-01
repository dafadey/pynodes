#pragma once
#include <string>
#include <Python.h>

namespace pynodes {

class pyExecutable {
private:
  std::string m_script{};
  mutable bool m_needToReloadPythonScript{false};
public:
  virtual bool run() = 0;
  void setScript(const char*);
  const char* getScript() const;
  //bool reloadModuleNeeded() const;

  PyObject* safeImportModule(const char* modname) const;
  
  static bool initPyExecutables();
  
};

} // namespace
