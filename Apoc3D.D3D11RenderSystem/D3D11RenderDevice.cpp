#include "D3D11RenderDevice.h"
#include "GraphicsDeviceManager.h"
#include "D3D11RenderStateManager.h"
#include "D3D11Utils.h"

namespace rex
{
	D3D11RenderDevice::D3D11RenderDevice(GraphicsDeviceManager* devManager)
		: RenderDevice(L"Direct3D11 RenderSystem"), m_devManager(devManager),
		m_stateManager(nullptr), m_nativeState(nullptr), m_caps(nullptr),
		m_cachedRenderTargets(nullptr)
	{

	}

	D3D11RenderDevice::~D3D11RenderDevice()
	{
		if (m_cachedRenderTargets)
			delete[] m_cachedRenderTargets;

		if (m_nativeState)
			delete m_nativeState;
		if (m_stateManager)
			delete m_stateManager;
		if (m_objectFactory)
			delete m_objectFactory;
	}

	void D3D11RenderDevice::Initialize()
	{
		OutputDebugString(L"[D3D11]Initializing D3D11 Render Device. ");

		ID3D11Device* rawDev = getRawDevice();

		m_nativeState = new NativeD3DStateManager(this);
		m_stateManager = new D3D11RenderStateManager(this, m_nativeState);
		m_renderStates = m_stateManager;
		m_objectFactory = new D3D11ObjectFactory(this);

		m_cachedRenderTargets = new D3D11RenderTarget*[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		for (int i=0;i<D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;i++)
			m_cachedRenderTargets[i] = nullptr;

		m_caps = new D3D11Capabilities(this);
	}

	void D3D11RenderDevice::BeginFrame()
	{
		RenderDevice::BeginFrame();
	}
	void D3D11RenderDevice::EndFrame()
	{
		
	}

	void D3D11RenderDevice::Clear(ClearFlags flags, uint color, float depth, int stencil)
	{
		ID3D11DeviceContext* dc = getRawDeviceContext();

		if ((flags & CLEAR_ColorBuffer) == CLEAR_ColorBuffer)
		{
			FLOAT clearColor[4];
			clearColor[0] = ((color >> 16) & 0xff) / 255.0f;
			clearColor[1] = ((color >> 8) & 0xff) / 255.0f;
			clearColor[2] = (color & 0xff) / 255.0f;
			clearColor[3] = ((color >> 24) & 0xff) / 255.0f;
			// clear each one bound
			//dc->ClearRenderTargetView(m_devManager->getDefaultRenderTargetView(), clearColor);
		}

		if ((flags & CLEAR_DepthBuffer) == CLEAR_DepthBuffer || (flags & CLEAR_Stencil) == CLEAR_Stencil )
		{
			UINT clearFlags;
			// clear each one bound
			//dc->ClearDepthStencilView(m_devManager->getDefaultDepthStencilView(), clearFlags, depth, stecil);
		}
	}

	void D3D11RenderDevice::SetRenderTarget(int index, RenderTarget* rt)
	{
	}

	RenderTarget* D3D11RenderDevice::GetRenderTarget(int index)
	{
	}

	Viewport D3D11RenderDevice::getViewport()
	{
		UINT count = 1;
		D3D11_VIEWPORT dvp;
		getRawDeviceContext()->RSGetViewports(&count, &dvp);

		Viewport vp((int)dvp.TopLeftX, (int)dvp.TopLeftY, (int)dvp.Width, (int)dvp.Height, dvp.MinDepth, dvp.MaxDepth);
		return vp;
	}
	void D3D11RenderDevice::setViewport(const Viewport& vp)
	{
		D3D11_VIEWPORT dvp;
		dvp.TopLeftX = vp.X;
		dvp.TopLeftY = vp.Y;
		dvp.Width = vp.Width;
		dvp.Height = vp.Height;
		dvp.MinDepth = vp.MinZ;
		dvp.MaxDepth = vp.MaxZ;

		getRawDeviceContext()->RSSetViewports(1, &dvp);
	}
	PixelFormat D3D11RenderDevice::GetDefaultRTFormat()
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		m_devManager->getDefaultRenderTargetView()->GetDesc(&desc);

		return dutConvertBackPixelFormat(desc.Format);
	}
	DepthFormat D3D11RenderDevice::GetDefaultDepthStencilFormat()
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		m_devManager->getDefaultDepthStencilView()->GetDesc(&desc);

