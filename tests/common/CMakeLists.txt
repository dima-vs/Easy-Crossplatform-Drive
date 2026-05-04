add_subdirectory(serialization)

set(COMMON_TEST_SOURCES 
    ${SERIALIZATION_JSON_TESTS} 
    PARENT_SCOPE
)