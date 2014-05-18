#-DBOOST_ROOT=/home/kuba/External/boost_1_55_0 -DBoostCompute_DIR=/home/kuba/External/compute/cmake -DCMAKE_BUILD_TYPE=Release

project(ComputeCL)
cmake_minimum_required(VERSION 2.8)

ADD_DEFINITIONS(
    -std=c++11 # support for new cpp standard
)

# directory with external Find*.cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_modules)

add_definitions(-D__CL_ENABLE_EXCEPTIONS)
add_definitions(-DCL_USE_DEPRECATED_OPENCL_1_1_APIS)


# find OpenCL
find_package(OpenCL REQUIRED)
include_directories(SYSTEM ${OPENCL_INCLUDE_DIRS})

# find Boost
#set(Boost_DEBUG 1)
find_package(Boost 1.55 REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")



set(BoostCompute_INCLUDE_DIRS "/home/kuba/External/compute/include")
include_directories(SYSTEM ${BoostCompute_INCLUDE_DIRS})


include_directories(include)

set(SRC_LIST
    main.cpp

)

set(INC_LIST
    include/primitves/Gather.h
    include/utils/MatrixUtils.h
    include/utils/StorageType.h
    include/utils/cmd_options/CommandLineOptions.h
    include/io/MatrixMarket.h
    include/io/Print.h
    include/datatypes/ClMatrixBase.h
    include/datatypes/ClMatrixCOO.h
    include/iterator/SortIterator.h
)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${INC_LIST})
target_link_libraries("${PROJECT_NAME}" ${OPENCL_LIBRARIES} ${BOOST_LIBRARIES} -lboost_program_options)
