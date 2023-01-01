#pragma once
#include <Python.h>
#include <vector>
#include "base_item.h"
#include "pyobj.h"
#include "pyexecutable.h"
namespace pynodes {

class graph;

class param : public base_item, public pyobj, public pyExecutable {
private:
  std::string m_value{};
  bool m_unapplied{false};
public:
  param() = delete;
  param(const graph* g);
  param(const graph* g, const char* lbl);
  virtual ~param();

  const char* getValue() const;
  void setValue(const char* val);

  virtual bool run();
  void setUnapplied();
  void setApplied();
  bool applied() const;

};

} //namespace
