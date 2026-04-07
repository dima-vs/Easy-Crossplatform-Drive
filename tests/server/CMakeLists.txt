add_subdirectory(service_layer)
add_subdirectory(data_access_layer)

set(SERVER_TEST_SOURCES 
    ${SERVICE_LAYER_TESTS} 
    ${DATA_ACCESS_LAYER_TESTS} 
    PARENT_SCOPE
)