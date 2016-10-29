file(GLOB SOURCES "Common/*.cpp" "Collision/*.cpp" "Collision/*/*.cpp" "Dynamics/*.cpp" "Dynamics/*/*.cpp")
file(GLOB HEADERS "Common/*.h" "Collision/*.h" "Collision/*/*.h" "Dynamics/*.h" "Dynamics/*/*.h")

add_library(Box2D STATIC ${SOURCES} ${HEADERS})
