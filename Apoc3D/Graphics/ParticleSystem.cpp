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
#include "ParticleSystem.h"

#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Core/ResourceHandle.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/VFS/PathUtils.h"

#include "LockData.h"
#include "RenderSystem/RenderDevice.h"
#include "RenderSystem/VertexDeclaration.h"
#include "RenderSystem/Buffer/HardwareBuffer.h"

namespace Apoc3D
{

	namespace Graphics
	{
		const VertexElement ParticleVertex::VtxElements[] =
		{
			VertexElement(0, VEF_Vector3, VEU_Position, 0),
			VertexElement(12, VEF_Vector3, VEU_Normal, 0),
			VertexElement(24, VEF_Color, VEU_Color, 0),
			VertexElement(28, VEF_Single, VEU_TextureCoordinate, 0)
		};

		ParticleSystem::ParticleSystem(RenderDevice* device, Material* mtrl)
			: m_device(device), m_mtrl(mtrl)
		{
			memset(&m_settings, 0, sizeof(m_settings));
		}

		ParticleSystem::~ParticleSystem()
		{
			delete[] m_particles;
			m_particles = nullptr;

			DELETE_AND_NULL(m_vertexBuffer);
			DELETE_AND_NULL(m_vertexDeclaration);
		}

		void ParticleSystem::Setup(FunctorReference<void(ParticleSettings&)> settingsFunc)
		{
			settingsFunc(m_settings);

			Load();
		}

		void ParticleSystem::Load()
		{
			InitializeSettings(m_settings);

			m_particles = new ParticleVertex[m_settings.MaxParticles];
			m_particleCount = m_settings.MaxParticles;

			ObjectFactory* fac = m_device->getObjectFactory();
			List<VertexElement> elems; 
			elems.Add(ParticleVertex::VtxElements[0]); elems.Add(ParticleVertex::VtxElements[1]);
			elems.Add(ParticleVertex::VtxElements[2]); elems.Add(ParticleVertex::VtxElements[3]);

			m_vertexDeclaration = fac->CreateVertexDeclaration(elems);

			m_vertexBuffer = fac->CreateVertexBuffer(m_particleCount, m_vertexDeclaration, (BufferUsageFlags)( BU_Dynamic | BU_WriteOnly | BU_PointSpriteVertex));

			m_geoData.VertexBuffer = m_vertexBuffer;
			m_geoData.VertexDecl = m_vertexDeclaration;
			m_geoData.PrimitiveType = PT_PointList;
			m_geoData.VertexSize = m_vertexDeclaration->GetVertexSize();
			m_geoData.UserData = this;

			m_geoDataAlt = m_geoData;
		}
		
