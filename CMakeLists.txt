project (LogAnalysis)
cmake_minimum_required (VERSION 2.6)

include_directories(/usr/include)
link_directories(/usr/lib)
add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Main program
set(sources 
    main.cpp 
    log_file_reader.cpp 
    log_file_processor.cpp
    log_common.cpp)
add_executable(log_analysis ${sources})

# log_file_reader test
set(log_file_reader_sources
    log_file_reader.cpp 
    log_file_reader_test.cpp)

# log_file_processor test
set(log_file_processor_sources
    log_file_processor.cpp 
    log_file_processor_test.cpp)

add_executable(log_analysis_test 
    test_main.cpp 
    log_common.cpp 
    ${log_file_processor_sources} 
    ${log_file_reader_sources})
target_link_libraries(log_analysis_test gtest)
target_link_libraries(log_analysis_test gmock)
target_link_libraries(log_analysis_test pthread)
