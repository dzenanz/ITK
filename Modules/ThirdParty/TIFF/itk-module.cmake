set(DOCUMENTATION "This module contains the third party <a
href=\"http://www.libtiff.org/\">Tag Image File Format (TIFF)</a>
image file format library.")

itk_module(ITKTIFF
  DEPENDS
    ITKZLIB
    ITKJPEG
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "libtiff"
  SPDX_COPYRIGHT_TEXT
    "Copyright (c) 1988-1997 Sam Leffler"
    "Copyright (c) 1991-1997 Silicon Graphics, Inc."
  SPDX_DOWNLOAD_LOCATION
    "https://gitlab.com/libtiff/libtiff"
)
