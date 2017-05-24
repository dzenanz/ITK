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
#include "itksys/SystemTools.hxx"

#include <algorithm>

namespace itk
{
SimpleFastMutexLock ThreadPool::m_Mutex;

ThreadPool::Pointer ThreadPool::m_ThreadPoolInstance;

ThreadPool::Pointer
ThreadPool
::New()
{
  return Self::GetInstance();
}


ThreadPool::Pointer
ThreadPool
::GetInstance()
{
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
  if( m_ThreadPoolInstance.IsNull() )
    {
    // Try the factory first
    m_ThreadPoolInstance  = ObjectFactory< Self >::Create();
    // if the factory did not provide one, then create it here
    if ( m_ThreadPoolInstance.IsNull() )
      {
      m_ThreadPoolInstance = new ThreadPool();
      // Remove extra reference from construction.
      m_ThreadPoolInstance->UnRegister();
      }
    }
    return m_ThreadPoolInstance;
}

ThreadPool
::ThreadPool() :
  m_ScheduleForDestruction(false),
  m_ThreadCount(0),
  m_IdCounter(1),
  m_ExceptionOccurred(false)
{
  AddThreads(this->OverloadFactor() * this->GetGlobalDefaultNumberOfThreads());
}

#ifndef ITK_USE_WIN32_THREADS
#ifndef ITK_USE_PTHREADS
ThreadIdType ThreadPool::GetGlobalDefaultNumberOfThreadsByPlatform()
{
    return 1; //no multithreading
}
#endif // !ITK_USE_PTHREADS
#endif // !ITK_USE_WIN32_THREADS

// Initialize static member that controls global default number of threads
// 0 => Not initialized.
ThreadIdType ThreadPool::m_GlobalDefaultNumberOfThreads = 0;

ThreadIdType
ThreadPool
::GetGlobalDefaultNumberOfThreads()
{
  // if default number has been set then don't try to update it; just
  // return the value
  if( m_GlobalDefaultNumberOfThreads != 0 )
    {
    return m_GlobalDefaultNumberOfThreads;
    }

  /* The ITK_NUMBER_OF_THREADS_ENV_LIST contains is an
   * environmental variable that holds a ':' separated
   * list of environmental variables that whould be
   * queried in order for setting the m_GlobalMaximumNumberOfThreads.
   *
   * This is intended to be a mechanism suitable to easy
   * runtime modification to ease using the proper number
   * of threads for load balancing batch processing
   * systems where the number of threads
   * authorized for use may be less than the number
   * of physical processors on the computer.
   *
   * This list contains the Sun|Oracle Grid Engine
   * environmental variable "NSLOTS" by default
   */
  std::vector<std::string> ITK_NUMBER_OF_THREADS_ENV_LIST;
  std::string       itkNumberOfThreadsEvnListString = "";
  if( itksys::SystemTools::GetEnv("ITK_NUMBER_OF_THREADS_ENV_LIST",
                                  itkNumberOfThreadsEvnListString) )
    {
    // NOTE: We always put "ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS" at the end
    // unconditionally.
    itkNumberOfThreadsEvnListString += ":ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS";
    }
  else
    {
    itkNumberOfThreadsEvnListString = "NSLOTS:ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS";
    }
    {
    std::stringstream numberOfThreadsEnvListStream(itkNumberOfThreadsEvnListString);
    std::string       item;
    while( std::getline(numberOfThreadsEnvListStream, item, ':') )
      {
      if( item.size() > 0 ) // Do not add empty items.
        {
        ITK_NUMBER_OF_THREADS_ENV_LIST.push_back(item);
        }
      }
    }
  // first, check for environment variable
  std::string itkGlobalDefaultNumberOfThreadsEnv = "0";
  for( std::vector<std::string>::const_iterator lit = ITK_NUMBER_OF_THREADS_ENV_LIST.begin();
       lit != ITK_NUMBER_OF_THREADS_ENV_LIST.end();
       ++lit )
    {
    if( itksys::SystemTools::GetEnv(lit->c_str(), itkGlobalDefaultNumberOfThreadsEnv) )
      {
      m_GlobalDefaultNumberOfThreads =
        static_cast<ThreadIdType>( atoi( itkGlobalDefaultNumberOfThreadsEnv.c_str() ) );
      }
    }

  // otherwise, set number of threads based on system information
  if( m_GlobalDefaultNumberOfThreads <= 0 )
    {
    const ThreadIdType num = GetGlobalDefaultNumberOfThreadsByPlatform();
    m_GlobalDefaultNumberOfThreads = num;
    }

  // limit the number of threads to m_GlobalMaximumNumberOfThreads
  m_GlobalDefaultNumberOfThreads  = std::min( m_GlobalDefaultNumberOfThreads,
                                              ThreadIdType(ITK_MAX_THREADS) );

  // verify that the default number of threads is larger than zero
  m_GlobalDefaultNumberOfThreads  = std::max( m_GlobalDefaultNumberOfThreads,
                                              NumericTraits<ThreadIdType>::OneValue() );

  return m_GlobalDefaultNumberOfThreads;
}

float
ThreadPool
::OverloadFactor()
{
  return 1.0f;
}

void
ThreadPool
::AddThreads(ThreadIdType count)
{
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
  if (this->m_ScheduleForDestruction)
    {
    return;
    }
  m_ThreadSemaphores.reserve(m_ThreadSemaphores.size() + count);
  for( unsigned int i = 0; i < count; ++i )
    {
    AddThread();
    m_IdleThreadIndices.insert(m_ThreadCount);
    m_ThreadCount++;
    }
}

void
ThreadPool
::DeleteThreads()
{
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
  for (size_t i = 0; i < m_ThreadSemaphores.size(); i++)
    {
    PlatformDelete(m_ThreadSemaphores[i].second);
    if (!PlatformClose(*m_ThreadSemaphores[i].first))
      {
      m_ExceptionOccurred = true;
      }
    delete m_ThreadSemaphores[i].first;
    }
}

ThreadIdType
ThreadPool
::GetNumberOfCurrentlyIdleThreads()
{
  ThreadIdType count = 0;
  try
    {
    itkDebugMacro(<< "GetNumberOfCurrentlyIdleThreads");

    MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
    if (m_IdleThreadIndices.size()>m_WorkQueue.size())
      {
      count = m_IdleThreadIndices.size() - m_WorkQueue.size();
      }
    }
  catch( std::exception & itkDebugStatement( e ) )
    {
    m_ExceptionOccurred = true;
    itkDebugMacro(<< "Exception occured in GetNumberOfCurrentlyIdleThreads()"
                  << std::endl << e.what());
    }
  return count;
}

ThreadPool
::~ThreadPool()
{
  itkDebugMacro(<< std::endl << "Thread pool being destroyed" << std::endl);

  std::vector<ThreadJobIdType> jobs(0);

  {//block for mutex holder
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
  this->m_ScheduleForDestruction = true;
  }

  for (ThreadIdType i = 0; i < m_ThreadSemaphores.size(); i++) //add dummy jobs for
    {
    ThreadJob threadJob; //dummy job to cleanly exit the thread
    ThreadJobIdType jobId = this->AddWork(threadJob);
    WaitForJob(jobId);
    }

  {//block for mutex holder
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex); //collect ids of orphaned jobs
  jobs.reserve(m_JobSemaphores.size());
  JobSemaphoreMap::iterator it = m_JobSemaphores.begin();
  for(; it!=m_JobSemaphores.end(); ++it)
    {
    jobs.push_back(it->first);
    }
  }