		void ParticleSystem::AddNewParticlesToVertexBuffer()
		{
			const int stride = sizeof(ParticleVertex);

			if (m_firstNewParticle < m_firstFreeParticle)
			{
				// If the new particles are all in one consecutive range,
				// we can upload them all in a single call.
				int length = (m_firstFreeParticle - m_firstNewParticle)* stride;

				void* data = m_vertexBuffer->Lock(m_firstNewParticle * stride,length, LOCK_NoOverwrite);
				
				memcpy(data, m_particles+m_firstNewParticle, length);
				m_vertexBuffer->Unlock();
			}
			else
			{
				// If the new particle range wraps past the end of the queue
				// back to the start, we must split them over two upload calls.
				int length = (m_particleCount - m_firstNewParticle)*stride;

				void* data = m_vertexBuffer->Lock(m_firstNewParticle * stride, length, LOCK_NoOverwrite);

				memcpy(data, m_particles+m_firstNewParticle, length);
				m_vertexBuffer->Unlock();


				if (m_firstFreeParticle > 0)
				{
					length = m_firstFreeParticle * stride;
					data = m_vertexBuffer->Lock(0, length, LOCK_NoOverwrite);

					memcpy(data, m_particles, length);

					m_vertexBuffer->Unlock();
				}
			}

			// Move the particles we just uploaded from the new to the active queue.
			m_firstNewParticle = m_firstFreeParticle;
		}
		void ParticleSystem::RetireActiveParticles()
		{
			float particleDuration = m_settings.Duration;

			while (m_firstActiveParticle != m_firstNewParticle)
			{
				// Is this particle old enough to retire?
				float particleAge = m_currentTime - m_particles[m_firstActiveParticle].Time;

				if (particleAge < particleDuration)
					break;

				// Remember the time at which we retired this particle.
				m_particles[m_firstActiveParticle].Time = static_cast<float>(m_drawCounter);

				// Move the particle from the active to the retired queue.
				m_firstActiveParticle++;

				if (m_firstActiveParticle >= m_particleCount)
					m_firstActiveParticle = 0;
			}
		}
		void ParticleSystem::FreeRetiredParticles()
		{
			while (m_firstRetiredParticle != m_firstActiveParticle)
			{
				// Has this particle been unused long enough that
				// the GPU is sure to be finished with it?
				int age = m_drawCounter - (int)m_particles[m_firstRetiredParticle].Time;

				// The GPU is never supposed to get more than 2 frames behind the CPU.
				// We add 1 to that, just to be safe in case of buggy drivers that
				// might bend the rules and let the GPU get further behind.
				if (age < 3)
					break;

				// Move the particle from the retired to the free queue.
				m_firstRetiredParticle++;

				if (m_firstRetiredParticle >= m_particleCount)
					m_firstRetiredParticle = 0;
			}
		}
		bool ParticleSystem::AddParticle(const Vector3 &position, const Vector3& _velocity)
		{
			Vector3 velocity = _velocity;

			// Figure out where in the circular queue to allocate the new particle.
			int nextFreeParticle = m_firstFreeParticle + 1;

			if (nextFreeParticle >= m_particleCount)
				nextFreeParticle = 0;

			// If there are no free particles, we just have to give up.
			if (nextFreeParticle == m_firstRetiredParticle)
				return false;

			velocity *= m_settings.EmitterVelocitySensitivity;


			// Add in some random amount of horizontal velocity.
			float horizontalVelocity = Math::Lerp(m_settings.MinHorizontalVelocity,
				m_settings.MaxHorizontalVelocity,
				Randomizer::NextFloat());

			float horizontalAngle = Randomizer::NextFloat() * Math::PI * 2;

			velocity.X += horizontalVelocity * cosf(horizontalAngle);
			velocity.Z += horizontalVelocity * sinf(horizontalAngle);

			// Add in some random amount of vertical velocity.
			velocity.Y += Math::Lerp(m_settings.MinVerticalVelocity,
				m_settings.MaxVerticalVelocity,
				Randomizer::NextFloat());

			// Choose four random control values. These will be used by the vertex
			// shader to give each particle a different size, rotation, and color.
			uint32 randomValues = CV_PackColor(Randomizer::NextInclusive(255), 
				Randomizer::NextInclusive(255), Randomizer::NextInclusive(255), Randomizer::NextInclusive(255));


			// Fill in the particle vertex structure.
			m_particles[m_firstFreeParticle].Position = position;
			m_particles[m_firstFreeParticle].Velocity = velocity;
			m_particles[m_firstFreeParticle].Random = randomValues;
			m_particles[m_firstFreeParticle].Time = m_currentTime;

			m_firstFreeParticle = nextFreeParticle;
			return true;
		}
		RenderOperationBuffer* ParticleSystem::GetRenderOperation(int lod)
		{
			m_opBuffer.Clear();
			// If there are any particles waiting in the newly added queue,
			// we'd better upload them to the GPU ready for drawing.
			if (m_firstNewParticle != m_firstFreeParticle)
			{
				AddNewParticlesToVertexBuffer();
			}
			// If there are any active particles, draw them now!
			if (m_firstActiveParticle != m_firstFreeParticle)
			{
				if (m_firstActiveParticle < m_firstFreeParticle)
				{
					// If the active particles are all in one consecutive range,
					// we can draw them all in a single call.
					m_geoData.BaseVertex = m_firstActiveParticle;
					m_geoData.VertexCount = m_firstFreeParticle - m_firstActiveParticle;
					m_geoData.PrimitiveCount = m_geoData.VertexCount;

					RenderOperation rop;
					rop.GeometryData = &m_geoData;
					rop.RootTransform = Matrix::Identity;
					rop.Material = m_mtrl;
					m_opBuffer.Add(rop);
				}
				else
				{
					// If the active particle range wraps past the end of the queue
					// back to the start, we must split them over two draw calls.
					m_geoData.BaseVertex = m_firstActiveParticle;
					m_geoData.VertexCount = m_particleCount - m_firstActiveParticle;
					m_geoData.PrimitiveCount = m_geoData.VertexCount;

					RenderOperation rop;
					rop.GeometryData = &m_geoData;
					rop.RootTransform = Matrix::Identity;
					rop.Material = m_mtrl;
					m_opBuffer.Add(rop);

					if (m_firstFreeParticle > 0)
					{
						m_geoDataAlt.BaseVertex = 0;
						m_geoDataAlt.VertexCount = m_firstFreeParticle;
						m_geoDataAlt.PrimitiveCount = m_geoDataAlt.VertexCount;

						RenderOperation rop2;
						rop2.GeometryData = &m_geoDataAlt;
						rop2.RootTransform = Matrix::Identity;
						rop2.Material = m_mtrl;
						m_opBuffer.Add(rop2);
					}
				}
			}
			m_drawCounter++;
			return &m_opBuffer;
		}
		void ParticleSystem::Update(float dt)
		{
			m_currentTime += dt;

			RetireActiveParticles();
			FreeRetiredParticles();

			// If we let our timer go on increasing for ever, it would eventually
			// run out of floating point precision, at which point the particles
			// would render incorrectly. An easy way to prevent this is to notice
			// that the time value doesn't matter when no particles are being drawn,
			// so we can reset it back to zero any time the active queue is empty.

			if (m_firstActiveParticle == m_firstFreeParticle)
				m_currentTime = 0;

			if (m_firstRetiredParticle == m_firstActiveParticle)
				m_drawCounter = 0;
		}

	}
}