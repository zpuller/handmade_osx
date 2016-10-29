file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(handmade SHARED ${SOURCES} ${HEADERS})
