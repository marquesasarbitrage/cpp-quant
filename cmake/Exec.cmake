# Black tools tests
add_executable(quant-test1 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test1.cpp)
target_link_libraries(quant-test1 PUBLIC cpp-quant)