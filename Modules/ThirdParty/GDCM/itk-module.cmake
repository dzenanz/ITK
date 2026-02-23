set(DOCUMENTATION "This module contains the third party <a
href=\"http://sourceforge.net/projects/gdcm/\">GDCM</a> library.
Grassroots DiCoM is a C++ library for DICOM medical files.")

if(ITK_USE_SYSTEM_GDCM)
  itk_module(ITKGDCM
    DESCRIPTION
      "${DOCUMENTATION}"
    EXCLUDE_FROM_DEFAULT
    SPDX_LICENSE_IDENTIFIER
      "BSD-3-Clause"
    SPDX_COPYRIGHT_TEXT
      "Copyright (c) 2006-2024 Mathieu Malaterre"
    SPDX_DOWNLOAD_LOCATION
      "https://github.com/malaterre/GDCM"
    )
else()
  itk_module(ITKGDCM
    DEPENDS
      ITKZLIB
      ITKExpat
      ITKOpenJPEG
    DESCRIPTION
      "${DOCUMENTATION}"
    SPDX_LICENSE_IDENTIFIER
      "BSD-3-Clause"
    SPDX_COPYRIGHT_TEXT
      "Copyright (c) 2006-2024 Mathieu Malaterre"
    SPDX_DOWNLOAD_LOCATION
      "https://github.com/malaterre/GDCM"
  )
endif()
