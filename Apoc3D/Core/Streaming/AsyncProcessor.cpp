#include "AsyncProcessor.h"
#include "apoc3d/Platform/Thread.h"
#include "GenerationTable.h"
#include "apoc3d/Core/Resource.h"

#include "tthread/tinythread.h"

#include <ctime>

using namespace Apoc3D::Platform;

namespace Apoc3D
{
	namespace Core
	{
		namespace Streaming
		{
			
			void AsyncProcessor::ThreadEntry(void* arg)
			{
				AsyncProcessor* obj = static_cast<AsyncProcessor*>(arg);

				obj->Main();
			}
			void AsyncProcessor::Main()
			{
				static const float CollectInterval = 1;
				static const float GenUpdateInterval = 0.25f;

				float accumulatedCollectWaitingTime = 0;
				float accumulatedGenUpdateWaitingTime = 0;


				float timeStart1 = (float)clock() / CLOCKS_PER_SEC;
				float timeStart2 = timeStart1;

				while (!m_closed)
				{
					bool rest = true;

					ResourceOperation* resOp = 0;
					m_syncMutex->lock();
					if (m_opQueue.getCount())
					{
						resOp = m_opQueue.Dequeue();
					}

					m_syncMutex->unlock();

					if (resOp)
					{
						resOp->Process();
						rest = false;
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
						if (!rest) ApocSleep(10);
					}


					if (accumulatedCollectWaitingTime > CollectInterval)
					{
						m_genTable->SubTask_Collect();
						accumulatedCollectWaitingTime = 0;
						if (!rest) ApocSleep(10);
					}

					if (rest) ApocSleep(10);
					
				}
			}
			bool AsyncProcessor::NeutralizeTask(ResourceOperation* op)
			{
				bool passed = false;
				ResourceOperation::OperationType type = op->getType();

				if (op->getResource() && op->getResource()->IsIndependent())
				{
					if (type == ResourceOperation::RESOP_Load)
					{
						m_syncMutex->lock();

						for (int i=0;i<m_opQueue.getCount();i++)
						{
							ResourceOperation* other = m_opQueue.GetElement(i);
							if (other && other->getResource() == op->getResource() && other->getType() == ResourceOperation::RESOP_Unload)
							{
								m_opQueue.SetElement(i,0);
								passed = true;
								break;
							}
						}

						m_syncMutex->unlock();
					}
					else if (type == ResourceOperation::RESOP_Unload)
					{
						m_syncMutex->lock();

						for (int i=0;i<m_opQueue.getCount();i++)
						{
							ResourceOperation* other = m_opQueue.GetElement(i);
							if (other && other->getResource() == op->getResource() && other->getType() == ResourceOperation::RESOP_Load)
							{
								m_opQueue.SetElement(i,0);
								passed = true;
								break;
							}
						}

						m_syncMutex->unlock();
					}
				}
				return passed;
			}
			void AsyncProcessor::AddTask(ResourceOperation* op)
			{
				
				//if (!ignored)
				{
					m_syncMutex->lock();

					m_opQueue.Enqueue(op);

					m_syncMutex->unlock();
				}
			}
			void AsyncProcessor::RemoveTask(ResourceOperation* op)
			{
				m_syncMutex->lock();

				m_opQueue.Replace(op, 0);

				m_syncMutex->unlock();
			}
			bool AsyncProcessor::TaskCompleted()
			{
				bool result;
				m_syncMutex->lock();
				result = m_opQueue.getCount() == 0;
				m_syncMutex->unlock();
				return result;
			}
			int AsyncProcessor::GetOperationCount()
			{
				int result;
				m_syncMutex->lock();
				result = m_opQueue.getCount();
				m_syncMutex->unlock();
				return result;
			}
			void AsyncProcessor::WaitForCompletion()
			{
				while (!TaskCompleted())
				{
					ApocSleep(10);
				}
			}
			void AsyncProcessor::Shutdown()
			{
				m_closed = true;
				m_genTable->ShutDown();
				m_processThread->join();
				
			}

			AsyncProcessor::AsyncProcessor(GenerationTable* gtable, const String& name)
				: m_closed(false), m_genTable(gtable)
			{
				m_syncMutex = new tthread::mutex();
				m_processThread = new tthread::thread(&AsyncProcessor::ThreadEntry, this);
				SetThreadName(m_processThread, name);
			}

			AsyncProcessor::~AsyncProcessor(void)
			{
				delete m_processThread;
			}
		}
	}
}