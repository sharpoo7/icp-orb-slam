cmake_minimum_required(VERSION 2.8)
project(icp-orb-slam)

# gtest
include("${PROJECT_SOURCE_DIR}/cmake/functions.cmake")
#google_initialize_cartographer_project()
#google_enable_testing()

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)  # Required by CMake 2.7+
        if(POLICY CMP0043)
                cmake_policy(SET CMP0043 OLD) #  Ignore COMPILE_DEFINITIONS_<Config> properties.
        endif()
endif(COMMAND cmake_policy)

# Check C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#extra lib
set(EXTERNAL_LIBS "")

#--------------------
# dependency:  boost
#--------------------
find_package(Boost COMPONENTS thread filesystem system program_options date_time REQUIRED)
if (Boost_MINOR_VERSION GREATER 47)
        find_package(Boost COMPONENTS thread filesystem system program_options date_time chrono REQUIRED)
endif (Boost_MINOR_VERSION GREATER 47)
include_directories(${Boost_INCLUDE_DIRS})

#--------------------
# dependency: opencv 3
#--------------------
find_package(OpenCV 3.0 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 2.4.3 QUIET)
   if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
   else()
      message("OpenCV 2.4.x found")
   endif()
else()
    message("OpenCV 3.x found")
endif()


#--------------------
# dependency: eigen 3
#--------------------
find_path(EIGEN_INCLUDE_DIR Eigen/Core
        /usr/local/include/eigen3
        /usr/include/eigen3
)
include_directories(${EIGEN_INCLUDE_DIR})

#--------------------
# dependency: libpointmatcher
#--------------------
find_package(libpointmatcher REQUIRED)
include_directories(${libpointmatcher_INCLUDE_DIRS})
message(STATUS "Using libpointmatcher version ${libpointmatcher_VERSION}")

#--------------------
# dependency: mrpt
#--------------------
find_package(MRPT REQUIRED slam;vision;gui;topography;obs)
include_directories(${MRPT_INCLUDE_DIRS})
message(STATUS "Using mrpt version ${MRPT_VERSION}")

####
INCLUDE_DIRECTORIES("./include")
INCLUDE_DIRECTORIES("./src")
INCLUDE_DIRECTORIES("./tests")

set(ALL_SRCS "")
set(ALL_TESTS "")
set(ALL_EXECUTABLES "")

file(GLOB_RECURSE ALL_SRCS "*.cc" "*.h")
file(GLOB_RECURSE ALL_TESTS "*_test.cc")
file(GLOB_RECURSE ALL_EXECUTABLES "*_main.cc")
list(REMOVE_ITEM ALL_SRCS ${ALL_TESTS})
list(REMOVE_ITEM ALL_SRCS ${ALL_EXECUTABLES})

# generate shared library
add_library(${PROJECT_NAME} SHARED
    ${ALL_SRCS})
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    ${libpointmatcher_LIBRARIES}
    ${EIGEN3_LIBS}
    )

# examples icp
add_executable(example_icp "./examples/test_icp_main.cc")
target_link_libraries(example_icp ${PROJECT_NAME})

# examples opencv
add_executable(example_opencv "./examples/example_opencv_main.cc")
target_link_libraries(example_opencv ${PROJECT_NAME})

# examples mrpt
add_executable(example_mrpt "./examples/example_mrpt_main.cc")
target_link_libraries(example_mrpt ${PROJECT_NAME} ${MRPT_LIBS})

# tests
#add_subdirectory("tests")
