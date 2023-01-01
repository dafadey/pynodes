#pragma once
#include <map>
#include <string>

namespace pynodes {

class graph;
class nodeLibrary;

void save_xml(const graph* g, const char* filename, const std::map<int, std::string>&);
std::map<int, std::string> append_xml(graph* g, const char* filename); // return id-s

void save_xml(const nodeLibrary* nl, const char* filename);
nodeLibrary* openNodeLibrary_xml(const char* filename);

} // namespace
