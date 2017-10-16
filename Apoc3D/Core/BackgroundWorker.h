#pragma once

#ifndef APOC3D_BACKGROUNDSEQUENCIALWORKER_H
#define APOC3D_BACKGROUNDSEQUENCIALWORKER_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2014-2015 Tao Xin
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

#include "apoc3d/Common.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Platform/Thread.h"
#include "apoc3d/Library/tinythread.h"
#include "apoc3d/Utility/StringUtils.h"

#include <atomic>

namespace Apoc3D
{
	namespace Core
	{
		template <typename T>
		class BackgroundWorker
		{
		public:
			bool WaitUntilClear(int32 timeOut = 0)
			{
				bool allowTimeOut = timeOut > 0;

				for (;;)
				{
					bool isIdle = QueryTaskCount() == 0;

					if (isIdle)
					{
						// check waiting threads
						for (const ThreadData& td : m_threadData)
						{
							if (!td.m_isIdle)
							{
								isIdle = false;
								break;
							}
						}
					}

					if (!isIdle)
					{
						Platform::ApocSleep(1);
						timeOut--;
					}
					else
						break;
					
					if (allowTimeOut && timeOut <= 0)
						return true;
				}

				return false;
			}

			void StartBackground(const String& name, int32 threadCount)
			{
				StopBackground();

				m_terminating = false;
				
				m_threadData.ReserveDiscard(threadCount);
				for (ThreadData& td : m_threadData)
				{
					td.m_owner = this;
					td.m_isRunning = false;
					td.m_isIdle = false;
				}

				for (int32 i = 0; i < threadCount; i++)
				{
					tthread::thread* th = new tthread::thread(BackgroundMainStatic, &m_threadData[i]);
					Apoc3D::Platform::SetThreadName(th, threadCount == 1 ? name : (name + L"_" + Apoc3D::Utility::StringUtils::IntToString(i)));
				}
			}
			void StopBackground()
			{
				if (m_threads.getCount())
				{
					m_terminating = true;
					m_queueEmptyWait.notify_all();

					for (tthread::thread* th : m_threads)
					{
						if (th->joinable())
							th->join();
					}

					m_threads.DeleteAndClear();
				}
			}

			void StopBackgroundAsync()
			{
				if (m_threads.getCount())
				{
					m_terminating = true;
					m_queueEmptyWait.notify_all();
				}
			}

			void AddWorkItem(const T& item)
			{
				m_queueMutex.lock();
				m_taskQueue.Enqueue(item);

				m_queueEmptyWait.notify_all();
				m_queueMutex.unlock();
			}

			void AddWorkItem(T&& item)
			{
				m_queueMutex.lock();
				m_taskQueue.Enqueue(std::move(item));

				m_queueEmptyWait.notify_all();
				m_queueMutex.unlock();
			}

			void ClearTasks()
			{
				m_queueMutex.lock();
				m_taskQueue.Clear();
				m_queueMutex.unlock();
			}

			int32 QueryTaskCount()
			{
				int32 r;
				m_queueMutex.lock();
				r = m_taskQueue.getCount();
				m_queueMutex.unlock();
				return r;
			}

			bool IsStopping() const { return m_terminating && IsRunning(); }
			bool IsRunning() const 
			{
				for (const ThreadData& td : m_threadData)
				{
					if (td.m_isRunning)
						return true;
				}
				return false;
			}

		protected:

			BackgroundWorker() { }

			~BackgroundWorker()
			{
				StopBackground();
			}

			virtual void BackgroundMainBegining() { };
			virtual void BackgroundMainEnding() { };

			virtual void BackgroundMainProcess(T& item) = 0;

			void SetAutoStop(bool v) { m_autoStop = v; }

		private:
			struct ThreadData
			{
				BackgroundWorker* m_owner;
				std::atomic<bool> m_isRunning;
				std::atomic<bool> m_isIdle;
			};

			static void BackgroundMainStatic(void* data) 
			{
				ThreadData* td = (ThreadData*)data;
				td->m_isRunning = true;
				td->m_owner->BackgroundMain(td); 
				td->m_isRunning = false;
			}

			void BackgroundMain(ThreadData* td)
			{
				BackgroundMainBegining();

				while (!m_terminating)
				{
					if (m_autoStop && QueryTaskCount() == 0)
					{
						m_terminating = true;
						break;
					}

					m_queueMutex.lock();
					volatile bool hasTask = false;
					T task;
					if (m_taskQueue.getCount() == 0)
					{
						td->m_isIdle = true;
						m_queueEmptyWait.wait(m_queueMutex);
						td->m_isIdle = false;
					}
					else
					{
						task = m_taskQueue.Dequeue();
						hasTask = true;
					}
					m_queueMutex.unlock();

					if (m_terminating)
						break;

					if (hasTask)
					{
						BackgroundMainProcess(task);
					}
				}

				BackgroundMainEnding();
			}

			tthread::condition_variable m_queueEmptyWait;
			tthread::mutex m_queueMutex;
			Apoc3D::Collections::Queue<T> m_taskQueue;

			Apoc3D::Collections::List<tthread::thread*> m_threads;
			Apoc3D::Collections::List<ThreadData> m_threadData;

			std::atomic<bool> m_terminating = true;
			std::atomic<bool> m_autoStop = false;
		};
	}
}

#endif