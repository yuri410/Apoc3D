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
			enum IndexBufferType    
			{
				IBT_Bit16,
				IBT_Bit32
			};
			/* 
			*/
			enum CullMode
			{
				/// <summary>
				///  Do not cull back faces.
				/// </summary>
				CULL_None = 1,
				/// <summary>
				///  Cull back faces with clockwise vertices.
				/// </summary>
				CULL_Clockwise = 2,
				/// <summary>
				///  Cull back faces with counterclockwise vertices.
				/// </summary>
				CULL_CounterClockwise = 3,
			};
			enum FillMode
			{
				/* Draw a point at each vertex.
				*/
				FILL_Point = 1,

				/* Draw lines connecting the vertices that define a primitive face.
				*/
				FILL_WireFrame = 2,
				
				/* Draw solid faces for each primitive.
				*/
				FILL_Solid = 3,
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
			enum CubeMapFace
			{
				/* Positive x-face of the cube map.
				*/
				CUBE_PositiveX = 0,
				/* Negative x-face of the cube map.
				*/
				CUBE_NegativeX = 1,
				/* Positive y-face of the cube map.
				*/
				CUBE_PositiveY = 2,
				/* Negative y-face of the cube map.
				*/
				CUBE_NegativeY = 3,
				/* Positive z-face of the cube map.
				*/
				CUBE_PositiveZ = 4,
				/* Negative z-face of the cube map.
				*/
				CUBE_NegativeZ = 5,
			};
			
			enum ClearFlags
			{
				CLEAR_Target = 1,
				CLEAR_DepthBuffer = 2,
				CLEAR_Stencil = 4
			};

			/* Defines stencil buffer operations.
			*/
			enum StencilOperation
			{
				/* Does not update the stencil-buffer entry. This is the default value.
				*/
				STOP_Keep = 1,
				/* Sets the stencil-buffer entry to 0.
				*/
				STOP_Zero = 2,

				/* Replaces the stencil-buffer entry with a reference value.
				*/
				STOP_Replace = 3,
				
				/* Increments the stencil-buffer entry, clamping to the maximum value.
				*/
				STOP_IncrementSaturation = 4,
				
				/*  Decrements the stencil-buffer entry, clamping to 0.
				*/
				STOP_DecrementSaturation = 5,

				/* Inverts the bits in the stencil-buffer entry.
				*/
				STOP_Invert = 6,

				/* Increments the stencil-buffer entry, wrapping to 0 if the new value exceeds
				*  the maximum value.
				*/
				STOP_Increment = 7,
				
				/* Decrements the stencil-buffer entry, wrapping to the maximum value if the
				*  new value is less than 0.
				*/
				STOP_Decrement = 8,
			};

			/* Defines how a texture will be filtered as it is minified for each mipmap level.
			*/
			enum TextureFilter
			{
				/* Mipmapping disabled. The rasterizer uses the magnification filter instead.
				*/
				TFLT_None = 0,

				/*  Point filtering used as a texture magnification or minification filter. The
				*   texel with coordinates nearest to the desired pixel value is used. The texture
				*    filter used between mipmap levels is based on the nearest point; that is,
				*    the rasterizer uses the color from the texel of the nearest mipmap texture.
				*/
				TFLT_Point = 1,

				/*  Bilinear interpolation filtering used as a texture magnification or minification
				*   filter. A weighted average of a 2¡Á2 area of texels surrounding the desired
				*    pixel is used. The texture filter used between mipmap levels is trilinear
				*    mipmap interpolation, in which the rasterizer performs linear interpolation
				*    on pixel color, using the texels of the two nearest mipmap textures.
				*/
				TFLT_Linear = 2,

				/*  Anisotropic texture filtering used as a texture magnification or minification
				*   filter. This type of filtering compensates for distortion caused by the difference
				*    in angle between the texture polygon and the plane of the screen.
				*/
				TFLT_Anisotropic = 3,
				
				/*  A 4-sample tent filter used as a texture magnification or minification filter.
				*/
				TFLT_PyramidalQuad = 6,
				
				/*  A 4-sample Gaussian filter used as a texture magnification or minification filter.
				*/
				TFLT_GaussianQuad = 7,
			};
			/* Defines constants that describe supported texture-addressing modes. 
			*/
			enum TextureAddressMode
			{
				/* Tile the texture at every integer junction. For example, for u values between
				*  0 and 3, the texture is repeated three times; no mirroring is performed.
				*/
				TA_Wrap = 1,

				/* Similar to Wrap, except that the texture is flipped at every integer junction.
				*  For u values between 0 and 1, for example, the texture is addressed normally;
				*  between 1 and 2, the texture is flipped (mirrored); between 2 and 3, the
				*  texture is normal again, and so on.
				*/
				TA_Mirror = 2,        
				
				/* Texture coordinates outside the range [0.0, 1.0] are set to the texture color
				*  at 0.0 or 1.0, respectively.
				*/
				TA_Clamp = 3,        

				/* Texture coordinates outside the range [0.0, 1.0] are set to the border color.
				*/
				TA_Border = 4,
				/* Similar to Mirror and Clamp. Takes the absolute value of the texture coordinate
				*  (thus, mirroring around 0), and then clamps to the maximum value. The most
				*  common usage is for volume textures, where support for the full MirrorOnce
				*  texture-addressing mode is not necessary, but the data is symmetrical around
				*  the one axis.
				*/
				TA_MirrorOnce = 5,
			};

			/* Defines format of vertex element
			*/
			enum VertexElementFormat
			{				
				/* One-component, float expanded to (float, 0, 0, 1)
				*/
				VEF_Single = 0,
				/* Two-component, float expanded to (float, float, 0, 1)
				*/
				VEF_Vector2 = 1,
				/* Three-component, float expanded to (float, float, float, 1)
				*/
				VEF_Vector3 = 2,
				/* Four-component, float expanded to (float, float, float, float)
				*/
				VEF_Vector4 = 3,
				/* Four-component, packed byte to (R, G, B, A)
				*/
				VEF_Color = 4,

				/* Four-component, byte.
				*/
				VEF_Byte4 = 5,
				/* Two-component, signed short expanded to (value, value, 0, 1).
				*/
				VEF_Short2 = 6,
				/* Four-component, signed short expanded to (value, value, value, value).
				*/
				VEF_Short4 = 7,
				
				/* Four-component byte with each byte normalized by dividing the component with
				*  255.0f. This type is valid for vertex shader version 2.0 or higher.
				*/
				VEF_Rgba32 = 8,

				/* Normalized, two-component, signed short, expanded to (first short/32767.0,
				*  second short/32767.0, 0, 1). This type is valid for vertex shader version
				*  2.0 or higher.
				*/
				VEF_NormalizedShort2 = 9,
				/*  Normalized, four-component, signed short, expanded to (first short/32767.0,
				*   second short/32767.0, third short/32767.0, fourth short/32767.0). This type
				*   is valid for vertex shader version 2.0 or higher.
				*/
				VEF_NormalizedShort4 = 10,

				/*  Normalized, two-component, unsigned short, expanded to (first byte/65535.0,
				*   second byte/65535.0, 0, 1). This type is valid for vertex shader version
				*   2.0 or higher.
				*/
				VEF_Rg32 = 11,
				/*  Normalized, four-component, unsigned short, expanded to (first byte/65535.0,
				*   second byte/65535.0, third byte/65535.0, fourth byte/65535.0). This type
				*   is valid for vertex shader version 2.0 or higher.
				*/
				VEF_Rgba64 = 12,
				/*  Three-component, unsigned, 10 10 10 format expanded to (value, value, value, 1).
				*   v[1]/511.0, v[2]/511.0, 1).
				*/
				VEF_UInt101010 = 13,
				/*  Three-component, signed, 10 10 10 format normalized and expanded to (v[0]/511.0,
				*   v[1]/511.0, v[2]/511.0, 1).
				*/
				VEF_Normalized101010 = 14,
				/*  Two-component, 16-bit floating point expanded to (value, value, value, value).
				*   This type is valid for vertex shader version 2.0 or higher.
				*/
				VEF_HalfVector2 = 15,
				/*  Four-component, 16-bit floating-point expanded to (value, value, value, value).
				*   This type is valid for vertex shader version 2.0 or higher.
				*/
				VEF_HalfVector4 = 16,
				/*  Type field in the declaration is unused. 
				*/
				VEF_Unused = 17,
			};


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
				/* Mipmaps will be automatically generated for this texture
				*/
				TU_AutoMipMap = 0x100,
				
				/* This texture will be a render target, ie. used as a target for render to texture
				*  setting this flag will ignore all other texture usages except AutoMipMap
				*/
				TU_RenderTarget = 0x200,
				
				/* Default to static textures
				*/
				TU_Default = TU_StaticWriteOnly
			};


		}
	}
}
#endif