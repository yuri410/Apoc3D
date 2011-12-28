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

#include "Camera.h"
#include "Core/GameTime.h"


namespace Apoc3D
{
	namespace Graphics
	{
		FpsCamera::FpsCamera(float aspectRatio)
			: m_aspectRatio(aspectRatio), m_velocity(1), m_position(Vector3Utils::Zero),
			m_fieldOfView(ToRadian(50)), m_near(0.5f), m_far(1500)
		{
			
		}


		FpsCamera::~FpsCamera(void)
		{
		}

		void FpsCamera::Update(const GameTime* time)
		{
			Vector3 at = Vector3Utils::Add(m_position, Vector3Utils::UnitZ);
			Matrix::CreateLookAtLH(m_view, m_position, at, Vector3Utils::UnitY);

			Matrix::CreatePerspectiveFovLH(m_proj, m_fieldOfView, m_aspectRatio, m_near, m_far);

			getFrustum().Update(m_view, m_proj);

			Camera::Update(time);
		}

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		ChaseCamera::ChaseCamera(float fov)
			: m_velocity(Vector3Utils::Zero), m_position(Vector3Utils::Zero), m_lootAt(Vector3Utils::Zero), m_desiredPosition(Vector3Utils::Zero),
			m_up(Vector3Utils::UnitY), m_chasePosition(Vector3Utils::Zero), m_chaseDirection(Vector3Utils::Zero),
			m_stiffness(1800), m_damping(600), m_mass(50), 
			m_fieldOfView(fov), m_aspectRatio(1),
			m_near(1), m_far(1000)
		{
			m_lootAtOfs = Vector3Utils::LDVector(0, 3.0f, 0);
			m_desiredPositionOfs = Vector3Utils::LDVector(0, 2, 2);

			//m_frustum = Frustum(&m_proj);
		}

		ChaseCamera::~ChaseCamera(void)
		{
		}

		void ChaseCamera::Reset()
		{
			UpdateWorldPositions();

			m_velocity = Vector3Utils::Zero;
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
			// Construct a matrix to transform from object space to world space
			Matrix trans;
			trans.LoadIdentity();

			trans.M31 = -_V3X(m_chaseDirection);
			trans.M32 = -_V3Y(m_chaseDirection);
			trans.M33 = -_V3Z(m_chaseDirection);

			trans.M21 = _V3X(m_up);
			trans.M22 = _V3Y(m_up);
			trans.M23 = _V3Z(m_up);

			Vector3 right = Vector3Utils::Cross(m_up, m_chaseDirection);
			right = Vector3Utils::Normalize(right);

			trans.M11 = _V3X(right);
			trans.M12 = _V3Y(right);
			trans.M13 = _V3Z(right);

			Vector3 desiredPositionOfsT;
			Vector3 lookAtOfsT;
			desiredPositionOfsT = Vector3Utils::TransformNormal(m_desiredPositionOfs, trans);
			lookAtOfsT = Vector3Utils::TransformNormal(m_lootAtOfs, trans);

			m_desiredPosition = Vector3Utils::Add(m_chasePosition, desiredPositionOfsT);
			m_lootAt = Vector3Utils::Add(m_chasePosition, lookAtOfsT);
		}
		void ChaseCamera::Update(const GameTime* const time)
		{
			UpdateWorldPositions();

			Vector3 stretch = Vector3Utils::Subtract(m_position , m_desiredPosition);
			Vector3 force = Vector3Utils::Multiply(stretch, -m_stiffness);
			force = Vector3Utils::Subtract(force, Vector3Utils::Multiply(m_velocity, m_damping));

			m_velocity = Vector3Utils::Add(m_velocity, Vector3Utils::Multiply(force, time->getElapsedTime() / m_mass ));
			m_position = Vector3Utils::Add(m_position, Vector3Utils::Multiply(m_velocity, time->getElapsedTime()));

			UpdateMatrices();

			Camera::Update(time);
			//D3DXMatrixPerspectiveFovLH(&m_proj, ToRadius(45), LabGame::ScreenWidth/(float)(LabGame::ScreenHeight),1,1000);

			//Vector3  backward;
			//D3DXVec3Cross(&backward, &UnitY3, &m_right);


			//Vector3 eyePos = Vector3(10, 7, 5);// m_position + backward;
			//
			//D3DXMatrixLookAtLH(&m_view, &eyePos, &m_position, &UnitY3);
		}

	};
};