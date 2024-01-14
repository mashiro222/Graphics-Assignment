# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\temp_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\temp_autogen.dir\\ParseCache.txt"
  "temp_autogen"
  )
endif()
