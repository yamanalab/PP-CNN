cmake_minimum_required(VERSION 3.15)

set(project_name PP-CNN)
project(${project_name})

#find_package(Threads REQUIRED)
#set(CMAKE_CXX_FLAGS "-O3 -std=c++17 -openmp -pthread -Wall -DNDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -std=c++17 -openmp -pthread -Wall")
#set(SEAL_USE_CXX17 OFF)
#
#find_package(SEAL 3.2.0 EXACT REQUIRED)
#find_package(OpenMP REQUIRED)
#if(OpenMP_FOUND)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)

find_package(SEAL 3.4.4 EXACT REQUIRED)
find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
find_package(HDF5 REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/ppcnn
  ${PROJECT_SOURCE_DIR}/stdsc
  /usr/local/include/SEAL-3.4
  PRIVATE ${HDF5_INCLUDE_DIRS})

set(COMMON_LIBS
  stdsc
  ppcnn_share
  SEAL::seal
  ${HDF5_LIBRARIES}
  hdf5
  hdf5_cpp)

add_subdirectory(stdsc)
add_subdirectory(ppcnn)
add_subdirectory(demo)
