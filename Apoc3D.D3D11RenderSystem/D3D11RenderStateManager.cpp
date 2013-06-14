#include "D3D11RenderStateManager.h"

namespace rex
{
	NativeD3DStateManager::NativeD3DStateManager(D3D11RenderDevice* device)
		: m_device(device), m_needsFlushOverall(false), 
		m_flushBlendState(false), m_flushDepthStencil(false), m_flushRasterizer(false)
	{
		InitializeDefaultState();
	}
	NativeD3DStateManager::~NativeD3DStateManager()
	{

	}

	void NativeD3DStateManager::InitializeDefaultState()
	{

	}

}