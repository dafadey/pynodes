#include <iostream>

#include "node.h"
#include "pin.h"
#include "param.h"
#include "monitor_label.h"
#include "graph.h"

#include "node_library.h"

namespace pynodes {
  
shallowParam::shallowParam(const char* name, const char* value, const char* script) : m_name(name), m_value(value), m_script(script) {}

shallowMonitor::shallowMonitor(const char* name, const char* type) : m_name(name), m_type(type) {}

shallowNode::shallowNode() {}

shallowNode::shallowNode(const node* nd) : m_name(nd->getLabel()) {
  m_script = std::string(nd->getScript());
  for(auto p : nd->pins()) {
    if(p->getType() == pin::ePinType::IN)
      m_inpins.push_back(std::string(p->getLabel()));
    if(p->getType() == pin::ePinType::OUT)
      m_outpins.push_back(std::string(p->getLabel()));
  }
  for(const auto& par : nd->params())
    m_params.push_back(shallowParam(par.getLabel(), par.getValue(), par.getScript()));
}

node* shallowNode::createNode(graph* g) const {
  auto nd = new pynodes::node(g);
  nd->setLabel(m_name.c_str());
  nd->setScript(m_script.c_str());
  
  for(auto& sp : m_inpins)
    nd->makePin(sp.c_str(), pin::ePinType::IN);
  
  for(auto& sp : m_outpins)
    nd->makePin(sp.c_str(), pin::ePinType::OUT);
  
  for(auto& spar : m_params) {
    param par(g, spar.name());
    par.setValue(spar.value());
    par.setScript(spar.script());
    nd->addParam(par);
  }
  
  for(auto& smon : m_monitors) {
    if(strcmp(smon.type(), monitor_label::sgetType())==0) {
      auto mon = new monitor_label(g, smon.name());
      nd->addMonitor(mon);
    }
    //add other monitors here!
  }
  return nd;
}

nodeLibrary::nodeLibrary(const char* _name) : m_name(_name) {}

nodeLibrary::~nodeLibrary() {
  for(auto c : m_children)
    delete c;
}

void nodeLibrary::addNode(node* nd)  {
  m_nodes.push_back(shallowNode(nd));
}

void nodeLibrary::addGroup(const char* name)  {
  m_children.push_back(new nodeLibrary(name));
}

void nodeLibrary::rename(const char* newname) {
  m_name = std::string(newname);
}

const char* nodeLibrary::name() const {
  return m_name.c_str();
}

const std::vector<nodeLibrary*>& nodeLibrary::children() const {
  return m_children;
}

std::vector<nodeLibrary*>& nodeLibrary::children() {
  return m_children;
}

const std::vector<shallowNode>& nodeLibrary::nodes() const {
  return m_nodes;
}

std::vector<shallowNode>& nodeLibrary::nodes() {
  return m_nodes;
}

const char* shallowNode::name() const {
  return m_name.c_str();
}

const char* shallowNode::script() const {
  return m_script.c_str();
}

const std::vector<std::string>& shallowNode::inpins() const {
  return m_inpins;
}
std::vector<std::string>& shallowNode::inpins() {
  return m_inpins;
}

const std::vector<std::string>& shallowNode::outpins() const {
  return m_outpins;
}
std::vector<std::string>& shallowNode::outpins() {
  return m_outpins;
}

const std::vector<shallowParam>& shallowNode::params() const {
  return m_params;
}
std::vector<shallowParam>& shallowNode::params() {
  return m_params;
}

const std::vector<shallowMonitor>& shallowNode::monitors() const {
  return m_monitors;
}
std::vector<shallowMonitor>& shallowNode::monitors() {
  return m_monitors;
}

void shallowNode::setName(const char* v) {
  m_name=std::string(v);
}

void shallowNode::setScript(const char* v) {
  m_script=std::string(v);
}

const char* shallowParam::name() const {
  return m_name.c_str();
}

const char* shallowParam::value() const {
  return m_value.c_str();
}

const char* shallowParam::script() const {
  return m_script.c_str();
}

const char* shallowMonitor::name() const {
  return m_name.c_str();
}

const char* shallowMonitor::type() const {
  return m_type.c_str();
}

std::ostream& operator<<(std::ostream& o, const shallowNode& snd) {
  o << "shallowNode '" << snd.name() << "'\n\twith pins:\n";
  for(auto& p : snd.inpins())
    o << "\t\t '" << p << "' : IN\n";
  for(auto& p : snd.outpins())
    o << "\t\t '" << p << "' : OUT\n";
  o << "\twith params:\n";
  for(auto& par : snd.params())
    o << "\t\t '" << par.name() << "'='" << par.value() << "'\n\t\tscript:\n" << par.script() << '\n';
  o << "\twith monitors:\n";
  for(auto& mon : snd.monitors())
    o << "\t\t '" << mon.name() << "' : '" << mon.type() << "\n";
  o << "\twith script:\n" << snd.script() << '\n';
  return o;
}

} //namespace
