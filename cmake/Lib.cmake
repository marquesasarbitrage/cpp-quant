add_library(
    cpp-quant 
    STATIC 
        src/errors.cpp
        src/tools/nss.cpp
        src/tools/black.cpp
        src/tools/scheduler.cpp
        src/valuation/marketdata/marketdata.cpp
        src/valuation/marketdata/termstructure/discountcurve.cpp)
target_link_libraries(cpp-quant  PUBLIC cpp-datetime)
target_link_libraries(cpp-quant  PUBLIC cpp-math)
target_include_directories(cpp-quant  PUBLIC include)
