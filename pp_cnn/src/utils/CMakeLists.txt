cmake_minimum_required(VERSION 3.15)

target_sources(utils
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/globals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/helper_functions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/load_dataset.cpp
)
target_include_directories(utils
  PUBLIC ${CMAKE_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR}/../include
)
