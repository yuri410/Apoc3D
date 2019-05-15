#pragma once

#ifndef APOC3D_ASYNCPROCESSOR_H
#define APOC3D_ASYNCPROCESSOR_H

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

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Collections/Queue.h"

using namespace Apoc3D::Collections;


namespace Apoc3D
{
	namespace Core
	{
		namespace Streaming
		{
			/**
			 *  Represents an operation that processes resources.
			 */
			struct ResourceOperation
			{
				enum OperationType
				{
					RESOP_Load,
					RESOP_Unload
				} Type = RESOP_Load;

				Resource* Subject = nullptr;

				ResourceOperation() { }
				ResourceOperation(Resource* res, OperationType type)
					: Subject(res), Type(type) { }

				void Invalidate() { Subject = nullptr; }
				bool isValid() const { return Subject != nullptr; }

				bool operator ==(const ResourceOperation& rhs) const { return Type == rhs.Type && Subject == rhs.Subject; }
				bool operator !=(const ResourceOperation& rhs) const { return !this->operator==(rhs); }
			};

			/**
			 *  This is used to initialize a thread for resource collection and processing.
			 *  At the same time it keeps track of current ResourceOperations and process queued ones in background .
			 */
			class APAPI AsyncProcessor
			{
			public:
				AsyncProcessor(GenerationTable* gTable, const String& name, bool isThreaded);
				~AsyncProcessor(void);

				/**
				 *  If a resource is IsIndependent(), this cancels(or removes) the corresponding opposite resource operation
				 *  from the queue.
				 */
				bool NeutralizeTask(const ResourceOperation& op);

				/**
				 *  Adds a ResourceOperation object to the queue.
				 */
				void AddTask(const ResourceOperation& op);
				/**
				 *  Removes a ResourceOperation object from the queue.
				 */
				void RemoveTask(const ResourceOperation& op);

				void RemoveTask(Resource* res);

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


				void ProcessPostSync(float& timeLeft);


			private:
				void LockQueue();
				void UnlockQueue();

				bool ClearMatchingResourceOperation(Resource* res, ResourceOperation::OperationType type);


				static void ThreadEntry(void* arg);
				void Main();

				Queue<ResourceOperation> m_opQueue;
				GenerationTable* m_genTable = nullptr;
				std::thread* m_processThread = nullptr;
				std::mutex* m_queueMutex = nullptr;

				Queue<ResourceOperation> m_postSyncQueue;

				bool m_closed = false;

			};
		}
	}
}
#endif