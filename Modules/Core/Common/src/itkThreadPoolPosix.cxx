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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

namespace itk
{
ThreadIdType
ThreadPool
::GetGlobalDefaultNumberOfThreadsByPlatform()
{
  ThreadIdType num;

  // Default the number of threads to be the number of available
  // processors if we are using pthreads()
#ifdef _SC_NPROCESSORS_ONLN
  num = static_cast<ThreadIdType>( sysconf(_SC_NPROCESSORS_ONLN) );
#elif defined( _SC_NPROC_ONLN )
  num = static_cast<ThreadIdType>( sysconf(_SC_NPROC_ONLN) );
#else
  num = 1;
#endif
#if defined( __SVR4 ) && defined( sun ) && defined( PTHREAD_MUTEX_NORMAL )
  pthread_setconcurrency(num);
#endif

#ifdef __APPLE__
  // Determine the number of CPU cores. Prefer sysctlbyname()
  // over MPProcessors() because it doesn't require CoreServices
  // (which is only available in 32bit on Mac OS X 10.4).
  // hw.logicalcpu takes into account cores/CPUs that are
  // disabled because of power management.
  size_t dataLen = sizeof( int ); // 'num' is an 'int'
  int result = sysctlbyname("hw.logicalcpu", &num, &dataLen, ITK_NULLPTR, 0);
  if( result == -1 )
    {
    num = 1;
    }
#endif
  return num;
}

void
ThreadPool
::PlatformCreate(Semaphore &semaphore)
{
  bool success = false;
#if defined(__APPLE__)
  success = semaphore_create(mach_task_self(), &semaphore, SYNC_POLICY_FIFO, 0) == KERN_SUCCESS;
#else
  success = sem_init(&semaphore, 0, 0) == 0;
#endif
  if (!success)
    {
    itkGenericExceptionMacro(<< std::endl << "m_Semaphore cannot be initialized. " << strerror(errno));
    }
}

void
ThreadPool
::PlatformWait(Semaphore &semaphore)
{
  bool success = false;
#if defined(__APPLE__)
  success = semaphore_wait(semaphore) == KERN_SUCCESS;
#else
  success = sem_wait(&semaphore) == 0;
#endif
  if (!success)
    {
    itkGenericExceptionMacro(<< "CreateSemaphore error" << strerror(errno));
    }
}

void
ThreadPool
::PlatformSignal(Semaphore &semaphore)
{
  bool success = false;
#if defined(__APPLE__)
  success = semaphore_signal(semaphore) == KERN_SUCCESS;
#else
  success = sem_post(&semaphore) == 0;
#endif
  if (!success)
    {
    //m_ExceptionOccurred = true;
    itkGenericExceptionMacro(<< "CreateSemaphore error");
    }
}

void
ThreadPool
::PlatformDelete(Semaphore &semaphore)
{
  bool success = false;
#if defined(__APPLE__)
  success = semaphore_destroy(mach_task_self(), semaphore) == KERN_SUCCESS;
#else
  success = sem_destroy(&semaphore) == 0;
#endif
  if (!success)
    {
    //m_ExceptionOccurred = true;
    itkGenericExceptionMacro(<< "CreateSemaphore error");
    }
}

bool
ThreadPool
::PlatformClose(ThreadProcessIdType &threadId)
{
  return pthread_join(threadId, ITK_NULLPTR) == 0;
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

  pthread_attr_t attr;
  pthread_attr_init(&attr);

#if !defined( __CYGWIN__ )
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
#endif
  const int rc = pthread_create(threadHandle, &attr, &ThreadPool::ThreadExecute, id);

  if (rc)
    {
    itkDebugStatement(std::cerr << "ERROR; return code from pthread_create() is " << rc << std::endl);
    itkExceptionMacro(<< "Cannot create thread. Error in return code from pthread_create()");
    }
}

}
