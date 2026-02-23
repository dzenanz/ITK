set(DOCUMENTATION "This module contains the third party <a
href=\"https://github.com/chokkan/liblbfgs\">libLBFGS</a> library,
a C++ implementaiton of the LBFGS implementation in Netlib.")

itk_module(ITKLIBLBFGS
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "MIT"
  SPDX_COPYRIGHT_TEXT
    "Copyright (c) 1990 Jorge Nocedal"
    "Copyright (c) 2007-2010 Naoaki Okazaki"
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/chokkan/liblbfgs"
)
