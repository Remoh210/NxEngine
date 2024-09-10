#find_program(git_executable NAMES git git.exe git.cmd)
#if(NOT git_executable)
#   message(FATAL_ERROR "Failed to find git.")
#endif()

#set(MYROOTDIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

#function(UpdateExternalLib name url rev)
#    set(need_checkout FALSE)
#    set(external_folder "${MYROOTDIR}/external")
#    set(external_lib_folder "${external_folder}/${name}")
#    if(EXISTS ${external_lib_folder})
#        message(STATUS "Updating ${name} to revision ${rev}...")
#        execute_process(COMMAND "${git_executable}" "fetch" "origin" WORKING_DIRECTORY "${external_lib_folder}")
#        execute_process(COMMAND "${git_executable}" "rev-parse" "HEAD" WORKING_DIRECTORY "${external_lib_folder}" OUTPUT_VARIABLE head_rev)
#        string(STRIP ${head_rev} head_rev)
#        if (${head_rev} STREQUAL ${rev})
#            set(need_checkout FALSE)
#        else()
#            set(need_checkout TRUE)
#        endif()
#    else()
#        message(STATUS "Cloning ${name} revision...")
#        execute_process(COMMAND "${git_executable}" "clone" ${url} "-n" WORKING_DIRECTORY "${external_folder}")
#        set(need_checkout TRUE)
#    endif()
#    if(need_checkout)
#        message(STATUS "Checking out to revision ${rev}...")
#        execute_process(COMMAND "${git_executable}" "checkout" "-q" ${rev} WORKING_DIRECTORY "${external_lib_folder}")
#    endif()
#endfunction()

#UpdateExternalLib("glfw" "https://github.com/glfw/glfw.git" "d7ae90a790416f150a39e8f42f8278598bafae29")
#UpdateExternalLib("assimp" "https://github.com/assimp/assimp" "8f0c6b04b2257a520aaab38421b2e090204b69df")

# Disable useless GLFW options.
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)


#Assimp Settings
option(ASSIMP_BUILD_TESTS "If the test suite for Assimp is built in addition to the library." OFF)
option(INJECT_DEBUG_POSTFIX "Inject debug postfix in .a/.so/.dll lib names" OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS "If the supplementary tools for Assimp are built in addition to the library." OFF)

add_subdirectory(glfw)
add_subdirectory(assimp)
add_subdirectory(soil)
add_subdirectory(imgui)
add_subdirectory(rttr)
add_subdirectory(bullet)
add_subdirectory(cereal)

#Grouping
set_target_properties(soil PROPERTIES FOLDER external/soil)
set_target_properties(glfw PROPERTIES FOLDER external/glfw)
set_target_properties(imgui PROPERTIES FOLDER external/imgui)
set_target_properties(rttr_core_lib PROPERTIES FOLDER external/rttr)
set_target_properties(cereal PROPERTIES FOLDER external/cereal)
set_target_properties(assimp PROPERTIES FOLDER external/assimp)
set_target_properties(zlibstatic PROPERTIES FOLDER external/assimp)
set_target_properties(zlib PROPERTIES FOLDER external/assimp)
set_target_properties(uninstall PROPERTIES FOLDER external/assimp)
set_target_properties(IrrXML PROPERTIES FOLDER external/assimp)

#Bullet
set_target_properties(LinearMath PROPERTIES FOLDER external/bullet)
set_target_properties(BulletDynamics PROPERTIES FOLDER external/bullet)
set_target_properties(Bullet3Common PROPERTIES FOLDER external/bullet)
set_target_properties(BulletInverseDynamics PROPERTIES FOLDER external/bullet)
set_target_properties(BulletSoftBody PROPERTIES FOLDER external/bullet)
set_target_properties(BulletCollision PROPERTIES FOLDER external/bullet)
set_target_properties(ConvexDecomposition PROPERTIES FOLDER external/bullet)


