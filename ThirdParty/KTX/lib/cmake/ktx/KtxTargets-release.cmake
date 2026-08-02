#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KTX::ktx" for configuration "Release"
set_property(TARGET KTX::ktx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KTX::ktx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/ktx.lib"
  )

list(APPEND _cmake_import_check_targets KTX::ktx )
list(APPEND _cmake_import_check_files_for_KTX::ktx "${_IMPORT_PREFIX}/lib/ktx.lib" )

# Import target "KTX::astcenc-avx2-static" for configuration "Release"
set_property(TARGET KTX::astcenc-avx2-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KTX::astcenc-avx2-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/astcenc-avx2-static.lib"
  )

list(APPEND _cmake_import_check_targets KTX::astcenc-avx2-static )
list(APPEND _cmake_import_check_files_for_KTX::astcenc-avx2-static "${_IMPORT_PREFIX}/lib/astcenc-avx2-static.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
