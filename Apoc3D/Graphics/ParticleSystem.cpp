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
#include "apoc3d/Math/MathCommon.h"
#include "apoc3d/Math/Matrix.h"
#include "apoc3d/VFS/PathUtils.h"

#include "LockData.h"
#include "RenderSystem/RenderDevice.h"
#include "RenderSystem/Buffer/HardwareBuffer.h"

namespace Apoc3D
{

	namespace Graphics
	{
		const VertexElement ParticleVertex::VtxElements[] =
		{
			VertexElement(0, VEF_Vector3, VEU_Position, 0),
			VertexElement(0, VEF_Vector3, VEU_Normal, 0),
			VertexElement(0, VEF_Color, VEU_Color, 0),
			VertexElement(0, VEF_Single, VEU_TextureCoordinate, 0)
		};

		ParticleSystem::ParticleSystem(RenderDevice* device)
			: m_device(device), 
			m_particles(0), m_particleCount(0), m_vertexBuffer(0), m_vertexDeclaration(0),
			m_firstActiveParticle(0),m_firstNewParticle(0),m_firstFreeParticle(0),m_firstRetiredParticle(0),
			m_currentTime(0),m_drawCounter(0),
			m_mtrl(device)
		{
			memset(&m_settings,0, sizeof(m_settings));
			
			m_mtrl.UsePointSprite = true;
			//m_device->CreateStateBlock(D3DSBT_VERTEXSTATE,&m_stateBlock);
		}

		ParticleSystem::~ParticleSystem(void)
		{
		}

		void ParticleSystem::Load()
		{
			InitializeSettings(m_settings);
			//m_mtrl.setTexture(0, m_settings.ParticleTexture);

			m_particles = new ParticleVertex[m_settings.MaxParticles];
			m_particleCount = m_settings.MaxParticles;


			//m_effect = LoadEffect(m_game);
			ObjectFactory* fac = m_device->getObjectFactory();
			

			FastList<VertexElement> elems;
			elems.Add(ParticleVertex::VtxElements[0]);
			elems.Add(ParticleVertex::VtxElements[1]);
			elems.Add(ParticleVertex::VtxElements[2]);
			elems.Add(ParticleVertex::VtxElements[3]);
			m_vertexDeclaration = fac->CreateVertexDeclaration(elems);

			// load effect
			m_vertexBuffer =
				m_device->getObjectFactory()->CreateVertexBuffer(m_particleCount, m_vertexDeclaration, BU_Dynamic);
		}
		

