add_library(
    cpp-quant 
    STATIC 
        src/errors.cpp)
target_link_libraries(cpp-quant  PUBLIC cpp-datetime)
target_link_libraries(cpp-quant  PUBLIC cpp-math)
target_include_directories(cpp-quant  PUBLIC include)
