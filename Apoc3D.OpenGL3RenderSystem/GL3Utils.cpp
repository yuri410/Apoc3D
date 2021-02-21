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
#include "Apoc3D/Utility/TypeConverter.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			// pre-calculated fast type cast tables
			static GLenum blendTable[(int)Blend::Count];
			static GLenum comfunTable[(int)CompareFunction::Count];
			static GLenum blendopTable[(int)BlendFunction::Count];
			static GLenum taTable[(int)TextureAddressMode::Count];
			static GLenum cubeTable[CUBE_Count];
			static GLenum ptTable[(int)PrimitiveType::Count];

			struct FieldInitilizer
			{
				FieldInitilizer();
			private:
				void InitCompareFunctionTable();
				void InitBlendTable();
				void InitBlendOperationTable();
				void InitTextureAddressTable();
				void InitCubeTable();
				void InitPrimitiveTypeTable();

			} static s_initializer;

			//////////////////////////////////////////////////////////////////////////

			GLenum GLUtils::ConvertCompare(CompareFunction cmp)
			{
				return comfunTable[static_cast<int>(cmp)]; 
			}

			GLenum GLUtils::ConvertBlend(Blend dv)
			{
				return blendTable[static_cast<int>(dv)]; 
			}

			GLenum GLUtils::ConvertBlendFunction(BlendFunction fun)
			{
				return blendopTable[static_cast<int>(fun)]; 
			}

			GLenum GLUtils::ConvertTextureAddress(TextureAddressMode ta) 
			{
				return taTable[static_cast<int>(ta)]; 
			}

			GLenum GLUtils::ConvertCubemapFace(CubeMapFace cubeface)
			{
				return cubeTable[cubeface];
			}

			GLenum GLUtils::ConvertPrimitiveType(PrimitiveType pt)
			{
				return ptTable[static_cast<int>(pt)];
			}

			GLint GLUtils::CalculatePrimitiveIndexCount(PrimitiveType pt, GLint primitiveCount)
			{
				switch (pt)
				{
					case PrimitiveType::PointList: return primitiveCount;
					case PrimitiveType::LineList:  return primitiveCount * 2;
					case PrimitiveType::LineStrip: return primitiveCount > 0 ? (primitiveCount + 1) : 0;
					case PrimitiveType::TriangleList:	return primitiveCount * 3;
					case PrimitiveType::TriangleStrip:
					case PrimitiveType::TriangleFan:	return primitiveCount > 0 ? (primitiveCount + 2) : 0;
				}
				return 0;
			}
			GLint GLUtils::CalculatePrimitiveCount(PrimitiveType pt, GLint indexCount)
			{
				switch (pt)
				{
					case PrimitiveType::PointList: return indexCount;
					case PrimitiveType::LineList:  return indexCount / 2;
					case PrimitiveType::LineStrip: return indexCount > 1 ? (indexCount - 1) : 0;
					case PrimitiveType::TriangleList:	return indexCount / 3;
					case PrimitiveType::TriangleStrip:
					case PrimitiveType::TriangleFan:	return indexCount > 2 ? (indexCount - 2) : 0;
				}
				return 0;
			}

			GLenum GLUtils::ConvertIndexBufferFormat(IndexBufferFormat ibf)
			{
				return ibf == IndexBufferFormat::Bit16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
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

			bool GLUtils::ConvertDepthFormat(DepthFormat fmt, GLenum& format, GLenum& type, GLenum& internalFormat)
			{
				switch (fmt)
				{
					case DEPFMT_Depth16:
					case DEPFMT_Depth16Lockable:
						format = GL_DEPTH_COMPONENT;
						internalFormat = GL_DEPTH_COMPONENT16;
						type = GL_UNSIGNED_SHORT;
						break;
					case DEPFMT_Depth24X8:
						format = GL_DEPTH_COMPONENT;
						internalFormat = GL_DEPTH_COMPONENT24;
						type = GL_UNSIGNED_INT;
						break;
					case DEPFMT_Depth24Stencil8:
						format = GL_DEPTH_STENCIL;
						internalFormat = GL_DEPTH24_STENCIL8;
						type = GL_UNSIGNED_INT_24_8;
						break;
					case DEPFMT_Depth32:
					case DEPFMT_Depth32Lockable:
					case DEPFMT_Depth32Single:
						format = GL_DEPTH_COMPONENT;
						internalFormat = GL_DEPTH_COMPONENT32F;
						type = GL_FLOAT;
						break;
					default:
						return false;
				}
				return true;
			}

			GLenum GLUtils::GetTextureTarget(TextureType type)
			{
				switch (type)
				{
					case TextureType::Texture1D: return GL_TEXTURE_1D;
					case TextureType::Texture2D: return GL_TEXTURE_2D;
					case TextureType::Texture3D: return GL_TEXTURE_3D;
					case TextureType::CubeTexture: return GL_TEXTURE_CUBE_MAP;
				}
				// keep the compiler happy
				return GL_TEXTURE_1D;
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

			bool GLUtils::ConvertVertexElementFormat(VertexElementFormat vef, GLenum& elementType, GLuint& elementCount, GLboolean& normalized)
			{
				normalized = GL_FALSE;

				switch (vef)
				{
					case VEF_Single:
						elementType = GL_FLOAT;
						elementCount = 1;
						break;
					case VEF_Vector2:
						elementType = GL_FLOAT;
						elementCount = 2;
						break;
					case VEF_Vector3:
						elementType = GL_FLOAT;
						elementCount = 3;
						break;
					case VEF_Vector4:
						elementType = GL_FLOAT;
						elementCount = 4;
						break;
					case VEF_Color:
						elementCount = GL_BGRA;
						elementType = GL_UNSIGNED_BYTE;
						normalized = GL_TRUE;
						break;
					case VEF_Byte4:
						elementType = GL_UNSIGNED_BYTE;
						elementCount = 4;
						break;
					case VEF_Short2:
						elementType = GL_SHORT;
						elementCount = 2;
						break;
					case VEF_Short4:
						elementType = GL_SHORT;
						elementCount = 4;
						break;
					case VEF_NormalizedByte4:
						elementType = GL_UNSIGNED_BYTE;
						elementCount = 4;
						normalized = GL_TRUE;
						break;
					case VEF_NormalizedShort2:
						elementType = GL_SHORT;
						elementCount = 2;
						normalized = GL_TRUE;
						break;
					case VEF_NormalizedShort4:
						elementType = GL_SHORT;
						elementCount = 4;
						normalized = GL_TRUE;
						break;
					case VEF_UInt101010:
						elementType = GL_UNSIGNED_INT_2_10_10_10_REV;
						elementCount = 1;
						break;
					case VEF_Normalized101010:
						elementType = GL_UNSIGNED_INT_2_10_10_10_REV;
						elementCount = 1;
						normalized = GL_TRUE;
						break;
					case VEF_HalfVector2:
						elementType = GL_HALF_FLOAT;
						elementCount = 2;
						break;
					case VEF_HalfVector4:
						elementType = GL_HALF_FLOAT;
						elementCount = 4;
						break;
					default:
						return false;
				}
				return true;
			}
		

			//////////////////////////////////////////////////////////////////////////

			bool GLUtils::CheckError(const char* file, unsigned line)
			{
				int err = glGetError();

				if (err != GL_NO_ERROR)
				{
					const char* errString = "UNKNOWN";

					switch (err)
					{
					case GL_INVALID_ENUM:					errString = "GL_INVALID_ENUM"; break;
					case GL_INVALID_VALUE:					errString = "GL_INVALID_VALUE"; break;
					case GL_INVALID_OPERATION:				errString = "GL_INVALID_OPERATION"; break;
					case GL_INVALID_FRAMEBUFFER_OPERATION:	errString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
					case GL_OUT_OF_MEMORY:					errString = "GL_OUT_OF_MEMORY"; break;
					}

					std::string msg = "OpenGL Error: ";
					msg += errString;
					Apoc3D::Error(ErrorID::Default, msg, file, line);

					return false;
				}
				return true;
			}

			bool GLUtils::CheckFramebufferError(const char* file, unsigned line)
			{
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

				if (status != GL_FRAMEBUFFER_COMPLETE)
				{
					const char* errString = "GL_FRAMEBUFFER_UNDEFINED";

					switch (status)
					{
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			errString = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	errString = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			errString = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			errString = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
					case GL_FRAMEBUFFER_UNSUPPORTED:					errString = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:			errString = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
					case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:		errString = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
					}

					std::string msg = "OpenGL Error: ";
					msg += errString;
					Apoc3D::Error(ErrorID::Default, msg, file, line);
					
					return false;
				}
				return true;
			}

			const TypeDualConverter<VertexElementUsage> GLUtils::VertexElementUsageConverter =
			{
				{ VEU_Position,				L"a_Position" },
				{ VEU_BlendWeight,			L"a_BlendWeight" },
				{ VEU_BlendIndices,			L"a_BlendIndices" },
				{ VEU_Normal,				L"a_Normal" },
				{ VEU_PointSize,			L"a_PointSize" },
				{ VEU_TextureCoordinate,	L"a_TextureCoordinate" },
				{ VEU_Tangent,				L"a_Tangent" },
				{ VEU_Binormal,				L"a_Binormal" },
				{ VEU_TessellateFactor,		L"a_TessellateFactor" },
				{ VEU_PositionTransformed,	L"a_PositionTransformed" },
				{ VEU_Color,				L"a_Color" },
				{ VEU_Fog,					L"a_Fog" },
				{ VEU_Depth,				L"a_Depth" },
				{ VEU_Sample,				L"a_Sample" }
			};

			//////////////////////////////////////////////////////////////////////////

			void FieldInitilizer::InitCompareFunctionTable()
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

			void FieldInitilizer::InitBlendTable()
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

			void FieldInitilizer::InitBlendOperationTable()
			{
				blendopTable[(int)BlendFunction::Add] = GL_FUNC_ADD;
				blendopTable[(int)BlendFunction::Subtract] = GL_FUNC_SUBTRACT;
				blendopTable[(int)BlendFunction::ReverseSubtract] = GL_FUNC_REVERSE_SUBTRACT;
				blendopTable[(int)BlendFunction::Min] = GL_MIN;
				blendopTable[(int)BlendFunction::Max] = GL_MAX;
			}

			void FieldInitilizer::InitTextureAddressTable()
			{
				taTable[(int)TextureAddressMode::Border] = GL_CLAMP_TO_EDGE;
				taTable[(int)TextureAddressMode::Clamp] = GL_CLAMP_TO_EDGE;
				taTable[(int)TextureAddressMode::Mirror] = GL_MIRRORED_REPEAT;
				taTable[(int)TextureAddressMode::MirrorOnce] = GL_MIRROR_CLAMP_TO_EDGE;
				taTable[(int)TextureAddressMode::Wrap] = GL_REPEAT;
			}

			void FieldInitilizer::InitCubeTable()
			{
				cubeTable[CUBE_PositiveX] = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
				cubeTable[CUBE_NegativeX] = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
				cubeTable[CUBE_PositiveY] = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
				cubeTable[CUBE_NegativeY] = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
				cubeTable[CUBE_PositiveZ] = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
				cubeTable[CUBE_NegativeZ] = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
			}

			void FieldInitilizer::InitPrimitiveTypeTable()
			{
				ptTable[(int)PrimitiveType::PointList] = GL_POINTS;
				ptTable[(int)PrimitiveType::LineList] = GL_LINES;
				ptTable[(int)PrimitiveType::LineStrip] = GL_LINE_STRIP;
				ptTable[(int)PrimitiveType::TriangleList] = GL_TRIANGLES;
				ptTable[(int)PrimitiveType::TriangleStrip] = GL_TRIANGLE_STRIP;
				ptTable[(int)PrimitiveType::TriangleFan] = GL_TRIANGLE_FAN;
			}


			FieldInitilizer::FieldInitilizer()
			{
				InitCompareFunctionTable();
				InitBlendTable();
				InitBlendOperationTable();
				InitTextureAddressTable();
				InitCubeTable();
				InitPrimitiveTypeTable();
			}

		}
	}
}