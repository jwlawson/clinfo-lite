cmake_minimum_required(VERSION 3.8.2)
project(clinfo-lite LANGUAGES C)

find_package(OpenCL REQUIRED)
add_executable(clinfo-lite clinfo-lite.c)
target_link_libraries(clinfo-lite OpenCL::OpenCL)
target_compile_features(clinfo-lite PRIVATE c_std_99)
