#pragma once

#include "apoc3D/ApocCommon.h"

using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	struct PngSaveContext { };

	PngSaveContext* BeginStreamPng(int32 w, int32 h, FileOutStream& strm);
	void StreamInPng(PngSaveContext* ctx, RenderTarget* rt, int32 startY, int32 height, bool removeAlpha);
	void EndStreamPng(PngSaveContext* ctx);

	void SavePng(RenderTarget* rt, FileOutStream& strm, bool removeAlpha);
	Texture* LoadPngTexture(RenderDevice* device, const ResourceLocation& rl);
}
