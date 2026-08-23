idf_component_register(
    SRCS
        "app_main.cpp"
        "scoopy_buttons.cpp"
        "scoopy_matter.cpp"
    INCLUDE_DIRS "."
)

target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
