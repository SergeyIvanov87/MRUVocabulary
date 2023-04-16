add_custom_target(decoders COMMENT "Decoders custom target")

add_subdirectory(txt)
add_subdirectory(fb2)

add_dependencies(plugins decoders)
