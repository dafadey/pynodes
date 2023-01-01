#include "monitor_base.h"
#include "graph.h"

namespace pynodes {

monitor_base::monitor_base(graph* g) : base_item(g->getNewUniqueId()) {}

monitor_base::~monitor_base() {}

} //namespace
