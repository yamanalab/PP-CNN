file(GLOB sources *.cpp)

set(name server)
add_executable(${name} ${sources})

target_link_libraries(${name} ppcnn_server ${COMMON_LIBS})
