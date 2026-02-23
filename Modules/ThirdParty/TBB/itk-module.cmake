set(DOCUMENTATION "This module contains the third party <a
href=\"https://www.threadingbuildingblocks.org/\">TBB</a> library.
TBB is Intel TBB threading library.")

# ITKTBB module needs to be defined even if ITK_USE_TBB
# is OFF, otherwise ITK cannot compile.
itk_module(ITKTBB
  DESCRIPTION
    "${DOCUMENTATION}"
  EXCLUDE_FROM_DEFAULT
  SPDX_LICENSE_IDENTIFIER
    "Apache-2.0"
  SPDX_COPYRIGHT_TEXT
    "Copyright (c) 2005-2024 Intel Corporation"
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/oneapi-src/oneTBB"
  )
