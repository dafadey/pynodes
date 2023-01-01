#include "utils.h"

#include "graph.h"
#include "node.h"
#include "pin.h"
#include "link.h"

ax::NodeEditor::NodeId locate::addNodeWithPins(const pynodes::node* item) {
  auto Id = ax::NodeEditor::NodeId(item->id() + 1);
  m_locNode.add(item->id(), Id);
  for(auto p : item->pins()) {
    auto pinId = ax::NodeEditor::PinId(p->id() + 1);
    m_locPin.add(p->id(), pinId);
  }
  return Id;
}

ax::NodeEditor::NodeId locate::addNode(const pynodes::node* item) {
  auto Id = ax::NodeEditor::NodeId(item->id() + 1);
  m_locNode.add(item->id(), Id);
  return Id;
}

ax::NodeEditor::PinId locate::addPin(const pynodes::pin* item) {
  auto Id = ax::NodeEditor::PinId(item->id() + 1);
  m_locPin.add(item->id(), Id);
  return Id;
}

ax::NodeEditor::LinkId locate::addLink(const pynodes::link* item) {
  auto Id = ax::NodeEditor::LinkId(item->id() + 1);
  m_locLink.add(item->id(), Id);
  return Id;
}

void locate::clear() {
  m_locNode.clear();
  m_locPin.clear();
  m_locLink.clear();
}

int locate::getNodeId(const ax::NodeEditor::NodeId& id) const {
  return m_locNode.getId(id);
}

int locate::getPinId(const ax::NodeEditor::PinId& id) const {
  return m_locPin.getId(id);
}

int locate::getLinkId(const ax::NodeEditor::LinkId& id) const {
  return m_locLink.getId(id);
}

const ax::NodeEditor::NodeId& locate::getNodeTypeId(int id) const {
  return m_locNode.getTypeId(id);
}

const ax::NodeEditor::PinId& locate::getPinTypeId(int id) const {
  return m_locPin.getTypeId(id);
}

const ax::NodeEditor::LinkId& locate::getLinkTypeId(int id) const {
  return m_locLink.getTypeId(id);
}

void locate::update(const pynodes::graph* g) {
  
  for(auto& it : g->nodes()) {
    auto nd = it.second;
    addNode(nd);
    for(auto& p : nd->pins())
      addPin(p);
  }
  for(auto& it : g->links())
    addLink(it.second);
}
