#include "GL3RenderTarget.h"
#include "GL3RenderDevice.h"
#include "GL3Texture.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			GL3RenderTarget::GL3RenderTarget(GL3RenderDevice* device, int32 width, int32 height, PixelFormat format, const String& multisampleMode)
				: RenderTarget(device, width, height, format, multisampleMode)
			{
				m_colorBuffer = new GL3Texture(device, width, height, 1, 1, format, TU_Default);
			}

			GL3RenderTarget::~GL3RenderTarget()
			{
				delete m_colorBuffer;
			}

			Texture* GL3RenderTarget::GetColorTexture()
			{
				return m_colorBuffer;
			}

			void GL3RenderTarget::PrecacheLockedData()
			{

			}

			DataRectangle GL3RenderTarget::lock(LockMode mode, const Apoc3D::Math::Rectangle& rect)
			{
				return m_colorBuffer->Lock(0, mode, rect);
			}

			void GL3RenderTarget::unlock()
			{
				m_colorBuffer->Unlock(0);
			}
		}
	}
}