/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef itkFastMutexLock_h
#define itkFastMutexLock_h

#include "itkMutexLock.h"

namespace itk
{
/** \class FastMutexLock
 * \brief Critical section locking class.
 *
 * FastMutexLock allows the locking of variables which are accessed
 * through different threads.  This header file also defines
 * SimpleFastMutexLock which is not a subclass of Object.
 * The API is identical to that of MutexLock, and the behavior is
 * identical as well. The distinction is kept for backwards compatbility.
 *
 * \ingroup OSSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT FastMutexLock:public MutexLock
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(FastMutexLock);

  /** Standard class type aliases. */
  using Self = FastMutexLock;
  using Superclass = MutexLock;
  using Pointer = SmartPointer< Self >;
  using ConstPointer = SmartPointer< const Self >;

  /** Method for creation. */
  itkNewMacro(Self);

  /** Run-time type information. */
  itkTypeMacro(FastMutexLock, MutexLock);

protected:
  FastMutexLock() = default;
  ~FastMutexLock() override = default;
};

} //end itk namespace
#endif
