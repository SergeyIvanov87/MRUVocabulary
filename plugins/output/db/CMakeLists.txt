cmake_minimum_required (VERSION 3.1)

set (PROC_NAME "DBPlugin")
project(${PROC_NAME})

set (BRANCH_ROOT ${CMAKE_SOURCE_DIR})
get_filename_component(BRANCH_ROOT ${BRANCH_ROOT} ABSOLUTE)

set (PROCESS_DIR ${CMAKE_SOURCE_DIR})
get_filename_component(PROCESS_DIR ${PROCESS_DIR} ABSOLUTE)

set (BRANCH_NAME ${PROC_NAME})

#==============================================================================

configure_file(${BRANCH_ROOT}/cmake/ExternalMariaDBConnector.cmake.in mariadb-connector-c-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c-download )
if(result)
    message(FATAL_ERROR "CMake step for mariadb-connector-c failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c-download )
if(result)
    message(FATAL_ERROR "Build step for mariadb-connector-c failed: ${result}")
endif()

set(INSTALL_PLUGINDIR ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c-src
                 ${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c-build
                 EXCLUDE_FROM_ALL)

#Headers dir
set (INC_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/..
                     ${CMAKE_CURRENT_SOURCE_DIR}/../..
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces/
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../inter_plugins_interfaces
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/base_command_parser
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/db
                     ${TTL_ROOT})

#Source dir
set (SRC_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../interfaces_impl/
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../../common/db)

#==============================================================================
#Compile flags
set (CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

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
include_directories(${CMAKE_SOURCE_DIR})

#==============================================================================
# Setup project

add_library(${PROC_NAME} MODULE ${SRC_ALL})
target_link_libraries(${PROC_NAME} m mariadbclient libmariadb)

set_target_properties(${PROC_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET ${PROC_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c-src/include)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c-build/include)

add_dependencies(outputs ${PROC_NAME})
