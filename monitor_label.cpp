#include <string>

#include <imgui.h>
#include <Python.h>

#include "monitor_label.h"
#include "graph.h"

namespace pynodes {

monitor_label::monitor_label(graph* g, const char* lbl) : monitor_base(g) {
  setLabel(lbl);
} 

bool monitor_label::show() {
  char buf[113];
  strcpy(buf,getLabel());
  ImGui::PushItemWidth(33);
  if(ImGui::InputText(("##label_"+std::to_string(id())).c_str(), buf, 113))
    setLabel(buf);
  ImGui::PopItemWidth();
  if(m_pyobj && PyUnicode_Check(m_pyobj)) {
    Py_ssize_t sz;
    wchar_t* wc = PyUnicode_AsWideCharString(m_pyobj, &sz);
    std::string txt;
    for(int i=0;i<sz;i++)
      txt += (char) wc[i];
    ImGui::SameLine();
    ImGui::PushItemWidth(133);
    ImGui::InputText(("##value_"+std::to_string(id())).c_str(),const_cast<char*>(txt.c_str()), txt.length(), ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();
    PyMem_Free(wc);
  }
  ImGui::SameLine();
  if(ImGui::Button(("x##"+std::to_string(id())+"_delete_button").c_str()))
    return false;
  
  return true;
}

const char* monitor_label::getType() const {
  return "monitor_label";
}

const char* monitor_label::sgetType() {
  return "monitor_label";
}

} //namespace
