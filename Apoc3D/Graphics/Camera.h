#pragma once
#ifndef APOC3D_CAMERA_H
#define APOC3D_CAMERA_H

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

#include "apoc3d/Math/Frustum.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		/**
		 *  Represents a view into a 3D scene. 
		 */
		class APAPI Camera
		{
		public:
			Frustum& getFrustum() { return m_frustum; }
			/**
			 *  Gets the view transform matrix
			 */
			const Matrix &getViewMatrix() const { return m_view; }
			/** 
			 *  Gets the projection matrix
			 */
			const Matrix &getProjMatrix() const { return m_proj; }

			const Matrix& getInvViewMatrix() const { return m_invView; }

			const Matrix& getViewProjMatrix() const { return m_viewProj; }

			/**
			 *  Gets the up vector of the camera view
			 */
			Vector3 getUp() const { return Vector3(m_invView.M21, m_invView.M22, m_invView.M23); }
			/**
			 *   Gets the right vector of the camera view
			 */
			Vector3 getRight() const { return Vector3(m_invView.M11, m_view.M12, m_invView.M13); }
			/**
			 *   Gets the forward vector of the camera view
			 */
			Vector3 getForward() const { return Vector3(m_invView.M31, m_invView.M32, m_invView.M33); }

			/**
			 *  Sets the view transform matrix
			 */
			void setViewMatrix(const Matrix &value) { m_view = value; }
			/**
			 *  Sets the projection transform matrix
			 */
			void setProjMatrix(const Matrix &value) { m_proj = value; }

			/**
			 *  Update the camera's state. 
			 */
			virtual void Update(const GameTime* const time) 
			{
				CalculateMatrices();
			}

			Camera(void)
			{
				m_view.LoadIdentity();
				m_proj.LoadIdentity();
				m_viewProj.LoadIdentity();
			}
			~Camera(void) {}

		protected:

			void CalculateMatrices()
			{
				Matrix::Inverse(m_invView, m_view);
				Matrix::Multiply(m_viewProj, m_view, m_proj);
			}

			Matrix m_view;
			Matrix m_invView;
			Matrix m_proj;
			Matrix m_viewProj;

			Frustum m_frustum;
		};
		class APAPI FpsCamera : public Camera
		{
		public:
			FpsCamera(float aspectRatio);
			~FpsCamera(void);

			/**
			 *  Gets the position of the view point
			 */
			const Vector3& getPosition() const { return m_position; }
			
			float getAspectRatio() const { return m_aspectRatio; }
			void setAspectRatio(float ar) { m_aspectRatio = ar; }
			//const float getVelocity() const { return m_velocity; } 

			void MoveForward()
			{
				Vector3 dir = m_invView.GetZ();
				dir.NormalizeInPlace();
				Move(dir);
				//Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitZ, m_velocity);
				//m_velChange = Vector3Utils::Add(m_velChange, Vector3Utils::UnitZ);
			}
			void MoveBackward()
			{
				//Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitZ, m_velocity);
				//m_velChange = Vector3Utils::Subtract(m_velChange, Vector3Utils::UnitZ);				
				Vector3 dir = -m_invView.GetZ();
				dir.NormalizeInPlace();
				Move(dir);
			}
			void MoveLeft()
			{
				Vector3 dir = -m_invView.GetX();
				dir.NormalizeInPlace();
				Move(dir);

				//Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitX, m_velocity);
				//m_velChange = Vector3Utils::Subtract(m_velChange, Vector3Utils::UnitX);
			}
			void MoveRight() 
			{
				Vector3 dir = m_invView.GetX();
				dir.NormalizeInPlace();
				Move(dir);
				//Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitX, m_velocity);
				//m_velChange = Vector3Utils::Add(m_velChange, Vector3Utils::UnitX);
			}
			void MoveUp()
			{
				Vector3 dir = m_invView.GetY();
				dir.NormalizeInPlace();
				Move(dir);
				//Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitY, m_velocity);
				//m_velChange = Vector3Utils::Add(m_velChange, Vector3Utils::UnitY);
			}
			void MoveDown() 
			{
				Vector3 dir = -m_invView.GetY();
				dir.NormalizeInPlace();
				Move(dir);
				//Vector3 ofs = Vector3Utils::Multiply(Vector3Utils::UnitY, m_velocity);
				//m_velChange = Vector3Utils::Subtract(m_velChange, Vector3Utils::UnitY);
			}
			void Move(const Vector3 &dir)
			{
				//Vector3 ofs = Vector3Utils::Multiply(dir, m_velocity);
				m_velChange += dir;
			}
			
			void Turn(float dx, float dy)
			{
				m_rotX += ToRadian(dx);
				m_rotY += ToRadian(dy);

				if (m_rotY<ToRadian(-89))
					m_rotY = ToRadian(-89);
				else if (m_rotY>ToRadian(89))
					m_rotY = ToRadian(89);
			}

			void Update(const GameTime* const time);
			void UpdateTransform();

			void setPosition(const Vector3& p)
			{
				m_position = p;
			}
		protected:
			Vector3 m_position;
			float m_maxVelocity;
			Vector3 m_velocity;
			Vector3 m_velChange;

		private:
			float m_aspectRatio;
			
			
			float m_fieldOfView;
			float m_near;
			float m_far;

			float m_rotX;
			float m_rotY;

		};

		/**
		 *  Implements a 3rd person chase camera with inertia
		 */
		class APAPI ChaseCamera : public Camera
		{
		public:
			ChaseCamera(float fov = ToRadian(45));
			~ChaseCamera(void);

			/**
			 *  Resets the camera to its desired position
			 */
			void Reset();
			/**
			 *  Update the camera's motion simulation and the view
			 */
			virtual void Update(const GameTime* const time);

			void ForceUpdateMatrix() { UpdateMatrices(); }

			/**
			 *  A unit vector showing the up direction of the camera
			 *  (0,1,0) will have no rolling
			 */
			const Vector3& getChaseUp() const { return m_up; }
			/**
			 *  A unit vector representing the view direction to the chase target
			 */
			const Vector3& getChaseDirection() const { return m_chaseDirection; }
			/**
			 *  The position of the chase target
			 */
			const Vector3& getChasePosition() const { return m_chasePosition; }

			/**
			 *  Gets the camera's current eye position in world space
			 */
			const Vector3& getPosition() const { return m_position; }
			/**
			 *  Gets the camera's current linear velocity in world space
			 */
			const Vector3& getVelocity() const { return m_velocity; }
			/**
			 *  Gets the offset from the target to the camera for the camera to move to.
			 *  This is relative to the chase direction.
			 */
			const Vector3& getDesiredOffset() const { return m_desiredPositionOfs; }
			/**
			 *  Gets the offset from the target to the look at position, in world space
			 */
			const Vector3& getLookAtOffset() const { return m_lootAtOfs; }
			/**
			 *  Get the current camera's position desired to be moving to.
			 */
			const Vector3& getDesiredPosition() const { return m_desiredPosition; }
			/**
			 *  Gets the current camera's look at position in world space
			 */
			const Vector3& getLookAt() const { return m_lootAt; }

			/**
			 *  How rigid the movement is.
			 */
			float getStiffness() const { return m_stiffness; }
			/**
			 *  The friction applied to movement using the damping model: f = vel*damp
			 */
			float getDamping() const { return m_damping; }
			/**
			 *  The mass of the camera
			 */
			float getMass() const { return m_mass; }
			/**
			 *  The near plane of the frustum
			 */
			float getNear() const { return m_near; }
			/**
			 *  The far plane of the frustum
			 */
			float getFar() const { return m_far; }
			/**
			 *  The aspect ratio of the projection
			 */
			float getAspectRatio() const { return m_aspectRatio; }
			/**
			 *  The y direction fov angle in radians
			 */
			float getFieldOfView() const { return m_fieldOfView; }


			void setStiffness(float val) { m_stiffness = val; }
			void setDamping(float val) { m_damping = val; }
			void setMass(float val) { m_mass = val; }
			void setNear(float val) { m_near = val; }
			void setFar(float val) { m_far = val; }
			void setAspectRatio(float val) { m_aspectRatio = val; }
			void setFieldOfView(float val) { m_fieldOfView = val; }

			void setChasePosition(const Vector3& val) { m_chasePosition = val; }
			void setChaseDirection(const Vector3& val) { m_chaseDirection = val; }
			void setVelocity(const Vector3& val) { m_velocity = val; }
			void setChaseUp(const Vector3& val) { m_up = val; }
			void setPosition(const Vector3& val) { m_position = val; }
			void setDesiredOffset(const Vector3& val) { m_desiredPositionOfs = val; }
			void setLookAtOffset(const Vector3& val) { m_lootAtOfs = val; }

		private:
			// Matrix properties
			//Matrix m_view;
			//Matrix m_proj;

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