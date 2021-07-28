# CMakeList.txt : projet CMake pour CookieEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

set (ECS_SRC_FILES

    "${SRC_DIR}/ECS/ComponentEditor.cpp"
    "${SRC_DIR}/ECS/ComponentHandler.cpp"
    "${SRC_DIR}/ECS/ComponentModel.cpp" 
    "${SRC_DIR}/ECS/ComponentPhysics.cpp"
    "${SRC_DIR}/ECS/ComponentScript.cpp"
    "${SRC_DIR}/ECS/Coordinator.cpp"
    "${SRC_DIR}/ECS/EntityHandler.cpp")


target_sources(CookieEngine PUBLIC ${ECS_SRC_FILES})