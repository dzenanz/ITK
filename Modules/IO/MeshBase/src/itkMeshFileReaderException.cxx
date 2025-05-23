/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkMeshFileReaderException.h"

namespace itk
{
MeshFileReaderException::~MeshFileReaderException() noexcept = default;

MeshFileReaderException::MeshFileReaderException(std::string  file,
                                                 unsigned int line,
                                                 std::string  message,
                                                 std::string  location)
  : ExceptionObject(std::move(file), line, std::move(message), std::move(location))
{}
} // namespace itk
