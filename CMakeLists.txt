cmake_minimum_required(VERSION 3.12)
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

#soil
target_include_directories(${PROJECT_NAME} PRIVATE external/soil/inc)

#imgui
# Add source files
file(GLOB_RECURSE IMGUI_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/external/imgui/*.cpp)
	
# Add header files
file(GLOB_RECURSE IMGUI_HEADER_FILES 
	${CMAKE_SOURCE_DIR}/external/imgui/*.h
    ${CMAKE_SOURCE_DIR}/external/imgui/*.hpp)
    
add_library(imgui STATIC ${IMGUI_HEADER_FILES} ${IMGUI_SOURCE_FILES})
set_target_properties(imgui PROPERTIES FOLDER external/imgui)
target_link_libraries(imgui PRIVATE glfw)
target_link_libraries(imgui PRIVATE glad)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/imgui)

# finally, link libs
target_link_libraries(${PROJECT_NAME} PRIVATE 
                     glfw 
                     glad 
                     assimp 
                     soil 
                     glm 
                     imgui)
