####################################################################################################################
##
## Tier2 Libraries
##
## Les librairies Tier 2 ne dépendent que de Addons, Tier 1 et Qt pour fonctionner
##
####################################################################################################################

include(Common)

add_subdirectory(Rest)
add_subdirectory(Unit)

if(NOT EMSCRIPTEN)
    add_subdirectory(Solid)
    add_subdirectory(Sql)
endif()
