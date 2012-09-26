# - find Find the official Spotify library
# LIBSPOTIFY_INCLUDE_DIR - Where to find Find the official Spotify library header files (directory)
# LIBSPOTIFY_LIBRARY - Location of the libspotify library
# LIBSPOTIFY_FOUND - Set to TRUE if we found everything (library, includes and executable)

IF(LIBSPOTIFY_INCLUDE_DIR AND LIBSPOTIFY_LIBRARY)
    SET(LIBSPOTIFY_FIND_QUIETLY TRUE)
ENDIF()

FIND_PATH(LIBSPOTIFY_INCLUDE_DIR libspotify/api.h)
FIND_LIBRARY(LIBSPOTIFY_LIBRARY NAMES spotify)

IF(LIBSPOTIFY_INCLUDE_DIR AND LIBSPOTIFY_LIBRARY)
    SET(LIBSPOTIFY_FOUND TRUE)
ENDIF()

IF(NOT LIBSPOTIFY_FOUND)
    IF(libspotify_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find libspotify")
    ELSE()
        MESSAGE(STATUS "Optional package libspotify was not found")
    ENDIF()
ENDIF()