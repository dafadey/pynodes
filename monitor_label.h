#pragma once
#include "monitor_base.h"

namespace pynodes {

class monitor_label : public monitor_base {
public:
  monitor_label() = delete;
  monitor_label(graph* g, const char* label);
  virtual bool show();
  virtual const char* getType() const;
  static const char* sgetType();
};
} //namespace
