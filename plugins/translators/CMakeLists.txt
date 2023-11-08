add_custom_target(translators COMMENT "Translators custom target")

add_subdirectory(multi_xdxf)
add_subdirectory(xdxf)

add_dependencies(plugins translators)

set(PLUGIN_TARGET_LIST ${PLUGIN_TARGET_LIST} PARENT_SCOPE)
