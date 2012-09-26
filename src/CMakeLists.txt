SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost REQUIRED thread system date_time chrono)
FIND_PACKAGE(libspotify REQUIRED)

FILE(GLOB sources "spotify/*.cpp" "spotify/Debug/*.cpp" "spotify/Core/*.cpp")
FILE(GLOB headers "spotify/*.hpp" "spotify/Debug/*.hpp" "spotify/Core/*.hpp")

SOURCE_GROUP("Source Files" FILES ${sources})
SOURCE_GROUP("Header Files" FILES ${headers})

ADD_LIBRARY(libspotifypp SHARED ${sources} ${headers})
TARGET_LINK_LIBRARIES(libspotifypp ${LIBSPOTIFY_LIBRARY} ${Boost_LIBRARIES})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS} ${LIBSPOTIFY_INCLUDE_DIR})
