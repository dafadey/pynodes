#pragma once
#include <string>
#include <vector>
#include "pin.h"
#include "param.h"
#include "monitor_base.h"
#include "base_item.h"
#include "pyexecutable.h"
namespace pynodes {

class graph;

class node : public base_item, public pyExecutable {
  graph* m_graph{};
  std::vector<pin*> m_pins;
  std::vector<param> m_params;
  std::vector<monitor_base*> m_monitors;

public:
  node() = delete;
  node(graph* g);
  ~node();

  const graph* getGraph() const;
  
  void addPin(pin*);
  void addParam(param&);
  void addMonitor(monitor_base*);
  
  void removePin(pin*);
  void removeParam(param&);
  void removeMonitor(monitor_base*);
  
  pin* makePin(const char* label, pin::ePinType pt);
  
  graph* getGraph();
  
  const std::vector<pin*>& pins() const;
  std::vector<param>& params();
  const std::vector<param>& params() const;
  const std::vector<monitor_base*>& monitors() const;
  
  pin* getPin(const char* label);
  param& getParam(const char* label);
  
  virtual bool run();
  bool isValid() const;
  void invalidateOutput();
};
}//namespace
