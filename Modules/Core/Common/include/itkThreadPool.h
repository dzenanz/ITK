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
#ifndef itkThreadPool_h
#define itkThreadPool_h

#include "itkConfigure.h"
#include "itkIntTypes.h"

#include <map>
#include <set>
#include "concurrentqueue.h"

#include "itkThreadJob.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkSimpleFastMutexLock.h"
#include "itkMutexLockHolder.h"

namespace itk
{

/**
 * \class ThreadPool
 * \brief Thread pool maintains a constant number of threads.
 *
 * Thread pool is called and initialized from within the MultiThreader.
 * Initially the thread pool is started with GlobalDefaultNumberOfThreads.
 * The ThreadJob class is used to submit jobs to the thread pool. The ThreadJob's
 * necessary members need to be set and then the ThreadJob can be passed to the
 * ThreadPool by calling its AddWork method which returns the job id.
 * One can then wait for the job by calling the WaitForJob method.
 *
 * \ingroup OSSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT ThreadPool : public Object
{
public:

  /** Standard class typedefs. */
  typedef ThreadPool               Self;
  typedef Object                   Superclass;
  typedef SmartPointer< Self >     Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  typedef ThreadJob::Semaphore Semaphore;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ThreadPool, Object);

  /** Returns the global instance of the ThreadPool */
  static Pointer New();

  /** Returns the global singleton instance of the ThreadPool
   *
   * This method is a Singleton and does not have a New method.
   */
  static Pointer GetInstance();

  /** Add this job to the thread pool queue.
   * All data members of the ThreadJob must be filled.
   * The semaphore pointer must point to a valid semaphore structure.
   * AddWork will initialize that semaphore, and the invoker must pass it
   * to WaitForJob in order to wait for the job's completion.
   */
  void AddWork(const ThreadJob& job);

  /** Can call this method if we want to add extra threads to the pool. */
  void AddThreads(ThreadIdType count);

  ThreadIdType GetNumberOfCurrentlyIdleThreads();

  /** This method blocks until the given (job) id has finished executing */
  void WaitForJob(Semaphore& jobSemaphore);

  /** Platform specific number of threads */
  static ThreadIdType GetGlobalDefaultNumberOfThreadsByPlatform();

  static ThreadIdType GetGlobalDefaultNumberOfThreads();

  /** Create this times more threads than processors */
  static float OverloadFactor();

protected:

  static void PlatformCreate(Semaphore &semaphore);
  static void PlatformWait(Semaphore &semaphore);
  static void PlatformSignal(Semaphore &semaphore);
  static void PlatformDelete(Semaphore &semaphore);
  static bool PlatformClose(ThreadProcessIdType &threadId); //returns success status

  /** Called to add a thread to the thread pool.
  This method add a thread to the thread pool and pushes the thread handle
  into the m_ThreadSemaphores vector.
   */
  void AddThread();

  /** Platform-specific function to clean up all the threads. */
  void DeleteThreads();
  ThreadPool();
  virtual ~ThreadPool();

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(ThreadPool);

  /** Set when the thread pool is to be stopped */
  bool m_ScheduleForDestruction;

  ThreadIdType m_ThreadCount;

  /** Set if exception occurs */
  bool m_ExceptionOccurred;

  /** This is a list of jobs(ThreadJob) submitted to the thread pool.
   * Filled by AddWork, emptied by ThreadExecute.
   */
  moodycamel::ConcurrentQueue<ThreadJob> m_WorkQueue;

  /** When a thread is idle, it is waiting on m_ThreadsSemaphore.
  * AddWork signals this semaphore to resume a (random) thread.
  */
  Semaphore m_ThreadsSemaphore;

  /** Vector to hold all thread handles.
   * Thread handles are used to delete the threads.
   */
  std::vector<ThreadProcessIdType> m_Threads;

  /** To lock on the internal variables */
  static SimpleFastMutexLock m_Mutex;

  static Pointer m_ThreadPoolInstance;

  /** The continuously running thread function */
  static void * ThreadExecute(void *param);

  /* Global variable defining the default number of threads to set at
  * construction time of a MultiThreader instance.
  * m_GlobalDefaultNumberOfThreads must always be less than or equal to the
  * m_GlobalMaximumNumberOfThreads and larger or equal to 1 once it has been
  * initialized in the constructor of the first MultiThreader instantiation.
  */
  static ThreadIdType m_GlobalDefaultNumberOfThreads;
};

}
#endif
