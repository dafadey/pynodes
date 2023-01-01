#pragma once
#include <vector>
#include <map>
#include "base_item.h"

namespace pynodes {

class pin;
class node;
class link;

class graph : public base_item {
  std::map<int, node*> m_nodeMap;
  std::map<int, link*> m_linkMap;
  std::map<int, pin*> m_pinMap;
  mutable int m_currentId{};

public:
//methods
  graph();
  ~graph();
  int getNewUniqueId() const;
  void addNode(node* nd);
  void removeNode(node* nd);
  void addLink(link* lnk);
  void removeLink(link* lnk);
  void addPin(pin*);
  void removePin(pin*);
  
  node* getNode(int id);
  link* getLink(int id);
  pin* getPin(int id);
  
  const std::map<int, node*>& nodes() const;
  const std::map<int, link*>& links() const;
  
};

void save_xml(graph*, const char*);
graph* read_xml(const char*);

} //namespace
