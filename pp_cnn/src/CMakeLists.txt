cmake_minimum_required(VERSION 3.15)

# SEAL version: 3.4.4
project(pp_cnn VERSION 3.4.4 LANGUAGES CXX)

# Specify C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# In Debug mode enable USE_DEBUG by default
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(USE_DEBUG ON)
endif()

# DEBUG option
if(USE_DEBUG)
  add_definitions(-D__DEBUG__)
endif()

# LOG option
if(USE_LOG)
  add_definitions(-D__LOG__)
endif()

add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)

# Executable files: /pp_cnn/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

# Try to find SEAL
find_package(SEAL 3.4.4 EXACT REQUIRED)

# For OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Create static libraries but add no source files yet
add_library(cnn STATIC)
add_library(utils STATIC)

# Add source files to libraries
add_subdirectory(cnn)
add_subdirectory(utils)

# Program: setup
add_executable(setup)
target_compile_features(setup PUBLIC cxx_std_17)
target_compile_options(setup
  PUBLIC
    -Wall                               # -D CMAKE_BUILD_TYPE=
    $<$<CONFIG:Debug>:-O0 -g>           # -D CMAKE_BUILD_TYPE=Debug
    $<$<CONFIG:Release>:-O3>            # -D CMAKE_BUILD_TYPE=Release
    $<$<CONFIG:MinSizeRel>:-Os>         # -D CMAKE_BUILD_TYPE=MinSizeRel
    $<$<CONFIG:RelWithDebInfo>:-O3 -g>  # -D CMAKE_BUILD_TYPE=RelWithDebInfo
)
target_compile_definitions(setup PUBLIC
  $<$<NOT:$<CONFIG:Debug>>:NDEBUG>      # Except -D CMAKE_BUILD_TYPE=Debug
)
target_sources(setup PRIVATE ${CMAKE_SOURCE_DIR}/setup.cpp)
target_link_libraries(setup
  SEAL::seal
  stdc++fs
  boost_program_options
)

# Program: main
add_executable(main)
target_compile_features(main PUBLIC cxx_std_17)
target_compile_options(main
  PUBLIC
    -Wall                               # -D CMAKE_BUILD_TYPE=
    $<$<CONFIG:Debug>:-O0 -g>           # -D CMAKE_BUILD_TYPE=Debug
    $<$<CONFIG:Release>:-O3>            # -D CMAKE_BUILD_TYPE=Release
    $<$<CONFIG:MinSizeRel>:-Os>         # -D CMAKE_BUILD_TYPE=MinSizeRel
    $<$<CONFIG:RelWithDebInfo>:-O3 -g>  # -D CMAKE_BUILD_TYPE=RelWithDebInfo
)
target_compile_definitions(main PUBLIC
  $<$<NOT:$<CONFIG:Debug>>:NDEBUG>      # Define NDEBUG if NOT CMAKE_BUILD_TYPE=Debug
)
target_sources(main PRIVATE ${CMAKE_SOURCE_DIR}/main.cpp)
target_link_libraries(main
  cnn
  utils
  SEAL::seal
  stdc++fs
  boost_program_options
)
