/*
 * Copyright 2019-2020 John Lawson
 *
 * clinfo-lite is a small utility tool to output OpenCL platform and device
 * information.
 *
 * Available for use under the BSD 3-Clause Clear License.
 */
/* SPDX-License-Identifier: BSD-3-Clause-Clear */

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

#define OPENCL_ERROR -1
#define ALLOCATION_ERROR -2

#define CL_CHECK_ERR(err)                                                \
  if (CL_SUCCESS != err) {                                               \
    printf("OpenCL error code: %d at %s:%d\n", err, __FILE__, __LINE__); \
    exit(OPENCL_ERROR);                                                  \
  }

#define CL_CALL(func, ...)              \
  do {                                  \
    cl_int error = (func)(__VA_ARGS__); \
    CL_CHECK_ERR(error);                \
  } while (0)

static void* checked_malloc(size_t num_bytes) {
  void* allocated = malloc(num_bytes);
  if (!allocated) {
    printf("Failed to allocate memory\n");
    exit(ALLOCATION_ERROR);
  }
  return allocated;
}

static cl_platform_info const all_platform_infos[] = {
    CL_PLATFORM_NAME,    CL_PLATFORM_VENDOR,     CL_PLATFORM_PROFILE,
    CL_PLATFORM_VERSION, CL_PLATFORM_EXTENSIONS,
};

static char const* get_platform_info_name(cl_platform_info info) {
  switch (info) {
    /* clang-format off */
    case CL_PLATFORM_PROFILE   : return "Platform profile";
    case CL_PLATFORM_VERSION   : return "Platform version";
    case CL_PLATFORM_NAME      : return "Platform name";
    case CL_PLATFORM_VENDOR    : return "Platform vendor";
    case CL_PLATFORM_EXTENSIONS: return "Platform extensions";
      /* clang-format on */
  }
  return "Unknown platform info";
}

static cl_device_info const string_device_infos[] = {
    CL_DEVICE_NAME,    CL_DEVICE_VENDOR,  CL_DEVICE_PROFILE,
    CL_DEVICE_VERSION, CL_DRIVER_VERSION, CL_DEVICE_EXTENSIONS,
};

static char const* get_device_info_name(cl_device_info info) {
  switch (info) {
    /* clang-format off */
    case CL_DEVICE_TYPE      : return "Device type";
    case CL_DEVICE_NAME      : return "Device name";
    case CL_DEVICE_VENDOR    : return "Device vendor";
    case CL_DRIVER_VERSION   : return "Driver version";
    case CL_DEVICE_PROFILE   : return "Device profile";
    case CL_DEVICE_VERSION   : return "Device version";
    case CL_DEVICE_EXTENSIONS: return "Device extensions";
      /* clang-format on */
  }
  return "Unknown device info";
}

static char const* get_device_type_name(cl_device_type type) {
  switch (type) {
    /* clang-format off */
    case CL_DEVICE_TYPE_CPU        : return "CPU";
    case CL_DEVICE_TYPE_GPU        : return "GPU";
    case CL_DEVICE_TYPE_ACCELERATOR: return "Accelerator";
    case CL_DEVICE_TYPE_DEFAULT    : return "Default";
      /* clang-format on */
  }
  return "Unknown device type";
}

static void print_platform_info(cl_platform_id platform,
                                cl_platform_info info_name) {
  size_t info_size;
  CL_CALL(clGetPlatformInfo,
          /*platform=*/platform,
          /*param_name=*/info_name,
          /*param_value_size=*/0,
          /*param_value=*/NULL,
          /*param_value_size_ret=*/&info_size);
  char* info = checked_malloc(info_size);
  CL_CALL(clGetPlatformInfo,
          /*platform=*/platform,
          /*param_name=*/info_name,
          /*param_value_size=*/info_size,
          /*param_value=*/info,
          /*param_value_size_ret=*/NULL);
  printf("  %20s: %s\n", get_platform_info_name(info_name), info);
  free(info);
}

static void print_all_platform_info(cl_platform_id platform) {
  unsigned num_platform_infos =
      sizeof(all_platform_infos) / sizeof(cl_platform_info);
  for (unsigned i = 0; i < num_platform_infos; ++i) {
    print_platform_info(platform, all_platform_infos[i]);
  }
}

static void print_string_device_info(cl_device_id device,
                                     cl_device_info info_name) {
  size_t info_size;
  CL_CALL(clGetDeviceInfo,
          /*device=*/device,
          /*param_name=*/info_name,
          /*param_value_size=*/0,
          /*param_value=*/NULL,
          /*param_value_size_ret=*/&info_size);
  char* info = checked_malloc(info_size);
  CL_CALL(clGetDeviceInfo,
          /*device=*/device,
          /*param_name=*/info_name,
          /*param_value_size=*/info_size,
          /*param_value=*/info,
          /*param_value_size_ret=*/NULL);
  printf("  %20s: %s\n", get_device_info_name(info_name), info);
  free(info);
}

static void print_device_type(cl_device_id device) {
  cl_uint error;
  cl_device_type type;
  CL_CALL(clGetDeviceInfo,
          /*device=*/device,
          /*param_name=*/CL_DEVICE_TYPE,
          /*param_value_size=*/sizeof(type),
          /*param_value=*/&type,
          /*param_value_size_ret=*/NULL);
  printf("  %20s: %s\n", get_device_info_name(CL_DEVICE_TYPE),
         get_device_type_name(type));
}

static void print_all_device_info(cl_device_id device) {
  print_device_type(device);

  unsigned num_string_device_infos =
      sizeof(string_device_infos) / sizeof(cl_device_info);
  for (unsigned i = 0; i < num_string_device_infos; ++i) {
    print_string_device_info(device, string_device_infos[i]);
  }
}

int main() {
  cl_uint num_platforms;
  cl_int error = clGetPlatformIDs(
      /*num_entries=*/0,
      /*platforms=*/NULL,
      /*num_platforms=*/&num_platforms);
  if (error != CL_SUCCESS || num_platforms == 0) {
    /* The Khronos ICD loader will return -1001 if no platforms are found,
     * rather than just setting num_platforms to 0. */
    puts("No OpenCL platforms found");
    return 0;
  }

  cl_platform_id* platforms =
      checked_malloc(num_platforms * sizeof(cl_platform_id));
  CL_CALL(clGetPlatformIDs,
          /*num_entries=*/num_platforms,
          /*platforms=*/platforms,
          /*num_platforms=*/NULL);

  for (unsigned i = 0; i < num_platforms; i++) {
    printf("Platform [%d]:\n", i);
    print_all_platform_info(platforms[i]);

    cl_uint num_devices;
    CL_CALL(clGetDeviceIDs,
            /*platform=*/platforms[i],
            /*device_type=*/CL_DEVICE_TYPE_ALL,
            /*num_entries=*/0,
            /*devices=*/NULL,
            /*num_devices=*/&num_devices);
    cl_device_id* devices = checked_malloc(num_devices * sizeof(cl_device_id));
    CL_CALL(clGetDeviceIDs,
            /*platform=*/platforms[i],
            /*device_type=*/CL_DEVICE_TYPE_ALL,
            /*num_entries=*/num_devices,
            /*devices=*/devices,
            /*num_devices=*/NULL);

    for (unsigned j = 0; j < num_devices; j++) {
      printf("\nDevice [%d]:\n", j);
      print_all_device_info(devices[j]);
    }
    free(devices);
  }
  free(platforms);
  return 0;
}
