#pragma once
#ifndef APOC3D_RENDEROPERATIONTABLE_H
#define APOC3D_RENDEROPERATIONTABLE_H

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

#include "apoc3d/Collections/List.h"

#include "RenderOperation.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Graphics
	{
		/**
		 *   A buffer used to store a list of RenderOperation.
		 *   When generated by Renderable SceneObjects, RenderOperation 
		 *   should be add into this buffer before passing to the rendering engine.
		 */
		class APAPI RenderOperationBuffer
		{
		private:
			FastList<RenderOperation> m_oplist;
		public:
			RenderOperationBuffer(void)
			{

			}
			~RenderOperationBuffer(void)
			{

			}
			/**
			 *  Adds a new RenderOperation to the buffer
			 */
			void Add(const RenderOperation& op)
			{
				m_oplist.Add(op);
			}
			/**
			 *  Adds some new RenderOperation to the buffer
			 */
			void Add(const RenderOperation* op, int count)
			{
				for (int i=0;i<count;i++)
				{
					m_oplist.Add(*(op+i));
				}
			}

			void Add(const RenderOperation* op, int count, const Matrix& transform);

			void Clear()
			{
				m_oplist.Clear();
			}

			
			void FastClear()
			{
				m_oplist.Clear();
			}
			void ReserveDiscard(int count)
			{
				m_oplist.ResizeDiscard(count);
			}
			const RenderOperation& get(int i) const
			{
				assert(i>=0 && i<getCount());
				return m_oplist[i];
			}
			RenderOperation& operator[](int i)
			{
				assert(i>=0 && i<getCount());
				return m_oplist[i];
			}
			//void MultiplyTransform(const Matrix& m);

			int getCount() const { return m_oplist.getCount(); }
		};

	}
}
#endif