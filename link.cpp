#include "link.h"
#include "graph.h"
#include "node.h"
#include "pin.h"
namespace pynodes {

link::~link() {
  m_graph->removeLink(this);
  m_begin->removeLink(this);
  m_end->removeLink(this);
}

link::link(graph* g, pin* pbeg, pin* pend) : base_item(g->getNewUniqueId()), m_graph(g), m_begin(pbeg), m_end(pend) {
  m_begin->addLink(this);
  m_end->addLink(this);
  m_graph->addLink(this);
}

const pin* link::begin() const {
  return m_begin;
}

const pin* link::end() const {
  return m_end;
}
} //namespace
