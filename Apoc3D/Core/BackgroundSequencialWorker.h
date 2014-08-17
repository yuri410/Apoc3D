#pragma once

#ifndef APOC3D_BACKGROUNDSEQUENCIALWORKER_H
#define APOC3D_BACKGROUNDSEQUENCIALWORKER_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Common.h"
#include "apoc3d/Platform/Thread.h"
#include "apoc3d/Library/tinythread.h"

namespace Apoc3D
{
	namespace Core
	{
		template <typename T>
		class BackgroundSequencialWorker
		{
		protected:
			BackgroundSequencialWorker()
				: m_terminated(true), m_thread(nullptr) { }

			void StartBackground(const String& name)
			{
				StopBackground();

				m_terminated = false;
				m_thread = new std::thread(BackgroundMainStatic, this);
				Apoc3D::Platform::SetThreadName(m_thread, name);
			}
			void StopBackground()
			{
				if (m_thread)
				{
					m_terminated = true;

					m_queueEmptyWait.notify_all();

					if (m_thread->joinable())
						m_thread->join();
					delete m_thread;
					m_thread = nullptr;
				}
			}

			void AddWorkItem(const T& item)
			{
				m_queueMutex.lock();
				m_taskQueue.Enqueue(item);

				m_queueEmptyWait.notify_all();
				m_queueMutex.unlock();
			}

			static void BackgroundMainStatic(void* thisInstance) { ((BackgroundSequencialWorker*)thisInstance)->BackgroundMain(); }
			virtual void BackgroundMain()
			{
				BackgroundMainBegining();

				while (!m_terminated)
				{
					m_queueMutex.lock();
					volatile int32 queueCount = m_taskQueue.getCount();
					if (queueCount == 0)
					{
						m_queueEmptyWait.wait(m_queueMutex);
					}
					m_queueMutex.unlock();

					if (m_terminated)
						break;

					if (queueCount>0)
					{
						m_queueMutex.lock();
						T task = m_taskQueue.Dequeue();
						m_queueMutex.unlock();

						BackgroundMainProcess(task);
					}
				}

				m_terminated = true;

				BackgroundMainEnding();
			}

			virtual void BackgroundMainBegining() { };
			virtual void BackgroundMainEnding() { };

			virtual void BackgroundMainProcess(T& item) = 0;

			tthread::condition_variable m_queueEmptyWait;
			tthread::mutex m_queueMutex;
			Queue<T> m_taskQueue;

			tthread::thread* m_thread;
			volatile bool m_terminated;
		};
	}
}

#endif