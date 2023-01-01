#pragma once

#include <imgui_node_editor.h>
#include "locator.h"

namespace pynodes {
class graph;
class node;
class pin;
class link;
}

class locate {
  locator<ax::NodeEditor::NodeId> m_locNode;
  locator<ax::NodeEditor::PinId> m_locPin;
  locator<ax::NodeEditor::LinkId> m_locLink;

public:
  void update(const pynodes::graph*);

  void clear();
  
  ax::NodeEditor::NodeId addNode(const pynodes::node*);
  ax::NodeEditor::NodeId addNodeWithPins(const pynodes::node*);
  ax::NodeEditor::PinId addPin(const pynodes::pin*);
  ax::NodeEditor::LinkId addLink(const pynodes::link*);
    
  int getNodeId(const ax::NodeEditor::NodeId& id) const;
  int getPinId(const ax::NodeEditor::PinId& id) const;
  int getLinkId(const ax::NodeEditor::LinkId& id) const;

  const ax::NodeEditor::NodeId& getNodeTypeId(int id) const;
  const ax::NodeEditor::PinId& getPinTypeId(int id) const;
  const ax::NodeEditor::LinkId& getLinkTypeId(int id) const;

};
