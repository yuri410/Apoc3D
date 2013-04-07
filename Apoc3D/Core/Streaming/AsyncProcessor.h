#pragma once

#ifndef APOC3D_ASYNCPROCESSOR_H
#define APOC3D_ASYNCPROCESSOR_H

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
#include "apoc3d/Collections/Queue.h"

using namespace Apoc3D::Collections;

namespace tthread
{
	class thread;
	class mutex;
}

namespace Apoc3D
{
	namespace Core
	{
		namespace Streaming
		{
			/**
			 *  Represents an operation that processes resources.
			 */
			class APAPI ResourceOperation
			{
			private:
				Resource* m_resource;

			protected:
				ResourceOperation(Resource* res)
					: m_resource(res){ }

			
			public:
				enum OperationType
				{
					RESOP_Load,
					RESOP_Unload,
					RESOP_Other
				};

				/**
				 *  Do the processing.
				 */
				virtual void Process() = 0;

				virtual OperationType getType() const = 0;
				Resource* getResource() const { return m_resource; }
			};

			/**
			 *  This is used to initialize a thread for resource collection and processing.
			 *  At the same time it keeps track of current ResourceOperations and process queued ones in background .
			 */
			class APAPI AsyncProcessor
			{
			public:
				/**
				 *  If a resource is IsIndependent(), this cancels(or removes) the corresponding opposite resource operation
				 *  from the queue.
				 */
				bool NeutralizeTask(ResourceOperation* op);

				/**
				 *  Adds a ResourceOperation object to the queue.
				 */
				void AddTask(ResourceOperation* op);
				/**
				 *  Removes a ResourceOperation object from the queue.
				 */
				void RemoveTask(ResourceOperation* op);

				/**
				 *  Check if there is no queued ResourceOperations at the moment.
				 */
				bool TaskCompleted();
				/**
				 *  Gets the current number of queued ResourceOperations.
				 */
				int GetOperationCount();

				/**
				 *  Suspends the caller's thread until all queued ResourceOperations are processed.
				 */
				void WaitForCompletion();
				
				/**
				 *  Shuts down the AsyncProcessor. Terminating the background thread.
				 */
				void Shutdown();

				AsyncProcessor(GenerationTable* gTable,const String& name);
				~AsyncProcessor(void);

			private:
				Queue<ResourceOperation*> m_opQueue;
				GenerationTable* m_genTable;
				tthread::thread* m_processThread;
				tthread::mutex* m_syncMutex;

				bool m_closed;

				static void ThreadEntry(void* arg);
				void Main();

			};
		}
	}
}
#endif