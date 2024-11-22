set(IMGUI_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/imgui
                ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends)
file(GLOB IMGUI_SOURCES  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp 
                         ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
                         ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_PATHS})
target_link_libraries(imgui PUBLIC glfw)