#include <iostream>
#include <limits>
#include <filesystem>
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
//#include <imgui_node_editor_internal.h>
#include <imgui_node_editor.h>
#include <TextEditor.h>
#include "graph.h"
#include "node.h"
#include "pin.h"
#include "monitor_label.h"
#include "link.h"
#include "pyexecutable.h"
#include "locator.h"
#include "pylangdef.h"
#include "xmlio.h"
#include "utils.h"
#include "evaluate.h"
#include "imgui_open_save.h"
#include "node_library.h"

void showNodeLidbrary(pynodes::nodeLibrary*& nl) {
  if(ImGui::Button("save"))
    ImGui::openSaveDialog::activateSaveFileDialog();
  if(ImGui::openSaveDialog::saveFileDialog())
    pynodes::save_xml(nl,  ImGui::openSaveDialog::file().c_str());
  ImGui::SameLine();
  if(ImGui::Button("open"))
    ImGui::openSaveDialog::activateOpenFileDialog();
  if(ImGui::openSaveDialog::openFileDialog()) {
    delete nl;
    nl = pynodes::openNodeLibrary_xml(ImGui::openSaveDialog::file().c_str());
  }

  if(ImGui::TreeNode(nl->name())) {
    for(auto& sn : nl->nodes()) {
      ImGui::Selectable(sn.name());
      // Our buttons are both drag sources and drag targets here!
      if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
      {
          //std::cout << "Dragging item " << sn.name() << '\n';
          // Set payload to carry the index of our item (could be anything)
          pynodes::shallowNode* shallowNode_ptr = &sn;
          ImGui::SetDragDropPayload("DND_DEMO_CELL", &shallowNode_ptr, sizeof(pynodes::shallowNode*));
          // Display preview (could be anything, e.g. when dragging an image we could decide to display
          // the filename and a small preview of the image, etc.)
          ImGui::Text(sn.name());
          ImGui::EndDragDropSource();
      }
    }
    
    for(auto& c : nl->children())
      showNodeLidbrary(c);
    ImGui::TreePop();
  }
}

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "Glfw Error " << error << ": " << description << '\n' << std::flush;
}

static ImGuiWindowFlags GetWindowFlags()
{
    return
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;
}

namespace testing {
  pynodes::node* addNodeABCD(ImVec2 pos, pynodes::graph* g, locate& loc, const char* label, const char* n1label, const char* n2label, const char* n3label, const char* n4label, const char* sct) {
    auto nd = new pynodes::node(g);
    nd->setLabel(label);
    auto p1=new pynodes::pin(nd);
    auto p2=new pynodes::pin(nd);
    auto p3=new pynodes::pin(nd);
    auto p4=new pynodes::pin(nd);
    p1->setType(pynodes::pin::ePinType::IN);
    p1->setLabel(n1label);
    p2->setType(pynodes::pin::ePinType::IN);
    p2->setLabel(n2label);
    p3->setType(pynodes::pin::ePinType::OUT);
    p3->setLabel(n3label);
    p4->setType(pynodes::pin::ePinType::OUT);
    p4->setLabel(n4label);
    nd->setScript(sct);
    loc.addNodeWithPins(nd);
    ax::NodeEditor::SetNodePosition(loc.getNodeTypeId(nd->id()), pos);
    return nd;
  }

  pynodes::node* addNodeABC(ImVec2 pos, pynodes::graph* g, locate& loc, const char* label, const char* n1label, const char* n2label, const char* n3label, const char* sct) {
    auto nd = new pynodes::node(g);
    nd->setLabel(label);
    auto p1=new pynodes::pin(nd);
    auto p2=new pynodes::pin(nd);
    auto p3=new pynodes::pin(nd);
    p1->setType(pynodes::pin::ePinType::IN);
    p1->setLabel(n1label);
    p2->setType(pynodes::pin::ePinType::IN);
    p2->setLabel(n2label);
    p3->setType(pynodes::pin::ePinType::OUT);
    p3->setLabel(n3label);
    nd->setScript(sct);
    loc.addNodeWithPins(nd);
    ax::NodeEditor::SetNodePosition(loc.getNodeTypeId(nd->id()), pos);
    return nd;
  }

