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

#ifndef GL1OBJECTFACTORY_H
#define GL1OBJECTFACTORY_H

#include "GL1Common.h"
#include "Graphics\RenderSystem\ObjectFactory.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			/** The place for creating graphics objects under OpenGL.
			 *  See ObjectFactory for general description.
			 */
			class GL1ObjectFactory : public ObjectFactory
			{
			private:
				GL1RenderDevice* m_device;
			public:
				GL1ObjectFactory(GL1RenderDevice* device);

				virtual Texture* CreateTexture(ResourceLocation* rl, TextureUsage usage, bool managed);
				virtual Texture* CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format);
				virtual Texture* CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format);
				virtual Texture* CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format);

				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, uint sampleCount);
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt);
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt);

				virtual IndexBuffer* CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage);
				virtual VertexBuffer* CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage);

				virtual VertexDeclaration* CreateVertexDeclaration(const FastList<VertexElement> &elements);

				virtual VertexShader* CreateVertexShader(const ResourceLocation* resLoc);
				virtual PixelShader* CreatePixelShader(const ResourceLocation* resLoc);

				virtual VertexShader* CreateVertexShader(const byte* byteCode);
				virtual PixelShader* CreatePixelShader(const byte* byteCode);

				virtual Sprite* CreateSprite();
			};
		}
	}
}

#endif