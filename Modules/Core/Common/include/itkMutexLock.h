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

#ifndef itkMutexLock_h
#define itkMutexLock_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkThreadSupport.h"

namespace itk
{
/** \class SimpleMutexLock
 * \brief Simple mutual exclusion locking class.
 *
 * SimpleMutexLock allows the locking of variables which are accessed
 * through different threads. This mutex is re-entrant (recursive).
 *
 * This class is inefficient, and is retained for backwards compatbility.
 * It is identical to std::recursive_mutex in functionality.
 *
 * \ingroup OSSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT SimpleMutexLock
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(SimpleMutexLock);

  /** Standard class type aliases.  */
  using Self = SimpleMutexLock;

  /** Constructor and destructor left public purposely. */
  SimpleMutexLock() = default;
  virtual ~SimpleMutexLock() = default;

  /** Methods for creation and destruction through the object factory. */
  static SimpleMutexLock * New();

  void Delete() { delete this; }

  /** Used for debugging and other run-time purposes. */
  virtual const char * GetNameOfClass() { return "itkSimpleMutexLock"; }

  /** Lock the MutexLock. */
  void Lock()
  {
    m_MutexLock.lock();
  }

  /** Non-blocking Lock access.
   \return bool - true if lock is captured, false if it was already held by someone else.
   */
  bool TryLock()
  {
    return m_MutexLock.try_lock();
  }

  /** Unlock the MutexLock. */
  void Unlock()
  {
    m_MutexLock.unlock();
  }

  /** Access the MutexType member variable from outside this class */
  MutexType & GetMutexLock()
  {
    return m_MutexLock;
  }

protected:
  MutexType m_MutexLock;
};

/** \class MutexLock
 * \brief Mutual exclusion locking class.
 *
 * MutexLock allows the locking of variables which are accessed
 * through different threads. This mutex is re-entrant (recursive).
 * This header file also defines SimpleMutexLock which is
 * not a subclass of itkObject.
 *
 * \ingroup OSSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT MutexLock:public Object
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(MutexLock);

  /** Standard class type aliases. */
  using Self = MutexLock;
  using Superclass = Object;
  using Pointer = SmartPointer< Self >;
  using ConstPointer = SmartPointer< const Self >;

  /** Method for creation. */
  itkNewMacro(Self);

  /** Run-time information. */
  itkTypeMacro(MutexLock, Object);

  /** Lock the itkMutexLock. */
  void Lock();

  /** Non-blocking Lock access.
   \return bool - true if lock is captured, false if it was already heald by someone else.
   */
  bool TryLock();

  /** Unlock the MutexLock. */
  void Unlock();

protected:
  MutexLock() = default;
  ~MutexLock() override = default;

  SimpleMutexLock m_SimpleMutexLock;
  void PrintSelf(std::ostream & os, Indent indent) const override;
};

inline void MutexLock::Lock()
{
  m_SimpleMutexLock.Lock();
}

inline bool MutexLock::TryLock()
{
  return m_SimpleMutexLock.TryLock();
}

inline void MutexLock::Unlock()
{
  m_SimpleMutexLock.Unlock();
}
} //end itk namespace
#endif
