#include "base_item.h"
namespace pynodes {

base_item::base_item(int id) : m_id(id) {}

base_item::~base_item() {}

int base_item::id() const {
  return m_id;
}

void base_item::setLabel(const char* lbl) {
  m_label = std::string(lbl);
}

const char* base_item::getLabel() const {
  return m_label.c_str();
}
} //namespace
