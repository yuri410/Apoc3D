#pragma once
#include "D3D11Common.h"

namespace rex
{
	struct D3D11DeviceSettings
	{
		UINT AdapterOrdinal;
		D3D_DRIVER_TYPE DriverType;
		UINT Output;
		DXGI_SWAP_CHAIN_DESC sd;
		UINT32 CreateFlags;
		UINT32 SyncInterval;
		DWORD PresentFlags;
		bool AutoCreateDepthStencil; // will create the depth stencil resource and view if true
		DXGI_FORMAT AutoDepthStencilFormat;
		D3D_FEATURE_LEVEL DeviceFeatureLevel;

		D3D11DeviceSettings();
	};

}