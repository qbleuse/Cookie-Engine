# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.8)

project ("CookieEngine")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCE_DIR "CookieEngine")
set(GAME_ONLY CACHE BOOL "ON/OFF")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Incluez les sous-projets.
add_subdirectory (${SOURCE_DIR})
