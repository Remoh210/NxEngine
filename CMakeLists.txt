cmake_minimum_required(VERSION 3.10)
project(NxEngine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Tell it to use c++ 11 for os x
if(APPLE)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
endif()


# Add header files
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
  ${CMAKE_SOURCE_DIR}/src/*.hpp)
  
# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
  ${CMAKE_SOURCE_DIR}/src/*.cpp
  ${CMAKE_SOURCE_DIR}/src/*.fs
  ${CMAKE_SOURCE_DIR}/src/*.gs
  ${CMAKE_SOURCE_DIR}/src/*.vs)


# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Load dependencies from github
add_subdirectory(external)

# once it's loaded, get glad
add_library(glad STATIC external/glfw/deps/glad_gl.c)
target_include_directories(glad PUBLIC external/glfw/deps)
set_target_properties(glad PROPERTIES FOLDER external/glad)

#GLM
add_library(glm INTERFACE)
include_directories(glm PRIVATE external/glm)

#Includes
target_include_directories(${PROJECT_NAME} PRIVATE external/soil/inc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/imgui)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/rttr)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/Include)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/bullet/src)


#Stb_image
add_library(stb_image STATIC ${CMAKE_SOURCE_DIR}/external/stb_image/stb_image.h ${CMAKE_SOURCE_DIR}/external/stb_image/stb_image.c)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/stb_image)
set_target_properties(stb_image PROPERTIES FOLDER external/stb_image)




# finally, link libs
target_link_libraries(${PROJECT_NAME} PRIVATE 
                     glfw 
                     glad 
                     assimp 
                     soil 
                     glm 
                     imgui
                     stb_image
					 rttr_core_lib
					 
					 LinearMath
					 Bullet3Common
					 BulletInverseDynamics
					 BulletCollision
					 BulletDynamics
					 BulletSoftBody
					 )
					 
					
					 

# Path to code
configure_file(Config/root_directory.h.in Config/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/Config)

# Macro to preserve source files hierarchy in the IDE
macro(GroupSources curdir)
	file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
	foreach(child ${children})
		if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			GroupSources(${curdir}/${child})
		else()
			string(REPLACE "/" "\\" groupname ${curdir})
			string(REPLACE "src" "Sources" groupname ${groupname})
			source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
		endif()
	endforeach()
endmacro()

# Run macro
GroupSources(src)
