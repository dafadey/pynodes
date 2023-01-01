#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <map>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "xmlio.h"
#include "graph.h"
#include "node.h"
#include "pin.h"
#include "param.h"
#include "monitor_label.h"
#include "link.h"
#include "node_library.h"

static const char* get_char_buff(int value) {
  auto s_ptr = new std::string(std::to_string(value));
  return s_ptr->c_str();
}

namespace pynodes {

void save_xml(const graph* g, const char* filename, const std::map<int,std::string>& attribs) {
  const char* IN="IN";
  const char* OUT="OUT";
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
  //adding attributes at the top of our xml
  decl->append_attribute(doc.allocate_attribute("version", "1.0"));
  decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
  doc.append_node(decl);
  rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_element, "pynodes::graph");
  root->append_attribute(doc.allocate_attribute("label", g->getLabel()));
  for(auto& itnd : g->nodes()) {
    rapidxml::xml_node<>* pynode_node = doc.allocate_node(rapidxml::node_element, "pynodes::node");
    auto nd=itnd.second;
    pynode_node->append_attribute(doc.allocate_attribute("id", get_char_buff(nd->id())));
    pynode_node->append_attribute(doc.allocate_attribute("label", nd->getLabel()));
    pynode_node->append_attribute(doc.allocate_attribute("script", nd->getScript()));
    auto itattrib = attribs.find(nd->id());
    if(itattrib != attribs.end())
      pynode_node->append_attribute(doc.allocate_attribute("attributes", itattrib->second.c_str()));
      
    for(auto p : nd->pins()) {
      rapidxml::xml_node<>* pin_node = doc.allocate_node(rapidxml::node_element, "pynodes::pin");
      pin_node->append_attribute(doc.allocate_attribute("id", get_char_buff(p->id())));
      pin_node->append_attribute(doc.allocate_attribute("label", p->getLabel()));
      pin_node->append_attribute(doc.allocate_attribute("type", p->getType() == pin::ePinType::IN ? IN : OUT));
      pynode_node->append_node(pin_node);
    }
    for(auto& par : nd->params()) {
      rapidxml::xml_node<>* param_node = doc.allocate_node(rapidxml::node_element, "pynodes::param");
      param_node->append_attribute(doc.allocate_attribute("id", get_char_buff(par.id())));
      param_node->append_attribute(doc.allocate_attribute("label", par.getLabel()));
      param_node->append_attribute(doc.allocate_attribute("value", par.getValue()));
      pynode_node->append_node(param_node);
    }
    for(auto mon : nd->monitors()) {
      rapidxml::xml_node<>* monitor_node = doc.allocate_node(rapidxml::node_element, "pynodes::monitor");
      monitor_node->append_attribute(doc.allocate_attribute("label", mon->getLabel()));
      monitor_node->append_attribute(doc.allocate_attribute("type", mon->getType()));
      pynode_node->append_node(monitor_node);
    }
    root->append_node(pynode_node);
  }

  for(auto& itlnk : g->links()) {
    rapidxml::xml_node<>* link_node = doc.allocate_node(rapidxml::node_element, "pynodes::link");
    auto lnk=itlnk.second;
    link_node->append_attribute(doc.allocate_attribute("id", get_char_buff(lnk->id())));
    link_node->append_attribute(doc.allocate_attribute("label", lnk->getLabel()));
    link_node->append_attribute(doc.allocate_attribute("begin", get_char_buff(lnk->begin()->id())));
    link_node->append_attribute(doc.allocate_attribute("end", get_char_buff(lnk->end()->id())));
    root->append_node(link_node);
  }
  
  doc.append_node(root);
  //dump to file:
  std::string xml_as_string;
  rapidxml::print(std::back_inserter(xml_as_string), doc);
  std::ofstream fileStored(filename);
  fileStored << xml_as_string;
  fileStored.close();
  doc.clear();
}