  pynodes::node* addNodeAB(ImVec2 pos, pynodes::graph* g, locate& loc, const char* label, const char* n1label, const char* n2label, const char* sct) {
    auto nd = new pynodes::node(g);
    nd->setLabel(label);
    auto p1=new pynodes::pin(nd);
    auto p2=new pynodes::pin(nd);
    p1->setType(pynodes::pin::ePinType::IN);
    p1->setLabel(n1label);
    p2->setType(pynodes::pin::ePinType::OUT);
    p2->setLabel(n2label);
    nd->setScript(sct);
    loc.addNodeWithPins(nd);
    ax::NodeEditor::SetNodePosition(loc.getNodeTypeId(nd->id()), pos);
    return nd;
  }

  pynodes::node* addNodeA(ImVec2 pos, pynodes::graph* g, locate& loc, const char* label, const char* n1label, const char* sct) {
    auto nd = new pynodes::node(g);
    nd->setLabel(label);
    auto p1=new pynodes::pin(nd);
    p1->setType(pynodes::pin::ePinType::OUT);
    p1->setLabel(n1label);
    nd->setScript(sct);
    loc.addNodeWithPins(nd);
    ax::NodeEditor::SetNodePosition(loc.getNodeTypeId(nd->id()), pos);
    return nd;
  }

  pynodes::node* monitor(ImVec2 pos, pynodes::graph* g, locate& loc, const char* label, const char* n1label, const char* sct) {
    auto nd = new pynodes::node(g);
    nd->setLabel(label);
    auto p1=new pynodes::pin(nd);
    p1->setType(pynodes::pin::ePinType::IN);
    p1->setLabel(n1label);
    nd->setScript(sct);
    auto mon = new pynodes::monitor_label(g,"mon");
    nd->addMonitor(mon);
    loc.addNodeWithPins(nd);
    ax::NodeEditor::SetNodePosition(loc.getNodeTypeId(nd->id()), pos);
    return nd;
  }

  void addLink(locate& loc, pynodes::graph* g, pynodes::pin* p1, pynodes::pin* p2) {
    auto lnk = new pynodes::link(g, p1, p2);
    loc.addLink(lnk);
  }
};

std::map<std::string, std::string> getAttribs(const std::string& attrib) {
  std::map<std::string, std::string> res{};
  int name_value=0; //0 name, 1 value
  std::string name{};
  std::string value{};
  for(char a : attrib) {
    if(a==' ')
      continue;
    
    if(a=='=') {
      name_value=1;
      value.clear();
      continue;
    }
      
    if(a==',') {
      name_value=0;
      res[name]=value;
      name.clear();
      value.clear();
      continue;
    }
    
    if(name_value==0)
      name+=a;
    if(name_value==1)
      value+=a;
  }
  if(!name.empty())
    res[name]=value;
  return res;
}

// 333, 243, some other info
ImVec2 getCoordsFromAttrib(const std::string& attrib) {
  ImVec2 pos;
  auto attrmap = getAttribs(attrib);
  return ImVec2(std::atoi(attrmap["x"].c_str()), std::atoi(attrmap["y"].c_str()));
}

