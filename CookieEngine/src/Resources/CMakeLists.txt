# CMakeList.txt�: projet CMake pour CookieEngine, incluez la source et d�finissez
# la logique sp�cifique au projet ici.
#
cmake_minimum_required (VERSION 3.8)

set (RESOURCES_SRC_FILES
    "${SRC_DIR}/Resources/Loader.cpp" 
    "${SRC_DIR}/Resources/Map.cpp" 
    "${SRC_DIR}/Resources/ResourcesManager.cpp"
    "${SRC_DIR}/Resources/Mesh.cpp"
    "${SRC_DIR}/Resources/Texture.cpp"
    "${SRC_DIR}/Resources/Scene.cpp"
    "${SRC_DIR}/Resources/Serialization.cpp" 
    #"${SRC_DIR}/Resources/SoundManager.cpp"
    
    "${SRC_DIR}/Resources/Particles/ParticlesGenerator.cpp"
    "${SRC_DIR}/Resources/Particles/ParticlesUpdate.cpp"
    "${SRC_DIR}/Resources/Particles/ParticlesHandler.cpp"
    "${SRC_DIR}/Resources/Particles/ParticlesSystem.cpp")


target_sources(CookieEngine PUBLIC ${RESOURCES_SRC_FILES})