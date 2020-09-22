cmake_minimum_required(VERSION 3.15)

ENABLE_LANGUAGE(C)

# Try to find HDF5
find_package(HDF5 REQUIRED)

target_sources(cnn
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/load_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network_builder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/layer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/activation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/average_pooling2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/batch_normalization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/conv2d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/conv2d_fused_bn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dense.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dense_fused_bn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flatten.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/global_average_pooling2d.cpp
)
target_include_directories(cnn
  PUBLIC ${CMAKE_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR}/../include
  PRIVATE ${HDF5_INCLUDE_DIRS}
)
target_link_libraries(utils
  ${HDF5_LIBRARIES}
  hdf5
  hdf5_cpp
)
