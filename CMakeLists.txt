cmake_minimum_required(VERSION 3.0)
project(bcjh)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
# set(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")

message(STATUS "operation system is ${CMAKE_SYSTEM_NAME}")
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:gbk>")
    message(STATUS "Now is windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    # Doesn't work on Windows: I don't know how to specify the charset.
    # If I use utf8, running on windows would be cause error C2001 etc;
    # If I use utf8 with BOM, there would be extra # before some file and invoke error.
    add_compile_options("-fexceptions")
    add_link_options("-fexceptions")
    message(STATUS "Now is Emscripten")
    # add_compile_options("--preload-file ../data")
    add_link_options("--preload-file ../data/")
    add_definitions(-DEMSCRIPTEN)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/)

aux_source_directory(${CMAKE_SOURCE_DIR}/src src)


file(GLOB src_list_cpp ${CMAKE_SOURCE_DIR}/src/*.cpp 
                       ${CMAKE_SOURCE_DIR}/src/*/*.cpp
                      )

# add_subdirectory(${CMAKE_SOURCE_DIR}/src/include)	
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/utils)	

add_executable(bcjh ${src_list_cpp})
if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set_target_properties(bcjh PROPERTIES LINK_FLAGS "--O3 --preload-file ${CMAKE_SOURCE_DIR}/data/data.min.json@./data.min.json -sASYNCIFY ")
endif()

# target_link_libraries(bcjh jsoncpp)
# target_link_libraries(bcjh jsonhelper)

