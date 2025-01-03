cmake_minimum_required(VERSION 3.20)

project(Simulation_PR3 VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

option(PR3_EMBEDDED_SHADERS "All shaders will be embedded into program binary" ON)
option(PR3_SHOW_DEBUG_CONSOLE "If On should open only window without a console" OFF)

add_subdirectory(externals)
add_subdirectory(utils)

# ===============================

set(PR3_LINK_LIBS 	PUBLIC glm
              		PUBLIC glfw
              		PUBLIC glad
					PUBLIC imgui
					PUBLIC assimp
					PUBLIC stb)

set(PR3_EXECUTABLE_NAME SimulationPr3)
file(GLOB RP3_SOURCE_FILES source/*.cpp)

add_executable(${PR3_EXECUTABLE_NAME} ${RP3_SOURCE_FILES})
target_link_libraries(${PR3_EXECUTABLE_NAME} ${PR3_LINK_LIBS})

if (PR3_EMBEDDED_SHADERS)
	message("-- RP3: PR3_EMBEDDED_SHADERS option is ON")

	target_compile_definitions(${PR3_EXECUTABLE_NAME} PRIVATE "EMBEDDED_SHADERS")
    shaders_to_embedded_hpp(${PR3_EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/shaders)
else()
	message("-- RP3: PR3_EMBEDDED_SHADERS option is OFF")
endif()

if (MSVC)
	target_compile_options(${PR3_EXECUTABLE_NAME} PRIVATE /W4)

	if (NOT PR3_SHOW_DEBUG_CONSOLE)

		message("-- RP3: PR3_SHOW_DEBUG_CONSOLE option is OFF")
		set_target_properties(${PR3_EXECUTABLE_NAME} PROPERTIES 
			LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
	else() 
		message("-- RP3: PR3_SHOW_DEBUG_CONSOLE option is ON")
	endif()

else()
	target_compile_options(${PR3_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()