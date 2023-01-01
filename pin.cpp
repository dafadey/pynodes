#include "pin.h"
#include "link.h"
#include "graph.h"
#include "node.h"

#include <iostream>
namespace pynodes {

pin::pin(node* nd) : base_item(nd->getGraph()->getNewUniqueId()), m_node(nd) {
  m_node->addPin(this);
  m_node->getGraph()->addPin(this);
}

pin::~pin() {
  for(int i=m_links.size()-1; i>=0; i--)
    delete m_links[i];
  m_node->removePin(this);
  m_node->getGraph()->removePin(this);
}

void pin::addLink(link* lnk) {
  m_links.push_back(lnk);
}

void pin::removeLink(link* lnk) {
  for(auto it = m_links.begin(); it != m_links.end(); it++) {
    if(*it==lnk) {
      m_links.erase(it);
      break;
    }
  }
}

const std::vector<link*>& pin::links() const {
  return m_links;
}

node* pin::getNode() {
  return m_node;
}

const node* pin::getNode() const {
  return m_node;
}

void pin::setType(ePinType pt) {
  m_pintype = pt;
}

pin::ePinType pin::getType() const {
  return m_pintype;
}

}//namespace
