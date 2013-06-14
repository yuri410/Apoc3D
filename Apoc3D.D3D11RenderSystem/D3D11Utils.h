#pragma once

#include "RenderSystem.h"

namespace rex
{
	DXGI_FORMAT dutConvertPixelFormat(PixelFormat fmt);
	DXGI_FORMAT dutConvertDepthFormat(DepthFormat fmt);

	PixelFormat dutConvertBackPixelFormat(DXGI_FORMAT fmt);
	DepthFormat dutConvertBackDepthFormat(DXGI_FORMAT fmt);
}