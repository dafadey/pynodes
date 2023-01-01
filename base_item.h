#pragma once

#include <string>
namespace pynodes {

class base_item {
  int m_id;
  std::string m_label;
public:
  base_item() = delete;
  base_item(int id);
  virtual ~base_item();
  int id() const;
  void setLabel(const char*);
  const char* getLabel() const;
};
} // namespace
