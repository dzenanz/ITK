# ITKSBOM.cmake - Generate SPDX Software Bill of Materials for ITK builds
#
# When ITK_GENERATE_SBOM is ON, this module generates an SPDX 2.3 document
# describing all enabled ITK modules and their third-party dependencies.
#
# The generated SBOM file is placed in the build directory and optionally
# installed alongside ITK.

function(itk_generate_sbom)
  if(NOT ITK_GENERATE_SBOM)
    return()
  endif()

  string(TIMESTAMP _sbom_created UTC)
  # Format: YYYY-MM-DDTHH:MM:SSZ
  string(REGEX REPLACE
    "^([0-9][0-9][0-9][0-9])([0-9][0-9])([0-9][0-9])([0-9][0-9])([0-9][0-9])([0-9][0-9])$"
    "\\1-\\2-\\3T\\4:\\5:\\6Z"
    _sbom_created
    "${_sbom_created}"
  )

  set(_sbom_content "SPDXVersion: SPDX-2.3\n")
  string(APPEND _sbom_content "DataLicense: CC0-1.0\n")
  string(APPEND _sbom_content "SPDXID: SPDXRef-DOCUMENT\n")
  string(APPEND _sbom_content "DocumentName: ITK-${ITK_VERSION}\n")
  string(APPEND _sbom_content "DocumentNamespace: https://itk.org/ITK-${ITK_VERSION}-${_sbom_created}\n")
  string(APPEND _sbom_content "Creator: Tool: CMake-${CMAKE_VERSION}\n")
  string(APPEND _sbom_content "Creator: Organization: NumFOCUS\n")
  string(APPEND _sbom_content "Created: ${_sbom_created}\n")
  string(APPEND _sbom_content "\n")

  # Root ITK package
  string(APPEND _sbom_content "##### Package: ITK\n")
  string(APPEND _sbom_content "\n")
  string(APPEND _sbom_content "PackageName: ITK\n")
  string(APPEND _sbom_content "SPDXID: SPDXRef-Package-ITK\n")
  string(APPEND _sbom_content "PackageVersion: ${ITK_VERSION}\n")
  string(APPEND _sbom_content "PackageDownloadLocation: https://github.com/InsightSoftwareConsortium/ITK\n")
  string(APPEND _sbom_content "FilesAnalyzed: false\n")
  string(APPEND _sbom_content "PackageLicenseConcluded: Apache-2.0\n")
  string(APPEND _sbom_content "PackageLicenseDeclared: Apache-2.0\n")
  string(APPEND _sbom_content "PackageLicenseInfoFromFiles: NOASSERTION\n")
  string(APPEND _sbom_content "PackageCopyrightText: <text>\n")
  string(APPEND _sbom_content "Copyright 1999-2019 Insight Software Consortium\n")
  string(APPEND _sbom_content "Copyright 2020-present NumFOCUS\n")
  string(APPEND _sbom_content "</text>\n")
  string(APPEND _sbom_content "\n")
  string(APPEND _sbom_content "Relationship: SPDXRef-DOCUMENT DESCRIBES SPDXRef-Package-ITK\n")
  string(APPEND _sbom_content "\n")

  # Track packages for relationship output
  set(_package_ids "")

  # Generate package entry for each enabled module
  foreach(_module IN LISTS ITK_MODULES_ENABLED)
    # Skip test modules
    if(${_module}_IS_TEST)
      continue()
    endif()

    # Create a sanitized SPDX ID
    string(REPLACE "::" "-" _spdx_id "${_module}")

    set(_license "NOASSERTION")
    if(NOT "${ITK_MODULE_${_module}_SPDX_LICENSE_IDENTIFIER}" STREQUAL "")
      set(_license "${ITK_MODULE_${_module}_SPDX_LICENSE_IDENTIFIER}")
    endif()

    set(_copyright "NOASSERTION")
    if(NOT "${ITK_MODULE_${_module}_SPDX_COPYRIGHT_TEXT}" STREQUAL "")
      set(_copyright "${ITK_MODULE_${_module}_SPDX_COPYRIGHT_TEXT}")
    endif()

    set(_download "NOASSERTION")
    if(NOT "${ITK_MODULE_${_module}_SPDX_DOWNLOAD_LOCATION}" STREQUAL "")
      set(_download "${ITK_MODULE_${_module}_SPDX_DOWNLOAD_LOCATION}")
    endif()

    string(APPEND _sbom_content "##### Package: ${_module}\n")
    string(APPEND _sbom_content "\n")
    string(APPEND _sbom_content "PackageName: ${_module}\n")
    string(APPEND _sbom_content "SPDXID: SPDXRef-Package-${_spdx_id}\n")
    string(APPEND _sbom_content "PackageDownloadLocation: ${_download}\n")
    string(APPEND _sbom_content "FilesAnalyzed: false\n")
    string(APPEND _sbom_content "PackageLicenseConcluded: ${_license}\n")
    string(APPEND _sbom_content "PackageLicenseDeclared: ${_license}\n")
    string(APPEND _sbom_content "PackageLicenseInfoFromFiles: NOASSERTION\n")
    string(APPEND _sbom_content "PackageCopyrightText: <text>\n")
    string(APPEND _sbom_content "${_copyright}\n")
    string(APPEND _sbom_content "</text>\n")
    string(APPEND _sbom_content "\n")

    list(APPEND _package_ids "${_spdx_id}")

    # Add dependency relationship from ITK root to this module
    string(APPEND _sbom_content "Relationship: SPDXRef-Package-ITK CONTAINS SPDXRef-Package-${_spdx_id}\n")

    # Add dependency relationships to other modules
    foreach(_dep IN LISTS ITK_MODULE_${_module}_DEPENDS)
      if(${_dep}_IS_TEST)
        continue()
      endif()
      string(REPLACE "::" "-" _dep_spdx_id "${_dep}")
      string(APPEND _sbom_content "Relationship: SPDXRef-Package-${_spdx_id} DEPENDS_ON SPDXRef-Package-${_dep_spdx_id}\n")
    endforeach()

    string(APPEND _sbom_content "\n")
  endforeach()

  # Write the SBOM file
  set(_sbom_file "${ITK_BINARY_DIR}/ITK-${ITK_VERSION}.spdx")
  file(WRITE "${_sbom_file}" "${_sbom_content}")
  message(STATUS "ITK SBOM generated: ${_sbom_file}")

  # Install the SBOM
  install(
    FILES "${_sbom_file}"
    DESTINATION ${ITK_INSTALL_DOC_DIR}
    COMPONENT Runtime
  )
endfunction()
