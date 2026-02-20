set(DOCUMENTATION "This module builds the DCMTK library <a
href=\"http://dicom.offis.de/dcmtk.php.en\">DCMTK</a> DICOM
library suite.")

if(ITK_USE_SYSTEM_DCMTK)
  itk_module(ITKDCMTK
    DESCRIPTION
    "${DOCUMENTATION}"
    EXCLUDE_FROM_DEFAULT
    SPDX_LICENSE_IDENTIFIER
      "BSD-3-Clause"
    SPDX_COPYRIGHT_TEXT
      "Copyright (C) 1994-2024 OFFIS e.V."
    SPDX_DOWNLOAD_LOCATION
      "https://dicom.offis.de/dcmtk"
    )

else()
  itk_module(ITKDCMTK
    DEPENDS
    ITKZLIB
    ITKJPEG
    ITKTIFF
    ITKPNG
    DESCRIPTION
    "${DOCUMENTATION}"
    EXCLUDE_FROM_DEFAULT
    SPDX_LICENSE_IDENTIFIER
      "BSD-3-Clause"
    SPDX_COPYRIGHT_TEXT
      "Copyright (C) 1994-2024 OFFIS e.V."
    SPDX_DOWNLOAD_LOCATION
      "https://dicom.offis.de/dcmtk"
    )
endif()