		return dutConvertBackDepthFormat(desc.Format);
	}


	Capabilities* const D3D11RenderDevice::getCapabilities() const { return m_caps; }

	ID3D11DeviceContext* D3D11RenderDevice::getRawDeviceContext() const { return m_devManager->getD3D11Context(); }
	ID3D11Device* D3D11RenderDevice::getRawDevice() const { return m_devManager->getD3D11Device(); }

	/************************************************************************/
	/*   D3D11Capabilities                                                  */
	/************************************************************************/

	D3D11Capabilities::D3D11Capabilities(D3D11RenderDevice* device)
		: m_device(device)
	{ }

	bool D3D11Capabilities::SupportsRenderTarget(uint multisampleCount, PixelFormat pixFormat, DepthFormat depthFormat)
	{
		bool result;
		DXGI_FORMAT clrFmt = dutConvertPixelFormat(pixFormat);

		UINT supporting;
		m_device->getRawDevice()->CheckFormatSupport(clrFmt, &supporting);

		if ((supporting & D3D11_FORMAT_SUPPORT_RENDER_TARGET) == D3D11_FORMAT_SUPPORT_RENDER_TARGET)
		{
			if (multisampleCount>0)
			{
				UINT qualityLevels;
				m_device->getRawDevice()->CheckMultisampleQualityLevels(clrFmt, multisampleCount, &qualityLevels);
				result = !!qualityLevels;
			}
			else
			{
				result = true;
			}
		}
		else
		{
			result = false;
		}
		
		// check for depth format
		if (result && depthFormat != DEPFMT_Count)
		{
			DXGI_FORMAT dsFmt = dutConvertDepthFormat(depthFormat);

			m_device->getRawDevice()->CheckFormatSupport(dsFmt, &supporting);

			if ((supporting & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL) == D3D11_FORMAT_SUPPORT_DEPTH_STENCIL)
			{
				if (multisampleCount>0)
				{
					UINT qualityLevels;
					m_device->getRawDevice()->CheckMultisampleQualityLevels(clrFmt, multisampleCount, &qualityLevels);
					if (qualityLevels == 0)
						result = false;
				}
			}
			else
			{
				result = false;
			}
		}
		return result;
	}
	bool D3D11Capabilities::SupportsPixelShader(const char* implType, int majorVer, int minorVer)
	{
		if (strcmp(implType, "hlsl"))
		{
			return false;
		}
		D3D_FEATURE_LEVEL lvl = m_device->getRawDevice()->GetFeatureLevel();
		if (lvl == D3D_FEATURE_LEVEL_11_0)
		{
			if (majorVer == 5 && minorVer == 0)
				return true;
		}

		if (lvl == D3D_FEATURE_LEVEL_10_1)
		{
			if (majorVer == 4)
				return true;
		}

		if (lvl == D3D_FEATURE_LEVEL_10_0)
		{
			if (majorVer == 4 && minorVer == 0)
				return true;
		}

		return false;
	}
	bool D3D11Capabilities::SupportsVertexShader(const char* implType, int majorVer, int minorVer)
	{
		return SupportsPixelShader(implType, majorVer, minorVer);
	}

	int D3D11Capabilities::GetMRTCount() { return D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; }

	/************************************************************************/
	/*   D3D11ObjectFactory                                                 */
	/************************************************************************/
	D3D11ObjectFactory::D3D11ObjectFactory(D3D11RenderDevice* dev)
		: ObjectFactory(dev), m_device(dev)
	{ }

	D3D11ObjectFactory::~D3D11ObjectFactory()
	{ }

	Texture* D3D11ObjectFactory::CreateTexture(const String& filePath, TextureUsage usage, bool managed);
	Texture* D3D11ObjectFactory::CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format);
	Texture* D3D11ObjectFactory::CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format);
	Texture* D3D11ObjectFactory::CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format);

	RenderTarget* D3D11ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, uint sampleCount);
	RenderTarget* D3D11ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt);
	RenderTarget* D3D11ObjectFactory::CreateRenderTarget(int width, int height, PixelFormat clrFmt);

	IndexBuffer* D3D11ObjectFactory::CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage);
	VertexBuffer* D3D11ObjectFactory::CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage);

	VertexDeclaration* D3D11ObjectFactory::CreateVertexDeclaration(const os::Vector<VertexElement> &elements);

	VertexShader* D3D11ObjectFactory::CreateVertexShader(const byte* byteCode);
	PixelShader* D3D11ObjectFactory::CreatePixelShader(const byte* byteCode);

	Sprite* D3D11ObjectFactory::CreateSprite();
}