int main(int argc, char* argv[]) {
  std::cout << "Hallo!\n";
    
  pynodes::pyExecutable::initPyExecutables();
    
  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) {
    std::cerr << "interface::init: ERROR: failed init glfw\n";
    return false;
  }

  bool reset_layout = !std::filesystem::exists("imgui.ini");
  
  ax::NodeEditor::EditorContext* node_editor_context{};
  
  GLFWwindow* window{};

  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
  
  // Create window with graphics context
  window = glfwCreateWindow(1280, 768, "pynodes", NULL, NULL);
  if (window == NULL) {
    std::cerr << "interface::init: ERROR: failed to create glfw window\n";
    return false;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();
  //ImGui::StyleColorsLight();
  ImGui::GetStyle().FrameBorderSize=0;
  ImGui::GetStyle().FrameRounding=7;
  ImGui::GetStyle().WindowRounding=7;

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ax::NodeEditor::Config config;
  config.SettingsFile = "BasicInteraction.json";
  node_editor_context = ax::NodeEditor::CreateEditor(&config);
    
  pynodes::graph* g = new pynodes::graph;
  g->setLabel("testing graph");
  
  ImVec2 lastImGuiMousePos{-1,-1};
  ax::NodeEditor::NodeId lastNodeAdded(0);
  
  locate loc;
  pynodes::evaluator eval;
  
  TextEditor editor;
  auto lang = PyhtonLangDef();
  editor.SetLanguageDefinition(lang);
    
  pynodes::node* scriptingNode=nullptr;
  
  std::map<int, std::string> appendedAttribs{}; // used to store nodes appended during open of new graph
  
  pynodes::nodeLibrary* library = new pynodes::nodeLibrary("root");
  
  auto duplicateNode = [&](pynodes::node* nd) {
    pynodes::shallowNode snd(nd);
    auto duplicated = snd.createNode(g);
    lastNodeAdded = loc.addNodeWithPins(duplicated);
    lastImGuiMousePos = ax::NodeEditor::CanvasToScreen(ImGui::GetMousePos());
  };
  
  auto goToScript = [&](pynodes::node* nd) {
    scriptingNode = nd;
    editor.SetText(nd->getScript());
  };
  
  while (!glfwWindowShouldClose(window)) {

    //glfwWaitEvents(); // never ever! it fucks up imgui. for god sake whyyyy?!
    glfwPollEvents();
    //ren.nocallbacks = ImGui::GetIO().WantCaptureMouse;
    //if(ren.nocallbacks)
    //  glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
    //else
    //  ren.set_callbacks(iface.window);

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    //std::cout << "============>ImGuiContext.Windows.size()=" << ImGui::GetCurrentContext()->Windows.size() << '\n';
    //for(int i=0;i<ImGui::GetCurrentContext()->Windows.size();i++)
    //  std::cout << "============>>" << ImGui::GetCurrentContext()->Windows[i]->Name << '\n';

    ax::NodeEditor::SetCurrentEditor(node_editor_context);

    if(reset_layout) {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x*2/3, io.DisplaySize.y));
    }

    ImGui::Begin(g->getLabel());//, nullptr, GetWindowFlags());
    if(!ImGui::IsWindowCollapsed()) {

      if(ImGui::Button("Open"))
        ImGui::openSaveDialog::activateOpenFileDialog();
      if(ImGui::openSaveDialog::openFileDialog()) {
        appendedAttribs = pynodes::append_xml(g, ImGui::openSaveDialog::file().c_str());
        loc.update(g);
        for(auto& itappended : appendedAttribs)
          ax::NodeEditor::SetNodePosition(loc.getNodeTypeId(itappended.first), getCoordsFromAttrib(itappended.second.c_str()));
        eval.evaluate(g);
      }

      ImGui::SameLine();
      if(ImGui::Button("Save"))
        ImGui::openSaveDialog::activateSaveFileDialog();
      if(ImGui::openSaveDialog::saveFileDialog()) {
        std::map<int, std::string> attrMap;
        for(auto itnd : g->nodes()) {
          int id=itnd.first;
          ImVec2 pos= ax::NodeEditor::GetNodePosition(loc.getNodeTypeId(id));
          attrMap[id] = "x="+std::to_string(pos.x)+", y="+std::to_string(pos.y);
        }
        pynodes::save_xml(const_cast<const pynodes::graph*>(g), ImGui::openSaveDialog::file().c_str(), attrMap);
      }
      ImGui::SameLine();
      if(ImGui::Button("Clear")) {
        delete g;
        loc.clear();
        g = new pynodes::graph;
        g->setLabel("new graph");
        // THERE IS NOT NEED TO DELETE any of ax::NodeEditor since they are created on the fly (you can check with GetNodeCount)
      }
      ImGui::SameLine();
      if(ImGui::Button("[]"))
        ax::NodeEditor::NavigateToContent();
      ImGui::SameLine();
      if(ImGui::Button("eval"))
        eval.evaluate(g);
      
      ax::NodeEditor::SetCurrentEditor(node_editor_context);

      //ax::NodeEditor::GetStyle().PinRadius = 13.f;
      //ax::NodeEditor::GetStyle().PinArrowSize = 11.f;
      //ax::NodeEditor::GetStyle().PinArrowWidth = 7.f;

      ax::NodeEditor::Begin(g->getLabel(), ImVec2(0.0, 0.0f));

      static bool first_run=true;
      if(first_run) {
        auto tn1=testing::addNodeA(ImVec2{100,100}, g, loc, "setA", "A", "def pynode(p1):\n\tprint('returning p1=',p1)\n\treturn {'A' : p1}");
        auto p1tn1=pynodes::param(g,"p1");
        tn1->addParam(p1tn1);
        auto tn2=testing::addNodeA(ImVec2{100,300}, g, loc, "setB", "B", "def pynode(p2):\n\tprint('returning p2=',p2)\n\treturn {'B' : p2}");
        auto p1tn2=pynodes::param(g,"p2");
        tn2->addParam(p1tn2);
        auto tn3=testing::addNodeABCD(ImVec2{333,200}, g, loc, "calc", "A", "B", "C", "D", "def pynode(A,B):\n\tprint('heeeey!')\n\tprint('going to sum '+str(A)+' and '+str(B))\n\treturn {'C' : A+B, 'D' : A-B}");
        auto tn4=testing::monitor(ImVec2{512,100}, g, loc, "monC", "A", "def pynode(A):\n\tprint('monitor: ', A)\n\treturn {'mon' : str(A)}");
        auto tn5=testing::monitor(ImVec2{512,300}, g, loc, "monD", "A", "def pynode(A):\n\tprint('monitor: ', A)\n\treturn {'mon' : str(A)}");
        testing::addLink(loc, g, tn1->getPin("A"), tn3->getPin("A"));
        testing::addLink(loc, g, tn2->getPin("B"), tn3->getPin("B"));

        testing::addLink(loc, g, tn3->getPin("C"), tn4->getPin("A"));
        testing::addLink(loc, g, tn3->getPin("D"), tn5->getPin("A"));

      }
      first_run=false;
      
      
      if (ImGui::BeginDragDropTarget())
      {
        //std::cout << "Drop!\n";
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
        {
          //IM_ASSERT(payload->DataSize == sizeof(int));
          auto payload_sn_ptr = static_cast<const pynodes::shallowNode**>(payload->Data);
          //std::cout << "droping shallowNode " << *payload_sn_ptr << '\n';
          auto n = (*payload_sn_ptr)->createNode(g);
          lastNodeAdded = loc.addNodeWithPins(n);
          lastImGuiMousePos = ax::NodeEditor::CanvasToScreen(ImGui::GetMousePos());
        }
        ImGui::EndDragDropTarget();
      }
      
      int pin_to_remove=-1;
      for(const auto& it : g->nodes()) {
        const pynodes::node* nd = it.second;
        ax::NodeEditor::NodeId nodeid = loc.getNodeTypeId(nd->id());
        
        if(lastNodeAdded == nodeid) {
          std::cout << "adding node at " << lastImGuiMousePos.x << ", " << lastImGuiMousePos.y << '\n';
          ax::NodeEditor::SetNodePosition(nodeid, ax::NodeEditor::ScreenToCanvas(lastImGuiMousePos));
          lastNodeAdded = ax::NodeEditor::NodeId(0);
        }
        ax::NodeEditor::BeginNode(nodeid);
        ImGui::PushID(nd->id());
        if(ImGui::Button("x"))
          ax::NodeEditor::DeleteNode(nodeid);
        ImGui::SameLine();
        if(ImGui::Button("||"))
          duplicateNode(const_cast<pynodes::node*>(nd));
        ImGui::SameLine();
        if(ImGui::Button("="))
          goToScript(const_cast<pynodes::node*>(nd));
        ImGui::SameLine();
        if(ImGui::Button("*"))
          library->addNode(const_cast<pynodes::node*>(nd));
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::Text(nd->getLabel());
        ImGui::SameLine();
        ImGui::Text(nd->isValid() ? "   valid" : "   invalid");

        /*
        ImGui::PushID(nd->id());
        if(ImGui::Button("Run"))
          const_cast<pynodes::node*>(nd)->run();
        ImGui::PopID();
        */
        
        ImGui::BeginGroup();
        for(const auto& p : nd->pins()) {
          if(p->getType() != pynodes::pin::ePinType::IN)
            continue;
          ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(p->id() + 1), ax::NodeEditor::PinKind::Input);
          ImGui::Text(p->getLabel());
          ImGui::SameLine();
          ImGui::PushID(p->id());
          if(ImGui::Button("x"))
            pin_to_remove = p->id();
          ImGui::PopID();
          ax::NodeEditor::EndPin();
        }
        ImGui::PushID(nd->id());
        if(ImGui::Button("+##IN")) {
          auto p=const_cast<pynodes::node*>(nd)->makePin("in", pynodes::pin::ePinType::IN);
          loc.addPin(p);
        }
        ImGui::PopID();
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::BeginGroup();
        float curpos0=ImGui::GetCursorPosX(); 
        float curpos=ImGui::GetCursorPosX(); 
        for(auto& par : const_cast<pynodes::node*>(nd)->params()) {
          ImGui::PushID(par.id());
          char buf[113];
          strcpy(buf,par.getLabel());
          ImGui::PushItemWidth(33);
          if(ImGui::InputText(("##"+std::to_string(par.id())).c_str(), buf, 113))
            par.setLabel(buf);
          ImGui::SameLine();
          ImGui::PopItemWidth();
          strcpy(buf,par.getValue());
          ImGui::PushItemWidth(49);
          ImGui::PushStyleColor(ImGuiCol_Text, par.applied() ? IM_COL32(255,255,255,255) : IM_COL32(255,0,0,255));
          if(ImGui::InputText(("##"+std::to_string(par.id())+"_value").c_str(), buf, 113)) {
            par.setValue(buf);
            par.setUnapplied();
          }
          ImGui::PopStyleColor();
          ImGui::PopItemWidth();

          if(!par.applied()) {
            ImGui::SameLine();
            if(ImGui::Button("v")) {
              par.run();
              eval.evaluate(const_cast<pynodes::node*>(nd));
              par.setApplied();
            }
          }

          ImGui::SameLine();
          curpos = std::max(curpos, ImGui::GetCursorPosX()+ImGui::CalcTextSize("x").x*2);
          if(ImGui::Button("x"))
            const_cast<pynodes::node*>(nd)->removeParam(par);
          ImGui::PopID();
        }
        ImGui::PushID(nd->id());
        ImGui::SetCursorPosX((curpos + curpos0)*0.5 - ImGui::CalcTextSize("+").x); 
        if(ImGui::Button("+##_add_param_button")) {
          pynodes::param par(g, "par");
          const_cast<pynodes::node*>(nd)->addParam(par);
        }
       
        for(auto mon : const_cast<pynodes::node*>(nd)->monitors()) {
          if(!mon->show())
            const_cast<pynodes::node*>(nd)->removeMonitor(mon);
        }
        ImGui::SetCursorPosX((curpos + curpos0)*0.5 - ImGui::CalcTextSize("+").x); 
        if(ImGui::Button("+##_add_monitor_button")) {
          auto mon = new pynodes::monitor_label(g, "mon");
          const_cast<pynodes::node*>(nd)->addMonitor(mon);
        }
        ImGui::PopID();
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();
        ImGui::BeginGroup();
        curpos=ImGui::GetCursorPosX(); 
        for(const auto& p : nd->pins()) {
          if(p->getType() != pynodes::pin::ePinType::OUT)
            continue;
          ax::NodeEditor::BeginPin(ax::NodeEditor::PinId(p->id() + 1), ax::NodeEditor::PinKind::Output);
          ImGui::PushID(p->id());
          if(ImGui::Button("x"))
            pin_to_remove = p->id();
          ImGui::PopID();
          ImGui::SameLine();
          ImGui::Text(p->getLabel());
          ax::NodeEditor::EndPin();
          curpos = std::max(curpos, ImGui::GetCursorPosX()+ImGui::GetItemRectSize().x);
        }

        ImGui::PushID(nd->id());
        ImGui::SetCursorPosX(curpos-ImGui::CalcTextSize("+").x); 
        if(ImGui::Button("+##OUT")) {
          auto p=const_cast<pynodes::node*>(nd)->makePin("out", pynodes::pin::ePinType::OUT);
          loc.addPin(p);
        }
        ImGui::PopID();

        ImGui::EndGroup();
        
        ax::NodeEditor::EndNode();
      }

      for(const auto& it : g->links()) {
        const pynodes::link* lnk = it.second;
        ax::NodeEditor::Link(loc.getLinkTypeId(lnk->id()), loc.getPinTypeId(lnk->begin()->id()), loc.getPinTypeId(lnk->end()->id()));
      }

      //select nodes
      for(const auto& it : appendedAttribs)
        ax::NodeEditor::SelectNode(loc.getNodeTypeId(it.first), true);

      if(appendedAttribs.size())
        appendedAttribs.clear();

      static ax::NodeEditor::NodeId contextNodeId = 0;
      static ax::NodeEditor::LinkId contextLinkId = 0;
      static ax::NodeEditor::PinId  contextPinId  = 0;

      ax::NodeEditor::Suspend();
      if (ax::NodeEditor::ShowNodeContextMenu(&contextNodeId))
          ImGui::OpenPopup("Node Context Menu");
      else if (ax::NodeEditor::ShowPinContextMenu(&contextPinId))
          ImGui::OpenPopup("Pin Context Menu");
      else if (ax::NodeEditor::ShowLinkContextMenu(&contextLinkId))
          ImGui::OpenPopup("Link Context Menu");
      else if (ax::NodeEditor::ShowBackgroundContextMenu())
          ImGui::OpenPopup("Bg context menu");
      ax::NodeEditor::Resume();

      ax::NodeEditor::Suspend();
              
      if (ImGui::BeginPopup("Node Context Menu"))
      {
          auto nd = g->getNode(loc.getNodeId(contextNodeId));
          char buf[113];
          strcpy(buf, nd->getLabel());
          if(ImGui::InputText("Label", buf, 113))
            nd->setLabel(buf);
          ImGui::Separator();
          if (ImGui::MenuItem("Duplicate"))
            duplicateNode(nd);
          ImGui::Separator();
          if (ImGui::MenuItem("Add in pin")) {
            if(nd) {
              auto p=nd->makePin("in", pynodes::pin::ePinType::IN);
              loc.addPin(p);
            }
          }
          if (ImGui::MenuItem("Add out pin")) {
            if(nd) {
              auto p=nd->makePin("out", pynodes::pin::ePinType::OUT);
              loc.addPin(p);
            }
          }
          if (ImGui::MenuItem("Add param")) {
            if(nd) {
              pynodes::param par(g, "par");
              nd->addParam(par);
            }
          }
          if (ImGui::MenuItem("Add monitor")) {
            if(nd) {
              auto mon = new pynodes::monitor_label(g, "mon");
              nd->addMonitor(mon);
            }
          }
          ImGui::Separator();
          if (ImGui::MenuItem("Go to script"))
            goToScript(nd);
          ImGui::Separator();

          if (ImGui::MenuItem("Delete")) {
            ax::NodeEditor::DeleteNode(contextNodeId);
          }
          ImGui::Separator();
          if (ImGui::MenuItem("Add to library"))
            library->addNode(nd);
          ImGui::EndPopup();
      }
      
      if(pin_to_remove != -1)
        delete g->getPin(pin_to_remove);
      
      if (ImGui::BeginPopup("Pin Context Menu")) {
        auto p = g->getPin(loc.getPinId(contextPinId));
        char buf[113];
        strcpy(buf, p->getLabel());
        if(ImGui::InputText("Label", buf, 113))
          p->setLabel(buf);
        if (ImGui::MenuItem("Delete")) {
          if(p)
            delete p;
        }
        ImGui::EndPopup();
      }
      
      if(ImGui::BeginPopup("Bg context menu")) {
        //ImGui::TextUnformatted("background context menu");
        static char buf[113]={0};
        if(buf[0] == 0)
          strcpy(buf, g->getLabel());
        ImGui::InputText("graph name", buf, 113);
        ImGui::SameLine();
        if(ImGui::Button("ok")) {
          g->setLabel(buf);
          ImGui::CloseCurrentPopup();
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Add node")) {
          lastImGuiMousePos = ImGui::GetMousePos();
          auto nd = new pynodes::node(g);
          nd->setLabel("node");
          auto p1=new pynodes::pin(nd);
          auto p2=new pynodes::pin(nd);
          auto p3=new pynodes::pin(nd);
          p1->setType(pynodes::pin::ePinType::IN);
          p1->setLabel("A");
          p2->setType(pynodes::pin::ePinType::IN);
          p2->setLabel("B");
          p3->setType(pynodes::pin::ePinType::OUT);
          p3->setLabel("C");
          lastNodeAdded = loc.addNodeWithPins(nd);
        }
          
        ImGui::EndPopup();
      }
      
      ax::NodeEditor::Resume();

      ax::NodeEditor::DeleteLink(ax::NodeEditor::GetDoubleClickedLink());

      if (ax::NodeEditor::BeginDelete()) {
        ax::NodeEditor::NodeId nodeId = 0;
        while (ax::NodeEditor::QueryDeletedNode(&nodeId)) {
          if (ax::NodeEditor::AcceptDeletedItem())  {
            auto ndToDelete = g->getNode(loc.getNodeId(nodeId));
            if(ndToDelete)
              delete ndToDelete;
          }
        }

        ax::NodeEditor::LinkId deletedLinkId = 0;
        while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId))
        {
          if (ax::NodeEditor::AcceptDeletedItem()) {
            auto lnk = g->getLink(loc.getLinkId(deletedLinkId));
            if(lnk)
              delete lnk;
          }
        }
        ax::NodeEditor::EndDelete();
      }
      
      if (ax::NodeEditor::BeginCreate())
      {
        ax::NodeEditor::PinId inputPinId, outputPinId;
        if (ax::NodeEditor::QueryNewLink(&inputPinId, &outputPinId))
        {
          if (inputPinId && outputPinId) // both are valid, let's accept link
          {
            auto p1=g->getPin(loc.getPinId(inputPinId));
            auto p2=g->getPin(loc.getPinId(outputPinId));
            //rearrange nodes as being and end node, link ends in input pin and starts from output pin
            auto pbegin = p1->getType() == pynodes::pin::ePinType::OUT ? p1 : p2;
            auto pend = p2->getType() == pynodes::pin::ePinType::IN ? p2 : p1;
            //now we need to make sure that rearranging was ok an pbegin is actually OUT and pend is IN, also make sure IN pin is not linked yet
            //only one link may be connected to input pin by design
            if(pbegin->getType() == pynodes::pin::ePinType::OUT && pend->getType() == pynodes::pin::ePinType::IN
               && pend->links().size() == 0
               && pbegin->getNode() != pend->getNode()) {
              if (ax::NodeEditor::AcceptNewItem())
              {
                  auto lnk = new pynodes::link(g, p1, p2);
                  ax::NodeEditor::Link(loc.addLink(lnk), inputPinId, outputPinId);
              }
            } else
              ax::NodeEditor::RejectNewItem();
          }
        }
      }
      ax::NodeEditor::EndCreate(); // Wraps up object creation action handling.

      ax::NodeEditor::End();
      ax::NodeEditor::SetCurrentEditor(nullptr);
    } // collapsed main frame
    ImGui::End();

    if(reset_layout) {
      ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x*2/3, 0));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x/3, io.DisplaySize.y/2));
    }

    ImGui::Begin("node python script");
    ImGui::Text(scriptingNode ? (std::string("script for \"") +std::string(scriptingNode->getLabel()) + std::string("\"")).c_str() : "");
    if(scriptingNode && strcmp(scriptingNode->getScript(), editor.GetText().c_str())) {
      ImGui::SameLine();
      if(ImGui::Button("Save"))
        scriptingNode->setScript(editor.GetText().c_str());
      ImGui::SameLine();
      if(ImGui::Button("Revert"))
        editor.SetText(std::string(scriptingNode->getScript()));
    }
    editor.Render("TextEditor");
    ImGui::End();

    if(reset_layout) {
      ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x*2/3, io.DisplaySize.y/2));
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x/3, io.DisplaySize.y/2));
    }

    ImGui::Begin("nodes library");
    showNodeLidbrary(library);
    ImGui::End();

    ImGui::Render();

    glClearColor(.3, .3, .3, 1.);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    reset_layout=false;
  }
  
  ax::NodeEditor::DestroyEditor(node_editor_context);
  
  delete g;
  delete library;
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

}
