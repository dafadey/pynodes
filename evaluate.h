#pragma once
#include <vector>

namespace pynodes {
class node;
class graph;

class evaluator {
  std::vector<node*> m_activeFront;
  void evaluate();
public:
  bool evaluate(node*); // returns true if node was added to m_activeFront
  int evaluate(graph*); // returns number of added active nodes
};

} // namespace
