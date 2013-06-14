#include "D3D11DeviceSettings.h"

namespace rex
{
	D3D11DeviceSettings::D3D11DeviceSettings()
	{
		memset(this, 0, sizeof(D3D11DeviceSettings));

		AdapterOrdinal = 0;
		AutoCreateDepthStencil = true;
		AutoDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
#if defined(DEBUG) || defined(_DEBUG)
		CreateFlags |= D3D10_CREATE_DEVICE_DEBUG;
#else
		CreateFlags = 0;
#endif
		DriverType = D3D_DRIVER_TYPE_HARDWARE;
		Output = 0;
		PresentFlags = 0;
		sd.BufferCount = 2;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Width = 0;
		sd.BufferUsage = 32;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;
		sd.OutputWindow = 0;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Windowed = 1;
		SyncInterval = 0;
	}
}