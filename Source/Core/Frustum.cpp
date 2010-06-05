/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/


#include "Frustum.h"

namespace Apoc3D
{
	namespace Core
	{
		Frustum::Frustum(void)
		{
			for (int i=0;i<ClipPlaneCount;i++)
			{
				m_planes[i].a = m_planes[i].b = m_planes[i].c = m_planes[i].d = 0;
			}
		}


		Frustum::~Frustum(void)
		{
		}

		void Frustum::Update(const Matrix& view, const Matrix& proj)
		{
			Matrix result;
			D3DXMatrixMultiply(&result, &view, &proj);

			Update(result);
		}
		void Frustum::Update(const Matrix& mvp)
		{
			m_planes[FPL_Right].a = mvp._14 - mvp._11;
			m_planes[FPL_Right].b = mvp._24 - mvp._21;
			m_planes[FPL_Right].c = mvp._34 - mvp._31;
            m_planes[FPL_Right].d = mvp._44 - mvp._41;
			D3DXPlaneNormalize(&m_planes[FPL_Right], &m_planes[FPL_Right]);
            
			m_planes[FPL_Left].a = mvp._14 + mvp._11;
			m_planes[FPL_Left].b = mvp._24 + mvp._21;
			m_planes[FPL_Left].c = mvp._34 + mvp._31;
            m_planes[FPL_Left].d = mvp._44 + mvp._41;
			D3DXPlaneNormalize(&m_planes[FPL_Left], &m_planes[FPL_Left]);
            
			m_planes[FPL_Bottom].a = mvp._14 + mvp._12;
			m_planes[FPL_Bottom].b = mvp._24 + mvp._22;
			m_planes[FPL_Bottom].c = mvp._34 + mvp._32;
            m_planes[FPL_Bottom].d = mvp._44 + mvp._42;
			D3DXPlaneNormalize(&m_planes[FPL_Bottom], &m_planes[FPL_Bottom]);
            
			m_planes[FPL_Top].a = mvp._14 - mvp._12;
			m_planes[FPL_Top].b = mvp._24 - mvp._22;
			m_planes[FPL_Top].c = mvp._34 - mvp._32;
            m_planes[FPL_Top].d = mvp._44 - mvp._42;
            D3DXPlaneNormalize(&m_planes[FPL_Top], &m_planes[FPL_Top]);

			m_planes[FPL_Far].a = mvp._14 - mvp._13;
			m_planes[FPL_Far].b = mvp._24 - mvp._23;
			m_planes[FPL_Far].c = mvp._34 - mvp._33;
            m_planes[FPL_Far].d = mvp._44 - mvp._43;
            D3DXPlaneNormalize(&m_planes[FPL_Far], &m_planes[FPL_Far]);
           
			m_planes[FPL_Near].a = mvp._14 + mvp._13;
			m_planes[FPL_Near].b = mvp._24 + mvp._23;
			m_planes[FPL_Near].c = mvp._34 + mvp._33;
            m_planes[FPL_Near].d = mvp._44 + mvp._43;
            D3DXPlaneNormalize(&m_planes[FPL_Near], &m_planes[FPL_Near]);
		}

	};
};