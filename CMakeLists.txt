cmake_minimum_required(VERSION 3.0)
project(bcjh)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_compile_options("-fexec-charset=gbk")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:gbk>")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
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
# target_link_libraries(bcjh jsoncpp)
# target_link_libraries(bcjh jsonhelper)

