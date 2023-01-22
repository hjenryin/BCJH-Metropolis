cmake_minimum_required(VERSION 3.14)

set(VCPKG_ROOT ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT})

project(bcjh)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/)



file(GLOB src_list_cpp ${CMAKE_SOURCE_DIR}/src/*.cpp 
                       ${CMAKE_SOURCE_DIR}/src/*/*.cpp
                       ${CMAKE_SOURCE_DIR}/src/*.c
                       ${CMAKE_SOURCE_DIR}/*.cpp)
include_directories(${CMAKE_SOURCE_DIR}/vcpkg/installed/x64-linux/include/ ${CMAKE_SOURCE_DIR}/src/)

set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/vcpkg/installed/x64-linux)
find_package(jsoncpp CONFIG REQUIRED)


add_executable(bcjh ${src_list_cpp})

set_target_properties(bcjh PROPERTIES 
	COMPILE_FLAGS "${WARNCXXFLAGS}"
	CXX_STANDARD_REQUIRED ON)


target_link_directories(bcjh PRIVATE ${CMAKE_SOURCE_DIR}/lib/ )

set(lib_DIR /usr/lib/x84_64-linux-gnu)

link_directories(${lib_DIR})

target_link_libraries(bcjh libjsoncpp.a)

