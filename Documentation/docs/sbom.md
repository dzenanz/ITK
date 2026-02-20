# SPDX & SBOM

## Overview

Software Bill of Materials (SBOM) are becoming increasingly important for
software development, especially when it comes to supply chain security.
[Software Package Data Exchange (SPDX)](https://spdx.dev/) is an open standard
for communicating SBOM information that supports accurate identification of
software components, explicit mapping of relationships between components,
and the association of security and licensing information with each component.

Configuring ITK with the CMake option `ITK_GENERATE_SBOM` set to `ON` enables
the generation of an SPDX SBOM document at configure time. The generated file
describes all ITK modules enabled in the current build configuration along with
their third-party dependencies and licensing information.

## Enabling SBOM Generation

To generate an SBOM when building ITK, set the `ITK_GENERATE_SBOM` option:

```bash
cmake -DITK_GENERATE_SBOM=ON /path/to/ITK
```

The SPDX document will be written to the build directory as
`ITK-<version>.spdx` and will be installed alongside ITK documentation.

## Module SPDX Metadata

Each ITK module (including third-party modules) can declare SPDX metadata
in its `itk-module.cmake` file using the following keywords:

- `SPDX_LICENSE_IDENTIFIER` — An [SPDX license expression](https://spdx.org/licenses/)
  describing the module's license (e.g. `Apache-2.0`, `BSD-3-Clause`).
- `SPDX_COPYRIGHT_TEXT` — One or more copyright statements for the module.
- `SPDX_DOWNLOAD_LOCATION` — The upstream URL where the module source can be
  obtained.

### Example

```cmake
itk_module(ITKZLIB
  DESCRIPTION
    "${DOCUMENTATION}"
  SPDX_LICENSE_IDENTIFIER
    "Zlib"
  SPDX_COPYRIGHT_TEXT
    "Copyright (C) 1995-2024 Jean-loup Gailly and Mark Adler"
  SPDX_DOWNLOAD_LOCATION
    "https://github.com/zlib-ng/zlib-ng"
)
```

Modules that do not provide SPDX metadata will appear in the generated SBOM
with `NOASSERTION` values.

## Remote Modules

Remote modules can also declare SPDX metadata using the same keywords. When a
remote module includes `SPDX_LICENSE_IDENTIFIER`, `SPDX_COPYRIGHT_TEXT`, and
`SPDX_DOWNLOAD_LOCATION` in its `itk-module.cmake`, the information will be
incorporated into the build-time SBOM alongside other enabled modules.

## Generated SBOM Format

The generated SBOM follows the [SPDX 2.3 specification](https://spdx.github.io/spdx-spec/v2.3/)
in tag-value format. It includes:

- A document header with creation metadata.
- A root package entry for ITK itself.
- A package entry for each enabled module, including third-party dependencies.
- `CONTAINS` relationships from the ITK root package to each module.
- `DEPENDS_ON` relationships reflecting module dependency information.

## Resources

- <https://spdx.dev/>
- <https://spdx.dev/specifications/>
- <https://spdx.org/licenses/>
- <https://en.wikipedia.org/wiki/Software_supply_chain>
- <https://www.linuxfoundation.org/blog/blog/spdx-its-already-in-use-for-global-software-bill-of-materials-sbom-and-supply-chain-security>
