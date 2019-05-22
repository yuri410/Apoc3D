/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 *
 * Copyright (c) 2011-2019 Tao Xin
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

#include "GL3Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			// pre-calculated fast type cast tables=
			GLenum GLUtils::blendTable[(int)Blend::Count];
			GLenum GLUtils::comfunTable[(int)CompareFunction::Count];
			GLenum GLUtils::blendopTable[(int)BlendFunction::Count];

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
				switch (so)
				{
					case StencilOperation::Keep:
						return GL_KEEP;
					case StencilOperation::Zero:
						return GL_ZERO;
					case StencilOperation::Replace:
						return GL_REPLACE;
					case StencilOperation::Increment:
						return invert ? GL_DECR : GL_INCR;
					case StencilOperation::Decrement:
						return invert ? GL_INCR : GL_DECR;
					case StencilOperation::IncrementSaturation:
						return invert ? GL_DECR_WRAP : GL_INCR_WRAP;
					case StencilOperation::DecrementSaturation:
						return invert ? GL_INCR_WRAP : GL_DECR_WRAP;
					case StencilOperation::Invert:
						return GL_INVERT;
				};
				// to make the compiler happy
				return GL_KEEP;
			}

			bool GLUtils::ConvertPixelFormat(PixelFormat fmt, GLenum& format, GLenum& type, GLenum& internalFormat)
			{
				switch (fmt)
				{
				case FMT_Luminance8:
					format = GL_RED;
					type = GL_UNSIGNED_BYTE;
					internalFormat = GL_R8;
					break;

				case FMT_Luminance16:
					format = GL_RED;
					type = GL_UNSIGNED_SHORT;
					internalFormat = GL_R16;
					break;

				//case FMT_Alpha1:
					// not supported
					
				//case FMT_Alpha8:
					//format = GL_ALPHA;
					
				//case FMT_A4L4:
					//format = GL_LUMINANCE_ALPHA;
					
				//case FMT_A8L8:

					//break;


				case FMT_R3G3B2:
					format = GL_BGR;
					type = GL_UNSIGNED_BYTE_3_3_2;
					internalFormat = GL_R3_G3_B2;
					break;

				case FMT_R5G6B5:
					format = GL_BGR;
					type = GL_UNSIGNED_SHORT_5_6_5;
					internalFormat = GL_RGB8;
					break;

				case FMT_B5G6R5:
					format = GL_RGB;
					type = GL_UNSIGNED_SHORT_5_6_5;
					internalFormat = GL_RGB8;
					break;

				case FMT_A4R4G4B4:
					format = GL_BGRA;
					type = GL_UNSIGNED_SHORT_4_4_4_4;
					internalFormat = GL_RGBA4;
					break;

				case FMT_B4G4R4A4:
					format = GL_BGRA;
					type = GL_UNSIGNED_SHORT_4_4_4_4_REV;
					internalFormat = GL_RGBA4;
					break;

				case FMT_A1R5G5B5:
					format = GL_BGRA;
					type = GL_UNSIGNED_SHORT_5_5_5_1;
					internalFormat = GL_RGB5_A1;
					break;

				case FMT_X1R5G5B5:
					format = GL_BGR;
					type = GL_UNSIGNED_SHORT_5_5_5_1;
					internalFormat = GL_RGB5;
					break;

				case FMT_R8G8B8:
					format = GL_BGR;
					type = GL_UNSIGNED_INT_8_8_8_8;
					internalFormat = GL_RGB8;
					break;

				case FMT_B8G8R8:
					format = GL_RGB;
					type = GL_UNSIGNED_INT_8_8_8_8;
					internalFormat = GL_RGB8;
					break;

				case FMT_A8R8G8B8:
					format = GL_BGRA;
					type = GL_UNSIGNED_INT_8_8_8_8;
					internalFormat = GL_RGBA8;
					break;

				case FMT_A8B8G8R8:
					format = GL_RGBA;
					type = GL_UNSIGNED_INT_8_8_8_8;
					internalFormat = GL_RGBA8;
					break;

				case FMT_B8G8R8A8:
					format = GL_BGRA;
					type = GL_UNSIGNED_INT_8_8_8_8_REV;
					internalFormat = GL_RGBA8;
					break;

				case FMT_X8R8G8B8:
					format = GL_BGR;
					type = GL_UNSIGNED_INT_8_8_8_8;
					internalFormat = GL_RGB8;
					break;

				case FMT_X8B8G8R8:
					format = GL_RGB;
					type = GL_UNSIGNED_INT_8_8_8_8;
					internalFormat = GL_RGB8;
					break;

				case FMT_R8G8B8A8:
					format = GL_RGBA;
					type = GL_UNSIGNED_INT_8_8_8_8_REV;
					internalFormat = GL_RGBA8;
					break;


				case FMT_A2R10G10B10:
					format = GL_BGRA;
					type = GL_UNSIGNED_INT_10_10_10_2;
					internalFormat = GL_RGB10_A2;
					break;

				case FMT_A2B10G10R10:
					format = GL_RGBA;
					type = GL_UNSIGNED_INT_10_10_10_2;
					internalFormat = GL_RGB10_A2;
					break;


				case FMT_G16R16:
					format = GL_RG;
					type = GL_UNSIGNED_SHORT;
					internalFormat = GL_RG16;
					break;

				case FMT_R16G16B16:
					format = GL_BGR;
					type = GL_UNSIGNED_SHORT;
					internalFormat = GL_RGB16;
					break;

				case FMT_A16B16G16R16:
					format = GL_RGBA16;
					type = GL_UNSIGNED_SHORT;
					internalFormat = GL_RGBA16;
					break;


				case FMT_R16F:
					format = GL_RED;
					type = GL_HALF_FLOAT;
					internalFormat = GL_R16F;
					break;

				case FMT_R32F:
					format = GL_RED;
					type = GL_FLOAT;
					internalFormat = GL_R32F;
					break;


				case FMT_G16R16F:
					format = GL_RG;
					type = GL_HALF_FLOAT;
					internalFormat = GL_RG16F;
					break;

				case FMT_G32R32F:
					format = GL_RG;
					type = GL_FLOAT;
					internalFormat = GL_RG32F;
					break;

				case FMT_A16B16G16R16F:
					format = GL_RGBA;
					type = GL_HALF_FLOAT;
					internalFormat = GL_RGBA16F;
					break;

				case FMT_A32B32G32R32F:
					format = GL_RGBA;
					type = GL_FLOAT;
					internalFormat = GL_RGBA32F;
					break;

				//case FMT_Palette8:
				//	break;
				//case FMT_Palette8Alpha8:
				//	break;

				// for use with glCompressedTexImage2D
				case FMT_DXT1:
					format = type = GL_ZERO;
					internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
					break;
				case FMT_DXT3:
					format = type = GL_ZERO;
					internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
					break;
				case FMT_DXT5:
					format = type = GL_ZERO;
					internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					break;

				default:
					return false;
				}
				return true;
			}

			GLbitfield GLUtils::ConvertLockMode(LockMode mode)
			{
				GLbitfield r = GL_MAP_WRITE_BIT;
				if (mode & LOCK_Discard)
					r |= GL_MAP_INVALIDATE_RANGE_BIT;
				if (mode & LOCK_NoOverwrite)
					r |= GL_MAP_UNSYNCHRONIZED_BIT;
				if (mode & LOCK_ReadOnly)
					r |= GL_MAP_READ_BIT;
				return r;
			}

			bool GLUtils::CheckError(const wchar_t* file, unsigned line)
			{
				int err = glGetError();

				if (err != GL_NO_ERROR)
				{
					const wchar_t* errString = L"UNKNOWN";

					switch (err)
					{
					case GL_INVALID_ENUM:					errString = L"GL_INVALID_ENUM"; break;
					case GL_INVALID_VALUE:					errString = L"GL_INVALID_VALUE"; break;
					case GL_INVALID_OPERATION:				errString = L"GL_INVALID_OPERATION"; break;
					case GL_INVALID_FRAMEBUFFER_OPERATION:	errString = L"GL_INVALID_FRAMEBUFFER_OPERATION"; break;
					case GL_OUT_OF_MEMORY:					errString = L"GL_OUT_OF_MEMORY"; break;
					}

					String msg = L"OpenGL Error: ";
					msg += errString;
					Apoc3D::Error(ErrorID::Default, msg, file, line);

					return false;
				}
				return true;
			}

			bool GLUtils::CheckFramebufferError(const wchar_t* file, unsigned line)
			{
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

				if (status != GL_FRAMEBUFFER_COMPLETE)
				{
					const wchar_t* errString = L"GL_FRAMEBUFFER_UNDEFINED";

					switch (status)
					{
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			errString = L"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	errString = L"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			errString = L"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			errString = L"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
					case GL_FRAMEBUFFER_UNSUPPORTED:					errString = L"GL_FRAMEBUFFER_UNSUPPORTED"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:			errString = L"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:		errString = L"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
					}

					String msg = L"OpenGL Error: ";
					msg += errString;
					Apoc3D::Error(ErrorID::Default, msg, file, line);

					return false;
				}
				return true;
			}


			void GLUtils::InitCompareFunctionTable()
			{
				comfunTable[(int)CompareFunction::Never] = GL_NEVER;
				comfunTable[(int)CompareFunction::Less] = GL_LESS;
				comfunTable[(int)CompareFunction::Equal] = GL_EQUAL;
				comfunTable[(int)CompareFunction::LessEqual] = GL_LEQUAL;
				comfunTable[(int)CompareFunction::Greater] = GL_GREATER;
				comfunTable[(int)CompareFunction::NotEqual] = GL_NOTEQUAL;
				comfunTable[(int)CompareFunction::GreaterEqual] = GL_GEQUAL;
				comfunTable[(int)CompareFunction::Always] = GL_ALWAYS;
			}
			void GLUtils::InitBlendTable()
			{
				blendTable[(int)Blend::Zero] = GL_ZERO;
				blendTable[(int)Blend::One] = GL_ONE;
				blendTable[(int)Blend::SourceColor] = GL_SRC_COLOR;
				blendTable[(int)Blend::InverseSourceColor] = GL_ONE_MINUS_SRC_COLOR;
				blendTable[(int)Blend::SourceAlpha] = GL_SRC_ALPHA;
				blendTable[(int)Blend::InverseSourceAlpha] = GL_ONE_MINUS_SRC_ALPHA;
				blendTable[(int)Blend::DestinationAlpha] = GL_DST_ALPHA;
				blendTable[(int)Blend::InverseDestinationAlpha] = GL_ONE_MINUS_DST_ALPHA;
				blendTable[(int)Blend::DestinationColor] = GL_DST_COLOR;
				blendTable[(int)Blend::InverseDestinationColor] = GL_ONE_MINUS_DST_COLOR;
				blendTable[(int)Blend::SourceAlphaSaturation] = GL_SRC_ALPHA_SATURATE;
				blendTable[(int)Blend::BlendFactor] = GL_CONSTANT_COLOR;
			}
			void GLUtils::InitBlendOperationTable()
			{
				blendopTable[(int)BlendFunction::Add] = GL_FUNC_ADD;
				blendopTable[(int)BlendFunction::Subtract] = GL_FUNC_SUBTRACT;
				blendopTable[(int)BlendFunction::ReverseSubtract] = GL_FUNC_REVERSE_SUBTRACT;
				blendopTable[(int)BlendFunction::Min] = GL_MIN;
				blendopTable[(int)BlendFunction::Max] = GL_MAX;
			}

		}
	}
}