cmake_minimum_required(VERSION 3.10)

project(PenInkRenderer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Paths
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(GLEW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/glew/include")
set(GLEW_LIB "${CMAKE_SOURCE_DIR}/include/glew/lib/Release/x64/glew32.lib")

set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/glfw/include")
set(GLFW_LIB "${CMAKE_SOURCE_DIR}/include/glfw/lib-vc2022/glfw3.lib")

file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# Find OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/mesh.cpp
    src/shader.cpp
    src/camera.cpp
    include/glad/src/glad.c

    # ImGui core
    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp

    # ImGui backends
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
)

# Includes
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLEW_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glad/include
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include/imgui/backends
)

link_directories(${CMAKE_SOURCE_DIR}/external/glfw/lib-vc2022)

# Link
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    ${GLEW_LIB}
    ${GLFW_LIB}
)