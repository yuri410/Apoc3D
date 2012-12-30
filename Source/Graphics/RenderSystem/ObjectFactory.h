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
#ifndef APOC3D_OBJECTFACTORY_H
#define APOC3D_OBJECTFACTORY_H

#include "Common.h"
#include "Graphics/PixelFormat.h"
#include "Graphics/GraphicsCommon.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			/* Factory that can create resources like textures, buffers and shaders. 
			 * Application can use this to create resources not managed by resource managers.
			 */
			class APAPI ObjectFactory
			{
			private:
				RenderDevice* m_renderDevice;

			protected:
				ObjectFactory(RenderDevice* rd);

			public:
				/** Creates a texture from a ResourceLocation. 
				 * @param managed tell if the Texture will be managed.
				 * @remarks
				 *  Texture Manager use this to create managed textures, with managed=true. 
				 *  The client application can call this:
				 *    1. with managed=false. This will be almost the same as TextureManager::CreateUnmanagedInstance.
				 *       But keep in mind that TextureManager have more features like ResourceLocation redirecting for 
				 *       debugging purposes.
				 *       And by the way. As the texture is unmanaged, it is loaded with this call.
				 *    2. with managed=true. No sense.
				 *       The texture can be created and use. No matter async or not, it should be loaded manually.
				 *       However the resource manager will not care about it at all. This will just bring more
				 *       trivia for developers *unless* special cases like making custom resource managers.
				 */
				virtual Texture* CreateTexture(ResourceLocation* rl, TextureUsage usage, bool managed) = 0;


				/** Creates a blank 2D or 1D texture.
				 *  An 1D texture will be created if either width or height is one.
				 * @remarks
				 *  This as well as all the following methods has nothing to do with any kind of Resource Managers.
				 *  They are almost purely API level.
				 *  Thus, no more comment unless clarification is needed.
				 */
				virtual Texture* CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format) = 0;

				/** Creates a blank 2D, 1D or 3D texture.
				 *  In addition to the above one, a volume texture will be created if depth is more than 1.
				 */
				virtual Texture* CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format) = 0;

				/** Creates a blank Cube map.
				*/
				virtual Texture* CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format) = 0;


				/** Creates a RenderTarget with a depth buffer and a color buffer.
				 */
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt) = 0;

				/** Creates a RenderTarget with a color buffer only.
				 */
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt) = 0;

				/** Creates a RenderTarget with a depth buffer and a color buffer. 
				 *  If sampleCount is more than 0, the render target will be multisampled. 
				 *  A value of 1 will take as a 2 sample RT.
				 *
				 * Notice:
				 *  When creating manually in client code, do check the device capabilities.
				 *
				 *  When using render targets in scene-render scripts, do not worry about checking the capabilities,
				 *  as the script will auto fall back if not supported.
				 */
				virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, uint sampleCount) = 0;


				virtual IndexBuffer* CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage) = 0;
				virtual VertexBuffer* CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage) = 0;


				virtual VertexDeclaration* CreateVertexDeclaration(const FastList<VertexElement>& elements) = 0;

				// TODO: Pre-built multi-profile shader data for different type of API shaders.

				/** Creates a vertex shader from a ResourceLocation.
				 *  The entire data will be treated as the code for the shader.
				 *  See CreateVertexShader(const byte* byteCode) for details.
				 */
				virtual VertexShader* CreateVertexShader(const ResourceLocation* resLoc) = 0;
				virtual PixelShader* CreatePixelShader(const ResourceLocation* resLoc) = 0;

				/** Creates a vertex shader from a piece of code.
				 *  The code could be byteCode(D3D9) or source(OpenGL) depends on 
				 *  the specific API.
				 */
				virtual VertexShader* CreateVertexShader(const byte* byteCode) = 0;
				virtual PixelShader* CreatePixelShader(const byte* byteCode) = 0;

				virtual Sprite* CreateSprite() = 0;

				//virtual InstancingData* CreateInstancingData() = 0;
			};
		}
	}
}


#endif