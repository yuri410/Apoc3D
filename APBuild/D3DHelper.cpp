#include "D3DHelper.h"

namespace APBuild
{
	IDirect3DDevice9* D3DHelper::m_device = 0;

	void D3DHelper::Initalize()
	{
		IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

		D3DDISPLAYMODE Mode;
		pD3D->GetAdapterDisplayMode(0, &Mode);

		D3DPRESENT_PARAMETERS pp;
		ZeroMemory( &pp, sizeof(D3DPRESENT_PARAMETERS) ); 
		pp.BackBufferWidth  = 1;
		pp.BackBufferHeight = 1;
		pp.BackBufferFormat = Mode.Format;
		pp.BackBufferCount  = 1;
		pp.SwapEffect       = D3DSWAPEFFECT_COPY;
		pp.Windowed         = TRUE;


		HRESULT hr = pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, GetConsoleWindow(), 
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE, &pp, &m_device );
		assert(SUCCEEDED(hr));
		pD3D->Release();		
	}
	void D3DHelper::Finalize()
	{
		assert(m_device);
		
		m_device->Release();
	}

}
