#pragma once
#include <map>
#include <functional>
#include <imgui_node_editor.h>

/*This is an interface between graph and ax::NodeEditor to map between ax::NodeEditor::NodeId and graph's id-s*/

template<typename T>
struct cmpByPointer {
  bool operator()(const T& a, const T& b) const {
    return a.AsPointer() < b.AsPointer();
  }
};

template<typename T>
class locator {
  const T m_nullTypeId{0};
  std::map<int, T> m_idToTypeId;
  std::map<T, int, cmpByPointer<T>> m_typeIdToId;

public:

  void clear() {
    m_idToTypeId.clear();
    m_typeIdToId.clear();
  }

  int getId(T typeId) const {
    auto it = m_typeIdToId.find(typeId); 
    if(it == m_typeIdToId.end())
      return -1;
    return it->second;    
  }

  const T& getTypeId(int id) const {
    auto it = m_idToTypeId.find(id); 
    if(it == m_idToTypeId.end())
      return m_nullTypeId;
    return it->second;
  }

  void remove(int id) {
    auto it = m_idToTypeId.find(id); 
    if(it == m_idToTypeId.end())
      return;
    m_typeIdToId.erase(it->second);
    m_idToTypeId.erase(id);
  }
  
  void remove(T typeId) {
    auto it = m_typeIdToId.find(typeId); 
    if(it == m_typeIdToId.end())
      return;
    m_idToTypeId.erase(it->second);
    m_typeIdToId.erase(typeId);
  }
  
  void add(int id, T typeId) {
    auto it = m_idToTypeId.find(id);
    if(it == m_idToTypeId.end()) {
      m_idToTypeId[id] = typeId;
      m_typeIdToId[typeId] = id;
    }
  }
};
