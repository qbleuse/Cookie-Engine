# CMakeList.txt : projet CMake pour CookieEngine, incluez la source et définissez
# la logique spécifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

set (GAMEPLAY_SRC_FILES

    "${SRC_DIR}/Gameplay/ArmyCoordinator.cpp"
    "${SRC_DIR}/Gameplay/ArmyHandler.cpp"
    "${SRC_DIR}/Gameplay/CGPAttack.cpp"
    "${SRC_DIR}/Gameplay/CGPLive.cpp"
    "${SRC_DIR}/Gameplay/CGPMove.cpp"
    "${SRC_DIR}/Gameplay/CGPProducer.cpp"
    "${SRC_DIR}/Gameplay/CGPWorker.cpp")


target_sources(CookieEngine PUBLIC ${GAMEPLAY_SRC_FILES})