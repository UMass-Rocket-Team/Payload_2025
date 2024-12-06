# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-src"
  "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-build"
  "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-subbuild/picotool-populate-prefix"
  "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-subbuild/picotool-populate-prefix/tmp"
  "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-subbuild/picotool-populate-prefix/src/picotool-populate-stamp"
  "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-subbuild/picotool-populate-prefix/src"
  "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-subbuild/picotool-populate-prefix/src/picotool-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-subbuild/picotool-populate-prefix/src/picotool-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/yizhang/rockets/Payload_2025/third-party/picotool/picotool-subbuild/picotool-populate-prefix/src/picotool-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
