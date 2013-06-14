#pragma once

#include "D3D11Common.h"
#include "RenderSystem.h"

namespace rex
{
	// this is not used in this experiment
	class D3D11DeviceContext : public DeviceContext { };

	class D3D11RenderDevice : public RenderDevice
	{
	public:
		D3D11RenderDevice(GraphicsDeviceManager* devManager);
		~D3D11RenderDevice();

		virtual void Initialize();

		virtual void BeginFrame();
		virtual void EndFrame();
		virtual void Clear(ClearFlags flags, uint color, float depth, int stencil);

		virtual void SetRenderTarget(int index, RenderTarget* rt);
		virtual RenderTarget* GetRenderTarget(int index);

		virtual PixelFormat GetDefaultRTFormat();
		virtual DepthFormat GetDefaultDepthStencilFormat();

		virtual void BindVertexShader(VertexShader* shader);
		virtual void BindPixelShader(PixelShader* shader);

		virtual void Render(Material* mtrl, const RenderOperation* op, int count, int passSelID);

		virtual Viewport getViewport();
		virtual void setViewport(const Viewport& vp);

		virtual Capabilities* const getCapabilities() const;

		ID3D11DeviceContext* getRawDeviceContext() const;
		ID3D11Device* getRawDevice() const;
	private:
		GraphicsDeviceManager* m_devManager;

		NativeD3DStateManager* m_nativeState;
		D3D11RenderStateManager* m_stateManager;

		D3D11Capabilities* m_caps;

		D3D11RenderTarget** m_cachedRenderTargets;
	};

	class D3D11Capabilities : public Capabilities
	{
	public:
		D3D11Capabilities(D3D11RenderDevice* device);

		virtual bool SupportsRenderTarget(uint multisampleCount, PixelFormat pixFormat, DepthFormat depthFormat);
		virtual bool SupportsPixelShader(const char* implType, int majorVer, int minorVer);
		virtual bool SupportsVertexShader(const char* implType, int majorVer, int minorVer);

		virtual int GetMRTCount();
	private:
		D3D11RenderDevice* m_device;
	};

	class D3D11ObjectFactory : public ObjectFactory
	{
	public:
		D3D11ObjectFactory(D3D11RenderDevice* dev);
		~D3D11ObjectFactory();

		virtual Texture* CreateTexture(const String& filePath, TextureUsage usage, bool managed);
		virtual Texture* CreateTexture(int width, int height, int levelCount, TextureUsage usage, PixelFormat format);
		virtual Texture* CreateTexture(int width, int height, int depth, int levelCount, TextureUsage usage, PixelFormat format);
		virtual Texture* CreateTexture(int length, int levelCount, TextureUsage usage, PixelFormat format);

		virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt, uint sampleCount);
		virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt, DepthFormat depthFmt);
		virtual RenderTarget* CreateRenderTarget(int width, int height, PixelFormat clrFmt);

		virtual IndexBuffer* CreateIndexBuffer(IndexBufferType type, int count, BufferUsageFlags usage);
		virtual VertexBuffer* CreateVertexBuffer(int vertexCount, VertexDeclaration* vtxDecl, BufferUsageFlags usage);

		virtual VertexDeclaration* CreateVertexDeclaration(const os::Vector<VertexElement> &elements);

		virtual VertexShader* CreateVertexShader(const byte* byteCode);
		virtual PixelShader* CreatePixelShader(const byte* byteCode);

		virtual Sprite* CreateSprite();
	private:
		D3D11RenderDevice* m_device;
	};
}