  //now clean up any jobs not waited for by the caller
  for (size_t i=0; i<jobs.size(); ++i)
    {
    WaitForJob(jobs[i]); //cleans up the semaphore
    }
  DeleteThreads();
}

void
ThreadPool
::WaitForJob(ThreadJobIdType jobId)
{
  JobSemaphoreMap::iterator it;
  {
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
  it = m_JobSemaphores.find(jobId);
  if (it == m_JobSemaphores.end()) //WaitForJob was concurrent with the destructor
    {
    return;
    }
  }

  PlatformWait(it->second);
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
  PlatformDelete(it->second);
  m_JobSemaphores.erase(it);
}

ThreadPool::ThreadJobIdType
ThreadPool
::AddWork(ThreadJob &threadJob)
{

  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
  threadJob.m_Id = m_IdCounter++;
  m_WorkQueue.push_back(threadJob);
  PlatformCreate(m_JobSemaphores[threadJob.m_Id]);
  if (!m_IdleThreadIndices.empty())
    {
    size_t tId = *m_IdleThreadIndices.begin();
    m_IdleThreadIndices.erase(m_IdleThreadIndices.begin());
    PlatformSignal(m_ThreadSemaphores[tId].second);
  }
  return threadJob.m_Id;
}


void *
ThreadPool
::ThreadExecute(void *param)
{
  ThreadIdType *myIdPtr = reinterpret_cast<ThreadIdType *>(param);
  ThreadIdType myId = *myIdPtr;
  Pointer threadPool = GetInstance();
  Semaphore *semaphore;
  {
  MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
  semaphore = &threadPool->m_ThreadSemaphores[myId].second;
  delete myIdPtr;
  }

  while (!threadPool->m_ScheduleForDestruction)
    {
    threadPool->PlatformWait(*semaphore);
    if (threadPool->m_ScheduleForDestruction)
      {
      return ITK_NULLPTR;
      }

    ThreadJob job;
    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
    if (threadPool->m_WorkQueue.empty()) //another thread stole work meant for me
      {
      threadPool->m_IdleThreadIndices.insert(myId);
      continue; //does not happen often
      }
    job = threadPool->m_WorkQueue.front();
    threadPool->m_WorkQueue.pop_front();
    } //releases the lock

    bool repeat = false;
    do
      {
      job.m_ThreadFunction(job.m_UserData); //execute the job, lock has been released

      MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
      PlatformSignal(threadPool->m_JobSemaphores.at(job.m_Id));

      if (threadPool->m_ScheduleForDestruction)
        {
        return ITK_NULLPTR;
        }

      repeat = false;
      if (!threadPool->m_WorkQueue.empty()) //take the next job before releasing the lock
        {
        repeat = true;
        job = threadPool->m_WorkQueue.front();
        threadPool->m_WorkQueue.pop_front();
        }

    } while (repeat); //ending the loop iteration releases the lock

    {
    MutexLockHolder<SimpleFastMutexLock> mutexHolder(m_Mutex);
    threadPool->m_IdleThreadIndices.insert(myId);
    }
  }
  return ITK_NULLPTR;
}

}