		void ParticleSystem::SetParticleRenderStates()
		{
			m_mtrl.SourceBlend = m_settings.SourceBlend;
			m_mtrl.DestinationBlend = m_settings.DestinationBlend;
			////const float ps = 1.f;
			////m_device->SetRenderState(D3DRS_POINTSIZE, reinterpret_cast<const DWORD&>(ps));

			//// Enable point sprites.
			//m_device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
			//const float ps = 256.f;
			//m_device->SetRenderState(D3DRS_POINTSIZE_MAX, reinterpret_cast<const DWORD&>(ps));

			//
			//// Set the alpha blend mode.
			//m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			//m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

			//m_device->SetRenderState(D3DRS_SRCBLEND, m_settings.SourceBlend);
			//m_device->SetRenderState(D3DRS_DESTBLEND, m_settings.DestinationBlend);
			//
			//// Set the alpha test mode.
			//m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			//m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			//m_device->SetRenderState(D3DRS_ALPHAREF, 0);
			//
			//// Enable the depth buffer (so particles will not be visible through
			//// solid objects like the ground plane), but disable depth writes
			//// (so particles will not obscure other particles).
			//m_device->SetRenderState(D3DRS_ZENABLE, TRUE);
			//m_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);						
		}
		void ParticleSystem::AddNewParticlesToVertexBuffer()
		{
			const int stride = sizeof(ParticleVertex);

			if (m_firstNewParticle < m_firstFreeParticle)
			{
				// If the new particles are all in one consecutive range,
				// we can upload them all in a single call.
				int length = (m_firstFreeParticle - m_firstNewParticle)* stride;

				void* data = m_vertexBuffer->Lock(m_firstNewParticle * stride, length, LOCK_NoOverwrite);
				
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
		void ParticleSystem::AddParticle(const Vector3 &position,  Vector3 velocity)
		{
			// Figure out where in the circular queue to allocate the new particle.
			int nextFreeParticle = m_firstFreeParticle + 1;

			if (nextFreeParticle >= m_particleCount)
				nextFreeParticle = 0;

			// If there are no free particles, we just have to give up.
			if (nextFreeParticle == m_firstRetiredParticle)
				return;

			// Adjust the input velocity based on how much
			// this particle system wants to be affected by it.
			velocity = Vector3Utils::Multiply(velocity, m_settings.EmitterVelocitySensitivity);	

			// Add in some random amount of horizontal velocity.
			float horizontalVelocity = Math::Lerp(m_settings.MinHorizontalVelocity,
				m_settings.MaxHorizontalVelocity,
				Randomizer::NextFloat());

			float horizontalAngle = Randomizer::NextFloat() * Math::PI * 2;

			_V3X(velocity) += horizontalVelocity * cosf(horizontalAngle);
			_V3Z(velocity) += horizontalVelocity * sinf(horizontalAngle);

			// Add in some random amount of vertical velocity.
			_V3Y(velocity) += Math::Lerp(m_settings.MinVerticalVelocity,
				m_settings.MaxVerticalVelocity,
				Randomizer::NextFloat());

			// Choose four random control values. These will be used by the vertex
			// shader to give each particle a different size, rotation, and color.
			uint32 randomValues = PACK_COLOR(Randomizer::Next(255), 
				Randomizer::Next(255), Randomizer::Next(255), Randomizer::Next(255));
			// new Color((byte)random.Next(255),
				//(byte)random.Next(255),
				//(byte)random.Next(255),
				//(byte)random.Next(255));

			// Fill in the particle vertex structure.
			m_particles[m_firstFreeParticle].Position = position;
			m_particles[m_firstFreeParticle].Velocity = velocity;
			m_particles[m_firstFreeParticle].Random = randomValues;
			m_particles[m_firstFreeParticle].Time = m_currentTime;

			m_firstFreeParticle = nextFreeParticle;
		}
		RenderOperationBuffer* ParticleSystem::GetRenderOperation(int lod)
		{
			m_opBuffer.FastClear();
			// If there are any particles waiting in the newly added queue,
			// we'd better upload them to the GPU ready for drawing.
			if (m_firstNewParticle != m_firstFreeParticle)
			{
				AddNewParticlesToVertexBuffer();
			}
			if (m_firstActiveParticle != m_firstFreeParticle)
			{
				m_geoData.VertexBuffer = m_vertexBuffer;
				m_geoData.VertexDecl = m_vertexDeclaration;
				m_geoData.PrimitiveType = PT_PointList;
				m_geoData.VertexSize = sizeof(ParticleVertex);
				m_geoData2.VertexBuffer = m_vertexBuffer;
				m_geoData2.VertexDecl = m_vertexDeclaration;
				m_geoData2.PrimitiveType = PT_PointList;
				m_geoData2.VertexSize = sizeof(ParticleVertex);

				if (m_firstActiveParticle < m_firstFreeParticle)
				{
					RenderOperation rop;
					rop.Material = &m_mtrl;
					rop.GeometryData = &m_geoData;
					rop.RootTransform.LoadIdentity();
					m_opBuffer.Add(rop);
					
					m_geoData.BaseVertex = m_firstActiveParticle;
					m_geoData.PrimitiveCount = m_firstFreeParticle - m_firstActiveParticle;
					m_geoData.VertexCount = m_geoData.PrimitiveCount;
				}
				else
				{
					RenderOperation rop;
					rop.Material = &m_mtrl;
					rop.GeometryData = &m_geoData;
					rop.RootTransform.LoadIdentity();
					m_opBuffer.Add(rop);
					m_geoData.BaseVertex = m_firstActiveParticle;
					m_geoData.PrimitiveCount = m_firstFreeParticle - m_firstActiveParticle;
					m_geoData.VertexCount = m_geoData.PrimitiveCount;

					if (m_firstFreeParticle > 0)
					{
						RenderOperation rop2;
						rop2.Material = &m_mtrl;
						rop2.GeometryData = &m_geoData2;
						rop2.RootTransform.LoadIdentity();
						m_opBuffer.Add(rop2);

						m_geoData2.BaseVertex = m_firstActiveParticle;
						m_geoData2.PrimitiveCount = m_firstFreeParticle - m_firstActiveParticle;
						m_geoData2.VertexCount = m_geoData2.PrimitiveCount;
					}
				}
			}

			m_drawCounter++;

			return &m_opBuffer;
		}
		//void ParticleSystem::Render(ModelEffect* effect)
		//{
		//	// If there are any particles waiting in the newly added queue,
		//	// we'd better upload them to the GPU ready for drawing.
		//	if (m_firstNewParticle != m_firstFreeParticle)
		//	{
		//		AddNewParticlesToVertexBuffer();
		//	}
		//	// If there are any active particles, draw them now!
		//	if (m_firstActiveParticle != m_firstFreeParticle)
		//	{
		//		//m_stateBlock->Capture();
		//		SetParticleRenderStates();

		//		m_device->SetStreamSource(0, m_vertexBuffer, 0, sizeof(ParticleVertex));
		//		m_device->SetVertexDeclaration(m_vertexDeclaration);

		//		Matrix world;
		//		D3DXMatrixIdentity(&world);
		//		effect->Begin();
		//		static_cast<ExplParticleEffect*>(effect)->SetParameters(this);
		//		effect->Setup(&m_mtrl, &world);
		//		
		//		// TODO: setup effect
		//		if (m_firstActiveParticle < m_firstFreeParticle)
		//		{
		//			// If the active particles are all in one consecutive range,
		//			// we can draw them all in a single call.
		//			m_device->DrawPrimitive(D3DPT_POINTLIST,
		//				m_firstActiveParticle,
		//				m_firstFreeParticle - m_firstActiveParticle);
		//		}
		//		else
		//		{
		//			// If the active particle range wraps past the end of the queue
		//			// back to the start, we must split them over two draw calls.
		//			m_device->DrawPrimitive(D3DPT_POINTLIST,
		//				m_firstActiveParticle,
		//				m_particleCount - m_firstActiveParticle);

		//			if (m_firstFreeParticle > 0)
		//			{
		//				m_device->DrawPrimitive(D3DPT_POINTLIST,
		//					0,
		//					m_firstFreeParticle);
		//			}
		//		}
		//		effect->End();

		//		//m_stateBlock->Apply();
		//		m_device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
		//		m_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		//		m_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		//		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		//	}

		//	m_drawCounter++;
		//}
		void ParticleSystem::Update(const GameTime* const time)
		{
			m_currentTime += time->getElapsedTime();

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