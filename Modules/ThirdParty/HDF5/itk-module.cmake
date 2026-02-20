set(DOCUMENTATION "This module contains the third party <a
href=\"http://www.hdfgroup.org/HDF5/\">HDF5</a> library.
HDF5 is a data model, library, and file format for storing and managing data.")

itk_module(ITKHDF5
  DEPENDS
    ITKZLIB
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "BSD-3-Clause"
  SPDX_COPYRIGHT_TEXT
    "Copyright 2006-2024 by The HDF Group."
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/HDFGroup/hdf5"
)
