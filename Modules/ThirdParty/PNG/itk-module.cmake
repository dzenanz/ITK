set(DOCUMENTATION "This module contains the third party <a
href=\"http://www.libpng.org/pub/png/libpng.html/\">Portable Network Graphics
(PNG)</a> image file format library.")

itk_module(ITKPNG
  DEPENDS
    ITKZLIB
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "Libpng"
  SPDX_COPYRIGHT_TEXT
    "Copyright (c) 1995-2024 The PNG Reference Library Authors"
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/glennrp/libpng"
)
