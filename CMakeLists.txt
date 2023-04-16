cmake_minimum_required (VERSION 3.1)

set (PROC_NAME "MRUDictionary")
set (PROC_LIB_NAME "mru_core")

project(${PROC_NAME})

set (BRANCH_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(BRANCH_ROOT ${BRANCH_ROOT} ABSOLUTE)

set (PROCESS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(PROCESS_DIR ${PROCESS_DIR} ABSOLUTE)

set (BRANCH_NAME ${PROC_NAME})

set(SYSTEM_NAME OFF CACHE BOOL "Configure for SYSTEM_NAME")
#==============================================================================
add_subdirectory(plugins)


#Common Directories
set (TTL_ROOT "/home/user/work/TTL")
set (TTL_INC ${TTL_ROOT})

set (DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
                 ${CMAKE_CURRENT_SOURCE_DIR}/src
                 ${CMAKE_CURRENT_SOURCE_DIR}/src/doc
                 ${CMAKE_CURRENT_SOURCE_DIR}/src/resources
                 ${CMAKE_CURRENT_SOURCE_DIR}/src/pluginWrapper
                 ${CMAKE_CURRENT_SOURCE_DIR}/src/command_parser)

#Headers dir
set (INC_DIRECTORIES ${DIRECTORIES}
                     ${CMAKE_CURRENT_SOURCE_DIR}/plugins
                     ${CMAKE_CURRENT_SOURCE_DIR}/common/base_command_parser
                     ${TTL_INC})

#Source dir
set (SRC_DIRECTORIES ${DIRECTORIES}
                     ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/impl
                     ${CMAKE_CURRENT_SOURCE_DIR}/plugins/interfaces_impl
                     ${CMAKE_CURRENT_SOURCE_DIR}/common/base_command_parser
                     )

#==============================================================================
#Compile flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -rdynamic -O0 -g3 -MD -Wall -Wno-invalid-offsetof -std=c++1z")

#DEBUG option
list (APPEND DEFINES_ALL -DDEBUG)

#==============================================================================
# Libraries
find_library (LIB_M m)
find_library (LIB_DL dl)
find_library (LIB_PTHREAD pthread)
list (APPEND LIBS ${LIB_M}
                  ${LIB_DL}
                  ${LIB_PTHREAD})
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
add_library(mru_objects OBJECT ${SRC_ALL})
add_dependencies(mru_objects plugins)

add_executable(${PROC_NAME} $<TARGET_OBJECTS:mru_objects>)
set_target_properties(${PROC_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROC_NAME} ${LIBS})

add_library(${PROC_LIB_NAME} STATIC $<TARGET_OBJECTS:mru_objects>)
set_target_properties(${PROC_LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROC_LIB_NAME} ${LIBS})


#execute_process(COMMAND mkdir ${CMAKE_CURRENT_BINARY_DIR}/ASSETS)
#execute_process(COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/ASSETS/ResourceA)
#execute_process(COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/ASSETS/ResourceB)
#execute_process(COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/ASSETS/ResourceC)
