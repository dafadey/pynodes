#include <set>

#include "evaluate.h"

#include "graph.h"
#include "node.h"
#include "pin.h"
#include "param.h"
#include "link.h"

namespace pynodes {

void evaluator::evaluate() {
  std::set<const node*> candidates;
  for(auto& nd : m_activeFront) {
    if(!nd->run()) {
      nd->invalidateOutput();
      continue;
    }
    for(auto p : nd->pins()) {
      if(p->getType() == pin::ePinType::IN)
        continue;
      for(auto lnk : p->links()) {
        const pin* sibling_p = lnk->end();
        if(sibling_p && sibling_p->getNode())
          candidates.insert(sibling_p->getNode());
      }
    }
  }
  //cool! we have run all active nodes and collected all candidates (nodes that connected to output of current active nodes). we need to check if those nodes are valid to add them to new active front. in general it is not always true.
  m_activeFront.clear();
  for(auto& nd : candidates) {
    if(nd->isValid())
      m_activeFront.push_back(const_cast<node*>(nd));
  }
  
  //ok use recurse call now
  if(m_activeFront.size())
    evaluate();
}

bool evaluator::evaluate(node* nd) {
  //try to add nd to activeFront if possible
  //nd can be added if all pins and all params are valid
  if(nd->isValid()) {
    m_activeFront.push_back(nd);
    evaluate();
    return true;
  }
  return false;
}

int evaluator::evaluate(graph* g) {
  //here we will just apply all parameters and try to add nodes to front.
  //this method is supposed to be used right after new flow is loaded
  //only pure input nodes will be actually added to active front
  //because they do not have input, just input parameters
  int count=0;
  for(auto& it : g->nodes()) {
    auto nd = it.second;
    if(nd->params().size() == 0)
      continue;
    for(auto& par : nd->params())
      par.run();
    if(nd->isValid()) {
      m_activeFront.push_back(nd);
      count++;
    }
  }
  evaluate();
  return count;
}

} // namespace
