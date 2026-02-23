set(DOCUMENTATION "This module contains the third party <a
href=\"http://www.openjpeg.org/\">OpenJPEG</a> library.
The OpenJPEG library is an open-source JPEG 2000 codec written in C language. It
has been developed in order to promote the use of JPEG 2000, the new still-image
compression standard from the Joint Photographic Experts Group (JPEG).")

itk_module(ITKOpenJPEG
  EXCLUDE_FROM_DEFAULT
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "BSD-2-Clause"
  SPDX_COPYRIGHT_TEXT
    "Copyright (c) 2002-2014 Universite catholique de Louvain (UCL), Belgium"
    "Copyright (c) 2002-2014 Professor Benoit Macq"
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/uclouvain/openjpeg"
)
