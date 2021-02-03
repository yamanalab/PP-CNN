file(GLOB sources *.cpp)

set(name client)
add_executable(${name} ${sources})

target_link_libraries(${name} ppcnn_client ${COMMON_LIBS})
