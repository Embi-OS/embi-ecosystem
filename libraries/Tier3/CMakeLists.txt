####################################################################################################################
##
## Tier3 Libraries
##
## Les librairies Tier 3 peuvent dépendre de Addons, Tier 1, Tier 2, Tier 3 et Qt pour fonctionner
##
####################################################################################################################

include(Common)

add_subdirectory(Axion)
add_subdirectory(Files)
add_subdirectory(Postman)

if(NOT EMSCRIPTEN)
    add_subdirectory(Database)
    add_subdirectory(Happy)
    add_subdirectory(Media)
    add_subdirectory(Network)
    add_subdirectory(System)
    add_subdirectory(Swupdate)
endif()
