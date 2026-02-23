set(
  DOCUMENTATION
  "This module contains the third party <a href=\"https://github.com/eigenteam/eigen-git-mirror\">Eigen</a> library.
Eigen is a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms."
)

itk_module(
  ITKEigen3
  DEPENDS
  DESCRIPTION "${DOCUMENTATION}"
  EXCLUDE_FROM_DEFAULT
  SPDX_LICENSE_IDENTIFIER
    "MPL-2.0 AND BSD-3-Clause"
  SPDX_COPYRIGHT_TEXT
    "Copyright (C) 2008-2024 Gael Guennebaud and Benoit Jacob"
  SPDX_DOWNLOAD_LOCATION
    "https://gitlab.com/libeigen/eigen"
)
