#pragma once

/* -----------------------------------------------------------------------
* This source file is part of Apoc3D Framework
*
* Copyright (c) 2009+ Tao Xin
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

#ifndef NRSOBJECTS_H
#define NRSOBJECTS_H

#include "NRSCommon.h"
#include "NRSRenderDevice.h"

#include "apoc3d/Graphics/RenderSystem/HardwareBuffer.h"
#include "Apoc3D/Graphics/RenderSystem/InstancingData.h"
#include "Apoc3D/Graphics/RenderSystem/VertexDeclaration.h"
#include "Apoc3D/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/RenderSystem/GraphicsAPI.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			class NRSIndexBuffer : public IndexBuffer
			{
			public:
				NRSIndexBuffer(NRSRenderDevice* device, IndexBufferFormat type, int32 size, BufferUsageFlags usage);
				~NRSIndexBuffer();

			protected:
				virtual void* lock(int offset, int size, LockMode mode) override;
				virtual void unlock() override;

			private:
				NRSRenderDevice* m_device;
				char* m_data;
			};

			class NRSVertexBuffer : public VertexBuffer
			{
			public:
				NRSVertexBuffer(NRSRenderDevice* device, int32 vertexCount, int32 vertexSize, BufferUsageFlags usage);
				~NRSVertexBuffer();

			protected:
				virtual void* lock(int offset, int size, LockMode mode) override;
				virtual void unlock() override;

			private:
				NRSRenderDevice* m_device;
				char* m_data;
			};

			class NRSDepthBuffer final : public DepthStencilBuffer
			{
				RTTI_DERIVED(NRSDepthBuffer, DepthStencilBuffer);

			public:
				NRSDepthBuffer(NRSRenderDevice* device, int32 width, int32 height, DepthFormat depthFormat, const String& multisampleMode);
				NRSDepthBuffer(NRSRenderDevice* device, int32 width, int32 height, DepthFormat depthFormat, const NRSCapabilities::AAProfile* aamode);

				~NRSDepthBuffer();

			private:
				const NRSCapabilities::AAProfile* m_explicitAAMode = nullptr;
			};

			class NRSVertexDeclaration final : public VertexDeclaration
			{
			public:
				NRSVertexDeclaration(NRSRenderDevice* device, const List<VertexElement>& elements);
				~NRSVertexDeclaration();

			private:
				NRSRenderDevice* m_device;
			};

			class NRSInstancingData final : public InstancingData
			{
			public:
				NRSInstancingData(NRSRenderDevice* device);
				~NRSInstancingData();

				virtual int Setup(const RenderOperation* op, int count, int beginIndex);
			};

			class NRSGraphicsAPIFactory final : public GraphicsAPIFactory
			{
			public:
				NRSGraphicsAPIFactory()
					: GraphicsAPIFactory(GetDescription())
				{ }

				virtual DeviceContext* CreateDeviceContext();
			private:
				static APIDescription GetDescription();

			};
		}
	}
}
#endif