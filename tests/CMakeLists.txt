add_executable(basic_test basic_test.cpp)

add_executable(persistence_test persistence_test.cpp)

add_executable(simple_test test.cpp)

target_link_libraries(basic_test PUBLIC templatedb BF gtest)
target_link_libraries(persistence_test PUBLIC templatedb BF gtest)
target_link_libraries(simple_test PUBLIC templatedb BF gtest)

add_test(NAME basic_test COMMAND ${CMAKE_BINARY_DIR}/tests/basic_test)
add_test(NAME persistence_test COMMAND ${CMAKE_BINARY_DIR}/tests/persistence_test)
