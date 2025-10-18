# Black tool tests
add_executable(quant-test1 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test1.cpp)
target_link_libraries(quant-test1 PUBLIC cpp-quant)

# Scheduler tool tests
add_executable(quant-test2 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test2.cpp)
target_link_libraries(quant-test2 PUBLIC cpp-quant)

# Nelson-Siegel-Svensson tests
add_executable(quant-test3 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test3/test3.cpp)
target_link_libraries(quant-test3 PUBLIC cpp-quant)

# Discount curve tests
add_executable(quant-test4 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test4/test4.cpp)
target_link_libraries(quant-test4 PUBLIC cpp-quant)