#include <iostream>
#include <fstream>
#include <filesystem>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "node.h"
#include "graph.h"
#include "param.h"
#include "pin.h"
#include "link.h"

namespace pynodes {

node::node(graph* g) : base_item(g->getNewUniqueId()), m_graph(g) {
  g->addNode(this);
}

node::~node() {
  m_graph->removeNode(this);
  for(int i=m_pins.size()-1; i>=0; i--)
    delete m_pins[i];
  for(auto mon : m_monitors)
    delete mon;
}

const graph* node::getGraph() const {
  return m_graph;
}

graph* node::getGraph() {
  return m_graph;
}

void node::addPin(pin* p) {
  m_pins.push_back(p);
}

void node::addParam(param& p) {
  m_params.push_back(p);
}

void node::addMonitor(monitor_base* mon) {
  m_monitors.push_back(mon);
}

void node::removePin(pin* p) {
  auto it=m_pins.begin();
  for(; it<m_pins.end(); it++) {
    if(*it==p)
      break;
  }
  if(it!=m_pins.end())
    m_pins.erase(it);
}

void node::removeParam(param& p) {
  auto it=m_params.begin();
  for(; it<m_params.end(); it++) {
    if(std::string(it->getLabel()) == std::string(p.getLabel()))
      break;
  }
  if(it!=m_params.end())
    m_params.erase(it);
}

void node::removeMonitor(monitor_base* mon) {
  auto it=m_monitors.begin();
  for(; it<m_monitors.end(); it++) {
    if(*it==mon)
      break;
  }
  if(it!=m_monitors.end())
    m_monitors.erase(it);
}

pin* node::makePin(const char* label, pin::ePinType pt) {
  auto p = new pin(this);
  p->setLabel(label);
  p->setType(pt);
  return p;
}

const std::vector<pin*>& node::pins() const {
  return m_pins;
}

std::vector<param>& node::params() {
  return m_params;
}

const std::vector<param>& node::params() const {
  return m_params;
}

const std::vector<monitor_base*>& node::monitors() const {
  return m_monitors;
}

static std::vector<std::string> get_arguments(const char* code, std::string function) {
  std::string start_sequence="def "+function;
  std::vector<std::string> result;
  for(int i=0;code[i] != char(0); i++) {
    if(code[i]=='d') {
      unsigned int j=0;
      for(;code[i+1] != char(0);i++) {
        if(code[i+1]==' ' && code[i]==' ') {
          i++;
          continue;
        }
        if(code[i]!=start_sequence[j])
          break;
        j++;
      }
      if(j==start_sequence.length()) {
        //serach for '('
        bool found=true;
        for(;i<code[i]!=0;i++) {
          if(code[i]==' ')
            continue;
          if(code[i]!='(')
            found=false;
          break;
        }
        i++;
        std::string item="";
        if(found) { //search for ')' and collect vector
          for(;code[i] != 0; i++) {
            if(code[i]==' ')
              continue;
            if(code[i]==',') {
              result.push_back(item);
              item.erase();
              continue;
            }
            if(code[i]==')') {
              result.push_back(item);
              break;
            }
            item+=code[i];
          }
        }
      }
    }
  }
  return result;  
}

pin* node::getPin(const char* label) {
  std::string lbl(label);
  for(auto& p : m_pins) {
    if(std::string(p->getLabel()) == lbl)
      return p;
  }
  return nullptr;
}

bool node::run() {
  //std::cout << "run for node '" << getLabel() << "'\n";

  auto args = get_arguments(getScript(), "pynode");
  
  //set input pins
  for(auto p : m_pins) {
    if(p->getType() == pin::ePinType::IN) {
      if(p->links().size() == 1) {
        auto lnk = p->links()[0];
        p->setPyObject(lnk->begin()->getPyObject());
      } else if(p->links().size() > 1) {
        std::cerr << "input pin '" << p->getLabel() << "' has more than one link connected\n";
        return false;
      } else {
        p->setPyObject(NULL);
        std::cout << "warning: input pin '" << p->getLabel() << "' has no links connected\n";
      }
      
    }
  }
  
  bool run_status=false;
  
  std::map<std::string, pin*> input_pin_map;
  std::vector<pin*> output_pins;
  std::map<std::string, param*> param_map;

  for(auto p : m_pins) {
    if(p->getType() == pin::ePinType::IN)
      input_pin_map[p->getLabel()] = p;
    
    if(p->getType() == pin::ePinType::OUT)
      output_pins.push_back(p);
  }
  
  for(auto& par : m_params)
    param_map[par.getLabel()] = &par;

  std::string modname = "node_"+std::string(getLabel())+"_"+std::to_string(id());

  auto pModule = safeImportModule(modname.c_str());
  
  if (pModule != NULL) {
    PyObject* pFunc = PyObject_GetAttrString(pModule, "pynode");
    if (pFunc && PyCallable_Check(pFunc)) {

      PyObject* pArgs = PyTuple_New(args.size());
      bool input_is_good = true;
      for(int i=0;i<args.size();i++) {
        std::string& arg = args[i];
        std::cout << "doing arg " << arg << '\n';
        auto itpin = input_pin_map.find(arg);
        if(itpin != input_pin_map.end()) {
          PyObject* obj = const_cast<PyObject*>(itpin->second->getPyObject()); // pin's PyObject is 'borrowed' by Tuple but tuple have to be DECREF-ed and it DECREFs all the items. so it looks like borrows and waste them, like fucking barfly... watda??? further reading: https://comp.lang.python.narkive.com/fk3CvnEW/reference-counting-and-pytuple-setitem
          if(obj==NULL) {
            input_is_good = false;
            std::cerr << "ERROR: script argument '" << arg << "' was set to NULL from pin '" << itpin->second->getLabel() << "'\n";
            continue;
          }
          Py_INCREF(obj);
          PyTuple_SetItem(pArgs, i, obj);
          continue;
        }
        auto itpar = param_map.find(arg);
        if(itpar != param_map.end()) {
          PyObject* obj = itpar->second->getPyObject();
          if(obj==NULL) {
            input_is_good = false;
            std::cerr << "ERROR: script argument '" << arg << "' was set to NULL from param '" << itpar->second->getLabel() << "'\n";
            continue;
          }
          Py_INCREF(obj);
          PyTuple_SetItem(pArgs, i, obj);
          continue;
        }
        std::cerr << "ERROR in preparing python run for node '" << getLabel() << "' : input argument '" << arg << "' was not found nor in input pins nor in parameters. Check pins/arameters namings and your node script\n";
        input_is_good = false;
      }
      
      if(input_is_good) {
        std::cout << "input is good\n";
        PyObject* res = PyObject_CallObject(pFunc, pArgs);

        Py_DECREF(pArgs);
        //ADD PYTHON DICT HERE!!! FOR NOW SETTING RESULT TO FIRST OUTPUT NODE! NO MULTIPLE OUTPUT FOR NOW!
       
        if(res != NULL) {
        //ok res is a dictionary let us parse it and assign values to corresponding output pins
          if(PyDict_Check(res)) {
            for(auto p : output_pins) {
              PyObject* item = PyDict_GetItemString(res, p->getLabel());
              if(item != NULL) {
                p->setPyObject(item);
                Py_INCREF(item);
              }
            }
            for(auto mon : m_monitors) {
              PyObject* item = PyDict_GetItemString(res, mon->getLabel());
              if(item != NULL) {
                mon->setPyObject(item);
                Py_INCREF(item);
              }
            }
            run_status = true;
          } else {
            std::cerr << "output for node '" << getLabel() << "' is not PyDict please provide output in the form of dictionary:\n\treturn {'outpin_a' : 13, 'outpin_b' : 'hey!', 'output_c' : my_custom_object}\nplease note that you can skip some of outputs. if they are not connected that is totally fine but make sure you set some output for every connected output pin otherwise it will receive NULL and all depending nodes will not be able to run with that inappropriate input\n";
          }
        } else
          std::cerr << "somethig went wrong with script for node " << getLabel() << ", got NULL output, first check what you are feeding to input pins and then node script\n";
        Py_XDECREF(res);
      }

      Py_XDECREF(pFunc);
    } else {
      if (PyErr_Occurred())
        PyErr_Print();
      std::cerr << "cannot find function 'pynode' in script of node " << getLabel() << '\n';
    }
    Py_DECREF(pModule);
  }
  else {
    if (PyErr_Occurred())
      PyErr_Print();
    std::cerr << "cannot load script for node " << getLabel() << '\n';
  }
  return run_status;
}

bool node::isValid() const {
  for(auto p : m_pins) {
    if(p->getType() == pin::ePinType::OUT)
      continue;
    if(p->links().size() && p->links()[0]->begin()->getPyObject() == NULL)
      return false;
  }
  for(const param& par : m_params) {
    if(par.getPyObject() == NULL)
      return false;
  }
  return true;
}

void node::invalidateOutput() {
  for(auto p : m_pins) {
    if(p->getType() == pin::ePinType::IN)
      continue;
    p->invalidatePyObject();
  }
}

}//namespace
