cmake_minimum_required(VERSION 3.20)
project(LinearScannerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "D:/DEV/MSYS2/ucrt64")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(OpenGL REQUIRED)

# Kumpulkan semua file C++ milik kita, ImGui, dan ImPlot
set(SOURCES 
    src/main.cpp
    src/AppUI.cpp
    src/SerialManager.cpp
    src/app.rc
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
    libs/implot/implot.cpp
    libs/implot/implot_items.cpp
)

add_executable(ScannerApp ${SOURCES}
    src/app.rc)

# Beri tahu compiler di mana letak file header (.h) nya
target_include_directories(ScannerApp PRIVATE 
    include
    libs/imgui
    libs/imgui/backends
    libs/implot
    ${GLFW_INCLUDE_DIRS}
)

target_link_directories(ScannerApp PRIVATE ${GLFW_LIBRARY_DIRS})
target_link_libraries(ScannerApp PRIVATE ${GLFW_LIBRARIES} OpenGL::GL)