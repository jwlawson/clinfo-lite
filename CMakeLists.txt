# Copyright 2019-2020 John Lawson
# Available for use under the BSD 3-Clause Clear License.

cmake_minimum_required(VERSION 3.8.2)
project(clinfo-lite LANGUAGES C)

option(DOWNLOAD_MISSING_DEPS
  "Whether to download any missing external dependencies"
  ON
)
option(FORCE_DOWNLOAD_OPENCL
  "Download OpenCL dependencies instead of searching the system"
  OFF
)

include(cmake/SetUpOpencl.cmake)

add_executable(clinfo-lite clinfo-lite.c)
target_link_libraries(clinfo-lite OpenCL::OpenCL)
target_compile_features(clinfo-lite PRIVATE c_std_99)
target_compile_definitions(clinfo-lite PRIVATE CL_TARGET_OPENCL_VERSION=120)
