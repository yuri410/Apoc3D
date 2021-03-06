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

#include "D3D9Utils.h"
#include "apoc3d/Graphics/RenderSystem/RenderTarget.h"
#include "apoc3d/Core/Logging.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			// pre-calculated fast type cast tables
			static D3DPRIMITIVETYPE ptTable[(int)PrimitiveType::Count];
			static D3DDECLTYPE vefTable[VEF_Count];
			static D3DDECLUSAGE veuTable[VEU_Count];
			static D3DFILLMODE fillTable[(int)FillMode::Count];
			static D3DCULL cullTable[(int)CullMode::Count];
			static D3DBLEND blendTable[(int)Blend::Count];
			static D3DCMPFUNC comfunTable[(int)CompareFunction::Count];
			static D3DBLENDOP blendopTable[(int)BlendFunction::Count];
			static D3DSTENCILOP stencilTable[(int)StencilOperation::Count];
			static D3DFORMAT pixFmtTable[FMT_Count];
			static D3DFORMAT depFmtTable[DEPFMT_Count];
			static D3DTEXTUREFILTERTYPE tfltTable[(int)TextureFilter::Count];
			static D3DCUBEMAP_FACES cubeTable[CUBE_Count];
			static D3DTEXTUREADDRESS taTable[(int)TextureAddressMode::Count];
			
			struct FieldInitilizer
			{
				FieldInitilizer();
			private:
				void InitPrimitiveTable();
				void InitVertexElementFormat();
				void InitVertexElementUsage();
				void InitFillTable();
				void InitCullTable();
				void InitBlendTable();
				void InitCompareFunctionTable();
				void InitBlendOperationTable();
				void InitStencilTable();
				void InitPixelFormatTable();
				void InitDepthFormatTable();
				void InitTFLTTable();
				void InitCubeTable();
				void InitTATable();

			} static s_initializer;

			BufferUsageFlags D3D9Utils::GetBufferUsage(DWORD usage)
			{
				uint32 result = 0;
				if ((usage & D3DUSAGE_WRITEONLY) == D3DUSAGE_WRITEONLY)
				{
					result |= BU_WriteOnly;
				}
				if ((usage & D3DUSAGE_DYNAMIC) == D3DUSAGE_DYNAMIC)
				{
					result |= BU_Dynamic;
				}
				return static_cast<BufferUsageFlags>(result);
			}
			PixelFormat D3D9Utils::ConvertBackPixelFormat(DWORD fmt)
			{
				switch (fmt)
				{
					case D3DFMT_A2R10G10B10: return FMT_A2R10G10B10;
					case D3DFMT_A8R8G8B8: return FMT_A8R8G8B8;
					case D3DFMT_X8R8G8B8: return FMT_X8R8G8B8;
					case D3DFMT_A1R5G5B5: return FMT_A1R5G5B5;
					case D3DFMT_X1R5G5B5: return FMT_X1R5G5B5;
					case D3DFMT_R5G6B5: return FMT_R5G6B5;

					case D3DFMT_DXT1: return FMT_DXT1;
					case D3DFMT_DXT2: return FMT_DXT2;
					case D3DFMT_DXT3: return FMT_DXT3;
					case D3DFMT_DXT4: return FMT_DXT4;
					case D3DFMT_DXT5: return FMT_DXT5;

					case D3DFMT_R16F: return FMT_R16F;
					case D3DFMT_G16R16F: return FMT_G16R16F;
					case D3DFMT_A16B16G16R16F: return FMT_A16B16G16R16F;

					case D3DFMT_R32F: return FMT_R32F;
					case D3DFMT_G32R32F: return FMT_G32R32F;
					case D3DFMT_A32B32G32R32F: return FMT_A32B32G32R32F;

					case D3DFMT_R8G8B8: return FMT_R8G8B8;
					case D3DFMT_A4R4G4B4: return FMT_A4R4G4B4;
					case D3DFMT_R3G3B2: return FMT_R3G3B2;
					case D3DFMT_A8: return FMT_Alpha8;
					case D3DFMT_A2B10G10R10: return FMT_A2B10G10R10;
					case D3DFMT_G16R16: return FMT_G16R16;
					case D3DFMT_A16B16G16R16: return FMT_A16B16G16R16;
					case D3DFMT_A8P8: return FMT_Palette8Alpha8;
					case D3DFMT_P8: return FMT_Palette8;
					case D3DFMT_L8: return FMT_Luminance8;
					case D3DFMT_L16: return FMT_Luminance16;
					case D3DFMT_A8L8: return FMT_A8L8;
					case D3DFMT_A4L4: return FMT_A4L4;
					case D3DFMT_A1: return FMT_Alpha1;

					case D3DFMT_X4R4G4B4:
					case D3DFMT_A8R3G3B2:
						return FMT_Unknown;
				}
				AP_EXCEPTION(ErrorID::NotSupported, L"ConvertBackPixelFormat");
				return FMT_Unknown;
			}
			DepthFormat D3D9Utils::ConvertBackDepthFormat(DWORD format)
			{
				switch (format)
				{
					case D3DFMT_D15S1:			return DEPFMT_Depth15Stencil1;
					case D3DFMT_D16:			return DEPFMT_Depth16;
					case D3DFMT_D16_LOCKABLE:	return DEPFMT_Depth16Lockable;
					case D3DFMT_D24X8:			return DEPFMT_Depth24X8;
					case D3DFMT_D24X4S4:		return DEPFMT_Depth24Stencil4;
					case D3DFMT_D24S8:			return DEPFMT_Depth24Stencil8;
					case D3DFMT_D24FS8:			return DEPFMT_Depth24Stencil8Single;
					case D3DFMT_D32:			return DEPFMT_Depth32;
					case D3DFMT_D32_LOCKABLE:	return DEPFMT_Depth32Lockable;
				}
				AP_EXCEPTION(ErrorID::NotSupported, L"ConvertBackDepthFormat");
				return DEPFMT_Depth16;
			}

			TextureUsage ConvertBackTextureUsage(DWORD usage)
			{
				uint result = 0;
				if ((usage & D3DUSAGE_AUTOGENMIPMAP) == D3DUSAGE_AUTOGENMIPMAP)
				{
					result |= TU_AutoMipMap;
				}
				if ((usage & D3DUSAGE_DYNAMIC) == D3DUSAGE_DYNAMIC)
				{
					result |= TU_Dynamic;
				}

				return static_cast<TextureUsage>(result);
			}

			uint32 D3D9Utils::ConvertBackMultiSample(D3DMULTISAMPLE_TYPE type)
			{
				switch (type)
				{
					case D3DMULTISAMPLE_NONE:		return 0;
					case D3DMULTISAMPLE_2_SAMPLES:	return 2;
					case D3DMULTISAMPLE_3_SAMPLES:	return 3;
					case D3DMULTISAMPLE_4_SAMPLES:	return 4;
					case D3DMULTISAMPLE_5_SAMPLES:	return 5;
					case D3DMULTISAMPLE_6_SAMPLES:	return 6;
					case D3DMULTISAMPLE_7_SAMPLES:	return 7;
					case D3DMULTISAMPLE_8_SAMPLES:	return 8;
					case D3DMULTISAMPLE_9_SAMPLES:	return 9;
					case D3DMULTISAMPLE_10_SAMPLES:	return 10;
					case D3DMULTISAMPLE_11_SAMPLES:	return 11;
					case D3DMULTISAMPLE_12_SAMPLES:	return 12;
					case D3DMULTISAMPLE_13_SAMPLES:	return 13;
					case D3DMULTISAMPLE_14_SAMPLES:	return 14;
					case D3DMULTISAMPLE_15_SAMPLES:	return 15;
					case D3DMULTISAMPLE_16_SAMPLES:	return 16;
				}
				// to make the compiler happy
				return 0;
			}
			D3DMULTISAMPLE_TYPE D3D9Utils::ConvertMultisample(uint32 sampleCount)
			{
				switch (sampleCount)
				{
					case 0:
						return D3DMULTISAMPLE_NONE;
					case 1:
					case 2:
						return D3DMULTISAMPLE_2_SAMPLES;

					case 3:		return D3DMULTISAMPLE_3_SAMPLES;
					case 4:		return D3DMULTISAMPLE_4_SAMPLES;
					case 5:		return D3DMULTISAMPLE_5_SAMPLES;
					case 6:		return D3DMULTISAMPLE_6_SAMPLES;
					case 7:		return D3DMULTISAMPLE_7_SAMPLES;
					case 8:		return D3DMULTISAMPLE_8_SAMPLES;
					case 9:		return D3DMULTISAMPLE_9_SAMPLES;
					case 10:	return D3DMULTISAMPLE_10_SAMPLES;
					case 11:	return D3DMULTISAMPLE_11_SAMPLES;
					case 12:	return D3DMULTISAMPLE_12_SAMPLES;
					case 13:	return D3DMULTISAMPLE_13_SAMPLES;
					case 14:	return D3DMULTISAMPLE_14_SAMPLES;
					case 15:	return D3DMULTISAMPLE_15_SAMPLES;
					case 16:	return D3DMULTISAMPLE_16_SAMPLES;
				}
				// to make the compiler happy
				return D3DMULTISAMPLE_NONE;
			}



			DWORD D3D9Utils::ConvertLockMode(LockMode mode)
			{
				DWORD result = 0;
				if ((mode & LOCK_Discard) == LOCK_Discard)
				{
					result |= D3DLOCK_DISCARD;
				}
				if ((mode & LOCK_ReadOnly) == LOCK_ReadOnly)
				{
					result |= D3DLOCK_READONLY;
				}
				if ((mode & LOCK_NoOverwrite) == LOCK_NoOverwrite)
				{
					result |= D3DLOCK_NOOVERWRITE;
				}
				return result;
			}

			DWORD D3D9Utils::ConvertClearFlags(ClearFlags flags)
			{
				DWORD result = 0;
				if ((flags & CLEAR_ColorBuffer) == CLEAR_ColorBuffer)
				{
					result |= D3DCLEAR_TARGET;
				}
				if ((flags & CLEAR_Stencil) == CLEAR_Stencil)
				{
					result |= D3DCLEAR_STENCIL;
				}
				if ((flags & CLEAR_DepthBuffer) == CLEAR_DepthBuffer)
				{
					result |= D3DCLEAR_ZBUFFER;
				}
				return result;
			}

			DWORD D3D9Utils::ConvertBufferUsage(BufferUsageFlags usage)
			{
				DWORD result = 0;
				if ((usage & BU_WriteOnly) == BU_WriteOnly)
				{
					result |= D3DUSAGE_WRITEONLY;
				}
				if ((usage & BU_Dynamic) == BU_Dynamic)
				{
					result |= D3DUSAGE_DYNAMIC;
				}
				if ((usage & BU_PointSpriteVertex) == BU_PointSpriteVertex)
				{
					result |= D3DUSAGE_POINTS;
				}

				return result;
			}

			D3DPRIMITIVETYPE D3D9Utils::ConvertPrimitiveType(PrimitiveType type) { return ptTable[static_cast<int>(type)]; }
			VertexElementFormat D3D9Utils::ConvertBackVertexElementFormat(D3DDECLTYPE type)
			{
				switch (type)
				{
					case D3DDECLTYPE_FLOAT1: return VEF_Single;
					case D3DDECLTYPE_FLOAT2: return VEF_Vector2;
					case D3DDECLTYPE_FLOAT3: return VEF_Vector3;
					case D3DDECLTYPE_FLOAT4: return VEF_Vector4;
					case D3DDECLTYPE_D3DCOLOR:	return VEF_Color;
					case D3DDECLTYPE_UBYTE4:	return VEF_Byte4;
					case D3DDECLTYPE_SHORT2:	return VEF_Short2;
					case D3DDECLTYPE_SHORT4:	return VEF_Short4;
					case D3DDECLTYPE_UBYTE4N:	return VEF_NormalizedByte4;
					case D3DDECLTYPE_SHORT2N:	return VEF_NormalizedShort2;
					case D3DDECLTYPE_SHORT4N:	return VEF_NormalizedShort4;
					case D3DDECLTYPE_UDEC3:		return VEF_UInt101010;
					case D3DDECLTYPE_DEC3N:		return VEF_Normalized101010;
					case D3DDECLTYPE_FLOAT16_2:	return VEF_HalfVector2;
					case D3DDECLTYPE_FLOAT16_4:	return VEF_HalfVector4;
				}
				AP_EXCEPTION(ErrorID::NotSupported, L"ConvertBackVertexElementFormat");
				return VEF_Vector4;
			}
			VertexElementUsage D3D9Utils::ConvertBackVertexElementUsage(D3DDECLUSAGE usage)
			{
				switch (usage)
				{
					case D3DDECLUSAGE_POSITION:		return VEU_Position;
					case D3DDECLUSAGE_BLENDWEIGHT:	return VEU_BlendWeight;
					case D3DDECLUSAGE_BLENDINDICES:	return VEU_BlendIndices;
					case D3DDECLUSAGE_NORMAL:		return VEU_Normal;
					case D3DDECLUSAGE_PSIZE:		return VEU_PointSize;
					case D3DDECLUSAGE_TEXCOORD:		return VEU_TextureCoordinate;
					case D3DDECLUSAGE_TANGENT:		return VEU_Tangent;
					case D3DDECLUSAGE_BINORMAL:		return VEU_Binormal;
					case D3DDECLUSAGE_TESSFACTOR:	return VEU_TessellateFactor;
					case D3DDECLUSAGE_POSITIONT:	return VEU_PositionTransformed;
					case D3DDECLUSAGE_COLOR:		return VEU_Color;
					case D3DDECLUSAGE_FOG:			return VEU_Fog;
					case D3DDECLUSAGE_DEPTH:		return VEU_Depth;
					case D3DDECLUSAGE_SAMPLE:		return VEU_Sample;
				}
				AP_EXCEPTION(ErrorID::NotSupported, L"ConvertBackVertexElementUsage");
				return VEU_Position;
			}

			D3DDECLUSAGE D3D9Utils::ConvertVertexElementUsage(VertexElementUsage usage) { return veuTable[static_cast<int>(usage)]; }
			D3DDECLTYPE D3D9Utils::ConvertVertexElementFormat(VertexElementFormat type) { return vefTable[static_cast<int>(type)]; }
			D3DFILLMODE D3D9Utils::ConvertFillMode(FillMode mode) { return fillTable[static_cast<int>(mode)]; }
			D3DCULL D3D9Utils::ConvertCullMode(CullMode mode) { return cullTable[static_cast<int>(mode)]; }

			D3DBLEND D3D9Utils::ConvertBlend(Blend dv) { return blendTable[static_cast<int>(dv)]; }
			D3DCMPFUNC D3D9Utils::ConvertCompare(CompareFunction fun) { return comfunTable[static_cast<int>(fun)]; }
			D3DBLENDOP D3D9Utils::ConvertBlendFunction(BlendFunction fun) { return blendopTable[static_cast<int>(fun)]; }
			D3DSTENCILOP D3D9Utils::ConvertStencilOperation(StencilOperation op) { return stencilTable[static_cast<int>(op)]; }
			D3DFORMAT D3D9Utils::ConvertPixelFormat(PixelFormat format) { return pixFmtTable[static_cast<int>(format)]; }
			D3DFORMAT D3D9Utils::ConvertDepthFormat(DepthFormat format) { return depFmtTable[static_cast<int>(format)]; }
			D3DTEXTUREFILTERTYPE D3D9Utils::ConvertTextureFilter(TextureFilter filter) { return tfltTable[static_cast<int>(filter)]; }

			DWORD D3D9Utils::ConvertTextureUsage(TextureUsage usage)
			{
				DWORD result = 0;
				
				if ((usage & TU_Dynamic) == TU_Dynamic)
				{
					result |= D3DUSAGE_DYNAMIC;
				}

				if ((usage & TU_AutoMipMap) == TU_AutoMipMap)
				{
					result |= D3DUSAGE_AUTOGENMIPMAP;
				}
				return result;
			}
			D3DCUBEMAP_FACES D3D9Utils::ConvertCubeMapFace(CubeMapFace face) { return cubeTable[static_cast<int>(face)]; }
			D3DTEXTUREADDRESS D3D9Utils::ConvertTextureAddress(TextureAddressMode ta) { return taTable[static_cast<int>(ta)]; }

			//////////////////////////////////////////////////////////////////////////


			int32 D3D9Utils::GetD3DTextureWidth(D3DTexture2D* tex)
			{
				D3DSURFACE_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return (int32)desc.Width;
			}
			int32 D3D9Utils::GetD3DTextureHeight(D3DTexture2D* tex)
			{
				D3DSURFACE_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return (int32)desc.Height;
			}
			PixelFormat D3D9Utils::GetD3DTextureFormat(D3DTexture2D* tex)
			{
				D3DSURFACE_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return ConvertBackPixelFormat(desc.Format);
			}
			TextureUsage D3D9Utils::GetD3DTextureUsage(D3DTexture2D* tex)
			{
				D3DSURFACE_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return ConvertBackTextureUsage(desc.Usage);
			}

			int32 D3D9Utils::GetD3DTextureWidth(D3DTexture3D* tex)
			{
				D3DVOLUME_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return (int32)desc.Width;
			}
			int32 D3D9Utils::GetD3DTextureHeight(D3DTexture3D* tex)
			{
				D3DVOLUME_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return (int32)desc.Height;
			}
			int32 D3D9Utils::GetD3DTextureDepth(D3DTexture3D* tex)
			{
				D3DVOLUME_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return (int32)desc.Depth;
			}
			PixelFormat D3D9Utils::GetD3DTextureFormat(D3DTexture3D* tex)
			{
				D3DVOLUME_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return ConvertBackPixelFormat(desc.Format);
			}
			TextureUsage D3D9Utils::GetD3DTextureUsage(D3DTexture3D* tex)
			{
				D3DVOLUME_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return ConvertBackTextureUsage(desc.Usage);
			}

			int32 D3D9Utils::GetD3DTextureLength(D3DTextureCube* tex)
			{
				D3DSURFACE_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return (int32)desc.Width;
			}
			PixelFormat D3D9Utils::GetD3DTextureFormat(D3DTextureCube* tex)
			{
				D3DSURFACE_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return ConvertBackPixelFormat(desc.Format);
			}
			TextureUsage D3D9Utils::GetD3DTextureUsage(D3DTextureCube* tex)
			{
				D3DSURFACE_DESC desc;
				tex->GetLevelDesc(0, &desc);

				return ConvertBackTextureUsage(desc.Usage);
			}

			int32 D3D9Utils::GetSurfaceWidth(IDirect3DSurface9* surface)
			{
				D3DSURFACE_DESC desc;
				surface->GetDesc(&desc);

				return (int32)desc.Width;
			}
			int32 D3D9Utils::GetSurfaceHeight(IDirect3DSurface9* surface)
			{
				D3DSURFACE_DESC desc;
				surface->GetDesc(&desc);

				return (int32)desc.Height;
			}
			DepthFormat D3D9Utils::GetSurfaceDepthFormat(IDirect3DSurface9* surface)
			{
				D3DSURFACE_DESC desc;
				surface->GetDesc(&desc);
				return D3D9Utils::ConvertBackDepthFormat(desc.Format);
			}

			PixelFormat D3D9Utils::GetSurfaceColorFormat(IDirect3DSurface9* color)
			{
				D3DSURFACE_DESC desc;
				color->GetDesc(&desc);
				return D3D9Utils::ConvertBackPixelFormat(desc.Format);
			}
			uint32 D3D9Utils::GetSurfaceMultiSampleCount(IDirect3DSurface9* color)
			{
				D3DSURFACE_DESC desc;
				color->GetDesc(&desc);
				return D3D9Utils::ConvertBackMultiSample(desc.MultiSampleType);
			}

			void D3D9Utils::logRTFailure(PixelFormat colorFormat, DepthFormat depFormat, const String& multisampleType, bool cubemap)
			{
				String formatString;
				if (colorFormat != FMT_Count)
				{
					formatString.append(L"C-");
					formatString += PixelFormatUtils::ToString(colorFormat);
				}
				if (depFormat != DEPFMT_Count)
				{
					if (formatString.size())
						formatString.append(L", ");
					formatString.append(L"D-");
					formatString += PixelFormatUtils::ToString(depFormat);
				}
				if (!RenderTarget::IsMultisampleModeStringNone(multisampleType))
				{
					if (formatString.size())
						formatString.append(L", ");
					formatString.append(L"MSAA=");
					formatString += multisampleType;
				}
				if (cubemap)
				{
					formatString += L" [Cubemap]";
				}

				ApocLog(LOG_Graphics, L"[D3D9] RenderTarget format not supported: " + formatString, LOGLVL_Error);
			}



			//////////////////////////////////////////////////////////////////////////

			void FieldInitilizer::InitPrimitiveTable()
			{
				ptTable[(int)PrimitiveType::PointList] = D3DPT_POINTLIST;
				ptTable[(int)PrimitiveType::LineList] = D3DPT_LINELIST;
				ptTable[(int)PrimitiveType::LineStrip] = D3DPT_LINESTRIP;
				ptTable[(int)PrimitiveType::TriangleList] = D3DPT_TRIANGLELIST;
				ptTable[(int)PrimitiveType::TriangleStrip] = D3DPT_TRIANGLESTRIP;
				ptTable[(int)PrimitiveType::TriangleFan] = D3DPT_TRIANGLEFAN;
			}
			void FieldInitilizer::InitVertexElementFormat()
			{
				vefTable[VEF_Single] = D3DDECLTYPE_FLOAT1;
				vefTable[VEF_Vector2] = D3DDECLTYPE_FLOAT2;
				vefTable[VEF_Vector3] = D3DDECLTYPE_FLOAT3;
				vefTable[VEF_Vector4] = D3DDECLTYPE_FLOAT4;
				vefTable[VEF_Color] = D3DDECLTYPE_D3DCOLOR;
				vefTable[VEF_Byte4] = D3DDECLTYPE_UBYTE4;
				vefTable[VEF_Short2] = D3DDECLTYPE_SHORT2;
				vefTable[VEF_Short4] = D3DDECLTYPE_SHORT4;
				vefTable[VEF_NormalizedByte4] = D3DDECLTYPE_UBYTE4N;
				vefTable[VEF_NormalizedShort2] = D3DDECLTYPE_SHORT2N;
				vefTable[VEF_NormalizedShort4] = D3DDECLTYPE_SHORT4N;
				vefTable[VEF_UInt101010] = D3DDECLTYPE_UDEC3;
				vefTable[VEF_Normalized101010] = D3DDECLTYPE_DEC3N;
				vefTable[VEF_HalfVector2] = D3DDECLTYPE_FLOAT16_2;
				vefTable[VEF_HalfVector4] = D3DDECLTYPE_FLOAT16_4;
			}
			void FieldInitilizer::InitVertexElementUsage()
			{
				veuTable[VEU_Position] = D3DDECLUSAGE_POSITION;
				veuTable[VEU_BlendWeight] = D3DDECLUSAGE_BLENDWEIGHT;
				veuTable[VEU_BlendIndices] = D3DDECLUSAGE_BLENDINDICES;
				veuTable[VEU_Normal] = D3DDECLUSAGE_NORMAL;
				veuTable[VEU_PointSize] = D3DDECLUSAGE_PSIZE;
				veuTable[VEU_TextureCoordinate] = D3DDECLUSAGE_TEXCOORD;
				veuTable[VEU_Tangent] = D3DDECLUSAGE_TANGENT;
				veuTable[VEU_Binormal] = D3DDECLUSAGE_BINORMAL;
				veuTable[VEU_TessellateFactor] = D3DDECLUSAGE_TESSFACTOR;
				veuTable[VEU_PositionTransformed] = D3DDECLUSAGE_POSITIONT;
				veuTable[VEU_Color] = D3DDECLUSAGE_COLOR;
				veuTable[VEU_Fog] = D3DDECLUSAGE_FOG;
				veuTable[VEU_Depth] = D3DDECLUSAGE_DEPTH;
				veuTable[VEU_Sample] = D3DDECLUSAGE_SAMPLE;

			}
			void FieldInitilizer::InitFillTable()
			{
				fillTable[(int)FillMode::Point] = D3DFILL_POINT;
				fillTable[(int)FillMode::WireFrame] = D3DFILL_WIREFRAME;
				fillTable[(int)FillMode::Solid] = D3DFILL_SOLID;
			}
			void FieldInitilizer::InitCullTable()
			{
				cullTable[(int)CullMode::None] = D3DCULL_NONE;
				cullTable[(int)CullMode::Clockwise] = D3DCULL_CW;
				cullTable[(int)CullMode::CounterClockwise] = D3DCULL_CCW;
			}
			void FieldInitilizer::InitBlendTable()
			{
				blendTable[(int)Blend::Zero] = D3DBLEND_ZERO;
				blendTable[(int)Blend::One] = D3DBLEND_ONE;
				blendTable[(int)Blend::SourceColor] = D3DBLEND_SRCCOLOR;
				blendTable[(int)Blend::InverseSourceColor] = D3DBLEND_INVSRCCOLOR;
				blendTable[(int)Blend::SourceAlpha] = D3DBLEND_SRCALPHA;
				blendTable[(int)Blend::InverseSourceAlpha] = D3DBLEND_INVSRCALPHA;
				blendTable[(int)Blend::DestinationAlpha] = D3DBLEND_DESTALPHA;
				blendTable[(int)Blend::InverseDestinationAlpha] = D3DBLEND_INVDESTALPHA;
				blendTable[(int)Blend::DestinationColor] = D3DBLEND_DESTCOLOR;
				blendTable[(int)Blend::InverseDestinationColor] = D3DBLEND_INVDESTCOLOR;
				blendTable[(int)Blend::SourceAlphaSaturation] = D3DBLEND_SRCALPHASAT;
				blendTable[(int)Blend::BlendFactor] = D3DBLEND_BLENDFACTOR;
			}
			void FieldInitilizer::InitCompareFunctionTable()
			{
				comfunTable[(int)CompareFunction::Never] = D3DCMP_NEVER;
				comfunTable[(int)CompareFunction::Less] = D3DCMP_LESS;
				comfunTable[(int)CompareFunction::Equal] = D3DCMP_EQUAL;
				comfunTable[(int)CompareFunction::LessEqual] = D3DCMP_LESSEQUAL;
				comfunTable[(int)CompareFunction::Greater] = D3DCMP_GREATER;
				comfunTable[(int)CompareFunction::NotEqual] = D3DCMP_NOTEQUAL;
				comfunTable[(int)CompareFunction::GreaterEqual] = D3DCMP_GREATEREQUAL;
				comfunTable[(int)CompareFunction::Always] = D3DCMP_ALWAYS;
			}
			void FieldInitilizer::InitBlendOperationTable()
			{
				blendopTable[(int)BlendFunction::Add] = D3DBLENDOP_ADD;
				blendopTable[(int)BlendFunction::Subtract] = D3DBLENDOP_SUBTRACT;
				blendopTable[(int)BlendFunction::ReverseSubtract] = D3DBLENDOP_REVSUBTRACT;
				blendopTable[(int)BlendFunction::Min] = D3DBLENDOP_MIN;
				blendopTable[(int)BlendFunction::Max] = D3DBLENDOP_MAX;
			}
			void FieldInitilizer::InitStencilTable()
			{
				stencilTable[(int)StencilOperation::Keep] = D3DSTENCILOP_KEEP;
				stencilTable[(int)StencilOperation::Zero] = D3DSTENCILOP_ZERO;
				stencilTable[(int)StencilOperation::Replace] = D3DSTENCILOP_REPLACE;
				stencilTable[(int)StencilOperation::IncrementSaturation] = D3DSTENCILOP_INCRSAT;
				stencilTable[(int)StencilOperation::DecrementSaturation] = D3DSTENCILOP_DECRSAT;
				stencilTable[(int)StencilOperation::Invert] = D3DSTENCILOP_INVERT;
				stencilTable[(int)StencilOperation::Increment] = D3DSTENCILOP_INCR;
				stencilTable[(int)StencilOperation::Decrement] = D3DSTENCILOP_DECR;
			}
			void FieldInitilizer::InitPixelFormatTable()
			{
				pixFmtTable[FMT_Unknown] = D3DFMT_UNKNOWN;
				pixFmtTable[FMT_Luminance8] = D3DFMT_L8;
				pixFmtTable[FMT_Luminance16] = D3DFMT_L16;
				pixFmtTable[FMT_Alpha8] = D3DFMT_A8;
				pixFmtTable[FMT_A4L4] = D3DFMT_A4L4;
				pixFmtTable[FMT_A8L8] = D3DFMT_A8L8;
				pixFmtTable[FMT_R5G6B5] = D3DFMT_R5G6B5;
				pixFmtTable[FMT_B5G6R5] = D3DFMT_UNKNOWN;
				pixFmtTable[FMT_A4R4G4B4] = D3DFMT_A4R4G4B4;
				pixFmtTable[FMT_A1R5G5B5] = D3DFMT_A1R5G5B5;
				pixFmtTable[FMT_R8G8B8] = D3DFMT_R8G8B8;
				pixFmtTable[FMT_B8G8R8] = D3DFMT_UNKNOWN;
				pixFmtTable[FMT_A8R8G8B8] = D3DFMT_A8R8G8B8;
				pixFmtTable[FMT_A8B8G8R8] = D3DFMT_A8B8G8R8;
				pixFmtTable[FMT_B8G8R8A8] = D3DFMT_UNKNOWN;
				pixFmtTable[FMT_A2R10G10B10] = D3DFMT_A2R10G10B10;
				pixFmtTable[FMT_A2B10G10R10] = D3DFMT_A2B10G10R10;
				pixFmtTable[FMT_DXT1] = D3DFMT_DXT1;
				pixFmtTable[FMT_DXT2] = D3DFMT_DXT2;
				pixFmtTable[FMT_DXT3] = D3DFMT_DXT3;
				pixFmtTable[FMT_DXT4] = D3DFMT_DXT4;
				pixFmtTable[FMT_DXT5] = D3DFMT_DXT5;
				pixFmtTable[FMT_A16B16G16R16F] = D3DFMT_A16B16G16R16F;
				pixFmtTable[FMT_A32B32G32R32F] = D3DFMT_A32B32G32R32F;
				pixFmtTable[FMT_X8R8G8B8] = D3DFMT_X8R8G8B8;
				pixFmtTable[FMT_X8B8G8R8] = D3DFMT_X8B8G8R8;
				pixFmtTable[FMT_X1R5G5B5] = D3DFMT_X1R5G5B5;
				pixFmtTable[FMT_R8G8B8A8] = D3DFMT_UNKNOWN;
				pixFmtTable[FMT_Depth] = D3DFMT_UNKNOWN;
				pixFmtTable[FMT_A16B16G16R16] = D3DFMT_A16B16G16R16;
				pixFmtTable[FMT_R3G3B2] = D3DFMT_R3G3B2;
				pixFmtTable[FMT_R16F] = D3DFMT_R16F;
				pixFmtTable[FMT_R32F] = D3DFMT_R32F;
				pixFmtTable[FMT_G16R16] = D3DFMT_G16R16;
				pixFmtTable[FMT_G16R16F] = D3DFMT_G16R16F;
				pixFmtTable[FMT_G32R32F] = D3DFMT_G32R32F;
				pixFmtTable[FMT_R16G16B16] = D3DFMT_UNKNOWN;
				pixFmtTable[FMT_B4G4R4A4] = D3DFMT_UNKNOWN;
				pixFmtTable[FMT_Palette8] = D3DFMT_P8;
				pixFmtTable[FMT_Palette8Alpha8] = D3DFMT_A8P8;
			}
			void FieldInitilizer::InitDepthFormatTable()
			{
				depFmtTable[DEPFMT_Depth15Stencil1] = D3DFMT_D15S1;
				depFmtTable[DEPFMT_Depth16] = D3DFMT_D16;
				depFmtTable[DEPFMT_Depth16Lockable] = D3DFMT_D16_LOCKABLE;
				depFmtTable[DEPFMT_Depth24X8] = D3DFMT_D24X8;
				depFmtTable[DEPFMT_Depth24Stencil4] = D3DFMT_D24X4S4;
				depFmtTable[DEPFMT_Depth24Stencil8] = D3DFMT_D24X8;
				depFmtTable[DEPFMT_Depth24Stencil8Single] = D3DFMT_D24FS8;
				depFmtTable[DEPFMT_Depth32] = D3DFMT_D32;
				depFmtTable[DEPFMT_Depth32Lockable] = D3DFMT_D32_LOCKABLE;
				depFmtTable[DEPFMT_Depth32Single] = D3DFMT_D32F_LOCKABLE;
			}
			void FieldInitilizer::InitTFLTTable()
			{
				tfltTable[(int)TextureFilter::None] = D3DTEXF_NONE;
				tfltTable[(int)TextureFilter::Point] = D3DTEXF_POINT;
				tfltTable[(int)TextureFilter::Linear] = D3DTEXF_LINEAR;
				tfltTable[(int)TextureFilter::Anisotropic] = D3DTEXF_ANISOTROPIC;
			}
			void FieldInitilizer::InitCubeTable()
			{
				cubeTable[CUBE_PositiveX] = D3DCUBEMAP_FACE_POSITIVE_X;
				cubeTable[CUBE_NegativeX] = D3DCUBEMAP_FACE_NEGATIVE_X;
				cubeTable[CUBE_PositiveY] = D3DCUBEMAP_FACE_POSITIVE_Y;
				cubeTable[CUBE_NegativeY] = D3DCUBEMAP_FACE_NEGATIVE_Y;
				cubeTable[CUBE_PositiveZ] = D3DCUBEMAP_FACE_POSITIVE_Z;
				cubeTable[CUBE_NegativeZ] = D3DCUBEMAP_FACE_NEGATIVE_Z;
			}
			void FieldInitilizer::InitTATable()
			{
				taTable[(int)TextureAddressMode::Border] = D3DTADDRESS_BORDER;
				taTable[(int)TextureAddressMode::Clamp] = D3DTADDRESS_CLAMP;
				taTable[(int)TextureAddressMode::Mirror] = D3DTADDRESS_MIRROR;
				taTable[(int)TextureAddressMode::MirrorOnce] = D3DTADDRESS_MIRRORONCE;
				taTable[(int)TextureAddressMode::Wrap] = D3DTADDRESS_WRAP;

			}

			FieldInitilizer::FieldInitilizer()
			{
				InitPrimitiveTable();
				InitVertexElementFormat();
				InitVertexElementUsage();
				InitFillTable();
				InitCullTable();
				InitBlendTable();
				InitCompareFunctionTable();
				InitBlendOperationTable();
				InitStencilTable();
				InitPixelFormatTable();
				InitDepthFormatTable();
				InitTFLTTable();
				InitCubeTable();
				InitTATable();
			}
		}
	}
}