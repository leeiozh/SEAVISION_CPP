set(TEST_FILE connect.cpp)
get_filename_component(TEST_FILE_NAME ${TEST_FILE} NAME_WE)
set(EXECUTABLE_NAME run_${TEST_FILE_NAME})
add_executable(${EXECUTABLE_NAME} ${TEST_FILE})
target_link_libraries(${EXECUTABLE_NAME} SeaVision)
message("Cmake success prepare : ${TEST_FILE} =)")

