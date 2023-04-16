cmake_minimum_required (VERSION 3.1)

set (PROC_NAME "FB2DecoderPlugin")
project(${PROC_NAME})

set (BRANCH_ROOT ${CMAKE_SOURCE_DIR})
get_filename_component(BRANCH_ROOT ${BRANCH_ROOT} ABSOLUTE)

set (PROCESS_DIR ${CMAKE_SOURCE_DIR})
get_filename_component(PROCESS_DIR ${PROCESS_DIR} ABSOLUTE)

set (BRANCH_NAME ${PROC_NAME})

#==============================================================================

#Common Directories
set (DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})


set (CMAKE_CXX_STANDARD 20)                               # yes, it uses c++20
find_package(TXml REQUIRED)                               # use CMake package

#Headers dir
set (INC_DIRECTORIES ${DIRECTORIES}
                     ${DIRECTORIES}/..
                     ${DIRECTORIES}/../..
                     ${DIRECTORIES}/../../interfaces/
                     ${DIRECTORIES}/../../../common/base_command_parser
                     ${DIRECTORIES}/../../decoders/
    )

#Source dir
set (SRC_DIRECTORIES ${DIRECTORIES}
                     ${DIRECTORIES}/../../interfaces_impl/)

#==============================================================================
#Compile flags
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -O0 -g3 -MD -Wall -Wno-invalid-offsetof ")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a -fPIC -O0 -g3 -MD -Wall -Wno-invalid-offsetof ")

#DEBUG option
list (APPEND DEFINES_ALL -DDEBUG)

#==============================================================================
# Libraries
find_library (LIB_M m)
list (APPEND LIBS ${LIB_M} ${LIB_MYSQL})
#==============================================================================
list(REMOVE_DUPLICATES INC_DIRECTORIES)
list(REMOVE_DUPLICATES SRC_DIRECTORIES)
list(REMOVE_DUPLICATES DEFINES_ALL)


add_definitions(${DEFINES_ALL})

foreach (_dir ${SRC_DIRECTORIES})
    file(GLOB _src ${_dir}/*.cpp)
    list(APPEND SRC_ALL ${_src})

    file(GLOB _src ${_dir}/*.c)
    list(APPEND SRC_ALL ${_src})
endforeach()

include_directories(${INC_DIRECTORIES})
include_directories(${3PP_INCLUDES_DIR})
include_directories(${CMAKE_SOURCE_DIR})

#==============================================================================
# Setup project
#message()
add_library(${PROC_NAME} MODULE ${SRC_ALL})
set_target_properties(${PROC_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROC_NAME} PUBLIC ${LIBS} TXml)
add_dependencies(decoders ${PROC_NAME})
