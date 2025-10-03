####################################################################################################################
##
## Samples
##
## Différents projets d'exemples
##
####################################################################################################################

include(Common)

add_subdirectory(01_Gallery)

if(NOT EMSCRIPTEN)
    add_subdirectory(00_Launcher)
    add_subdirectory(02_Clock)
endif()
