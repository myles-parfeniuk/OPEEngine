idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES cmock OPEEngine)

# supress unused var warnings from tests
idf_build_set_property(COMPILE_OPTIONS "-Wno-unused-but-set-variable" APPEND)