cmake_minimum_required(VERSION 3.20)

project(Simulation_PR3 VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(externals)

# ===============================

set(PR3_LINK_LIBS 	PUBLIC glm
              		PUBLIC glfw
              		PUBLIC glad
					PUBLIC imgui)

set(PR3_EXECUTABLE_NAME SimulationPr3)
file(GLOB RP3_SOURCE_FILES source/*.cpp)

add_executable(${PR3_EXECUTABLE_NAME} ${RP3_SOURCE_FILES})
target_link_libraries(${PR3_EXECUTABLE_NAME} ${PR3_LINK_LIBS})

if (MSVC)
	target_compile_options(${PR3_EXECUTABLE_NAME} PRIVATE /W4)
else()
	target_compile_options(${PR3_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
