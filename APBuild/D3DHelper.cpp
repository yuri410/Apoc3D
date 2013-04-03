/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
#include "D3DHelper.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")

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
