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
#ifndef RENDER_OPERATION_H
#define RENDER_OPERATION_H
#pragma once

#include "Common.h"
#include "Math\Matrix.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		/*
		  Represents an operation to render a mesh part in the scene.
		  RenderOperation is used by the engine to manage the scene rendering pipeline.
		*/
		class _Export RenderOperation
		{
		private:
			GeometryData* m_data;
			Material* m_mtrl;
			Matrix m_transform;
		public:

			Material* getMaterial() const { return m_mtrl; }
			void setMaterial(Material* mtrl) { m_mtrl = mtrl; }

			Matrix& getTransform() { return m_transform; }
			void setTransform(const Matrix& value) { m_transform = value; }

			GeometryData* getGeomentryData() const { return m_data; }
			void setGeomentryData(GeometryData* data) { m_data = data; }

			RenderOperation(void) { }
			~RenderOperation(void) { }
		};
	};
};
#endif