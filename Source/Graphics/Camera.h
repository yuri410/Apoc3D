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

#ifndef CAMREA_H
#define CAMERA_H
#pragma once

#include "Common.h"
#include "Math\Frustum.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		/* Represents a view into a 3D scene. 
		*/
		class APAPI Camera
		{
		private:
	
			Matrix m_view;
			Matrix m_invView;
			Matrix m_proj;

			Frustum frustum;
		public:
	
			/* Gets the view transform matrix
			*/
			const Matrix &getViewMatrix() const { return m_view; }
			/* Gets the projection matrix
			*/
			const Matrix &getProjMatrix() const { return m_proj; }

			/* Gets the up vector of the camera view
			*/
			Vector3 getUp() const { return Vector3Utils::LDVector(m_view.M21, m_view.M22, m_view.M23); }
			/* Gets the right vector of the camera view
			*/
			Vector3 getRight() const { return Vector3Utils::LDVector(m_view.M11, m_view.M12, m_view.M13); }
			/* Gets the forward vector of the camera view
			*/
			Vector3 getForward() const { return Vector3Utils::LDVector(m_view.M31, m_view.M32, m_view.M33); }

			/* Sets the view transform matrix
			*/
			void setViewMatrix(const Matrix &value) { m_view = value; }
			/* Sets the projection transform matrix
			*/
			void setProjMatrix(const Matrix &value) { m_proj = value; }

			/* Update the camera's state. 
			*/
			virtual void Update(const GameTime* const time) 
			{
				Matrix::Inverse(m_invView, m_view);				
			}


			Camera(void)
			{
				m_view.LoadIdentity();
				m_proj.LoadIdentity();
			}
			~Camera(void) {}
		};

	};
};
#endif