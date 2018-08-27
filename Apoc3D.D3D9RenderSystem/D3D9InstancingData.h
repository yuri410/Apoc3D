#pragma once
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

#ifndef D3D9INSTANCINGDATA_H
#define D3D9INSTANCINGDATA_H

#include "D3D9Common.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Graphics/RenderSystem/InstancingData.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			/** The instancing is done using vertex streams by mixing a per-instance index to
			 *  the geometry vertex in vertex element TEX15.
			 *
			 *  The expected way of instancing in d3d shaders is use the TEX15 as the index
			 *  of the EPUSAGE_Trans_InstanceWorlds array to obtain the transformation,
			 *  then use this as the world transform.
			 */
			class D3D9InstancingData final : public InstancingData
			{
			public:
				D3D9InstancingData(D3D9RenderDevice* device);
				~D3D9InstancingData();

				virtual int Setup(const RenderOperation* op, int count, int beginIndex);


				IDirect3DVertexBuffer9* GetInstanceBuffer() const { return m_vertexBuffer; }
				IDirect3DVertexDeclaration9* ExpandVertexDecl(VertexDeclaration* decl);

				int getInstanceDataSize() const { return sizeof(float); }
			private:
				HashMap<VertexDeclaration*, IDirect3DVertexDeclaration9*> m_vtxDeclExpansionTable;

				/** Per-instance index stored in this vertex buffer */
				IDirect3DVertexBuffer9* m_vertexBuffer;
				D3D9RenderDevice* m_d3ddev;
			};
		}
	}
}
#endif