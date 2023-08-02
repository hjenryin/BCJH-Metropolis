cmake_minimum_required(VERSION 3.0)

# set(CMAKE_CXX_COMPILER "/usr/bin/g++-11")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
# set(CMAKE_BUILD_TYPE "Debug")

set(PJNAME bcjh)
project(${PJNAME})

message("-- CMAKE_C_COMPILER_VERSION: ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "operation system is ${CMAKE_SYSTEM_NAME}")
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:gbk>")
    message(STATUS "Now is windows")
elseif(CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    # Doesn't work on Windows: I don't know how to specify the charset.
    # If I use utf8, running on windows would be cause error C2001 etc;
    # If I use utf8 with BOM, there would be extra # before some file and invoke error.
    # add_compile_options("-pthread")
    # add_compile_options("-sNO_DISABLE_EXCEPTION_CATCHING")
    message(STATUS "Now is Emscripten")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    add_definitions(-DEMSCRIPTEN)
else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/build/)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src)


file(GLOB src_list_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp 
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp
                      )
if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    list(REMOVE_ITEM src_list_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/bcjh_js_cpp.cpp)
endif()

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/include)	
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/utils)	
message(STATUS "target name is ${PJNAME}")
message(STATUS "src_list_cpp is ${src_list_cpp}")

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    # set multithread make
    set(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j$(nproc)")
    
    foreach(i 2 1 3)
        set(target_name ${PJNAME}_${i})
        add_executable(${target_name} ${src_list_cpp})
        target_compile_definitions(${target_name} PRIVATE -DNUM_GUESTS=${i})
        set_target_properties(${target_name} PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data/data.min.json@./data.min.json -lembind -sALLOW_MEMORY_GROWTH -sEXPORTED_RUNTIME_METHODS=run -sNO_DISABLE_EXCEPTION_CATCHING")
    endforeach()
else()
    add_executable(${PJNAME} ${src_list_cpp})
endif()

# target_link_libraries(bcjh jsoncpp)
# target_link_libraries(bcjh jsonhelper)
# -sNO_DISABLE_EXCEPTION_CATCHING

