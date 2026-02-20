set(DOCUMENTATION "This module contains the third party <a
href=\"http://www.nitrc.org/projects/gifti/\">GIFTI</a> library.
Geometry format under the Neuroimaging Informatics Technology Initiative")

itk_module(ITKGIFTI
  DEPENDS
    ITKZLIB
    ITKExpat
    ITKNIFTI
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "LicenseRef-GIFTI-Public-Domain"
  SPDX_COPYRIGHT_TEXT
    "NOASSERTION"
  SPDX_DOWNLOAD_LOCATION
    "https://www.nitrc.org/projects/gifti/"
)
