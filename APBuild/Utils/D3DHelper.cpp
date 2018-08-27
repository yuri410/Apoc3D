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

#include "D3DHelper.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "dxerr.lib")

namespace APBuild
{
	namespace Utils
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
			m_device = nullptr;
		}

	}
	
}
