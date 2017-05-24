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

#ifndef itkThreadJob_h
#define itkThreadJob_h

#include "itkMacro.h"

namespace itk
{
/**
 * \class ThreadJob
 *
 * \brief This class is used to submit jobs to the thread pool.
 *
 * The thread job contains information of the submitted job:
 * Job Id, the function to be executed in parallel and its argument.
 *
 * \ingroup OSSystemObjects
 * \ingroup ITKCommon
 */
struct ThreadJob
{
public:
  typedef size_t JobIdType;

  ThreadJob() :
    m_ThreadFunction(ITK_NULLPTR),
    m_Id(-1),
    m_UserData(ITK_NULLPTR)
  {
  }

  ~ThreadJob()
  {
  }


/** Function that will be called. */
#if defined(_WIN32) || defined(_WIN64)
    DWORD ( __stdcall *m_ThreadFunction )( void * ptr );
#else
    void * (*m_ThreadFunction)(void *ptr);
#endif

  /** This is the Job's id. Used for waiting on this job's completion. */
  JobIdType m_Id;

 /** Stores the user's data that needs to be passed into the function. */
  void *m_UserData;
};

} // end namespace itk

#endif // itkThreadJob_h
