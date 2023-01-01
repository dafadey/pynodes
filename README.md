# pynodes
node based visual python programming tool <>

## Build
##### Dependencies:
 - glfw3 (install from your repository)
 - pyhton3 (install from your repository)
 - imgui (git clone --depth 1 https://github.com/ocornut/imgui.git)
 - imgui node editor (git clone --depth 1 https://github.com/thedmd/imgui-node-editor.git)
 - imgui text editor (git clone --depth 1 https://github.com/BalazsJako/ImGuiColorTextEdit.git)
 - rapidxml (git clone --depth 1 https://github.com/discord/rapidxml.git)
##### Linux build && run:
if previous commands were executed from inside pynodes directory (easy way) then just run
```
make -j8 && ./pynodes
```
Otherwise run something like:
```
make -j8 IMGUI_PATH=/home/dan/imgui IMGUI_NODE_EDITOR_PATH=/home/dan/imgui-node-editor IMGUI_TEXT_EDITOR_PATH=/home/dan/ImGuiColorTextEdit RAPIDXML_PATH=/home/dan/rapidxml PYTHON_INCLUDE=/usr/include/python3.10/ PYTHON_LIB=-lpython3.10
```
The latter is more preferable.
##### Windows build:
Try it with cygwin/MSYS2/mingw there should be make tool.

## Description
Pynodes provides graphical interfaces for executing python scripts attached to nodes. Input and output of python functions are represented by node pins.
Pins are connected with links. Links Rules:
 - link starts at output pin and ends at input pin
 - multiple links can be attached to output pin
 - only one link can be connected to each input pin
 - link cannot connect pins of the same node
Node also have parameters and monitors. Parameter receives input as a plaintext and thus can receive integers (1,2,3), strings('a','ab'), lists([1,2,'a']) etc, node script should correctly manage input. Monitors display data. Currently only text monitors (monitor_label) are supported. Monitor script provide proper conversion of input data for corresponding type of monitor (e.g. str(A) to show plain text created from integer, list, etc.).</br>
Each node script takes input as arguments of 'pynode' function. Naming of argunments should be the same as pins/parameters label. Output should be a dictionary with keys equal to output pins/monitors labels. Look through sampple testing graph.

## Usage
Context menu for background allows to cteate new node. Context menu for pin, parameter and monitor provide renaming of correponding entity. Node script is available from node context menu and also by clicking '=' button. '||' button duplicates the node. '*' adds node to library. Double click or press 'Del' on selected link to remove it.
