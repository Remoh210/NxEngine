cmake_minimum_required(VERSION 3.10)
#imgui
# Add source files
file(GLOB_RECURSE IMGUI_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/external/imgui/*.cpp)
	
# Add header files
file(GLOB_RECURSE IMGUI_HEADER_FILES 
	${CMAKE_SOURCE_DIR}/external/imgui/*.h
    ${CMAKE_SOURCE_DIR}/external/imgui/*.hpp)

add_library(imgui STATIC ${IMGUI_HEADER_FILES} ${IMGUI_SOURCE_FILES})
target_link_libraries(imgui PRIVATE glfw)
target_link_libraries(imgui PRIVATE glad)