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

#include "Camera.h"
#include "apoc3d/Core/AppTime.h"


namespace Apoc3D
{
	namespace Graphics
	{
		Camera::Camera()
		{
			m_view.LoadIdentity();
			m_invView.LoadIdentity();
			m_proj.LoadIdentity();
			m_viewProj.LoadIdentity();
		}

		Camera::Camera(const Matrix& view, const Matrix& proj)
			: m_view(view), m_proj(proj)
		{
			CalculateMatrices();
		}

		void Camera::CalculateMatrices()
		{
			Matrix::Inverse(m_invView, m_view);
			Matrix::Multiply(m_viewProj, m_view, m_proj);
			m_frustum.Update(m_view, m_proj);
		}

		void Camera::GetCornerRays(RaySegment* topLeft, RaySegment* topRight, RaySegment* bottomLeft, RaySegment* bottomRight)
		{
			Matrix matrix = m_viewProj;
			matrix.Inverse();

			if (topLeft)
			{
				topLeft->Start = Vector3::TransformCoordinate(Vector3(-1, 1, 0), matrix);
				topLeft->End = Vector3::TransformCoordinate(Vector3(-1, 1, 1), matrix);
			}

			if (topRight)
			{
				topRight->Start = Vector3::TransformCoordinate(Vector3(1, 1, 0), matrix);
				topRight->End = Vector3::TransformCoordinate(Vector3(1, 1, 1), matrix);
			}
			
			if (bottomLeft)
			{
				bottomLeft->Start = Vector3::TransformCoordinate(Vector3(-1, -1, 0), matrix);
				bottomLeft->End = Vector3::TransformCoordinate(Vector3(-1, -1, 1), matrix);
			}
			
			if (bottomRight)
			{
				bottomRight->Start = Vector3::TransformCoordinate(Vector3(1, -1, 0), matrix);
				bottomRight->End = Vector3::TransformCoordinate(Vector3(1, -1, 1), matrix);
			}
		}
		void Camera::GetEdgeRays(RaySegment* left, RaySegment* right, RaySegment* top, RaySegment* bottom)
		{
			Matrix matrix = m_viewProj;
			matrix.Inverse();

			if (left)
			{
				left->Start = Vector3::TransformCoordinate(Vector3(-1, 0, 0), matrix);
				left->End = Vector3::TransformCoordinate(Vector3(-1, 0, 1), matrix);
			}
			
			if (right)
			{
				right->Start = Vector3::TransformCoordinate(Vector3(1, 0, 0), matrix);
				right->End = Vector3::TransformCoordinate(Vector3(1, 0, 1), matrix);
			}

			if (top)
			{
				top->Start = Vector3::TransformCoordinate(Vector3(0, 1, 0), matrix);
				top->End = Vector3::TransformCoordinate(Vector3(0, 1, 1), matrix);
			}
			
			if (bottom)
			{
				bottom->Start = Vector3::TransformCoordinate(Vector3(0, -1, 0), matrix);
				bottom->End = Vector3::TransformCoordinate(Vector3(0, -1, 1), matrix);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		FreeCamera::FreeCamera(float aspectRatio, bool rightHand)
			: m_aspectRatio(aspectRatio), 
			m_fieldOfView(ToRadian(50)), m_near(0.5f), m_far(1500), m_rightHand(rightHand)
		{
			
		}


		FreeCamera::~FreeCamera()
		{
		}

		void FreeCamera::Update(const AppTime* time)
		{
			float dt = time->ElapsedTime;

			m_velocity += m_velChange * (Acceleration + Deacceleration) * dt;
			m_velChange = Vector3::Zero;
			
			{
				float vLen = m_velocity.Length();
				if (vLen > 0.01f)
				{
					m_velocity.NormalizeInPlace();

					vLen -= dt * Deacceleration;

					if (vLen > MaxVelocity)
						vLen = MaxVelocity;

					if (vLen < 0)
						vLen = 0;

					m_velocity *= vLen;
				}
				else
				{
					m_velocity = Vector3::Zero;
				}
			}

			Vector3 dp = m_velocity * dt;
			m_position += dp;

			UpdateTransform();
		}
		void FreeCamera::UpdateTransform()
		{
			Vector3 dir = Vector3::UnitZ;

			Matrix oriX;
			Matrix::CreateRotationX(oriX, m_rotY);
			Matrix oriY;
			Matrix::CreateRotationY(oriY, m_rotX);
			Matrix temp;
			Matrix::Multiply(temp, oriX, oriY);

			dir = Vector3::TransformNormal(dir, temp);

			Vector3 at = m_position + dir;

			if (m_rightHand)
			{
				Matrix::CreateLookAtRH(m_view, m_position, at, Vector3::UnitY);
				Matrix::CreatePerspectiveFovRH(m_proj, m_fieldOfView, m_aspectRatio, m_near, m_far);
			}
			else
			{
				Matrix::CreateLookAtLH(m_view, m_position, at, Vector3::UnitY);
				Matrix::CreatePerspectiveFovLH(m_proj, m_fieldOfView, m_aspectRatio, m_near, m_far);
			}
			
			CalculateMatrices();
		}

		void FreeCamera::MoveForward()
		{
			Vector3 dir = m_invView.GetZ();
			dir.NormalizeInPlace();
			Move(dir);
		}
		void FreeCamera::MoveBackward()
		{
			Vector3 dir = -m_invView.GetZ();
			dir.NormalizeInPlace();
			Move(dir);
		}
		void FreeCamera::MoveLeft()
		{
			Vector3 dir = -m_invView.GetX();
			dir.NormalizeInPlace();
			Move(dir);
		}
		void FreeCamera::MoveRight()
		{
			Vector3 dir = m_invView.GetX();
			dir.NormalizeInPlace();
			Move(dir);
		}
		void FreeCamera::MoveUp()
		{
			Vector3 dir = m_invView.GetY();
			dir.NormalizeInPlace();
			Move(dir);
		}
		void FreeCamera::MoveDown()
		{
			Vector3 dir = -m_invView.GetY();
			dir.NormalizeInPlace();
			Move(dir);
		}
		void FreeCamera::Move(const Vector3 &dir)
		{
			m_velChange += dir;
		}

		void FreeCamera::Turn(float dx, float dy)
		{
			m_rotX += ToRadian(dx);
			m_rotY += ToRadian(dy);

			if (m_rotY < ToRadian(-89))
				m_rotY = ToRadian(-89);
			else if (m_rotY > ToRadian(89))
				m_rotY = ToRadian(89);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

		ChaseCamera::ChaseCamera(float fov)
			: m_velocity(Vector3::Zero), m_position(Vector3::Zero), m_lootAt(Vector3::Zero), m_desiredPosition(Vector3::Zero),
			m_up(Vector3::UnitY), m_chasePosition(Vector3::Zero), m_chaseDirection(Vector3::Zero),
			m_stiffness(1800), m_damping(600), m_mass(50), 
			m_fieldOfView(fov), m_aspectRatio(1),
			m_near(1), m_far(1000)
		{
			m_lootAtOfs = Vector3(0, 3.0f, 0);
			m_desiredPositionOfs = Vector3(0, 0, 2);

			//m_frustum = Frustum(&m_proj);
		}

		ChaseCamera::~ChaseCamera()
		{
		}

		void ChaseCamera::Reset()
		{
			UpdateWorldPositions();

			m_velocity = Vector3::Zero;
			m_position = m_desiredPosition;

			UpdateMatrices();
		}

		void ChaseCamera::UpdateMatrices()
		{
			Matrix::CreateLookAtLH(m_view, m_position, m_lootAt, m_up);
			Matrix::CreatePerspectiveFovLH(m_proj, m_fieldOfView, m_aspectRatio, m_near, m_far);
			
			CalculateMatrices();
		}

		void ChaseCamera::UpdateWorldPositions()
		{
			// Construct a matrix to transform from the chase space to world space
			// chase space is the space made up of 3 basis: chase direction , up, and their cross product 
			// this is required as the provided position offset is in this space
			
			Matrix trans;
			trans.LoadIdentity();

			trans.M31 = -m_chaseDirection.X;;
			trans.M32 = -m_chaseDirection.Y;
			trans.M33 = -m_chaseDirection.Z;

			trans.M21 = m_up.X;
			trans.M22 = m_up.Y;
			trans.M23 = m_up.Z;

			Vector3 right = Vector3::Cross(m_up, m_chaseDirection);
			right.NormalizeInPlace();

			trans.M11 = right.X;
			trans.M12 = right.Y;
			trans.M13 = right.Z;

			Vector3 desiredPositionOfsT;
			Vector3 lookAtOfsT;
			desiredPositionOfsT = Vector3::TransformNormal(m_desiredPositionOfs, trans);
			lookAtOfsT = Vector3::TransformNormal(m_lootAtOfs, trans);

			m_desiredPosition = m_chasePosition + desiredPositionOfsT;
			m_lootAt = m_chasePosition + lookAtOfsT;
		}

		void ChaseCamera::Update(const AppTime* time)
		{
			UpdateWorldPositions();

			Vector3 stretch = m_position - m_desiredPosition;
			Vector3 force = stretch * (-m_stiffness);
			force -= m_velocity * m_damping;

			m_velocity += force * (time->ElapsedTime / m_mass );
			m_position += m_velocity * time->ElapsedTime;

			UpdateMatrices();
		}

	};
};