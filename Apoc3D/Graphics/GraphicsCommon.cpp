/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
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

#include "GraphicsCommon.h"

#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			const TypeDualConverter<Blend> BlendConverter = 
			{
				{ Blend::Zero, L"Zero" },
				{ Blend::One, L"One" },
				{ Blend::SourceColor, L"SourceColor" },
				{ Blend::InverseSourceColor, L"InverseSourceColor" },
				{ Blend::SourceAlpha, L"SourceAlpha" },
				{ Blend::InverseSourceAlpha, L"InverseSourceAlpha" },
				{ Blend::DestinationAlpha, L"DestinationAlpha" },
				{ Blend::InverseDestinationAlpha, L"InverseDestinationAlpha" },
				{ Blend::DestinationColor, L"DestinationColor" },
				{ Blend::InverseDestinationColor, L"InverseDestinationColor" },
				{ Blend::SourceAlphaSaturation, L"SourceAlphaSaturation" },
				{ Blend::BlendFactor, L"BlendFactor" },
			};

			const TypeDualConverter<BlendFunction> BlendFunctionConverter = 
			{
				{ BlendFunction::Add, L"Add" },
				{ BlendFunction::Subtract, L"Subtract" },
				{ BlendFunction::ReverseSubtract, L"ReverseSubtract" },
				{ BlendFunction::Min, L"Min" },
				{ BlendFunction::Max, L"Max" }
			};

			const TypeDualConverter<CullMode> CullModeConverter = 
			{
				{ CullMode::None, L"None" },
				{ CullMode::Clockwise, L"Clockwise" },
				{ CullMode::CounterClockwise, L"CounterClockwise" }
			};

			const TypeDualConverter<TextureAddressMode> TextureAddressModeConverter = 
			{
				{ TextureAddressMode::Wrap, L"Wrap" },
				{ TextureAddressMode::Mirror, L"Mirror" },
				{ TextureAddressMode::Clamp, L"Clamp" },
				{ TextureAddressMode::Border, L"Border" },
				{ TextureAddressMode::MirrorOnce, L"MirrorOnce" }
			};

			const TypeDualConverter<TextureFilter> TextureFilterConverter = 
			{
				{ TextureFilter::None, L"None" },
				{ TextureFilter::Point, L"Point" },
				{ TextureFilter::Linear, L"Linear" },
				{ TextureFilter::Anisotropic, L"Anisotropic" }

			};

			const TypeDualConverter<ShaderType> ShaderTypeConverter = 
			{
				{ ShaderType::Pixel, L"PixelShader" },
				{ ShaderType::Vertex, L"VertexShader" },
				{ ShaderType::Geometry, L"GeometryShader" },
				{ ShaderType::All, L"All" }
			};

			const TypeDualConverter<VertexElementUsage> VertexElementUsageConverter = 
			{
				{ VEU_Position, L"VEU_Position" },
				{ VEU_BlendWeight, L"VEU_BlendWeight" },
				{ VEU_BlendIndices, L"VEU_BlendIndices" },
				{ VEU_Normal, L"VEU_Normal" },
				{ VEU_PointSize, L"VEU_PointSize" },
				{ VEU_TextureCoordinate, L"VEU_TextureCoordinate" },
				{ VEU_Tangent, L"VEU_Tangent" },
				{ VEU_Binormal, L"VEU_Binormal" },
				{ VEU_TessellateFactor, L"VEU_TessellateFactor" },
				{ VEU_PositionTransformed, L"VEU_PositionTransformed" },
				{ VEU_Color, L"VEU_Color" },
				{ VEU_Fog, L"VEU_Fog" },
				{ VEU_Depth, L"VEU_Depth" },
				{ VEU_Sample, L"VEU_Sample" }
			};

		}
	}
}