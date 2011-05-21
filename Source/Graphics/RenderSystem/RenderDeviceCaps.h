
/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

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
#ifndef RENDERDEVICECAPS_H
#define RENDERDEVICECAPS_H

#include "Common.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			//enum CompareCaps
			//{
			//	COMPCAP_Always = 128,
			//	COMPCAP_Equal = 4,
			//	COMPCAP_Greater = 16,
			//	COMPCAP_GreaterEqual = 64,
			//	COMPCAP_Less = 2,
			//	COMPCAP_LessEqual = 8,
			//	COMPCAP_Never = 1,
			//	COMPCAP_NotEqual = 32
			//};
			//enum StencilCaps
			//{
			//	STCCAP_Decrement = 128,
			//	STCCAP_DecrementClamp = 16,
			//	STCCAP_Increment = 64,
			//	STCCAP_IncrementClamp = 8,
			//	STCCAP_Invert = 32,
			//	STCCAP_Keep = 1,
			//	STCCAP_Replace = 4,
			//	STCCAP_TwoSided = 256,
			//	STCCAP_Zero = 2
			//};
			//enum ShadeCaps
			//{
			//	SHDCAP_AlphaGouraudBlend = 16384,
			//	SHDCAP_ColorGouraudRgb = 8,
			//	SHDCAP_FogGouraud = 524288,
			//	SHDCAP_SpecularGouraudRgb = 512
			//};
			//enum TextureCaps
			//{
			//	TEXCAP_Alpha = 4,
			//	TEXCAP_AlphaPalette = 128,
			//	TEXCAP_CubeMap = 2048,
			//	TEXCAP_CubeMapPow2 = 131072,
			//	TEXCAP_MipCubeMap = 65536,
			//	TEXCAP_MipMap = 16384,
			//	TEXCAP_MipVolumeMap = 32768,
			//	TEXCAP_NonPow2Conditional = 256,
			//	TEXCAP_NoProjectedBumpEnvironment = 2097152,
			//	TEXCAP_Perspective = 1,
			//	TEXCAP_Pow2 = 2,
			//	TEXCAP_Projected = 1024,
			//	TEXCAP_SquareOnly = 32,
			//	TEXCAP_TextureRepeatNotScaledBySize = 64,
			//	TEXCAP_VolumeMap = 8192,
			//	TEXCAP_VolumeMapPow2 = 262144
			//};
			//enum TextureAddressCaps
			//{
			//	TACAP_Border = 8,
			//	TACAP_Clamp = 4,
			//	TACAP_IndependentUV = 16,
			//	TACAP_Mirror = 2,
			//	TACAP_MirrorOnce = 32,
			//	TACAP_Wrap = 1
			//};
			//enum VertexProcessingCaps
			//{
			//	VPCAP_DirectionalLights = 8,
			//	VPCAP_LocalViewer = 32,
			//	VPCAP_MaterialSource7 = 2,
			//	VPCAP_NoTexGenNonLocalViewer = 512,
			//	VPCAP_PositionalLights = 16,
			//	VPCAP_TexGenSphereMap = 256,
			//	VPCAP_TextureGen = 1,
			//	VPCAP_Tweening = 64
			//};

			//enum VertexShaderCaps
			//{
			//	VSCAP_None,
			//	VSCAP_Predication
			//};

			//enum DeclarationTypeCaps
			//{
			//	VDECLCAP_UInt101010N = 128,
			//	VDECLCAP_HalfVector4 = 512,
			//	VDECLCAP_HalfVector2 = 256,
			//	VDECLCAP_Short2N = 4,
			//	VDECLCAP_Short4N = 8,
			//	VDECLCAP_UByte4 = 1,
			//	VDECLCAP_UByte4N = 2,
			//	VDECLCAP_UInt101010 = 64,
			//	VDECLCAP_UShort2N = 16,
			//	VDECLCAP_UShort4N = 32
			//};

			//enum BlendCaps
			//{
			//	BLENDCAP_BlendFactor = 8192,
			//	BLENDCAP_BothInverseSourceAlpha = 4096,
			//	BLENDCAP_DestinationAlpha = 64,
			//	BLENDCAP_DestinationColor = 256,
			//	BLENDCAP_InverseDestinationAlpha = 128,
			//	BLENDCAP_InverseDestinationColor = 512,
			//	BLENDCAP_InverseSourceAlpha = 32,
			//	BLENDCAP_InverseSourceColor = 8,
			//	BLENDCAP_One = 2,
			//	BLENDCAP_SourceAlpha = 16,
			//	BLENDCAP_SourceAlphaSaturated = 1024,
			//	BLENDCAP_SourceColor = 4,
			//	BLENDCAP_Zero = 1
			//};

			//enum DeviceCaps
			//{
			//	DEVCAP_CanRenderSysToNonLocal = 131072,
			//	DEVCAP_CanRenderAfterFlip = 2048,
			//	DEVCAP_DrawPrimitives2 = 8192,
			//	DEVCAP_DrawPrimitives2Extended = 32768,
			//	DEVCAP_DrawPrimTLVertex = 1024,
			//	DEVCAP_ExecuteSystemMemory = 16,
			//	DEVCAP_ExecuteVideoMemory = 32,
			//	DEVCAP_HWRasterization = 524288,
			//	DEVCAP_HWTransformAndLight = 65536,
			//	//NPatches = 16777216,
			//	DEVCAP_PureDevice = 1048576,
			//	//QuinticRTPatches = 2097152,
			//	//RTPatches = 4194304,
			//	//RTPatchHandleZero = 8388608,
			//	DEVCAP_SeparateTextureMemory = 16384,
			//	DEVCAP_TextureNonLocalVideoMemory = 4096,
			//	DEVCAP_TextureSystemMemory = 256,
			//	DEVCAP_TextureVideoMemory = 512,
			//	DEVCAP_TLVertexSystemMemory = 64,
			//	DEVCAP_TLVertexVideoMemory = 128,
			//	DEVCAP_CanStretchRectFromTextures,
			//	DEVCAP_StreamOffset,
			//	DEVCAP_VertexElementsCanShareStreamOffset

			//};

			//enum FilterCaps
			//{
			//	FLTCAP_MagAnisotropic = 67108864,
			//	FLTCAP_MagGaussianQuad = 268435456,
			//	FLTCAP_MagLinear = 33554432,
			//	FLTCAP_MagPoint = 16777216,
			//	FLTCAP_MagPyramidalQuad = 134217728,
			//	FLTCAP_MinAnisotropic = 1024,
			//	FLTCAP_MinGaussianQuad = 4096,
			//	FLTCAP_MinLinear = 512,
			//	FLTCAP_MinPoint = 256,
			//	FLTCAP_MinPyramidalQuad = 2048,
			//	FLTCAP_MipLinear = 131072,
			//	FLTCAP_MipPoint = 65536
			//};

			//enum LineCaps
			//{
			//	LNCAP_AlphaCompare = 8,
			//	LNCAP_Antialias = 32,
			//	LNCAP_Blend = 4,
			//	LNCAP_DepthTest = 2,
			//	LNCAP_Fog = 16,
			//	LNCAP_Texture = 1
			//};

			//enum PixelShaderCaps
			//{
			//	PSCAP_ArbitrarySwizzle = 1,
			//	PSCAP_GradientInstructions = 2,
			//	PSCAP_NoDependentReadLimit = 8,
			//	PSCAP_None = 0,
			//	PSCAP_NoTextureInstructionLimit = 16,
			//	PSCAP_Predication = 4
			//};

			//enum PrimitiveMiscCaps
			//{
			//	PRIMCAP_BlendOperation = 2048,
			//	PRIMCAP_ClipPlanesScaledPoints = 256,
			//	PRIMCAP_ClipTLVertices = 512,
			//	PRIMCAP_ColorWriteEnable = 128,
			//	PRIMCAP_CullCCW = 64,
			//	PRIMCAP_CullCW = 32,
			//	PRIMCAP_CullNone = 16,
			//	PRIMCAP_FogAndSpecularAlpha = 65536,
			//	PRIMCAP_FogVertexClamped = 1048576,
			//	PRIMCAP_IndependentWriteMasks = 16384,
			//	PRIMCAP_MaskZ = 2,
			//	PRIMCAP_MrtIndependentBitDepths = 262144,
			//	PRIMCAP_MrtPostPixelShaderBlending = 524288,
			//	PRIMCAP_NullReference = 4096,
			//	PRIMCAP_PerStageConstant = 32768,
			//	PRIMCAP_PostBlendSrgbConvert = 2097152,
			//	PRIMCAP_SeparateAlphaBlend = 131072,
			//	PRIMCAP_TssArgTemp = 1024
			//};

			//enum RasterCaps
			//{
			//	Anisotropy = 131072,
			//	ColorPerspective = 4194304,
			//	DepthBias = 67108864,
			//	DepthTest = 16,
			//	Dither = 1,
			//	FogRange = 65536,
			//	FogTable = 256,
			//	FogVertex = 128,
			//	MipMapLodBias = 8192,
			//	MultisampleToggle = 134217728,
			//	ScissorTest = 16777216,
			//	SlopeScaleDepthBias = 33554432,
			//	WBuffer = 262144,
			//	WFog = 1048576,
			//	ZBufferLessHsr = 32768,
			//	ZFog = 2097152
			//};

			class APAPI Capabilities
			{

			};
		}
	}
}
#endif