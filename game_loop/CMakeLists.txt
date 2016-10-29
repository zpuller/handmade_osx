file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

#TODO: move all the depedencies to SFML to libs
find_package(SFML 2 REQUIRED system window graphics network audio)

add_executable(game ${SOURCES} ${HEADERS})
include_directories(${SFML_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/libs)
target_link_libraries(game ${SFML_LIBRARIES})
target_link_libraries(game Box2D)

