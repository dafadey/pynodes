#pragma once
#include <filesystem>

namespace ImGui::openSaveDialog {
  //Dialogues are modal so there is no need in class/struct we just use static global vars here
  static std::filesystem::path path;
  static std::filesystem::path path_backup;
  static int selected;
  static std::filesystem::path selected_file;

  void activateOpenFileDialog();
  bool openFileDialog();
  void activateSaveFileDialog();
  bool saveFileDialog();
  std::string file();
}
