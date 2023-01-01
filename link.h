#pragma once
#include "base_item.h"
namespace pynodes {

class node;
class pin;
class graph;

class link : public base_item {
  graph* m_graph;
  pin* m_begin;
  pin* m_end;

public:
  link() = delete;
  link(graph* g, pin* p1, pin* p2);
  ~link();
  const pin* begin() const;
  const pin* end() const;
};
}//namespace
