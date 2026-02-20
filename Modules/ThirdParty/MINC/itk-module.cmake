set(DOCUMENTATION "This module contains the third party <a
href=\"http://www.bic.mni.mcgill.ca/ServicesSoftware/MINC\">MINC</a>
image file format library.")

if(ITK_USE_SYSTEM_MINC)
  itk_module(ITKMINC
    DESCRIPTION
      "${DOCUMENTATION}"
    EXCLUDE_FROM_DEFAULT
    SPDX_LICENSE_IDENTIFIER
      "BSD-3-Clause"
    SPDX_COPYRIGHT_TEXT
      "Copyright 1993-2006 Peter Neelin and David MacDonald, McConnell Brain Imaging Centre, Montreal Neurological Institute, McGill University"
    SPDX_DOWNLOAD_LOCATION
      "https://github.com/BIC-MNI/libminc"
    )
else()
  itk_module(ITKMINC
    DEPENDS
      ITKHDF5
      ITKKWSys
      ITKZLIB
    DESCRIPTION
      "${DOCUMENTATION}"
    EXCLUDE_FROM_DEFAULT
    SPDX_LICENSE_IDENTIFIER
      "BSD-3-Clause"
    SPDX_COPYRIGHT_TEXT
      "Copyright 1993-2006 Peter Neelin and David MacDonald, McConnell Brain Imaging Centre, Montreal Neurological Institute, McGill University"
    SPDX_DOWNLOAD_LOCATION
      "https://github.com/BIC-MNI/libminc"
  )
endif()
