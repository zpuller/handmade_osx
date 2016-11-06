file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(handmade STATIC ${SOURCES} ${HEADERS})
