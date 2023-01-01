#pragma once
#include <vector>
#include "base_item.h"
#include "pyobj.h"

namespace pynodes {
class node;
class link;

class pin : public base_item, public pyobj {
public:
  enum ePinType {IN, OUT};
private:
  node* m_node{};
  ePinType m_pintype{ePinType::IN};
  std::vector<link*> m_links;
public:
  pin() = delete;
  pin(node*);
  virtual ~pin();

  void setType(ePinType);

  ePinType getType() const;
  
  const node* getNode() const;
  node* getNode();
  void addLink(link* lnk);
  void removeLink(link* lnk);
  const std::vector<link*>& links() const;

};

} //namespace
