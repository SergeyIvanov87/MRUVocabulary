cmake_minimum_required (VERSION 3.1)

SET(PLUGIN_TARGET_LIST  "${PLUGIN_TARGET_LIST}" CACHE INTERNAL "PLUGIN_TARGET_LIST")

set (PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)

# Download `templatedXML` from github or use external sources
if (NOT TEMPLATE_XML_PATH)
    message ("Use GITHUB version of TXML")
    configure_file(${BRANCH_ROOT}/cmake/ExternalTXML.cmake.in templatexml-download/CMakeLists.txt)
else()
    message ("Use STANDALONE version of TXML in TEMPLATE_XML_PATH: ${TEMPLATE_XML_PATH}")
    configure_file(${BRANCH_ROOT}/cmake/ExternalStandaloneTXML.cmake.in templatexml-download/CMakeLists.txt)
endif()

# Configure & Build (optionally) `templatedXML`
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output
                ERROR_VARIABLE output
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/templatexml-download )
if(result)
    message(FATAL_ERROR "CMake step for templatexml failed: ${result}. Log: ${output}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
                RESULT_VARIABLE result
                OUTPUT_VARIABLE output
                ERROR_VARIABLE output
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/templatexml-download )
if(result)
    message(FATAL_ERROR "Build step for templatexml failed: ${result}. Log: ${output}")
endif()
message("Build Log: ${output}")
# Postprocessing of `templatedXML` configuration
if (NOT TEMPLATE_XML_PATH)
    set(TEMPLATE_XML_PATH ${CMAKE_CURRENT_BINARY_DIR})
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/src/templatexml
                     ${CMAKE_CURRENT_BINARY_DIR}/src/templatexml-build
                     EXCLUDE_FROM_ALL)
    #find_package(TXml REQUIRED)
else()
#    set(TEMPLATE_XML_PATH ${CMAKE_CURRENT_BINARY_DIR})
#    find_package(TXml REQUIRED)
set(TEMPLATE_XML_PATH ${CMAKE_CURRENT_BINARY_DIR})
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/src/templatexml
                     ${CMAKE_CURRENT_BINARY_DIR}/src/templatexml-build
                     EXCLUDE_FROM_ALL)
endif()


include(${CMAKE_CURRENT_BINARY_DIR}/src/templatexml/cmake/FindLibXML2.cmake)
#include(${TEMPLATE_XML_PATH}/lib/txml/cmake/FindLibXML2.cmake)
target_link_libraries(TXml INTERFACE ${XML_TARGET})
add_dependencies(TXml ${XML_TARGET})

add_custom_target(plugins COMMENT "Plugins custom target")
add_subdirectory(decoders)
add_subdirectory(output)
add_subdirectory(translators)


foreach (p ${PLUGIN_TARGET_LIST})
    install(TARGETS ${p} DESTINATION  ${CMAKE_INSTALL_PREFIX}/plugins)
endforeach()
