#pragma once

#include "base_item.h"
#include "pyobj.h"

namespace pynodes {

class graph;

class monitor_base : public base_item, public pyobj {
public:
  monitor_base() = delete;
  monitor_base(graph* g);
  virtual ~monitor_base();
  
  virtual const char* getType() const = 0;
  
  virtual bool show() = 0;
};
} //namespace
