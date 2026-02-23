set(DOCUMENTATION "This module contains the third party <a
href=\"http://www.netlib.org/slatec/\">netlib slatec</a> routines.  They are
used by the probability distributions in ITK.")

itk_module(ITKNetlib
  DEPENDS
    ITKVNL
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "LicenseRef-Netlib-Public-Domain"
  SPDX_COPYRIGHT_TEXT
    "NOASSERTION"
  SPDX_DOWNLOAD_LOCATION
    "https://www.netlib.org/slatec/"
)
