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
#include "itkThreadPool.h"
#include <utility>

namespace itk
{
ThreadIdType
ThreadPool
::GetGlobalDefaultNumberOfThreadsByPlatform()
{
  SYSTEM_INFO sysInfo;

  GetSystemInfo(&sysInfo);
  ThreadIdType num = sysInfo.dwNumberOfProcessors;
  return num;
}

void
ThreadPool
::PlatformCreate(Semaphore &semaphore)
{
  semaphore = CreateSemaphore(ITK_NULLPTR, 0, 1000, ITK_NULLPTR);
  if (semaphore == ITK_NULLPTR)
    {
    itkGenericExceptionMacro(<< "CreateSemaphore error" << GetLastError());
    }
}

void
ThreadPool
::PlatformWait(Semaphore &semaphore)
{
  DWORD dwWaitResult = WaitForSingleObject(semaphore, INFINITE);
  if (dwWaitResult != WAIT_OBJECT_0)
    {
    itkGenericExceptionMacro(<< "CreateSemaphore error" << GetLastError());
    }
}

void
ThreadPool
::PlatformSignal(Semaphore &semaphore)
{
  if (!ReleaseSemaphore(semaphore, 1, ITK_NULLPTR))
    {
    itkGenericExceptionMacro(<< "CreateSemaphore error" << GetLastError());
    }
}

void
ThreadPool
::PlatformDelete(Semaphore &semaphore)
{
  if (!CloseHandle(semaphore))
    {
    itkGenericExceptionMacro(<< "CreateSemaphore error" << GetLastError());
    }
}

bool
ThreadPool
::PlatformClose(ThreadProcessIdType &threadId)
{
  return CloseHandle(threadId);
}

void
ThreadPool
::AddThread()
{
  ThreadProcessIdType *threadHandle = new ThreadProcessIdType;
  ThreadIdType *id = new ThreadIdType;
  *id = m_ThreadCount;

  Semaphore sem;
  m_ThreadSemaphores.push_back(std::make_pair(threadHandle, sem));
  PlatformCreate(m_ThreadSemaphores.back().second);

  *threadHandle = CreateThread(
    ITK_NULLPTR,
    0,
    (LPTHREAD_START_ROUTINE) ThreadPool::ThreadExecute,
    id,
    0,
    ITK_NULLPTR);

  if( threadHandle == ITK_NULLPTR )
    {
    itkDebugMacro(<< "ERROR adding thread to thread pool");
    itkExceptionMacro(<< "Cannot create thread.");
    }
}

}
