#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "imgui.h"

#include "imgui_open_save.h"

static std::vector<std::string> split(const std::string& input, const std::string& delimiter) {
  size_t pos = 0;
  std::vector<std::string> out;
  while (true) {
    size_t end = input.find(delimiter, pos);
    out.push_back(input.substr(pos, end - pos));
    pos = end + delimiter.size();
    if (end == std::string::npos)
      break;
  }
  return out;
}

namespace ImGui::openSaveDialog {

std::string file() {
  return selected_file.string();
}

const char* openFileDialogName="open file##imgui open file modal";
const char* saveFileDialogName="save file##imgui save file modal";

void activateOpenFileDialog() {
  OpenPopup(openFileDialogName);
  if(path.empty()) {
    path = std::filesystem::current_path();
    selected=-1;
    selected_file = std::filesystem::path();
  }
  path_backup = path;
}

void activateSaveFileDialog() {
  OpenPopup(saveFileDialogName);
  if(path.empty()) {
    path = std::filesystem::current_path();
    selected=-1;
    selected_file = std::filesystem::path();
  }
  path_backup = path;
}

enum eDialogType{OPEN, SAVE};

static bool common(eDialogType dt) {
  bool state = false;
  bool modalState = dt == eDialogType::OPEN ? BeginPopupModal(openFileDialogName, NULL, ImGuiWindowFlags_None) : BeginPopupModal(saveFileDialogName, NULL, ImGuiWindowFlags_None);
  
  if (modalState) {
    int items_in_dir=0;

    #ifdef WIN32
    const std::string path_delimiter = "\\";
    #else
    const std::string path_delimiter = "/";
    #endif
    auto dirs = split(path.string(), path_delimiter);
    std::stringstream newpath{};
    #ifndef WIN32
    newpath << '/';
    #endif
    for(int dir_id=0; dir_id<dirs.size(); dir_id++) {
      auto& d = dirs[dir_id];
      if (d.empty())
        continue;
      newpath << d << path_delimiter;
      if(Button(d.c_str())) {
        selected = -1;
        break;
      }
      SameLine();
    }
    //std::cout << "new path is " << newpath.str() << '\n';
    newpath >> path;
    Text("");

    std::vector<std::filesystem::path> pathes;
    for (auto const& dir_entry : std::filesystem::directory_iterator(path)) {
      if(dir_entry.path().extension() == std::filesystem::path(".xml") || std::filesystem::is_directory(dir_entry.path()))
        pathes.push_back(dir_entry.path());
    }
    std::stable_sort(pathes.begin(), pathes.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)->bool {
      if(std::filesystem::is_directory(a) == std::filesystem::is_directory(b))
        return a.string() > b.string();
      return std::filesystem::is_directory(a) && !std::filesystem::is_directory(b);
    });

    std::vector<std::string> strings;
    for(const auto& p : pathes)
      strings.push_back(p.filename().string());
    std::vector<const char*> cstrings;
    for (const auto& s : strings)
      cstrings.push_back(s.c_str());
    
    if (ListBox(" ", &selected, cstrings.data(), cstrings.size(), 13)) {
      if (selected != -1) {
        if(std::filesystem::is_directory(pathes[selected])) {
          newpath.clear();
          auto dirs = split(pathes[selected].string().c_str(), path_delimiter);
          for (auto d : dirs) {
            if (d.empty())
              continue;
            newpath << d << path_delimiter;
          }
          newpath >> path;
          selected = -1;
        } else
          selected_file = pathes[selected];
      } 
    }
    if(dt == eDialogType::SAVE) {
      char buf[256];
      strcpy(buf,selected_file.string().c_str());
      if(ImGui::InputText("filename", buf, 256)) {
        std::stringstream ssf{std::string(buf)};
        selected_file.clear();
        ssf >> selected_file;
      }
    }
    if (Button("cancel")) {
      CloseCurrentPopup();
      path = path_backup;
    }
    SameLine();
    if(selected != -1) {
      bool buttonRes = dt == eDialogType::OPEN ? Button("open") : Button("save");
      if(buttonRes) {
        state = true;
        CloseCurrentPopup();
      }
    }
    EndPopup();
  }
  return state;
}

bool openFileDialog() {
  return common(eDialogType::OPEN);
}

bool saveFileDialog() {
  return common(eDialogType::SAVE);
}



} //namespace
