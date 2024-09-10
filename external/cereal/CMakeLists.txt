cmake_minimum_required (VERSION 3.1)


project ("cereal" LANGUAGES CXX)


# Add header files
file(GLOB_RECURSE HEADER_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
  
# Add source files
file(GLOB_RECURSE SOURCE_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp)


add_library(${PROJECT_NAME} INTERFACE ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
