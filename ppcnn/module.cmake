option(BUILD_SHARED_LIBS "build as a shared library" ON)

file(GLOB sources *.cpp)

if (BUILD_SHARED_LIBS)
  add_library(${module_name} SHARED ${sources})
else()
  add_library(${module_name} STATIC ${sources})
endif()
