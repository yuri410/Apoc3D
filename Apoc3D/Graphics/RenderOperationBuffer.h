#pragma once
#ifndef APOC3D_RENDEROPERATIONTABLE_H
#define APOC3D_RENDEROPERATIONTABLE_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#include "apoc3d/Collections/List.h"

#include "RenderOperation.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Graphics
	{
		/**
		 *   A buffer used to store a list of RenderOperation.
		 *   RenderOperations generated by Renderable objects
		 *   should be put into this buffer before passing to the rendering engine.
		 */
		class APAPI RenderOperationBuffer
		{
		public:
			RenderOperationBuffer() { }
			~RenderOperationBuffer() { }

			/** Adds a new RenderOperation to the buffer */
			void Add(const RenderOperation& op) { m_oplist.Add(op); }

			/** Adds some new RenderOperation to the buffer */
			void Add(const RenderOperation* op, int count) { m_oplist.AddArray(op, count); }

			void AddWithParamAndMtrl(const RenderOperation* op, int count, Material* mtrl, void* userPointer);
			void AddWithParamAndMtrl(const RenderOperation* op, int count, const Matrix& transform, Material* mtrl, void* userPointer);
			void AddWithParamAndMtrl(const RenderOperation* op, int count, const Matrix& transform, bool isFinal, Material* mtrl, void* userPointer);
			
			void AddWithParam(const RenderOperation* op, int count, const Matrix& transform);
			void AddWithParam(const RenderOperation* op, int count, const Matrix& transform, bool isFinal);
			void AddWithParam(const RenderOperation* op, int count, const Matrix& transform, bool isFinal, void* userPointer);
			void AddWithParam(const RenderOperation* op, int count, const Matrix& transform, void* userPointer);
			void AddWithParam(const RenderOperation* op, int count, void* userPointer);
			


			void AddBufferWithMtrl(const RenderOperationBuffer* ops, Material* mtrl, void* userPointer)
			{
				AddWithParamAndMtrl(ops->m_oplist.getElements(), ops->getCount(), mtrl, userPointer);
			}
			void AddBufferWithMtrl(const RenderOperationBuffer* ops, const Matrix& transform, Material* mtrl, void* userPointer)
			{
				AddWithParamAndMtrl(ops->m_oplist.getElements(), ops->getCount(), transform, mtrl, userPointer);
			}
			void AddBufferWithMtrl(const RenderOperationBuffer* ops, const Matrix& transform, bool isFinal, Material* mtrl, void* userPointer)
			{
				AddWithParamAndMtrl(ops->m_oplist.getElements(), ops->getCount(), transform, isFinal, mtrl, userPointer);
			}

			void AddBuffer(const RenderOperationBuffer* ops)
			{
				Add(ops->m_oplist.getElements(), ops->getCount());
			}
			void AddBuffer(const RenderOperationBuffer* ops, const Matrix& transform, bool isFinal)
			{
				AddWithParam(ops->m_oplist.getElements(), ops->getCount(), transform, isFinal);
			}
			void AddBuffer(const RenderOperationBuffer* ops, const Matrix& transform)
			{
				AddWithParam(ops->m_oplist.getElements(), ops->getCount(), transform);
			}
			void AddBuffer(const RenderOperationBuffer* ops, const Matrix& transform, void* userPointer)
			{
				AddWithParam(ops->m_oplist.getElements(), ops->getCount(), transform, userPointer);
			}
			void AddBuffer(const RenderOperationBuffer* ops, const Matrix& transform, bool isFinal, void* userPointer)
			{
				AddWithParam(ops->m_oplist.getElements(), ops->getCount(), transform, isFinal, userPointer);
			}
			void AddBuffer(const RenderOperationBuffer* ops, void* userPointer)
			{
				AddWithParam(ops->m_oplist.getElements(), ops->getCount(), userPointer);
			}


			void SetForAllWithMtrl(Material* mtrl, void* userPointer);
			void SetForAllWithMtrl(const Matrix& transform, Material* mtrl, void* userPointer);
			void SetForAllWithMtrl(const Matrix& transform, bool isFinal, Material* mtrl, void* userPointer);

			void SetForAll(const Matrix& transform);
			void SetForAll(const Matrix& transform, bool isFinal);
			void SetForAll(const Matrix& transform, bool isFinal, void* userPointer);
			void SetForAll(const Matrix& transform, void* userPointer);
			void SetForAll(void* userPointer);

			void Clear() { m_oplist.Clear(); }

			void ReserveDiscard(int count) { m_oplist.ResizeDiscard(count); }

			const RenderOperation& get(int i) const { return m_oplist[i]; }
			RenderOperation& operator[](int i) { return m_oplist[i]; }
			
			int getCount() const { return m_oplist.getCount(); }

			const RenderOperation* begin() const { return m_oplist.begin(); }
			const RenderOperation* end() const { return m_oplist.end(); }

			RenderOperation* begin() { return m_oplist.begin(); }
			RenderOperation* end() { return m_oplist.end(); }

		private:
			List<RenderOperation> m_oplist;
		};



	}
}
#endif