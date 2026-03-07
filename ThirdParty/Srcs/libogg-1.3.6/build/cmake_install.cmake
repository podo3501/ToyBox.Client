# Install script for directory: D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/ogg")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/Debug/ogg.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/Release/ogg.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/MinSizeRel/ogg.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/RelWithDebInfo/ogg.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ogg" TYPE FILE FILES
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/include/ogg/config_types.h"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/include/ogg/ogg.h"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/include/ogg/os_types.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake"
         "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg/OggTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/CMakeFiles/Export/dee6fd410a50d06b294b496f57355584/OggTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/Ogg" TYPE FILE FILES
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/OggConfig.cmake"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/OggConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/ogg.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/ogg/html" TYPE FILE FILES
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/framing.html"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/index.html"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/oggstream.html"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/ogg-multiplex.html"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/fish_xiph_org.png"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/multiplex1.png"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/packets.png"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/pages.png"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/stream.png"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/vorbisword2.png"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/white-ogg.png"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/white-xifish.png"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/rfc3533.txt"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/rfc5334.txt"
    "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/skeleton.html"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/ogg/html" TYPE DIRECTORY FILES "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/doc/libogg" REGEX "/[^/]*\\.am$" EXCLUDE)
endif()

if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
  file(WRITE "D:/ProgrammingStudy/ToyBox/ThirdParty/Srcs/libogg-1.3.6/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
