// Copyright 2018 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if __cplusplus
extern "C"
{
#endif

#include "rcutils/time.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/snprintf.h"

rcutils_ret_t
rcutils_time_point_value_as_nanoseconds_string(
  const rcutils_time_point_value_t * time_point,
  char * str,
  size_t str_size)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(time_point, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(str, RCUTILS_RET_INVALID_ARGUMENT);
  if (0 == str_size) {
    return RCUTILS_RET_OK;
  }
  if (rcutils_snprintf(str, str_size, "%.19" PRId64, *time_point) < 0) {
    RCUTILS_SET_ERROR_MSG("failed to format time point into string as nanoseconds");
    return RCUTILS_RET_ERROR;
  }
  return RCUTILS_RET_OK;
}

rcutils_ret_t
rcutils_time_point_value_as_seconds_string(
  const rcutils_time_point_value_t * time_point,
  char * str,
  size_t str_size)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(time_point, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(str, RCUTILS_RET_INVALID_ARGUMENT);
  if (0 == str_size) {
    return RCUTILS_RET_OK;
  }
  // best to abs it to avoid issues with negative values in C89, see:
  //   https://stackoverflow.com/a/3604984/671658
  uint64_t abs_time_point = (uint64_t)llabs(*time_point);
  // break into two parts to avoid floating point error
  uint64_t seconds = abs_time_point / (1000u * 1000u * 1000u);
  uint64_t nanoseconds = abs_time_point % (1000u * 1000u * 1000u);
  if (
    rcutils_snprintf(
      str, str_size, "%s%.10" PRId64 ".%.9" PRId64,
      (*time_point >= 0) ? "" : "-", seconds, nanoseconds) < 0)
  {
    RCUTILS_SET_ERROR_MSG("failed to format time point into string as float seconds");
    return RCUTILS_RET_ERROR;
  }
  return RCUTILS_RET_OK;
}

rcutils_ret_t 
rcutils_time_point_value_as_hmsms_string(
  const rcutils_time_point_value_t * time_point,
  char * str,
  size_t str_size)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(time_point, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(str, RCUTILS_RET_INVALID_ARGUMENT);
  if (0 == str_size) {
    return RCUTILS_RET_OK;
  }
  // best to abs it to avoid issues with negative values in C89, see:
  //   https://stackoverflow.com/a/3604984/671658
  uint64_t abs_time_point = (uint64_t)llabs(*time_point);
  // Calculate hours, minutes, seconds, and milliseconds
  uint64_t milliseconds = abs_time_point / 1000000u; // Convert nanoseconds to milliseconds
  uint64_t seconds = milliseconds / 1000u;
  uint64_t minutes = seconds / 60u;
  uint64_t hours = minutes / 60u;
  milliseconds %= 1000u;
  seconds %= 60u;
  minutes %= 60u;
  hours += 8u; //GMT +8:00
  hours %= 24u;
  
  if (rcutils_snprintf(
      str, str_size, "%s%02" PRId64 ":%02" PRId64 ":%02" PRId64 ".%03" PRId64,
      (*time_point >= 0) ? "" : "->", hours, minutes, seconds, milliseconds) < 0)
  {
    RCUTILS_SET_ERROR_MSG("failed to format time point into string");
    return RCUTILS_RET_ERROR;
  }
  return RCUTILS_RET_OK;
}

rcutils_ret_t
rcutils_time_point_value_as_date_string(
        const rcutils_time_point_value_t * time_point,
        char * str,
        size_t str_size)
{
    RCUTILS_CHECK_ARGUMENT_FOR_NULL(time_point, RCUTILS_RET_INVALID_ARGUMENT);
    RCUTILS_CHECK_ARGUMENT_FOR_NULL(str, RCUTILS_RET_INVALID_ARGUMENT);
    if (0 == str_size) {
        return RCUTILS_RET_OK;
    }
    // best to abs it to avoid issues with negative values in C89, see:
    //   https://stackoverflow.com/a/3604984/671658
    uint64_t abs_time_point = (uint64_t)llabs(*time_point);
    // break into two parts to avoid floating point error
    uint64_t seconds = abs_time_point / (1000u * 1000u * 1000u);
    uint64_t nanoseconds = abs_time_point % (1000u * 1000u * 1000u);
    char str2[21]; // Make sure the buffer is large enough to hold the largest possible uint64_t

    snprintf(str2, sizeof(str2), "%" PRIu64, nanoseconds);

    time_t now_t = (time_t)(seconds);
    struct tm * ptm = localtime(&now_t );

    strftime(str, 32, "%Y-%m-%d %H:%M:%S", ptm);
    int position = 19;
    snprintf(&str[position], str_size - position, ".%.3s" , str2);
    return RCUTILS_RET_OK;
}

#if __cplusplus
}
#endif
