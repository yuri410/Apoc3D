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


#ifndef APOC3D_GR_ENUMS_H
#define APOC3D_GR_ENUMS_H

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			static const int32 MaxScenePass = 64;

			enum TextureType
			{
				TT_Texture1D = 0,
				TT_Texture2D,
				TT_Texture3D,
				TT_CubeTexture
			};


			enum LockMode
			{
				LOCK_None = 0,
				LOCK_Discard = 1,
				LOCK_ReadOnly = 2,
				LOCK_NoOverwrite = 4,
			};

			/* 
			*/
			enum BufferUsageFlags
			{
				BU_Static = 1,
				BU_Dynamic = 2,
				BU_WriteOnly = 4,
				BU_PointSpriteVertex = 8
			};
			/* 
			*/
			enum IndexBufferType    
			{
				IBT_Bit16 = 0,
				IBT_Bit32 = 1
			};
			/* 
			*/
			enum CullMode
			{
				/** 
				 *  Do not cull back faces.
				 */
				CULL_None = 0,
				
				/**
				 *  Cull back faces with clockwise vertices.
				 */
				CULL_Clockwise = 1,
				
				/**
				 *  Cull back faces with counterclockwise vertices.
				 */
				CULL_CounterClockwise = 2,
				CULL_Count = 3
			};
			enum FillMode
			{
				/**
				 *  Draw a point at each vertex.
				 */
				FILL_Point = 0,

				/**
				 *  Draw lines connecting the vertices that define a primitive face.
				 */
				FILL_WireFrame = 1,
				
				/**
				 *  Draw solid faces for each primitive.
				 */
				FILL_Solid = 2,
				FILL_Count = 3
			};
			
			enum PrimitiveType
			{
				/**
				 *  Render the vertices as individual points.
				 */
				PT_PointList = 0,
				/**
				 *  Render the vertices as a series of individual lines.
				 */
				PT_LineList = 1,
				/**
				 *  Render the vertices as a continuous line.
				 */
				PT_LineStrip = 2,
				/**
				 *  Render the vertices as a series of individual triangles.
				 */
				PT_TriangleList = 3,				
				/** 
				 *  Render the vertices as a continous set of triangles in a zigzag type fashion.
				 */
				PT_TriangleStrip = 4,
				/**
				 *  Render the vertices as a set of trinagles in a fan like formation.
				 */
				PT_TriangleFan = 5,
				PT_Count = 6
			};
			enum CubeMapFace
			{
				/**
				 *  Positive x-face of the cube map.
				 */
				CUBE_PositiveX = 0,
				/**
				 *  Negative x-face of the cube map.
				 */
				CUBE_NegativeX = 1,
				/**
				 *  Positive y-face of the cube map.
				 */
				CUBE_PositiveY = 2,
				/**
				 *  Negative y-face of the cube map.
				 */
				CUBE_NegativeY = 3,
				/**
				 *  Positive z-face of the cube map.
				 */
				CUBE_PositiveZ = 4,
				/**
				 *  Negative z-face of the cube map.
				 */
				CUBE_NegativeZ = 5,
				CUBE_Count = 6
			};
			
			enum ClearFlags
			{
				CLEAR_ColorBuffer = 1,
				CLEAR_DepthBuffer = 2,
				CLEAR_Stencil = 4,
				CLEAR_ColorAndDepth = CLEAR_ColorBuffer | CLEAR_DepthBuffer,
				CLEAR_All = CLEAR_ColorBuffer | CLEAR_DepthBuffer | CLEAR_Stencil
			};

			/**
			 *  Defines stencil buffer operations.
			 */
			enum StencilOperation
			{
				/**
				 *  Does not update the stencil-buffer entry. This is the default value.
				 */
				STOP_Keep = 0,
				/**
				 *  Sets the stencil-buffer entry to 0.
				 */
				STOP_Zero = 1,

				/**
				 *  Replaces the stencil-buffer entry with a reference value.
				 */
				STOP_Replace = 2,
				
				/**
				 *  Increments the stencil-buffer entry, clamping to the maximum value.
				 */
				STOP_IncrementSaturation = 3,
				
				/**
				 *  Decrements the stencil-buffer entry, clamping to 0.
				 */
				STOP_DecrementSaturation = 4,

				/**
				 *  Inverts the bits in the stencil-buffer entry.
				 */
				STOP_Invert = 5,

				/**
				 *  Increments the stencil-buffer entry, wrapping to 0 if the new value exceeds
				 *  the maximum value.
				 */
				STOP_Increment = 6,
				
				/**
				 *  Decrements the stencil-buffer entry, wrapping to the maximum value if the
				 *  new value is less than 0.
				 */
				STOP_Decrement = 7,
				STOP_Count = 8
			};

			/**
			 *  Defines constants that describe supported texture-addressing modes.
			 */
			enum TextureAddressMode
			{
				/** Tile the texture at every integer junction. For example, for u values between
					0 and 3, the texture is repeated three times; no mirroring is performed.
				*/
				TA_Wrap = 0,

				/** Similar to Wrap, except that the texture is flipped at every integer junction.
					For u values between 0 and 1, for example, the texture is addressed normally;
					between 1 and 2, the texture is flipped (mirrored); between 2 and 3, the
					texture is normal again, and so on.
				*/
				TA_Mirror = 1,

				/** Texture coordinates outside the range [0.0, 1.0] are set to the texture color
					at 0.0 or 1.0, respectively.
				*/
				TA_Clamp = 2,        
				
				/** Texture coordinates outside the range [0.0, 1.0] are set to the border color.
				*/
				TA_Border = 3,

				/** Similar to Mirror and Clamp. Takes the absolute value of the texture coordinate
					(thus, mirroring around 0), and then clamps to the maximum value. The most
					common usage is for volume textures, where support for the full MirrorOnce
					texture-addressing mode is not necessary, but the data is symmetrical around
					the one axis.
				*/
				TA_MirrorOnce = 4,
				TA_Count = 5
			};
			/**
			 *  Defines how a texture will be filtered as it is minified for each mipmap level.
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
				*   filter. A weighted average of a 2×2 area of texels surrounding the desired
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
				TFLT_Count = 8
			};


			/**
			 *  Defines format of vertex element
			 */
			enum VertexElementFormat
			{				
				/**
				 *   One-component, float expanded to (float, 0, 0, 1)
				 */
				VEF_Single = 0,
				/**
				 *  Two-component, float expanded to (float, float, 0, 1)
				 */
				VEF_Vector2 = 1,
				/**
				 *  Three-component, float expanded to (float, float, float, 1)
				 */
				VEF_Vector3 = 2,
				/**
				 *  Four-component, float expanded to (float, float, float, float)
				 */
				VEF_Vector4 = 3,
				/**
				 *  Four-component, packed byte to (R, G, B, A)
				 */
				VEF_Color = 4,

				/**
				 *  Four-component, byte.
				 */
				VEF_Byte4 = 5,
				/**
				 *  Two-component, signed short expanded to (value, value, 0, 1).
				 */
				VEF_Short2 = 6,
				/**
				 *  Four-component, signed short expanded to (value, value, value, value).
				 */
				VEF_Short4 = 7,
				
				/**
				 *  Four-component byte with each byte normalized by dividing the component with
				 *  255.0f. This type is valid for vertex shader version 2.0 or higher.
				 */
				VEF_NormalizedByte4 = 8,

				/**
				 *  Normalized, two-component, signed short, expanded to (first short/32767.0,
				 *  second short/32767.0, 0, 1). This type is valid for vertex shader version
				 *  2.0 or higher.
				 */
				VEF_NormalizedShort2 = 9,
				/*
				 *  Normalized, four-component, signed short, expanded to (first short/32767.0,
				 *   second short/32767.0, third short/32767.0, fourth short/32767.0). This type
				 *   is valid for vertex shader version 2.0 or higher.
				 */
				VEF_NormalizedShort4 = 10,

				/**
				 *  Three-component, unsigned, 10 10 10 format expanded to (value, value, value, 1).
				 */
				VEF_UInt101010 = 11,
				/**
				 *  Three-component, signed, 10 10 10 format normalized and expanded to (v[0]/511.0,
				 *   v[1]/511.0, v[2]/511.0, 1).
				 */
				VEF_Normalized101010 = 12,
				/** 
				 *  Two-component, 16-bit floating point expanded to (value, value, value, value).
				 *   This type is valid for vertex shader version 2.0 or higher.
				 */
				VEF_HalfVector2 = 13,
				/**
				 *   Four-component, 16-bit floating-point expanded to (value, value, value, value).
				 *   This type is valid for vertex shader version 2.0 or higher.
				 */
				VEF_HalfVector4 = 14,
				
				VEF_Count = 15,
			};


			enum VertexElementUsage
			{
				VEU_Position = 0,
				VEU_BlendWeight = 1,
				VEU_BlendIndices = 2,
				VEU_Normal = 3,
				VEU_PointSize = 4,
				VEU_TextureCoordinate = 5,
				VEU_Tangent = 6,
				VEU_Binormal = 7,
				VEU_TessellateFactor = 8,
				VEU_PositionTransformed = 9,
				VEU_Color = 10,
				VEU_Fog = 11,
				VEU_Depth = 12,
				VEU_Sample = 13,
				VEU_Count = 14
			};
			
			/**
			 *  Specifies how a texture is to be used in the engine.
			 */
			enum TextureUsage
			{
				TU_Static = BU_Static,
				TU_Dynamic = BU_Dynamic,
				/* Mipmaps will be automatically generated for this texture
				*/
				TU_AutoMipMap = 0x100,
				
				/* Default to static textures
				*/
				TU_Default = TU_Static,
			};

			enum CompareFunction
			{
				/**
				 *   Always fail
				 */
				COMFUN_Never = 1,

				/**
				 *  Pass if the determined pixel is less than current pixel.
				 */
				COMFUN_Less = 2,

				/**
				 *  Pass if the determined pixel is equal to current pixel.
				 */
				COMFUN_Equal = 3,
				/**
				 *  Pass if the determined pixel is equal or less than current pixel.
				 */
				COMFUN_LessEqual = 4,

				/**
				 *  Pass if the determined pixel is greater than current pixel.
				 */
				COMFUN_Greater = 5,

				/**
				 *  Pass if the determined pixel is not equal to current pixel.
				 */
				COMFUN_NotEqual = 6,

				/**
				 *  Pass if the determined pixel is equal or greater than current pixel.
				 */
				COMFUN_GreaterEqual = 7,

				/**
				 *  Always pass
				 */
				COMFUN_Always = 8,
				COMFUN_Count = 9
			};
			enum BlendFunction
			{				
				/** The result is the destination added to the source.Result = (Source Color
					* Source Blend) + (Destination Color * Destination Blend)
				*/
				BLFUN_Add = 0,

				/** The result is the destination subtracted from the source.Result = (Source
					Color * Source Blend) − (Destination Color * Destination Blend)
				*/
				BLFUN_Subtract = 1,

				/** The result is the source subtracted from the destination.Result = (Destination
					Color * Destination Blend) −(Source Color * Source Blend)
				*/
				BLFUN_ReverseSubtract = 2,

				/** The result is the minimum of the source and destination.Result = min( (Source
					 Color * Source Blend), (Destination Color * Destination Blend) )
				*/
				BLFUN_Min = 3,

				/** The result is the maximum of the source and destination.Result = max( (Source
					Color * Source Blend), (Destination Color * Destination Blend) )
				*/
				BLFUN_Max = 4,
				BLFUN_Count = 5
			};

			enum Blend
			{
				/** Each component of the color is multiplied by 0
				*/
				BLEND_Zero = 0,
				/** Each component of the color is multiplied by 1
				*/
				BLEND_One = 1,
				/** Each component color is multiplied by the source color.
				*/
				BLEND_SourceColor = 2,

				/** Each component of the color is multiplied by the inverse of the source color.
					This can be represented as (1 − Rs, 1 − Gs, 1 − Bs, 1 − As) where R, G, B,
					and A respectively stand for the red, green, blue, and alpha destination
					values.
				*/
				BLEND_InverseSourceColor = 3,
				
				/** Each component of the color is multiplied by the alpha value of the source.
					This can be represented as (As, As, As, As), where As is the alpha source
					value.
				*/
				BLEND_SourceAlpha = 4,

				/** Each component of the color is multiplied by the inverse of the alpha value
					of the source. This can be represented as (1 − As, 1 − As, 1 − As, 1 − As),
					where As is the alpha destination value.
				*/
				BLEND_InverseSourceAlpha = 5,

				
				/** Each component of the color is multiplied by the alpha value of the destination.
					This can be represented as (Ad, Ad, Ad, Ad), where Ad is the destination
					alpha value.
				*/
				BLEND_DestinationAlpha = 6,
				
				/** Each component of the color is multiplied by the inverse of the alpha value
					of the destination. This can be represented as (1 − Ad, 1 − Ad, 1 − Ad, 1
					− Ad), where Ad is the alpha destination value.
				*/
				BLEND_InverseDestinationAlpha = 7,

				/** Each component color is multiplied by the destination color. This can be
					represented as (Rd, Gd, Bd, Ad), where R, G, B, and A respectively stand
					for red, green, blue, and alpha destination values.
				*/
				BLEND_DestinationColor = 8,

				/** Each component of the color is multiplied by the inverse of the destination
					color. This can be represented as (1 − Rd, 1 − Gd, 1 − Bd, 1 − Ad), where
					Rd, Gd, Bd, and Ad respectively stand for the red, green, blue, and alpha
					destination values.
				*/
				BLEND_InverseDestinationColor = 9,

				/** Each component of the color is multiplied by either the alpha of the source
					color, or the inverse of the alpha of the source color, whichever is greater.
					This can be represented as (f, f, f, 1), where f = min(A, 1 − Ad).
				*/
				BLEND_SourceAlphaSaturation = 10,

				/** This mode is obsolete. The same effect can be achieved by setting the source
					and destination blend factors to SourceAlpha and InverseSourceAlpha in separate
					calls.
				*/
				BLEND_BothSourceAlpha = 11,

				/**
				 *  Each component of the color is multiplied by BlendFactor.
				 */
				BLEND_BlendFactor = 12,
				BLEND_Count = 13
			};
			enum ShaderType
			{
				SHDT_Vertex=1<<0,
				SHDT_Pixel=1<<1,
				SHDT_All = SHDT_Pixel | SHDT_Vertex
			};

			class APAPI GraphicsCommonUtils
			{
			public:
				static Blend ParseBlend(const String& value);
				static String ToString(Blend blend);

				static BlendFunction ParseBlendFunction(const String& value);
				static String ToString(BlendFunction func);

				static CullMode ParseCullMode(const String& value);
				static String ToString(CullMode cull);

				static TextureAddressMode ParseTextureAddressMode(const String& value);
				static String ToString(TextureAddressMode mode);

				static TextureFilter ParseTextureFilter(const String& value);
				static String ToString(TextureFilter filter);

				static ShaderType ParseShaderType(const String& value);
				static String ToString(ShaderType type);

				static VertexElementUsage ParseVertexElementUsage(const String& value);
				static String ToString(VertexElementUsage usage);
			protected:
			private:
			};
		}
	}
}
#endif