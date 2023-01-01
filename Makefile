TARGET_EXEC := pynodes

#make -j8 IMGUI_PATH=/home/dan/imgui IMGUI_NODE_EDITOR_PATH=/home/dan/imgui-node-editor IMGUI_TEXT_EDITOR_PATH=/home/dan/ImGuiColorTextEdit RAPIDXML_PATH=/home/dan/rapidxml PYTHON_INCLUDE=/usr/include/python3.10/ PYTHON_LIB=-lpython3.10

IMGUI_PATH := imgui

IMGUI_NODE_EDITOR_PATH := imgui-node-editor

IMGUI_TEXT_EDITOR_PATH := ImGuiColorTextEdit

PYTHON_INCLUDE := /usr/include/python3.10/

PYTHON_LIB := -lpython3.10

RAPIDXML_PATH := rapidxml

SOURCES := imgui_open_save.cpp node_library.cpp monitor_label.cpp utils.cpp xmlio.cpp pylangdef.cpp pyexecutable.cpp pyobj.cpp evaluate.cpp monitor_base.cpp base_item.cpp param.cpp pin.cpp link.cpp node.cpp graph.cpp main.cpp

DEPS := #locator.h base_item.h pin.h link.h node.h graph.h

SOURCES_IMGUI := ${IMGUI_PATH}/imgui.cpp ${IMGUI_PATH}/imgui_draw.cpp ${IMGUI_PATH}/imgui_tables.cpp ${IMGUI_PATH}/imgui_widgets.cpp ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp

SOURCES_IMGUI_NODE_EDITOR := ${IMGUI_NODE_EDITOR_PATH}/crude_json.cpp ${IMGUI_NODE_EDITOR_PATH}/imgui_canvas.cpp ${IMGUI_NODE_EDITOR_PATH}/imgui_node_editor.cpp ${IMGUI_NODE_EDITOR_PATH}/imgui_node_editor_api.cpp

SOURCES_IMGUI_TEXT_EDITOR := ${IMGUI_TEXT_EDITOR_PATH}/TextEditor.cpp

OBJS := $(SOURCES:%=%.o) $(SOURCES_IMGUI:%=%.o) $(SOURCES_IMGUI_NODE_EDITOR:%=%.o) $(SOURCES_IMGUI_TEXT_EDITOR:%=%.o)

$(TARGET_EXEC): $(OBJS)
	g++ -g -O3 $(OBJS) -o $@ -lglfw -lGLEW -lGL ${PYTHON_LIB}

%.cpp.o: %.cpp $(DEPS)
	g++ --std=c++17 -g -O3 -I${IMGUI_PATH} -I${IMGUI_PATH}/backends -I${IMGUI_NODE_EDITOR_PATH} -I${IMGUI_TEXT_EDITOR_PATH} -I${PYTHON_INCLUDE} -I${RAPIDXML_PATH} -c $< -o $@

.PHONY: clean
clean:
	rm -f `ls *.o`
	rm -f $(TARGET_EXEC)
