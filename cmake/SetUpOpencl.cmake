# Copyright 2019-2020 John Lawson
# Available for use under the BSD 3-Clause Clear License.

if(OpenCL_LIBRARY OR NOT FORCE_DOWNLOAD_OPENCL)
  # Find system OpenCL, or previously set up external OpenCL project
  find_package(OpenCL QUIET)
endif()

if(NOT OpenCL_FOUND AND DOWNLOAD_MISSING_DEPS)
  include(ExternalProject)
  ExternalProject_Add(opencl_headers
    GIT_REPOSITORY    https://github.com/KhronosGroup/OpenCL-Headers
    GIT_TAG           master
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
  )
  ExternalProject_Get_Property(opencl_headers SOURCE_DIR)
  file(MAKE_DIRECTORY ${SOURCE_DIR})
  set(OpenCL_INCLUDE_DIR ${SOURCE_DIR}
    CACHE PATH "OpenCL header directory" FORCE
  )
  message(STATUS "inlcude_dir: ${OpenCL_INCLUDE_DIR}")

  ExternalProject_Add(opencl_icd_loader
    GIT_REPOSITORY  https://github.com/KhronosGroup/OpenCL-ICD-Loader
    GIT_TAG         master
    DEPENDS         opencl_headers
    CMAKE_ARGS      -DOPENCL_ICD_LOADER_HEADERS_DIR=${OpenCL_INCLUDE_DIR}
                    -DBUILD_TESTING=OFF
    INSTALL_COMMAND ""
  )
  ExternalProject_Get_Property(opencl_icd_loader BINARY_DIR)
  set(OpenCL_LIBRARY ${BINARY_DIR}/libOpenCL.so
    CACHE PATH "OpenCL library location" FORCE
  )
  message(STATUS "library: ${OpenCL_LIBRARY}")

  find_package(OpenCL REQUIRED)
  add_dependencies(OpenCL::OpenCL opencl_headers opencl_icd_loader)
  mark_as_advanced(OpenCL_FOUND OpenCL_LIBRARY OpenCL_INCLUDE_DIR)
endif()

if(NOT OpenCL_FOUND)
  message(FATAL_ERROR "Could not find OpenCL library")
endif()
