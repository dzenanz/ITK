set(DOCUMENTATION "This module contains the third party JPEG
library published by the
<a href=\"http://www.ijg.org/\">Independent JPEG Group</a> and libjpeg-turbo.")

itk_module(ITKJPEG
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "BSD-3-Clause AND IJG"
  SPDX_COPYRIGHT_TEXT
    "Copyright (C) 2009-2024 D. R. Commander. All Rights Reserved."
    "Copyright (C) 2015 Viktor Szathmary. All Rights Reserved."
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/libjpeg-turbo/libjpeg-turbo"
)
