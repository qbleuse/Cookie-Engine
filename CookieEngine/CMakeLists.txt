# CMakeList.txt : projet CMake pour CookieEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(RESOURCE_DIR "Assets/")
set(UI_SETUP "${CMAKE_SOURCE_DIR}/imgui.ini")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(DEPS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/src")
set(DEPS_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/include")


include_directories("${INC_DIR}/")
include_directories("${INC_DIR}/Core/")
include_directories("${INC_DIR}/Core/Math/")
include_directories("${INC_DIR}/UI/")
include_directories("${INC_DIR}/UI/Widgets/")
include_directories("${INC_DIR}/UI/Widgets/EditorWs/")
include_directories("${INC_DIR}/UI/Widgets/GameWs/")
include_directories("${INC_DIR}/Render/")
include_directories("${INC_DIR}/ECS/")
include_directories("${INC_DIR}/Resources/")
include_directories("${INC_DIR}/Resources/Particles")
include_directories("${INC_DIR}/Gameplay/")
include_directories("${DEPS_INC_DIR}/ImGui/")
include_directories("${DEPS_INC_DIR}/ImGuizmo/")
include_directories("${DEPS_INC_DIR}/")
include_directories("${DEPS_INC_DIR}/assimp/")
include_directories("${DEPS_INC_DIR}/assimp/Compiler/")
include_directories("${DEPS_INC_DIR}/DirectXTex/")
include_directories("${DEPS_INC_DIR}/reactphysics3d/")
include_directories("${DEPS_INC_DIR}/Lua/")
include_directories("${DEPS_INC_DIR}/Sol/")
#include_directories("${DEPS_INC_DIR}/FMOD/")

# Record SUB_SYS configuration (WIN32 for release Windows to remove console)
IF(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Generate CMake cache for Release")
    IF(WIN32)
        set(SUB_SYS WIN32)
    ENDIF(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
ELSE()
    message(STATUS "Generate CMake cache for Debug")
    add_definitions(-DDEBUG)
ENDIF(CMAKE_BUILD_TYPE MATCHES Release)

# List source files
set (SRC_FILES

    "${SRC_DIR}/Core/Window.cpp"
    "${SRC_DIR}/Physics/PhysicsSimulator.cpp"
    "${SRC_DIR}/Game.cpp")

#choose the right main
if(GAME_ONLY)

set (SRC_FILES ${SRC_FILES}
    "${SRC_DIR}/gameMain.cpp")

else()

set (SRC_FILES ${SRC_FILES}
    "${SRC_DIR}/Editor.cpp" 
    "${SRC_DIR}/editorMain.cpp")
endif(GAME_ONLY)

set(DLL "${CMAKE_CURRENT_SOURCE_DIR}/libs/dll/assimp-vc142-mt.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/dll/lua54.dll")
#        "${CMAKE_CURRENT_SOURCE_DIR}/libs/dll/fmod.dll"
#        "${CMAKE_CURRENT_SOURCE_DIR}/libs/dll/fmodL.dll"
#        "${CMAKE_CURRENT_SOURCE_DIR}/libs/dll/fmodstudio.dll"
#        "${CMAKE_CURRENT_SOURCE_DIR}/libs/dll/fmodstudioL.dll")

set(LIB_DIR "${CookieEngine_SOURCE_DIR}/CookieEngine/libs")

IF(FAST)
    add_compile_options(-O2)
ENDIF(FAST)

# Ajoutez une source à l'exécutable de ce projet.
add_executable (CookieEngine ${SUB_SYS} ${SRC_FILES})

#list all the source files
add_subdirectory(${DEPS_SRC_DIR})
add_subdirectory("${SRC_DIR}/ECS")
add_subdirectory("${SRC_DIR}/Gameplay")
add_subdirectory("${SRC_DIR}/UI")
add_subdirectory("${SRC_DIR}/Render")
add_subdirectory("${SRC_DIR}/Resources")

set_target_properties(CookieEngine PROPERTIES CXX_STANDARD 17)

target_include_directories(CookieEngine INTERFACE CXX_STANDARD 17)

set_property(TARGET CookieEngine PROPERTY CXX_STANDARD 17)
set_property(TARGET CookieEngine PROPERTY CXX_STANDARD_REQUIRED ON)

target_compile_features(CookieEngine PUBLIC cxx_std_17)
target_compile_features(CookieEngine PRIVATE cxx_std_17)

target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/d3d11.lib")
target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/d3dcompiler.lib")
target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/glfw3.lib")
target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/assimp-vc142-mt.lib")
target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/zlibstatic.lib")
target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/liblua54.a")

IF(CMAKE_BUILD_TYPE MATCHES Release)
    target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/reactphysics3d.lib")
#    target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/fmod_vc.lib")
#    target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/fmodstudio_vc.lib")
ELSE()
    target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/debug_reactphysics3d.lib")
#    target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/fmodL_vc.lib")
#    target_link_libraries(CookieEngine PRIVATE "${LIB_DIR}/fmodstudioL_vc.lib")
ENDIF(CMAKE_BUILD_TYPE MATCHES Release)


# TODO: Ajoutez des tests et installez des cibles si nécessaire.

#copies the Assets file
add_custom_command(TARGET CookieEngine POST_BUILD COMMAND 
    ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_SOURCE_DIR}/${RESOURCE_DIR} 
        ${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_DIR})

#copies the imgui.ini
add_custom_command(TARGET CookieEngine POST_BUILD COMMAND 
    ${CMAKE_COMMAND} -E copy
        ${UI_SETUP} $<TARGET_FILE_DIR:CookieEngine>)

#copies the dlls
add_custom_command(TARGET CookieEngine POST_BUILD COMMAND 
    ${CMAKE_COMMAND} -E copy
        ${DLL} $<TARGET_FILE_DIR:CookieEngine>)
