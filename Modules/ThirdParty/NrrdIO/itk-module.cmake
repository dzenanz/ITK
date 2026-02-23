set(DOCUMENTATION "This module contains the third party <a
href=\"http://teem.sourceforge.net/nrrd/lib.html\">NRRD</a> image file format.")

itk_module(ITKNrrdIO
  DEPENDS
    ITKZLIB
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "LGPL-2.1-or-later"
  SPDX_COPYRIGHT_TEXT
    "Copyright (C) 2004-2024 University of Utah"
  SPDX_DOWNLOAD_LOCATION
    "https://sourceforge.net/projects/teem/"
)
