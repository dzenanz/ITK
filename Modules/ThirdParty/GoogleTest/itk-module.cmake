set(DOCUMENTATION "This module contains the third party <a href=\"https://github.com/google/googletest\">googletest</a> library,
Google's C++ test framework.")

itk_module(ITKGoogleTest
  DEPENDS
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "BSD-3-Clause"
  SPDX_COPYRIGHT_TEXT
    "Copyright 2008 Google Inc. All Rights Reserved."
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/google/googletest"
)