std::map<int, std::string> append_xml(graph* g, const char* filename) {
  std::map<int, std::string> appendingAttribs{};
  
  std::map<int, int> newid; // newid[oldid] gives new id by old id
  std::map<int, int> oldid; // oldid[newid] fives old id
  
  std::ifstream inf(filename);
  std::stringstream buffer;
  buffer << inf.rdbuf();
  inf.close();
  std::string content(buffer.str());
  rapidxml::xml_document<> doc;
  doc.parse<0>(&content[0]);
  rapidxml::xml_node<> *root = doc.first_node();
  //loop through attribs
  for (rapidxml::xml_attribute<> *attr = root->first_attribute(); attr; attr = attr->next_attribute()) {
    std::cout << root->name() << ':' << attr->name() << ':' << attr->value() << '\n';
  }

  //first do py nodes
  for(rapidxml::xml_node<> *xmlnode=root->first_node(); xmlnode; xmlnode=xmlnode->next_sibling()) {
    if(strcmp(xmlnode->name(), "pynodes::node")==0) {
      auto nd = new pynodes::node(g);
      appendingAttribs[nd->id()] = std::string();
      //get pynode attribs
      for (rapidxml::xml_attribute<> *attr = xmlnode->first_attribute(); attr; attr = attr->next_attribute()) {
        if(strcmp(attr->name(), "label") == 0)
          nd->setLabel(attr->value());
        if(strcmp(attr->name(), "script") == 0)
          nd->setScript(attr->value());
        if(strcmp(attr->name(), "attributes") == 0)
          appendingAttribs[nd->id()] = std::string(attr->value());
      }
      for(rapidxml::xml_node<> *xmlpin=xmlnode->first_node(); xmlpin; xmlpin=xmlpin->next_sibling()) {
        //do pynode pins
        if(strcmp(xmlpin->name(),"pynodes::pin")==0) {
          auto p = new pin(nd);
          //get pin attribs
          for (rapidxml::xml_attribute<> *attr = xmlpin->first_attribute(); attr; attr = attr->next_attribute()) {
            if(strcmp(attr->name(), "id") == 0)
              newid[std::atoi(attr->value())] = p->id();
            if(strcmp(attr->name(), "label") == 0)
              p->setLabel(attr->value());
            if(strcmp(attr->name(), "type") == 0)
              p->setType(strcmp(attr->value(),"IN")==0 ? pin::ePinType::IN : pin::ePinType::OUT);
          }
        }
        //do pynode params
        if(strcmp(xmlpin->name(),"pynodes::param")==0) {
          param par(g);
          //get pin attribs
          for (rapidxml::xml_attribute<> *attr = xmlpin->first_attribute(); attr; attr = attr->next_attribute()) {
            if(strcmp(attr->name(), "label") == 0)
              par.setLabel(attr->value());
            if(strcmp(attr->name(), "value") == 0)
              par.setValue(attr->value());
          }
          nd->addParam(par);
        }
        //do pynode monitors
        if(strcmp(xmlpin->name(),"pynodes::monitor")==0) {
          //get pin attribs
          std::string type;
          std::string lbl;
          for (rapidxml::xml_attribute<> *attr = xmlpin->first_attribute(); attr; attr = attr->next_attribute()) {
            if(strcmp(attr->name(), "label") == 0)
              lbl = std::string(attr->value());
            if(strcmp(attr->name(), "type") == 0)
              type = std::string(attr->value());
          }
          if(type == "monitor_label")
            nd->addMonitor(new monitor_label(g, lbl.c_str()));
        }
      }
    }
  }
  
  //now do links
  for(rapidxml::xml_node<> *xmlnode=root->first_node(); xmlnode; xmlnode=xmlnode->next_sibling()) {
    if(strcmp(xmlnode->name(), "pynodes::link")==0) {
      int begin_id=-1;
      int end_id=-1;
      for (rapidxml::xml_attribute<> *attr = xmlnode->first_attribute(); attr; attr = attr->next_attribute()) {
        if(strcmp(attr->name(), "begin") == 0)
          begin_id = std::atoi(attr->value());
        if(strcmp(attr->name(), "end") == 0)
          end_id = std::atoi(attr->value());
      }
      auto itbeg = newid.find(begin_id);
      auto itend = newid.find(end_id);
      if(itbeg != newid.end() && itend != newid.end())
        new link(g, g->getPin(itbeg->second), g->getPin(itend->second));
    }
  }
  return appendingAttribs;
}


