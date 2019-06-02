#pragma once

#include "GL3Common.h"

#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::IO;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			class GL3RenderTarget : public RenderTarget
			{
			public:
				GL3RenderTarget(GL3RenderDevice* device, int32 width, int32 height, PixelFormat format, const String& multisampleMode);
				~GL3RenderTarget();

				virtual Texture* GetColorTexture() override;

				virtual void PrecacheLockedData() override;

			private:
				DataRectangle lock(LockMode mode, const Apoc3D::Math::Rectangle& rect) override;
				void unlock() override;

				GL3Texture* m_colorBuffer;
			};
		}
	}
}