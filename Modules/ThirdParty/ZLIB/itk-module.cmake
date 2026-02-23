set(DOCUMENTATION "This module contains the third party <a
href=\"https://github.com/zlib-ng/zlib-ng\">zlib-ng</a>
general purpose data compression library,
designed as a drop-in replacement for ZLIB.")

itk_module(ITKZLIB
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "Zlib"
  SPDX_COPYRIGHT_TEXT
    "Copyright (C) 1995-2024 Jean-loup Gailly and Mark Adler"
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/zlib-ng/zlib-ng"
)
