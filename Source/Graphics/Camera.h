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
using namespace Apoc3D::Core;

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

			Frustum m_frustum;
		public:
			Frustum& getFrustum() { return m_frustum; }
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
		class APAPI FpsCamera : public Camera
		{
		private:
			float m_aspectRatio;
			float m_velocity;
			Vector3 m_position;

		public:
			FpsCamera(float aspectRatio);
			~FpsCamera(void);

			/* Gets the position of the view point
			*/
			const Vector3 &getPosition() const { return m_position; }
			
			const float getAspectRatio() const { return m_aspectRatio; }
			const float getVelocity() const { return m_velocity; } 

			void MoveForward()
			{
				Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitZ, m_velocity);
				m_position = Vector3Utils::Add(m_position, ofs);
			}
			void MoveBackward()
			{
				Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitZ, m_velocity);
				m_position = Vector3Utils::Subtract(m_position, ofs);				
			}
			void MoveLeft()
			{
				Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitX, m_velocity);
				m_position = Vector3Utils::Add(m_position, ofs);
			}
			void MoveRight() 
			{
				Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitX, m_velocity);
				m_position = Vector3Utils::Subtract(m_position, ofs);
			}
			void MoveUp()
			{
				Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitY, m_velocity);
				m_position = Vector3Utils::Add(m_position, ofs);
			}
			void MoveDown() 
			{
				Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitY, m_velocity);
				m_position = Vector3Utils::Subtract(m_position, ofs);
			}
			void Move(const Vector3 &dir)
			{
				Vector3 ofs = Vector3Utils::Multiply(dir, m_velocity);
				m_position = Vector3Utils::Add(m_position, ofs);
			}
			
			void Update(const GameTime* const time);
		};

		class APAPI ChaseCamera : public Camera
		{
		public:
			ChaseCamera(void);
			~ChaseCamera(void);

			void Reset();
			virtual void Update(const GameTime* const time);

			const Vector3& getChaseUp() const { return m_up; }
			const Vector3& getChaseDirection() const { return m_chaseDirection; }
			const Vector3& getChasePosition() const { return m_chasePosition; }

			const Vector3& getPosition() const { return m_position; }
			const Matrix& getView() const { return m_view; }
			const Matrix& getProjection() const { return m_proj; }
			const Vector3& getVelocity() const { return m_velocity; }
			const Vector3& getDesiredOffset() const { return m_desiredPositionOfs; }
			const Vector3& getLookAtOffset() const { return m_lootAtOfs; }
			const Vector3& getDesiredPosition() const { return m_desiredPosition; }
			const Vector3& getLookAt() const { return m_lootAt; }

			const float getStiffness() const { return m_stiffness; }
			const float getDamping() const { return m_damping; }
			const float getMass() const { return m_mass; }
			const float getNear() const { return m_near; }
			const float getFar() const { return m_far; }
			const float getAspectRatio() const { return m_aspectRatio; }
			const float getFieldOfView() const { return m_fieldOfView; }

			//const Vector3* getChaseUpPtr() const { return& m_up; }
			//const Vector3* getPositionPtr() const { return &m_position; }
			//const Matrix* getViewPtr() const { return &m_view; }
			//const Matrix* getProjectionPtr() const { return &m_proj; }
			//const Vector3* getVelocityPtr() const { return &m_velocity; }

			//const Frustum* getFrustum() const { return &m_frustum; }

			void setStiffness(const float val) { m_stiffness = val; }
			void setDamping(const float val) { m_damping = val; }
			void setMass(const float val) { m_mass = val; }
			void setNear(const float val) { m_near = val; }
			void setFar(const float val) { m_far = val; }
			void setAspectRatio(const float val) { m_aspectRatio = val; }
			void setFieldOfView(const float val) { m_fieldOfView = val; }

			void setChasePosition(const Vector3& val) { m_chasePosition = val; }
			void setChaseDirection(const Vector3& val) { m_chaseDirection = val; }
			void setVelocity(const Vector3& val) { m_velocity = val; }
			void setChaseUp(const Vector3& val) { m_up = val; }
			void setPosition(const Vector3& val) { m_position = val; }
			void setDesiredOffset(const Vector3& val) { m_desiredPositionOfs = val; }
			void setLookAtOffset(const Vector3& val) { m_lootAtOfs = val; }
		private:
			// Matrix properties
			Matrix m_view;
			Matrix m_proj;

			// Perspective properties
			float m_aspectRatio;
			float m_fieldOfView;
			float m_near;
			float m_far;

			// Current camera properties
			Vector3 m_position;
			Vector3 m_velocity;

			// chased object prop
			Vector3 m_up;
			Vector3 m_chasePosition;
			Vector3 m_chaseDirection;

			// Desired camera positioning
			Vector3 m_desiredPositionOfs;
			Vector3 m_desiredPosition;
			Vector3 m_lootAtOfs;
			Vector3 m_lootAt;

			// Camera physics
			float m_stiffness;
			float m_damping;
			float m_mass;


			void UpdateWorldPositions();
			void UpdateMatrices();
		};
	};
};
#endif