# CMakeList.txt : fichier projet CMake de niveau supérieur, effectuez une configuration globale
# et incluez les sous-projets ici.
#
cmake_minimum_required (VERSION 3.8)

project ("Spring")

# Incluez les sous-projets.
add_subdirectory ("Spring")

#if(MSVC)
#  target_compile_options(<target> PUBLIC "/ZI")
#  target_link_options(<target> PUBLIC "/INCREMENTAL")
#endif()