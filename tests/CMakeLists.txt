SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost REQUIRED unit_test_framework filesystem date_time thread signals)
FIND_PACKAGE(libspotify REQUIRED)
FIND_PACKAGE(Log4cplus REQUIRED)

IF(NOT EXISTS "${CMAKE_SOURCE_DIR}/tests/appkeys.cpp")
    CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/tests/appkeys.cpp.TEMPLATE"
                   "${CMAKE_SOURCE_DIR}/tests/appkeys.cpp"
                   COPYONLY)
ENDIF()

ADD_EXECUTABLE(SpotifyppTests "SessionTests.cpp" "appkeys.cpp" "appkeys.hpp")
TARGET_LINK_LIBRARIES(SpotifyppTests ${Boost_LIBRARIES} libspotifypp)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/src" ${LIBSPOTIFY_INCLUDE_DIR} ${LOG4CPLUS_INCLUDE_DIR})
