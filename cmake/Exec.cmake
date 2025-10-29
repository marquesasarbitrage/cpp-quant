add_executable(quant-tools-black ${CMAKE_CURRENT_SOURCE_DIR}/tests/tools/black.cpp)
target_link_libraries(quant-tools-black PUBLIC cpp-quant)

add_executable(quant-tools-nss ${CMAKE_CURRENT_SOURCE_DIR}/tests/tools/nss/nss.cpp)
target_link_libraries(quant-tools-nss PUBLIC cpp-quant)

add_executable(quant-tools-scheduler ${CMAKE_CURRENT_SOURCE_DIR}/tests/tools/scheduler.cpp)
target_link_libraries(quant-tools-scheduler PUBLIC cpp-quant)

add_executable(quant-valuation-termstructures-discountcurve ${CMAKE_CURRENT_SOURCE_DIR}/tests/valuation/marketdata/termstructures/discountcurve/discountcurve.cpp)
target_link_libraries(quant-valuation-termstructures-discountcurve PUBLIC cpp-quant)

# Scheduler tool tests
#add_executable(quant-test2 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test2.cpp)
#target_link_libraries(quant-test2 PUBLIC cpp-quant)

# Nelson-Siegel-Svensson tests
#add_executable(quant-test3 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test3/test3.cpp)
#target_link_libraries(quant-test3 PUBLIC cpp-quant)

# Discount curve tests
#add_executable(quant-test4 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test4/test4.cpp)
#target_link_libraries(quant-test4 PUBLIC cpp-quant)

# Compound rate tests
#add_executable(quant-test5 ${CMAKE_CURRENT_SOURCE_DIR}/tests/test5.cpp)
#target_link_libraries(quant-test5 PUBLIC cpp-quant)

#add_executable(quant-scheduler ${CMAKE_CURRENT_SOURCE_DIR}/tests/scheduler.cpp)
#target_link_libraries(quant-scheduler PUBLIC cpp-quant)