void save_xml(const nodeLibrary* nl, const char* filename) {
  const char* IN="IN";
  const char* OUT="OUT";
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
  //adding attributes at the top of our xml
  decl->append_attribute(doc.allocate_attribute("version", "1.0"));
  decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
  doc.append_node(decl);
  
  std::function<rapidxml::xml_node<>*(const nodeLibrary* libgroup, rapidxml::xml_node<>*)> addGroup;
  
  addGroup = [&](const nodeLibrary* libgroup, rapidxml::xml_node<>* parent)->rapidxml::xml_node<>* {
    rapidxml::xml_node<>* libgroup_node = doc.allocate_node(rapidxml::node_element, "pynodes::group");
    libgroup_node->append_attribute(doc.allocate_attribute("name", libgroup->name()));
    for(const auto& nd : libgroup->nodes()) {
      rapidxml::xml_node<>* libnode_node = doc.allocate_node(rapidxml::node_element, "pynodes::node");
      libnode_node->append_attribute(doc.allocate_attribute("name", nd.name()));
      libnode_node->append_attribute(doc.allocate_attribute("script", nd.script()));
      for(const auto& p : nd.inpins()) {
        rapidxml::xml_node<>* libpin_node = doc.allocate_node(rapidxml::node_element, "pynodes::pin");
        libpin_node->append_attribute(doc.allocate_attribute("name", p.c_str()));
        libpin_node->append_attribute(doc.allocate_attribute("type", IN));
        libnode_node->append_node(libpin_node);
      }
      for(const auto& p : nd.outpins()) {
        rapidxml::xml_node<>* libpin_node = doc.allocate_node(rapidxml::node_element, "pynodes::pin");
        libpin_node->append_attribute(doc.allocate_attribute("name", p.c_str()));
        libpin_node->append_attribute(doc.allocate_attribute("type", OUT));
        libnode_node->append_node(libpin_node);
      }
      for(const auto& par : nd.params()) {
        rapidxml::xml_node<>* libparam_node = doc.allocate_node(rapidxml::node_element, "pynodes::param");
        libparam_node->append_attribute(doc.allocate_attribute("name", par.name()));
        libparam_node->append_attribute(doc.allocate_attribute("value", par.value()));
        libparam_node->append_attribute(doc.allocate_attribute("script", par.script()));
        libnode_node->append_node(libparam_node);
      }
      for(const auto& mon : nd.monitors()) {
        rapidxml::xml_node<>* libmonitor_node = doc.allocate_node(rapidxml::node_element, "pynodes::monitor");
        libmonitor_node->append_attribute(doc.allocate_attribute("name", mon.name()));
        libmonitor_node->append_attribute(doc.allocate_attribute("type", mon.type()));
        libnode_node->append_node(libmonitor_node);
      }
      libgroup_node->append_node(libnode_node);
    }
    
    for(auto c : libgroup->children())
      addGroup(c, libgroup_node);

    //recurse
    if(parent)
      parent->append_node(libgroup_node);
    
    return libgroup_node;
  };
  
  rapidxml::xml_node<>* root = addGroup(nl, nullptr);
  
  doc.append_node(root);
  //dump to file:
  std::string xml_as_string;
  rapidxml::print(std::back_inserter(xml_as_string), doc);
  std::ofstream fileStored(filename);
  fileStored << xml_as_string;
  fileStored.close();
  doc.clear();

}

