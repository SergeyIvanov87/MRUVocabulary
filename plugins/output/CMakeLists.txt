add_custom_target(outputs COMMENT "Outputs custom target")

add_subdirectory(db)
add_subdirectory(xdxf)

add_dependencies(plugins outputs)
