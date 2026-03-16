add_library(test_utils STATIC utils.c)
target_include_directories(test_utils PUBLIC .)
target_link_libraries(test_utils PUBLIC gauge)