nodeLibrary* openNodeLibrary_xml(const char* filename) {
  const std::string IN("IN");
  const std::string OUT("OUT");
  
  std::ifstream inf(filename);
  std::stringstream buffer;
  buffer << inf.rdbuf();
  inf.close();
  std::string content(buffer.str());
  rapidxml::xml_document<> doc;
  doc.parse<0>(&content[0]);

  std::function<nodeLibrary*(rapidxml::xml_node<>*)> makeGroup;

  makeGroup = [&](rapidxml::xml_node<>* root)->nodeLibrary* {
    //collect library attribs name, etc...
    nodeLibrary* group{};
    for (rapidxml::xml_attribute<>* xmlattr = root->first_attribute(); xmlattr; xmlattr = xmlattr->next_attribute()) {
      if(strcmp(xmlattr->name(),"name")==0)
        group = new nodeLibrary(xmlattr->value());
    }
    
    for(rapidxml::xml_node<> *xmlnode=root->first_node(); xmlnode; xmlnode=xmlnode->next_sibling()) {
      if(strcmp(xmlnode->name(),"pynodes::node")==0) {
        shallowNode snd;
        for (rapidxml::xml_attribute<>* xmlattr = xmlnode->first_attribute(); xmlattr; xmlattr = xmlattr->next_attribute()) {
          if(strcmp(xmlattr->name(),"name")==0)
            snd.setName(xmlattr->value());
          if(strcmp(xmlattr->name(),"script")==0)
            snd.setScript(xmlattr->value());
        }
        for(rapidxml::xml_node<> *xmlsubnode=xmlnode->first_node(); xmlsubnode; xmlsubnode=xmlsubnode->next_sibling()) {
          //pin
          if(strcmp(xmlsubnode->name(),"pynodes::pin")==0) {
            std::string pinname;
            std::string type;
            for (rapidxml::xml_attribute<>* xmlattr = xmlsubnode->first_attribute(); xmlattr; xmlattr = xmlattr->next_attribute()) {
              if(strcmp(xmlattr->name(),"name")==0)
                pinname = std::string(xmlattr->value());
              if(strcmp(xmlattr->name(),"type")==0)
                type = std::string(xmlattr->value());
            }
            if(type==IN)
              snd.inpins().push_back(pinname);
            if(type==OUT)
              snd.outpins().push_back(pinname);
          }
          //param
          if(strcmp(xmlsubnode->name(),"pynodes::param")==0) {
            std::string parname;
            std::string value;
            std::string script;
            for (rapidxml::xml_attribute<>* xmlattr = xmlsubnode->first_attribute(); xmlattr; xmlattr = xmlattr->next_attribute()) {
              if(strcmp(xmlattr->name(),"name")==0)
                parname = std::string(xmlattr->value());
              if(strcmp(xmlattr->name(),"value")==0)
                value = std::string(xmlattr->value());
              if(strcmp(xmlattr->name(),"script")==0)
                script = std::string(xmlattr->value());
            }
            snd.params().push_back(shallowParam(parname.c_str(),value.c_str(),script.c_str()));
          }
          //monitor
          if(strcmp(xmlsubnode->name(),"pynodes::monitor")==0) {
            std::string monname;
            std::string type;
            for (rapidxml::xml_attribute<>* xmlattr = xmlsubnode->first_attribute(); xmlattr; xmlattr = xmlattr->next_attribute()) {
              if(strcmp(xmlattr->name(),"name")==0)
                monname = std::string(xmlattr->value());
              if(strcmp(xmlattr->name(),"type")==0)
                type = std::string(xmlattr->value());
            }
            snd.monitors().push_back(shallowMonitor(monname.c_str(),type.c_str()));
          }
        }
        //std::cout << snd;
        group->nodes().push_back(snd);
      }

      //recurse
      if(strcmp(xmlnode->name(),"pynodes::group")==0)
        group->children().push_back(makeGroup(xmlnode));
    }
    return group;
  };
  
  return makeGroup(doc.first_node());
}



} // namespace
