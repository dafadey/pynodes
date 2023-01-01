#include <iostream>

#include "graph.h"
#include "pin.h"
#include "link.h"
#include "node.h"
namespace pynodes {

graph::graph() : base_item(-1) {}

graph::~graph() {
  std::vector<link*> links_to_remove;
  for(auto& it : m_linkMap)
    links_to_remove.push_back(it.second);
  for(auto lnk : links_to_remove)
    delete lnk;

  std::vector<node*> nodes_to_remove;
  for(auto& it : m_nodeMap)
    nodes_to_remove.push_back(it.second);
  for(auto nd : nodes_to_remove)
    delete nd;
  
}

void graph::addNode(node* nd) {
  m_nodeMap[nd->id()] = nd;
}
void graph::addLink(link* lnk) {
  m_linkMap[lnk->id()] = lnk;
}

void graph::removeLink(link* lnk) {
  m_linkMap.erase(lnk->id());
}

void graph::removeNode(node* nd) {
  m_nodeMap.erase(nd->id());
}

void graph::addPin(pin* p) {
  m_pinMap[p->id()] = p;
}

void graph::removePin(pin* p) {
  m_pinMap.erase(p->id());
}

int graph::getNewUniqueId() const {
  return m_currentId++;
}

const std::map<int, node*>& graph::nodes() const {
  return m_nodeMap;
}

const std::map<int, link*>& graph::links() const {
  return m_linkMap;
}

node* graph::getNode(int id) {
  auto it = m_nodeMap.find(id);
  if(it==m_nodeMap.end())
    return nullptr;
  return it->second;
}

link* graph::getLink(int id) {
  auto it = m_linkMap.find(id);
  if(it==m_linkMap.end())
    return nullptr;
  return it->second;
}

pin* graph::getPin(int id) {
  auto it = m_pinMap.find(id);
  if(it==m_pinMap.end())
    return nullptr;
  return it->second;
}
} //namespace
