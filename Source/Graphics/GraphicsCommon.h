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


#ifndef GR_ENUMS_H
#define GR_ENUMS_H

#pragma once

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			enum LockMode
			{
				LOCK_None,
				LOCK_Discard,
				LOCK_ReadOnly,
				LOCK_NoOverwrite
			};

			/* 
			*/
			enum BufferUsage
			{
				BU_Static = 1,
				BU_Dynamic = 2,
				BU_WriteOnly = 4,
				BU_Discardable = 8
			};
			/* 
			*/
			enum CullMode
			{
				/// <summary>
				///  Do not cull back faces.
				/// </summary>
				None = 1,
				/// <summary>
				///  Cull back faces with clockwise vertices.
				/// </summary>
				Clockwise = 2,
				/// <summary>
				///  Cull back faces with counterclockwise vertices.
				/// </summary>
				CounterClockwise = 3,
			};
			/* 
			*/
			enum IndexBufferType    
			{
				IBT_Bit16,
				IBT_Bit32
			};
			enum PrimitiveType
			{
				/* Render the vertices as individual points.
				*/
				PT_PointList = 1,
				/* Render the vertices as a series of individual lines.
				*/
				PT_LineList = 2,
				/* Render the vertices as a continuous line.
				*/
				PT_LineStrip = 3,
				/* Render the vertices as a series of individual triangles.
				*/
				PT_TriangleList = 4,				
				/* Render the vertices as a continous set of triangles in a zigzag type fashion.
				*/
				PT_TriangleStrip = 5,
				/* Render the vertices as a set of trinagles in a fan like formation.
				*/
				PT_TriangleFan = 6
			};

			/// <summary>
			///  表示顶点元素中的类型
			/// </summary>
			/// <remarks>可以直接转换的API：XNA</remarks>
			enum VertexElementFormat
			{
				/// <summary>
				///  一个成员，32位浮点数，可以自动展开为 (float, 0, 0, 1)。
				/// </summary>
				VEF_Single = 0,
				/// <summary>
				///  两个成员，32位浮点数，可以自动展开为 (float, float, 0, 1)。
				/// </summary>
				VEF_Vector2 = 1,
				/// <summary>
				///  三个成员，32位浮点数，可以自动展开为 (float, float, float, 1)。
				/// </summary>
				VEF_Vector3 = 2,
				/// <summary>
				///  四个成员，32位浮点数，可以自动展开为 (float, float, float, float)。
				/// </summary>
				VEF_Vector4 = 3,
				/// <summary>
				///  四个成员，打包后的byte，映射到了0-1范围内。输入为Int32类型(ARGB)，会扩展为(R, G, B, A)。
				/// </summary>
				VEF_Color = 4,
				/// <summary>
				///  四个成员，byte。
				/// </summary>
				VEF_Byte4 = 5,
				/// <summary>
				///  Two-component, signed short expanded to (value, value, 0, 1).
				/// </summary>
				VEF_Short2 = 6,
				/// <summary>
				///  Four-component, signed short expanded to (value, value, value, value).
				/// </summary>
				VEF_Short4 = 7,
				/// <summary>
				///  Four-component byte with each byte normalized by dividing the component with
				///     255.0f. This type is valid for vertex shader version 2.0 or higher.
				/// </summary>
				VEF_Rgba32 = 8,
				/// <summary>
				///  Normalized, two-component, signed short, expanded to (first short/32767.0,
				///     second short/32767.0, 0, 1). This type is valid for vertex shader version
				///     2.0 or higher.
				/// </summary>
				VEF_NormalizedShort2 = 9,
				/// <summary>
				///  Normalized, four-component, signed short, expanded to (first short/32767.0,
				///     second short/32767.0, third short/32767.0, fourth short/32767.0). This type
				///     is valid for vertex shader version 2.0 or higher.
				/// </summary>
				VEF_NormalizedShort4 = 10,
				/// <summary>
				///  Normalized, two-component, unsigned short, expanded to (first byte/65535.0,
				///     second byte/65535.0, 0, 1). This type is valid for vertex shader version
				///     2.0 or higher.
				/// </summary>
				VEF_Rg32 = 11,
				/// <summary>
				///  Normalized, four-component, unsigned short, expanded to (first byte/65535.0,
				///     second byte/65535.0, third byte/65535.0, fourth byte/65535.0). This type
				///     is valid for vertex shader version 2.0 or higher.
				/// </summary>
				VEF_Rgba64 = 12,
				/// <summary>
				///  Three-component, unsigned, 10 10 10 format expanded to (value, value, value, 1).
				/// </summary>
				VEF_UInt101010 = 13,
				/// <summary>
				///  Three-component, signed, 10 10 10 format normalized and expanded to (v[0]/511.0,
				///     v[1]/511.0, v[2]/511.0, 1).
				/// </summary>
				VEF_Normalized101010 = 14,
				/// <summary>
				///  Two-component, 16-bit floating point expanded to (value, value, value, value).
				///     This type is valid for vertex shader version 2.0 or higher.
				/// </summary>
				VEF_HalfVector2 = 15,
				/// <summary>
				///  Four-component, 16-bit floating-point expanded to (value, value, value, value).
				///     This type is valid for vertex shader version 2.0 or higher.
				/// </summary>
				VEF_HalfVector4 = 16,
				/// <summary>
				///  Type field in the declaration is unused. This is designed for use with VertexElementMethod.UV
				///     and VertexElementMethod.LookUpPresampled.
				/// </summary>
				VEF_Unused = 17,
			};

			/// <summary>
			/// 
			/// </summary>
			/// <remarks>可以直接转换的API：XNA</remarks>
			enum VertexElementUsage
			{
				VEU_Binormal = 7,
				VEU_BlendIndices = 2,
				VEU_BlendWeight = 1,
				VEU_Color = 10,
				VEU_Depth = 12,
				VEU_Fog = 11,
				VEU_Normal = 3,
				VEU_PointSize = 4,
				VEU_Position = 0,
				VEU_PositionTransformed = 9,
				VEU_Sample = 13,
				VEU_Tangent = 6,
				VEU_TessellateFactor = 8,
				VEU_TextureCoordinate = 5
			};
			
			/* Specifies how a texture is to be used in the engine.
			*/
			enum TextureUsage
			{
				TU_Static = BU_Static,
				TU_Dynamic = BU_Dynamic,
				TU_WriteOnly = BU_WriteOnly,
				TU_StaticWriteOnly = BU_Static | BU_WriteOnly,
				TU_DynamicWriteOnly = BU_Dynamic | BU_WriteOnly,
				TU_Discardable = BU_Discardable,
				/// <summary>
				///    Mipmaps will be automatically generated for this texture
				///	 </summary>
				TU_AutoMipMap = 0x100,
				/// <summary>
				///    This texture will be a render target, ie. used as a target for render to texture
				///    setting this flag will ignore all other texture usages except AutoMipMap
				///	 </summary>
				TU_RenderTarget = 0x200,
				/// <summary>
				///    Default to static textures
				///	</summary>
				TU_Default = TU_StaticWriteOnly
			};


		}
	}
}
#endif