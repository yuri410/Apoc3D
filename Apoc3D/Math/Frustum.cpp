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

#include "Frustum.h"

namespace Apoc3D
{
	namespace Math
	{
		Frustum::Frustum()
		{
			for (int i = 0; i < ClipPlaneCount; i++)
			{
				m_planes[i].X = m_planes[i].Y = m_planes[i].Z = m_planes[i].D = 0;
			}
		}


		Frustum::~Frustum()
		{
		}

		void Frustum::Update(const Matrix& view, const Matrix& proj)
		{
			Matrix result;			
			Matrix::Multiply(result, view, proj);

			Update(result);
		}
		void Frustum::Update(const Matrix& mvp)
		{
			//m_planes[FPL_Left].X = mvp.M41 - mvp.M11;
			//m_planes[FPL_Left].Y = mvp.M42 - mvp.M12;
			//m_planes[FPL_Left].Z = mvp.M43 - mvp.M13;
			//m_planes[FPL_Left].D = mvp.M44 - mvp.M14;
			//m_planes[FPL_Left].NormalizeInPlace();

			//m_planes[FPL_Right].X = mvp.M41 + mvp.M11;
			//m_planes[FPL_Right].Y = mvp.M42 + mvp.M12;
			//m_planes[FPL_Right].Z = mvp.M43 + mvp.M13;
			//m_planes[FPL_Right].D = mvp.M44 + mvp.M14;
			//m_planes[FPL_Right].NormalizeInPlace();

			//m_planes[FPL_Top].X = mvp.M41 - mvp.M21;
			//m_planes[FPL_Top].Y = mvp.M42 - mvp.M22;
			//m_planes[FPL_Top].Z = mvp.M43 - mvp.M23;
			//m_planes[FPL_Top].D = mvp.M44 - mvp.M24;
			//m_planes[FPL_Top].NormalizeInPlace();

			//m_planes[FPL_Bottom].X = mvp.M41 + mvp.M21;
			//m_planes[FPL_Bottom].Y = mvp.M42 + mvp.M22;
			//m_planes[FPL_Bottom].Z = mvp.M43 + mvp.M23;
			//m_planes[FPL_Bottom].D = mvp.M44 + mvp.M24;
			//m_planes[FPL_Bottom].NormalizeInPlace();

			//m_planes[FPL_Far].X = mvp.M41 - mvp.M31;
			//m_planes[FPL_Far].Y = mvp.M42 - mvp.M32;
			//m_planes[FPL_Far].Z = mvp.M43 - mvp.M33;
			//m_planes[FPL_Far].D = mvp.M44 - mvp.M34;
			//m_planes[FPL_Far].NormalizeInPlace();

			//m_planes[FPL_Near].X = mvp.M41 + mvp.M31;
			//m_planes[FPL_Near].Y = mvp.M42 + mvp.M32;
			//m_planes[FPL_Near].Z = mvp.M43 + mvp.M33;
			//m_planes[FPL_Near].D = mvp.M44 + mvp.M34;
			//m_planes[FPL_Near].NormalizeInPlace();

			m_planes[FPL_Right].X = mvp.M14 - mvp.M11;
			m_planes[FPL_Right].Y = mvp.M24 - mvp.M21;
			m_planes[FPL_Right].Z = mvp.M34 - mvp.M31;
			m_planes[FPL_Right].D = mvp.M44 - mvp.M41;
			m_planes[FPL_Right].NormalizeInPlace();

			m_planes[FPL_Left].X = mvp.M14 + mvp.M11;
			m_planes[FPL_Left].Y = mvp.M24 + mvp.M21;
			m_planes[FPL_Left].Z = mvp.M34 + mvp.M31;
			m_planes[FPL_Left].D = mvp.M44 + mvp.M41;
			m_planes[FPL_Left].NormalizeInPlace();			

			m_planes[FPL_Bottom].X = mvp.M14 + mvp.M12;
			m_planes[FPL_Bottom].Y = mvp.M24 + mvp.M22;
			m_planes[FPL_Bottom].Z = mvp.M34 + mvp.M32;
			m_planes[FPL_Bottom].D = mvp.M44 + mvp.M42;
			m_planes[FPL_Bottom].NormalizeInPlace();			

			m_planes[FPL_Top].X = mvp.M14 - mvp.M12;
			m_planes[FPL_Top].Y = mvp.M24 - mvp.M22;
			m_planes[FPL_Top].Z = mvp.M34 - mvp.M32;
			m_planes[FPL_Top].D = mvp.M44 - mvp.M42;
			m_planes[FPL_Top].NormalizeInPlace();            

			m_planes[FPL_Far].X = mvp.M14 - mvp.M13;
			m_planes[FPL_Far].Y = mvp.M24 - mvp.M23;
			m_planes[FPL_Far].Z = mvp.M34 - mvp.M33;
			m_planes[FPL_Far].D = mvp.M44 - mvp.M43;
			m_planes[FPL_Far].NormalizeInPlace();            

			m_planes[FPL_Near].X = mvp.M14 + mvp.M13;
			m_planes[FPL_Near].Y = mvp.M24 + mvp.M23;
			m_planes[FPL_Near].Z = mvp.M34 + mvp.M33;
			m_planes[FPL_Near].D = mvp.M44 + mvp.M43;
            m_planes[FPL_Near].NormalizeInPlace();
		}

		bool Frustum::Intersects(const BoundingSphere& sp) const
		{
			for (int i = 0; i < ClipPlaneCount; i++)
			{
				float d = m_planes[i].Dot3(sp.Center);
				if (d <= -sp.Radius)
				{
					return false;
				}
			}
			return true;
		}
	};
};