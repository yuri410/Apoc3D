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

namespace Apoc3D
{
	namespace Core
	{
		template <typename T>
		class BackgroundSequencialWorker
		{
		public:
			int32 WaitUntilClear(int32 timeOut = 0)
			{
				bool allowTimeOut = timeOut > 0;

				int32 count;
				do 
				{
					m_queueMutex.lock();
					count = m_taskQueue.getCount();
					m_queueMutex.unlock();

					if (count == 0)
						break;

					Platform::ApocSleep(1);
					timeOut--;

				} while (count > 0 && (!allowTimeOut || timeOut > 0));
				return count;
			}

			void StartBackground(const String& name)
			{
				StopBackground();

				m_terminating = false;
				m_thread = new tthread::thread(BackgroundMainStatic, this);
				Apoc3D::Platform::SetThreadName(m_thread, name);
			}
			void StopBackground()
			{
				if (m_thread)
				{
					m_terminating = true;

					m_queueEmptyWait.notify_all();

					if (m_thread->joinable())
						m_thread->join();
					delete m_thread;
					m_thread = nullptr;
				}
			}

			void StopBackgroundAsync()
			{
				if (m_thread)
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

			int32 QueryTaskCount()
			{
				int32 r;
				m_queueMutex.lock();
				r = m_taskQueue.getCount();
				m_queueMutex.unlock();
				return r;
			}

			bool IsRunning() const { return m_running; }
			bool IsStopping() const { return m_running && m_terminating; }
		protected:
			BackgroundSequencialWorker() { }

			~BackgroundSequencialWorker()
			{
				StopBackground();
			}
			
			static void BackgroundMainStatic(void* thisInstance) { ((BackgroundSequencialWorker*)thisInstance)->BackgroundMain(); }
			virtual void BackgroundMain()
			{
				m_running = true;
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
						m_queueEmptyWait.wait(m_queueMutex);
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
				m_running = false;
			}

			virtual void BackgroundMainBegining() { };
			virtual void BackgroundMainEnding() { };

			virtual void BackgroundMainProcess(T& item) = 0;

			static void AsyncCleanupMainStatic(void* thisInstance) { }

			tthread::condition_variable m_queueEmptyWait;
			tthread::mutex m_queueMutex;
			Apoc3D::Collections::Queue<T> m_taskQueue;

			tthread::thread* m_thread = nullptr;
			volatile bool m_terminating = true;
			volatile bool m_running = false;
			volatile bool m_autoStop = false;
		};
	}
}

#endif