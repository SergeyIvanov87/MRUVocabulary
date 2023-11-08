add_custom_target(decoders COMMENT "Decoders custom target")

add_subdirectory(txt)
add_subdirectory(fb2)

add_dependencies(plugins decoders)

set(PLUGIN_TARGET_LIST ${PLUGIN_TARGET_LIST} PARENT_SCOPE)
