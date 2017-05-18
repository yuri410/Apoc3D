/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2010-2017 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "AsyncProcessor.h"
#include "apoc3d/Platform/Thread.h"
#include "GenerationTable.h"
#include "apoc3d/Core/Resource.h"
#include "apoc3d/Library/tinythread.h"

#include <chrono>

using namespace Apoc3D::Platform;

namespace Apoc3D
{
	namespace Core
	{
		namespace Streaming
		{
			
			AsyncProcessor::AsyncProcessor(GenerationTable* gtable, const String& name, bool isThreaded)
				: m_genTable(gtable)
			{
				if (isThreaded)
				{
					m_queueMutex = new tthread::mutex();
					m_processThread = new tthread::thread(&AsyncProcessor::ThreadEntry, this);
					SetThreadName(m_processThread, name);
				}
			}

			AsyncProcessor::~AsyncProcessor(void)
			{
				DELETE_AND_NULL(m_processThread);
				DELETE_AND_NULL(m_queueMutex);
			}

			void AsyncProcessor::ThreadEntry(void* arg) { static_cast<AsyncProcessor*>(arg)->Main(); }

			void AsyncProcessor::Main()
			{
				const float CollectInterval = 1;
				const float GenUpdateInterval = 0.25f;

				float accumulatedCollectWaitingTime = 0;
				float accumulatedGenUpdateWaitingTime = 0;


				float timeStart1 = (float)clock() / CLOCKS_PER_SEC;
				float timeStart2 = timeStart1;

				while (!m_closed)
				{
					bool haveRest = true;

					ResourceOperation resOp;
					m_queueMutex->lock();
					if (m_opQueue.getCount() > 0)
					{
						resOp = m_opQueue.Dequeue();
					}

					m_queueMutex->unlock();

					if (resOp.isValid())
					{
						Resource::ProcessResourceOperation(resOp);

						if (resOp.Subject->isPostSyncNeeded())
						{
							m_postSyncQueue.Enqueue(resOp);
						}

						haveRest = false;
					}
					
					float t = (float)clock() / CLOCKS_PER_SEC;
					float timeSpent1 = t - timeStart1;
					if (timeSpent1<0) timeSpent1 = 0;
					float timeSpent2 = t - timeStart2;
					if (timeSpent2<0) timeSpent2 = 0;


					accumulatedCollectWaitingTime += timeSpent1;
					accumulatedGenUpdateWaitingTime += timeSpent2;
					timeStart1 = t;
					timeStart2 = t;

					if (accumulatedGenUpdateWaitingTime > GenUpdateInterval)
					{
						m_genTable->SubTask_GenUpdate();
						accumulatedGenUpdateWaitingTime = 0;
						if (!haveRest) ApocSleep(10);
					}


					if (accumulatedCollectWaitingTime > CollectInterval)
					{
						m_genTable->SubTask_Collect();
						accumulatedCollectWaitingTime = 0;
						if (!haveRest) ApocSleep(10);
					}

					if (haveRest) ApocSleep(10);
					
				}
			}

			bool AsyncProcessor::NeutralizeTask(const ResourceOperation& op)
			{
				bool passed = false;

				Resource* res = op.Subject;

				if (res && res->isIndependent())
				{
					if (op.Type == ResourceOperation::RESOP_Load)
					{
						LockQueue();

						passed = ClearMatchingResourceOperation(res, ResourceOperation::RESOP_Unload);

						UnlockQueue();
					}
					else if (op.Type == ResourceOperation::RESOP_Unload)
					{
						LockQueue();

						passed = ClearMatchingResourceOperation(res, ResourceOperation::RESOP_Load);

						UnlockQueue();
					}
				}
				return passed;
			}
			void AsyncProcessor::AddTask(const ResourceOperation& op)
			{
				LockQueue();

				m_opQueue.Enqueue(op);

				UnlockQueue();
			}
			void AsyncProcessor::RemoveTask(const ResourceOperation& op)
			{
				LockQueue();

				for (int32 i = 0; i < m_opQueue.getCount(); i++)
				{
					ResourceOperation& e = m_opQueue[i];
					if (e == op)
					{
						e.Invalidate();
					}
				}

				UnlockQueue();
			}
			void AsyncProcessor::RemoveTask(Resource* res)
			{
				LockQueue();

				for (int32 i = 0; i < m_opQueue.getCount(); i++)
				{
					ResourceOperation& e = m_opQueue[i];
					if (e.Subject == res)
					{
						e.Invalidate();
					}
				}

				UnlockQueue();
			}

			bool AsyncProcessor::TaskCompleted()
			{
				volatile bool result;
				LockQueue();
				result = m_opQueue.getCount() == 0;
				UnlockQueue();
				return result;
			}
			int AsyncProcessor::GetOperationCount()
			{
				volatile int result;
				LockQueue();
				result = m_opQueue.getCount();
				UnlockQueue();
				return result;
			}
			void AsyncProcessor::WaitForCompletion()
			{
				assert(m_processThread);
				while (!TaskCompleted())
				{
					ApocSleep(10);
				}
			}
			void AsyncProcessor::Shutdown()
			{
				m_closed = true;
				m_genTable->ShutDown();
				if (m_processThread)
				{
					m_processThread->join();
				}
			}

			void AsyncProcessor::ProcessPostSync(float& timeLeft)
			{
				using namespace std::chrono;
				
				if (m_postSyncQueue.getCount()>0)
				{
					high_resolution_clock::time_point t1 = high_resolution_clock::now();

					bool processed = false;
					while (m_postSyncQueue.getCount() > 0)
					{
						const ResourceOperation& resop = m_postSyncQueue.Head();

						for (int32 i = 0; i < 5; i++)
						{
							processed = true;

							Resource::ProcessResourceOperationPostSyc(resop, i * 25);

							high_resolution_clock::time_point t2 = high_resolution_clock::now();
							duration<float> time_span = duration_cast<duration<float>>(t2 - t1);

							if (time_span.count() >= timeLeft)
							{
								timeLeft = 0;
								break;
							}
						}
					}

					if (processed && timeLeft > 0)
					{
						high_resolution_clock::time_point t2 = high_resolution_clock::now();
						duration<float> time_span = duration_cast<duration<float>>(t2 - t1);
						timeLeft -= time_span.count();
					}
				}
				
			}

			void AsyncProcessor::LockQueue()
			{
				if (m_queueMutex)
				{
					m_queueMutex->lock();
				}
			}
			void AsyncProcessor::UnlockQueue()
			{
				if (m_queueMutex)
				{
					m_queueMutex->unlock();
				}
			}

			bool AsyncProcessor::ClearMatchingResourceOperation(Resource* res, ResourceOperation::OperationType type)
			{
				for (int i = 0; i < m_opQueue.getCount(); i++)
				{
					ResourceOperation& other = m_opQueue[i];
					if (other.Subject == res && other.Type == type)
					{
						other.Invalidate();
						return true;
					}
				}
				return false;
			}
		}
	}
}