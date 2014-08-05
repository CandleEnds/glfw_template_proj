cmake_minimum_required(VERSION 2.6)

set(PROJECT_NAME x02)
project(${PROJECT_NAME})

#### GLFW ####
message($ENV{GLFW_SOURCE})
if(NOT DEFINED ENV{GLFW_SOURCE})
	message(FATAL_ERROR "Please set GLFW_SOURCE variable")
endif()
set(GLFW_DIR $ENV{GLFW_SOURCE})

add_subdirectory(${GLFW_DIR} glfw)
include_directories(${GLFW_DIR}/include)
set(Link_libs glfw ${GLFW_LIBRARIES})

if (WIN32)
	#### GLEW ####
	if (NOT DEFINED ENV{GLEW_LIBRARY})
		message(FATAL_ERROR "Please set GLEW_LIBRARY variable")
	endif()
	set(GLEW_DIR $ENV{GLEW_LIBRARY})


	add_library(glew_static STATIC IMPORTED)
	set_target_properties(glew_static PROPERTIES
		IMPORTED_LOCATION ${GLEW_DIR}/lib/Release/Win32/glew32.lib)
	list(APPEND Link_libs glew_static)
include_directories(${GLEW_DIR}/include)
elseif (APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++0x -stdlib=libc++ -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
endif()

#### GLM ####
if (NOT DEFINED ENV{GLM_DIR})
	message(FATAL_ERROR "Please set GLM_DIR variable")
endif()
set(GLM_DIR $ENV{GLM_DIR})

include_directories(${GLM_DIR})

#### Application ####

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

FILE(GLOB Source_files
	*.cpp
)

add_executable(${PROJECT_NAME} ${Source_files})

target_link_libraries(${PROJECT_NAME} ${Link_libs})

add_custom_target(postbuild ALL)

#add_custom_command(TARGET postbuild POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders)
