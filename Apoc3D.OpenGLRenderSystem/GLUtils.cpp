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

#include "GLUtils.h"


namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			// pre-calculated fast type cast tables
			GLenum GLUtils::comfunTable[COMFUN_Count];
			GLenum GLUtils::blendTable[BLEND_Count];
			GLenum GLUtils::blendopTable[BLFUN_Count];
			GLenum GLUtils::stencilTable[STOP_Count];

			/************************************************************************/
			/*                                                                      */
			/************************************************************************/

			GLUtils::GLUtils()
			{
				InitCompareFunctionTable();
				InitBlendTable();
				InitBlendOperationTable();
			}

			GLenum GLUtils::ConvertStencilOperation(StencilOperation so, bool invert)
			{
				switch(so)
				{
				case STOP_Keep:
					return GL_KEEP;
				case STOP_Zero:
					return GL_ZERO;
				case STOP_Replace:
					return GL_REPLACE;
				case STOP_Increment:
					return invert ? GL_DECR : GL_INCR;
				case STOP_Decrement:
					return invert ? GL_INCR : GL_DECR;
				case STOP_IncrementSaturation:
					return invert ? GL_DECR_WRAP_EXT : GL_INCR_WRAP_EXT;
				case STOP_DecrementSaturation:
					return invert ? GL_INCR_WRAP_EXT : GL_DECR_WRAP_EXT;
				case STOP_Invert:
					return GL_INVERT;
				};
				// to make the compiler happy
				return STOP_Keep;
			}

			/**
			  format determines the composition of each element in data. It can assume one of these symbolic values:
			  GL_RED​
			  GL_RG​
			  GL_RGB​
			  GL_BGR​
			  GL_RGBA​
			  GL_BGRA​
			  GL_DEPTH_COMPONENT​
			  GL_DEPTH_STENCIL​

			  The following symbolic values are accepted: 
			  GL_UNSIGNED_BYTE​, GL_BYTE​, GL_UNSIGNED_SHORT​, 
			  GL_SHORT​, GL_UNSIGNED_INT​, GL_INT​, 
			  GL_FLOAT​, GL_UNSIGNED_BYTE_3_3_2​, GL_UNSIGNED_BYTE_2_3_3_REV​, 
			  GL_UNSIGNED_SHORT_5_6_5​, GL_UNSIGNED_SHORT_5_6_5_REV​, GL_UNSIGNED_SHORT_4_4_4_4​, 
			  GL_UNSIGNED_SHORT_4_4_4_4_REV​, GL_UNSIGNED_SHORT_5_5_5_1​, GL_UNSIGNED_SHORT_1_5_5_5_REV​, 
			  GL_UNSIGNED_INT_8_8_8_8​, GL_UNSIGNED_INT_8_8_8_8_REV​, GL_UNSIGNED_INT_10_10_10_2​, 
			  GL_UNSIGNED_INT_2_10_10_10_REV​
			*/
			PixelFormat GLUtils::ConvertBackPixelFormat(GLenum format, GLenum type)
			{
				
			}
			void GLUtils::ConvertPixelFormat(PixelFormat fmt, GLenum& format, GLenum& type)
			{

			}

			void GLUtils::InitCompareFunctionTable()
			{
				comfunTable[COMFUN_Never] = GL_NEVER;
				comfunTable[COMFUN_Less] = GL_LESS;
				comfunTable[COMFUN_Equal] = GL_EQUAL;
				comfunTable[COMFUN_LessEqual] = GL_LEQUAL;
				comfunTable[COMFUN_Greater] = GL_GREATER;
				comfunTable[COMFUN_NotEqual] = GL_NOTEQUAL;
				comfunTable[COMFUN_GreaterEqual] = GL_GEQUAL;
				comfunTable[COMFUN_Always] = GL_ALWAYS;
			}
			void GLUtils::InitBlendTable()
			{
				blendTable[BLEND_Zero] = GL_ZERO;
				blendTable[BLEND_One] = GL_ONE;
				blendTable[BLEND_SourceColor] = GL_SRC_COLOR;
				blendTable[BLEND_InverseSourceColor] = GL_ONE_MINUS_SRC_COLOR;
				blendTable[BLEND_SourceAlpha] = GL_SRC_ALPHA;
				blendTable[BLEND_InverseSourceAlpha] = GL_ONE_MINUS_SRC_ALPHA;
				blendTable[BLEND_DestinationAlpha] = GL_DST_ALPHA;
				blendTable[BLEND_InverseDestinationAlpha] = GL_ONE_MINUS_DST_ALPHA;
				blendTable[BLEND_DestinationColor] = GL_DST_COLOR;
				blendTable[BLEND_InverseDestinationColor] = GL_ONE_MINUS_DST_COLOR;
				blendTable[BLEND_SourceAlphaSaturation] = GL_SRC_ALPHA_SATURATE;
				blendTable[BLEND_BothSourceAlpha] = GL_SRC_ALPHA; // not supported
				blendTable[BLEND_BlendFactor] = GL_CONSTANT_COLOR;
			}
			void GLUtils::InitBlendOperationTable()
			{
				blendopTable[BLFUN_Add] = GL_FUNC_ADD;
				blendopTable[BLFUN_Subtract] = GL_FUNC_SUBTRACT;
				blendopTable[BLFUN_ReverseSubtract] = GL_FUNC_REVERSE_SUBTRACT;
				blendopTable[BLFUN_Min] = GL_MIN;
				blendopTable[BLFUN_Max] = GL_MAX;
			}
			void GLUtils::InitStencilTable()
			{
				stencilTable[STOP_Keep] = GL_KEEP;
				stencilTable[STOP_Zero] = GL_ZERO;
				stencilTable[STOP_Replace] = GL_REPLACE;
				stencilTable[STOP_IncrementSaturation] = GL_INCR_WRAP;
				stencilTable[STOP_DecrementSaturation] = GL_DECR_WRAP;
				stencilTable[STOP_Invert] = GL_INVERT;
				stencilTable[STOP_Increment] = GL_INCR;
				stencilTable[STOP_Decrement] = GL_DECR;
			}
		}
	}
}