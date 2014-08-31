/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "Camera.h"
#include "apoc3d/Core/GameTime.h"


namespace Apoc3D
{
	namespace Graphics
	{
		FpsCamera::FpsCamera(float aspectRatio)
			: m_aspectRatio(aspectRatio), m_maxVelocity(5), m_position(Vector3::Zero),
			m_fieldOfView(ToRadian(50)), m_near(0.5f), m_far(1500), m_velocity(Vector3::Zero), m_velChange(Vector3::Zero),
			m_rotX(0), m_rotY(0)
		{
			
		}


		FpsCamera::~FpsCamera(void)
		{
		}

		void FpsCamera::Update(const GameTime* time)
		{
			m_velocity += m_velChange;
			m_velChange = Vector3::Zero;
			Vector2 hozV(m_velocity.X, m_velocity.Z);

			if (hozV.LengthSquared()>1)
			{
				hozV.NormalizeInPlace();
			}

			{
				float vLen = hozV.Length();

				if (vLen > 0.05f)
				{
					vLen -= time->getElapsedTime() * 1.5f;

					hozV.NormalizeInPlace();
					hozV *= vLen;;
					m_velocity.X = hozV.X;
					m_velocity.Z = hozV.Y;
				}
				else
				{
					m_velocity.X = m_velocity.Z = 0;
				}

			}

			Vector3 dp = m_velocity * (time->getElapsedTime() * m_maxVelocity);
			m_position += dp;


			UpdateTransform();
			Camera::Update(time);
		}
		void FpsCamera::UpdateTransform()
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
			Matrix::CreateLookAtLH(m_view, m_position, at, Vector3::UnitY);

			Matrix::CreatePerspectiveFovLH(m_proj, m_fieldOfView, m_aspectRatio, m_near, m_far);

			getFrustum().Update(m_view, m_proj);

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

		ChaseCamera::~ChaseCamera(void)
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
			//D3DXMatrixPerspectiveFovLH(&m_proj, m_fieldOfView, m_aspectRatio, m_near, m_far);
			getFrustum().Update(m_view, m_proj);
			//m_frustum.SetMatrix(&m_proj);
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
		void ChaseCamera::Update(const GameTime* time)
		{
			UpdateWorldPositions();

			Vector3 stretch = m_position - m_desiredPosition;
			Vector3 force = stretch * (-m_stiffness);
			force -= m_velocity * m_damping;

			m_velocity += force * (time->getElapsedTime() / m_mass );
			m_position += m_velocity * time->getElapsedTime();

			UpdateMatrices();

			Camera::Update(time);
		}

	};
};