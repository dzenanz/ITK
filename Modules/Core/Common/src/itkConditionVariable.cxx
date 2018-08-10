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
#include "itkConditionVariable.h"
#include <type_traits>

namespace itk
{
ConditionVariable::ConditionVariable() {}

ConditionVariable::~ConditionVariable() {}

void ConditionVariable::Signal()
{
  m_ConditionVariable.notify_one();
}

void ConditionVariable::Broadcast()
{
  m_ConditionVariable.notify_all();
}

void ConditionVariable::Wait(SimpleMutexLock *mutex)
{
  static_assert(std::is_same<MutexType, std::mutex>::value,
    "std::unique_lock only works with std::mutex");
  std::unique_lock<std::mutex> lock(mutex->GetMutexLock(), std::adopt_lock);
  m_ConditionVariable.wait(lock);
  lock.release();
}
} //end of namespace itk
