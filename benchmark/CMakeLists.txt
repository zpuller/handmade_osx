file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_executable(bench ${SOURCES} ${HEADERS})
include_directories(${PROJECT_SOURCE_DIR}/libs)
target_link_libraries(bench Box2D)

