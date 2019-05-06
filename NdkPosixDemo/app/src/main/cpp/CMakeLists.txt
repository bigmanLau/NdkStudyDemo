cmake_minimum_required(VERSION 3.4.1)

add_library(native-library SHARED  native-library.cpp)
find_library(log-lib log)
target_link_libraries(native-library ${log-lib})