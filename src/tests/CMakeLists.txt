include(CTest)

#Dummy library:
add_library(RLL_dummy_lib SHARED dummy_library/dumb_lib.cpp)
set_target_properties(RLL_dummy_lib PROPERTIES PREFIX "" SUFFIX ".library" OUTPUT_NAME "dummy_library")

############################################################
#Test file sources:
set(test_sources
    src/flags_test.cpp
    src/shared_library_test.cpp
)
set(test_names
    RLL.tests.flags
    RLL.tests.shared_library
)

list(LENGTH test_sources num_test_sources)
math(EXPR lists_len "${num_test_sources} - 1")

foreach(index RANGE ${lists_len})
    #
    list(GET test_sources ${index} test_source)
    list(GET test_names ${index} test_name)
    #
    add_executable(${test_name} ${test_source})
    add_test(NAME ${test_name} COMMAND ${test_name})
    #
    target_include_directories(${test_name}
        PRIVATE include/ 
        PRIVATE ${PROJECT_SOURCE_DIR}/include/
    )
    if(NOT WIN32)
        target_link_libraries(${test_name} PRIVATE dl)
    endif()
endforeach(index RANGE ${lists_len})

