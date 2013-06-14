#pragma once

#include "D3D11Common.h"
#include "RenderSystem.h"

namespace rex
{
	class D3D11RenderTarget : public RenderTarget
	{
	public:
		D3D11RenderTarget(D3D11RenderDevice* renderDevice, int32 width, int32 height, DXGI_FORMAT colorFormat, DXGI_FORMAT depthFormat, uint32 multiSampleCount);
		D3D11RenderTarget(D3D11RenderDevice* renderDevice, int32 width, int32 height, DXGI_FORMAT colorFormat, uint32 multiSampleCount);

		virtual ~D3D11RenderTarget();

		uint32 getMultiSampleCount() const { return m_sampleCount; }
		int32 getWidth() const { return m_width; }
		int32 getHeight() const { return m_height; }

		DepthFormat getDepthFormat() const { return m_depthFormat; }
		PixelFormat getColorFormat() const { return m_pixelFormat; }

		virtual Texture* GetColorTexture();
		virtual DepthBuffer* GetDepthBuffer();

	private:
		int32 m_width;
		int32 m_height;
		DepthFormat m_depthFormat;
		PixelFormat m_pixelFormat;
		uint32 m_sampleCount;

	};
}
