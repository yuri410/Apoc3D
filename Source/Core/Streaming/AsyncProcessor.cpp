#include "AsyncProcessor.h"
#include "Platform/Thread.h"
#include "GenerationTable.h"

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
				static const int ManageInterval = 50;
				int times = 0;
				while (!m_closed)
				{
					ResourceOperation* resOp = 0;
					m_syncMutex.lock();
					if (m_opQueue.getCount())
					{
						resOp = m_opQueue.Dequeue();
					}

					m_syncMutex.unlock();

					if (resOp)
					{
						resOp->Process();
					}

					m_genTable->SubTask_GenUpdate();
					if ((times++) % ManageInterval == 0)
					{
						m_genTable->SubTask_Manage();
						times = 0;
					}
					ApocSleep(10);
				}
			}

			void AsyncProcessor::AddTask(ResourceOperation* op)
			{
				m_syncMutex.lock();

				m_opQueue.Enqueue(op);

				m_syncMutex.unlock();
			}

			bool AsyncProcessor::TaskCompleted()
			{
				bool result;
				m_syncMutex.lock();
				result = m_opQueue.getCount() == 0;
				m_syncMutex.unlock();
				return true;
			}
			int AsyncProcessor::GetOperationCount()
			{
				int result;
				m_syncMutex.lock();
				result = m_opQueue.getCount();
				m_syncMutex.unlock();
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
				m_processThread = new thread(&AsyncProcessor::ThreadEntry, this);
				SetThreadName(m_processThread, name);
			}

			AsyncProcessor::~AsyncProcessor(void)
			{
				delete m_processThread;
			}
		}
	}
}