#pragma once
#include <vector>
#include <string>
#include <ostream>

namespace pynodes {
  
class node;
class graph;
  
class shallowParam {
public:
  shallowParam(const char* name, const char* value, const char* script);
  const char* name() const;
  const char* value() const;
  const char* script() const;
private:
  std::string m_name;
  std::string m_value;
  std::string m_script;
};

class shallowMonitor {
public:
  shallowMonitor(const char* name, const char* classType);
  const char* name() const;
  const char* type() const;
private:
  std::string m_name;
  std::string m_type;
};

class shallowNode {
public:
  shallowNode();
  shallowNode(const node*);
  const char* name() const;
  const char* script() const;
  const std::vector<std::string>& inpins() const;
  const std::vector<std::string>& outpins() const;
  const std::vector<shallowParam>& params() const;
  const std::vector<shallowMonitor>& monitors() const;
  void setName(const char*);
  void setScript(const char*);
  std::vector<std::string>& inpins();
  std::vector<std::string>& outpins();
  std::vector<shallowParam>& params();
  std::vector<shallowMonitor>& monitors();
  node* createNode(graph*) const;

private:
  std::string m_name;
  std::string m_script;
  std::vector<std::string> m_outpins;
  std::vector<std::string> m_inpins;
  std::vector<shallowParam> m_params;
  std::vector<shallowMonitor> m_monitors;
};

class nodeLibrary {
  std::string m_name;
  std::vector<nodeLibrary*> m_children;
  std::vector<shallowNode> m_nodes;
public:
  ~nodeLibrary();
  nodeLibrary(const char* name);
  const std::vector<shallowNode>& nodes() const;
  std::vector<shallowNode>& nodes();
  const std::vector<nodeLibrary*>& children() const;
  std::vector<nodeLibrary*>& children();
  const char* name() const;
  void addNode(node*);
  void addGroup(const char* name);
  void rename(const char* newname);
  void saveToXml(const char* filename);
};

std::ostream& operator<<(std::ostream& o, const shallowNode& snd);

} //namespace
