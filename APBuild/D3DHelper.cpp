#include "D3DHelper.h"

namespace APBuild
{
	void D3DHelper::Initalize()
	{
		m_d3d = Direct3DCreate9(DIRECT3D_VERSION);

		D3DPRESENT_PARAMETERS pm;
		memset(&pm,0,sizeof(pm));
		pm.Windowed = TRUE;
		pm.SwapEffect = D3DSWAPEFFECT_DISCARD;
		HRESULT hr = m_d3d->CreateDevice(0, D3DDEVTYPE_REF, 0, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pm, &m_device);
		assert(SUCCEEDED(hr));
	}
	void D3DHelper::Finalize()
	{
		m_device->Release();
		m_d3d->Release();
	}

}
