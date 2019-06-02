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

#include "GL3RenderStateManager.h"

#include "GL3RenderDevice.h"
#include "GL3Utils.h"
#include "GL3Texture.h"
#include "GL/GLSampler.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL3RenderSystem
		{
			NativeGL3StateManager::NativeGL3StateManager(GL3RenderDevice* device)
				: m_device(device)
			{
				InitializeDefaultState();
			}
			NativeGL3StateManager::~NativeGL3StateManager()
			{

			}

			void NativeGL3StateManager::SetCullMode(CullMode mode)
			{
				m_cachedCullMode = mode;

				switch (mode)
				{
					case CullMode::None:
						glDisable(GL_CULL_FACE);
						return;
					default:
					case CullMode::Clockwise:
						glCullFace(GL_BACK);
						break;
					case CullMode::CounterClockwise:
						glCullFace(GL_FRONT);
						break;
				}

				glEnable( GL_CULL_FACE );
				glFrontFace(GL_CCW);

			}
			void NativeGL3StateManager::SetFillMode(FillMode mode)
			{
				m_cachedFillMode = mode;
				GLenum pm = 0;
				switch (mode)
				{
					case FillMode::Point: pm = GL_POINT; break;
					case FillMode::Solid: pm = GL_FILL; break;
					case FillMode::WireFrame: pm = GL_LINE; break;
				}

				glPolygonMode(GL_FRONT_AND_BACK, pm);
			}
			
			void NativeGL3StateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_cachedAlphaBlendEnable = enable;
				m_cachedAlphaBlendFunction = func;
				m_cachedAlphaSourceBlend = srcBlend;
				m_cachedAlphaDestBlend = dstBlend;

				if (enable)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);

				glBlendFunc(GLUtils::ConvertBlend(srcBlend), GLUtils::ConvertBlend(dstBlend));
				glBlendEquation(GLUtils::ConvertBlendFunction(func));
			}
			void NativeGL3StateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor)
			{
				SetAlphaBlend(enable, func, srcBlend, dstBlend);

				m_cachedAlphaBlendFactor = factor;

				glBlendColor(CV_GetColorR(factor) / 255.0f, CV_GetColorG(factor) / 255.0f, CV_GetColorB(factor) / 255.0f, CV_GetColorA(factor) / 255.0f);
			}

			void NativeGL3StateManager::setAlphaBlendEnable(bool enable)
			{
				m_cachedAlphaBlendEnable = enable;

				if (enable)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);
			}
			void NativeGL3StateManager::setAlphaBlendOperation(BlendFunction func)
			{
				m_cachedAlphaBlendFunction = func;
				
				glBlendEquation(GLUtils::ConvertBlendFunction(func));
			}
			void NativeGL3StateManager::setAlphaSourceBlend(Blend srcBlend)
			{
				m_cachedAlphaSourceBlend = srcBlend;

				glBlendFunc(GLUtils::ConvertBlend(srcBlend), GLUtils::ConvertBlend(m_cachedAlphaDestBlend));

			}
			void NativeGL3StateManager::setAlphaDestinationBlend(Blend dstBlend)
			{
				m_cachedAlphaDestBlend = dstBlend;

				glBlendFunc(GLUtils::ConvertBlend(m_cachedAlphaSourceBlend), GLUtils::ConvertBlend(dstBlend));
			}

			void NativeGL3StateManager::SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_cachedSepAlphaBlendEnable = enable;
				m_cachedSepAlphaBlendFunction = func;
				m_cachedSepAlphaSourceBlend = srcBlend;
				m_cachedSepAlphaDestBlend = dstBlend;

				
				if (enable)
				{
					glBlendEquationSeparate(GLUtils::ConvertBlendFunction(m_cachedAlphaBlendFunction), 
						GLUtils::ConvertBlendFunction(func));

					glBlendFuncSeparate(GLUtils::ConvertBlend(m_cachedAlphaSourceBlend), 
						GLUtils::ConvertBlend(m_cachedAlphaDestBlend),
						GLUtils::ConvertBlend(srcBlend),
						GLUtils::ConvertBlend(dstBlend));

				}
				else
				{
					glBlendEquationSeparate(GLUtils::ConvertBlendFunction(func), 
						GLUtils::ConvertBlendFunction(func));

					// OpenGL has these values, when not using separate blending.
					// "The initial value is GL_ONE" for src alpha.
					// "The initial value is GL_ZERO" for dst alpha.

					glBlendFuncSeparate(GLUtils::ConvertBlend(m_cachedAlphaSourceBlend), 
						GLUtils::ConvertBlend(m_cachedAlphaDestBlend),
						GL_ONE,
						GL_ZERO);
				}
			}

			void NativeGL3StateManager::SetDepth(bool enable, bool writeEnable)
			{
				m_cachedDepthBufferEnabled = enable;
				m_cachedDepthBufferWriteEnabled = writeEnable;

				if (enable)
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);

				if (writeEnable)
					glDepthMask(GL_TRUE);
				else
					glDepthMask(GL_FALSE);
			}
			void NativeGL3StateManager::SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare)
			{
				SetDepth(enable, writeEnable);

				m_cachedDepthBias = bias;
				m_cachedSlopeScaleDepthBias = slopebias;
				m_cachedDepthBufferFunction = compare;

				if (bias != 0 || slopebias != 0)
				{
					glEnable(GL_POLYGON_OFFSET_FILL);
					glEnable(GL_POLYGON_OFFSET_POINT);
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-slopebias, -bias);
				}
				else
				{
					glDisable(GL_POLYGON_OFFSET_FILL);
					glDisable(GL_POLYGON_OFFSET_POINT);
					glDisable(GL_POLYGON_OFFSET_LINE);
				}

				glDepthFunc(GLUtils::ConvertCompare(compare));
			}
			void NativeGL3StateManager::SetPointParameters(float size, float maxSize, float minSize, bool pointSprite)
			{
				m_cachedPointSize = size;
				m_cachedPointSizeMax = maxSize;
				m_cachedPointSizeMin = minSize;
				m_cachedPointSpriteEnabled = pointSprite;

				glPointSize(size);
				/*
				glPointParameterf(GL_POINT_SIZE_MIN, minSize);
				glPointParameterf(GL_POINT_SIZE_MAX, maxSize);
				
				
				if (pointSprite)
				{
					glEnable(GL_POINT_SPRITE);
				}
				else
				{
					glDisable(GL_POINT_SPRITE);
				}

				// Manually set up tex coord generation for point sprite
				GLint maxTexCoords = 1;

				{
					GLint units;
					glGetIntegerv(GL_MAX_TEXTURE_UNITS, &units);

					for (int i = 0; i < units; i++)
					{
						glActiveTexture(GL_TEXTURE0 + i);
						glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, pointSprite ? GL_TRUE : GL_FALSE);
					}
					glActiveTexture(GL_TEXTURE0);
				}*/
			}
			void NativeGL3StateManager::SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, 
				uint32 ref, CompareFunction func, uint32 mask, uint32 writemask)
			{
				m_cachedStencilEnabled = enabled;
				m_cachedStencilFail = fail;
				m_cachedStencilDepthFail = depthFail;
				m_cachedStencilPass = pass;
				m_cachedRefrenceStencil = ref;
				m_cachedStencilFunction = func;
				m_cachedStencilMask = mask;
				m_cachedStencilWriteMask = writemask;
				
				glStencilMask(mask);
				glStencilFunc(GLUtils::ConvertCompare(func), ref, mask);
				glStencilOp(
					GLUtils::ConvertStencilOperation(fail, false),
					GLUtils::ConvertStencilOperation(depthFail, false),
					GLUtils::ConvertStencilOperation(pass, false));

			}
			void NativeGL3StateManager::SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func)
			{
				m_cachedTwoSidedStencilMode = enabled;
				m_cachedCounterClockwiseStencilFail = fail;
				m_cachedCounterClockwiseStencilDepthBufferFail = depthFail;
				m_cachedCounterClockwiseStencilPass = pass;
				m_cachedCounterClockwiseStencilFunction = func;

				// Front is the 2nd side
				glStencilMaskSeparate(GL_FRONT, m_cachedStencilMask);
				glStencilFuncSeparate(GL_FRONT, GLUtils::ConvertCompare(func), m_cachedRefrenceStencil, m_cachedStencilMask);
				glStencilOpSeparate(GL_FRONT, 
					GLUtils::ConvertStencilOperation(fail, true),
					GLUtils::ConvertStencilOperation(depthFail, true), 
					GLUtils::ConvertStencilOperation(pass, true));
			}

			/************************************************************************/
			/* Color Write                                                          */
			/************************************************************************/

			ColorWriteMasks NativeGL3StateManager::GetColorWriteMasks(int32 rtIndex)
			{
				if (rtIndex >= 0 && rtIndex < countof(m_colorWriteMasks))
				{
					return m_colorWriteMasks[rtIndex];
				}
				return ColorWrite_All;
			}

			void NativeGL3StateManager::SetColorWriteMasks(int32 rtIndex, ColorWriteMasks masks)
			{
				if (rtIndex >= 0 && rtIndex < countof(m_colorWriteMasks))
				{
					m_colorWriteMasks[rtIndex] = masks;

					glColorMaski(rtIndex, 
								 masks & ColorWrite_Red,
								 masks & ColorWrite_Green,
								 masks & ColorWrite_Blue, 
								 masks & ColorWrite_Alpha);
				}
			}


			/************************************************************************/
			/* Samplers                                                             */
			/************************************************************************/

			void NativeGL3StateManager::SetSampler(int32 samplerIndex, const ShaderSamplerState& state)
			{
				if (m_textureSlotSamplers)
				{
					assert(samplerIndex >= 0 && samplerIndex < 4);

					ShaderSamplerState& curState = m_cachedSamplerStates[samplerIndex];

					SetSampler(samplerIndex + GL_TEXTURE0, curState, state);
				}
			}

			void NativeGL3StateManager::SetSampler(int32 slotIdx, ShaderSamplerState& curState, const ShaderSamplerState& state)
			{
				if (state.AddressU != curState.AddressU)
				{
					curState.AddressU = state.AddressU;
					m_textureSlotSamplers[slotIdx].SetAddressModeS(GLUtils::ConvertTextureAddress(state.AddressU));
				}

				if (curState.AddressV != state.AddressV)
				{
					curState.AddressV = state.AddressV;
					m_textureSlotSamplers[slotIdx].SetAddressModeT(GLUtils::ConvertTextureAddress(state.AddressV));
				}

				if (curState.AddressW != state.AddressW)
				{
					curState.AddressW = state.AddressW;
					m_textureSlotSamplers[slotIdx].SetAddressModeR(GLUtils::ConvertTextureAddress(state.AddressW));
				}

				if (curState.BorderColor != state.BorderColor)
				{
					curState.BorderColor = state.BorderColor;
					
					Color4 borderColor(state.BorderColor);
					GLfloat borderColorArr[4] = { borderColor.Red, borderColor.Green, borderColor.Blue, borderColor.Alpha };
					
					m_textureSlotSamplers[slotIdx].SetBorderColor(borderColorArr);
				}

				if (curState.MagFilter != state.MagFilter)
				{
					curState.MagFilter = state.MagFilter;

					GLenum filter = GL_NEAREST;
					if (state.MagFilter == TextureFilter::Linear)
						filter = GL_LINEAR;

					m_textureSlotSamplers[slotIdx].SetMagFilter(filter);
				}

				if (curState.MinFilter != state.MinFilter || curState.MipFilter != state.MipFilter)
				{
					curState.MinFilter = state.MinFilter;
					curState.MipFilter = state.MipFilter;

					GLenum filter;
					if (state.MipFilter == TextureFilter::None)
					{
						if (state.MinFilter == TextureFilter::Point)
							filter = GL_NEAREST;
						else
							filter = GL_LINEAR;
					}
					else if (state.MipFilter == TextureFilter::Point)
					{
						if (state.MinFilter == TextureFilter::Point)
							filter = GL_NEAREST_MIPMAP_NEAREST;
						else
							filter = GL_LINEAR_MIPMAP_NEAREST;
					}
					else
					{
						if (state.MinFilter == TextureFilter::Point)
							filter = GL_NEAREST_MIPMAP_LINEAR;
						else
							filter = GL_LINEAR_MIPMAP_LINEAR;
					}

					m_textureSlotSamplers[slotIdx].SetMinFilter(filter);
				}

				int maxAnis = Math::Max(state.MaxAnisotropy, 1);
				if (curState.MaxAnisotropy != maxAnis)
				{
					curState.MaxAnisotropy = maxAnis;

				}

				if (curState.MaxMipLevel != state.MaxMipLevel)
				{
					curState.MaxMipLevel = state.MaxMipLevel;
					m_textureSlotSamplers[slotIdx].SetMaxMip(state.MaxMipLevel);
				}

				if (curState.MipMapLODBias != state.MipMapLODBias)
				{
					curState.MipMapLODBias = state.MipMapLODBias;
					m_textureSlotSamplers[slotIdx].SetMipmapBias(state.MipMapLODBias);
				}
			}

			const ShaderSamplerState& NativeGL3StateManager::getSampler(int32 samplerIndex) const { return m_cachedSamplerStates[samplerIndex]; }

			void NativeGL3StateManager::SetTexture(int32 i, GL3Texture* tex)
			{
				assert(i >= 0 && i < m_textureSlotCount);
				if (m_textureSlots[i] != tex)
				{
					GLuint textureId = 0;
					if (tex)
					{
						if (!tex->isManaged() || tex->getState() == ResourceState::Loaded)
						{
							textureId = tex->getGLTexID();
						}
					}

					if (tex && textureId)
					{
						glActiveTexture(GL_TEXTURE0 + i);
						glBindTexture(tex->getGLTexTarget(), textureId);
					}
					else
					{
						glActiveTexture(GL_TEXTURE0 + i);
						glBindTexture(GL_TEXTURE_1D, 0);
						glBindTexture(GL_TEXTURE_2D, 0);
						glBindTexture(GL_TEXTURE_3D, 0);
						glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
					}
					
					m_textureSlots[i] = tex;
				}
			}

			void NativeGL3StateManager::InitializeDefaultState()
			{
				GLboolean zEnabled = glIsEnabled(GL_DEPTH_TEST);

				//SetAlphaTestParameters(false, CompareFunction::Always, 0);
				SetAlphaBlend(false, BlendFunction::Add, Blend::One, Blend::Zero, 0xffffffff);
				SetSeparateAlphaBlend(false, BlendFunction::Add, Blend::One, Blend::Zero);
				SetDepth(!!zEnabled, !!zEnabled, 0, 0, CompareFunction::LessEqual);

				float psize;
				glGetFloatv(GL_POINT_SIZE, &psize);
				SetPointParameters(psize, 1, 64, false);

				SetStencil(false, StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep, 0, CompareFunction::Always, 0xFFFFFFFF, 0xFFFFFFFF);
				SetStencilTwoSide(false, StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep, CompareFunction::Always);

				SetFillMode(FillMode::Solid);
				SetCullMode(CullMode::None);

				SetColorWriteMasks(0, ColorWrite_All);
				SetColorWriteMasks(1, ColorWrite_All);
				SetColorWriteMasks(2, ColorWrite_All);
				SetColorWriteMasks(3, ColorWrite_All);

				m_textureSlotCount = 16;
				glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_textureSlotCount);
				
				if (m_cachedSamplerStates)
					delete[] m_cachedSamplerStates;

				if (m_textureSlotSamplers)
					delete[] m_textureSlotSamplers;

				if (m_textureSlots)
					delete[] m_textureSlots;

				m_cachedSamplerStates = new ShaderSamplerState[m_textureSlotCount];
				m_textureSlotSamplers = new GLSampler[m_textureSlotCount];
				m_textureSlots = new GL3Texture * [m_textureSlotCount]();

				for (int i = 0; i < m_textureSlotCount; i++)
				{
					m_textureSlotSamplers[i].Bind(i);

					ShaderSamplerState& sss = m_cachedSamplerStates[i];
					sss.AddressU = TextureAddressMode::Wrap;
					sss.AddressV = TextureAddressMode::Wrap;
					sss.AddressW = TextureAddressMode::Wrap;
					sss.BorderColor = 0x00000000;
					sss.MagFilter = TextureFilter::Point;
					sss.MinFilter = TextureFilter::Point;
					sss.MipFilter = TextureFilter::None;
					sss.MipMapLODBias = 0;
					sss.MaxMipLevel = 1000;
					sss.MaxAnisotropy = 1;

					GLfloat borderColor[4] = { 0 };
					m_textureSlotSamplers[i].SetAddressModeS(GL_REPEAT);
					m_textureSlotSamplers[i].SetAddressModeT(GL_REPEAT);
					m_textureSlotSamplers[i].SetAddressModeR(GL_REPEAT);
					m_textureSlotSamplers[i].SetBorderColor(borderColor);
					m_textureSlotSamplers[i].SetMinFilter(GL_NEAREST);
					m_textureSlotSamplers[i].SetMagFilter(GL_NEAREST);
					m_textureSlotSamplers[i].SetMaxMip(1000);
					m_textureSlotSamplers[i].SetMipmapBias(0);
				}
			}


			//////////////////////////////////////////////////////////////////////////
			
			GL3RenderStateManager::GL3RenderStateManager(GL3RenderDevice* device, NativeGL3StateManager* nsmgr)
				: RenderStateManager(device), m_device(device), m_stMgr(nsmgr)
			{
			}
			GL3RenderStateManager::~GL3RenderStateManager()
			{

			}

			bool GL3RenderStateManager::getScissorTestEnabled()
			{
				GLboolean scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);
				return !!scissorTestEnabled;
			}
			Apoc3D::Math::Rectangle GL3RenderStateManager::getScissorTestRect()
			{
				GLint scissor[4];
				glGetIntegerv(GL_SCISSOR_BOX, scissor);

				return Apoc3D::Math::Rectangle(scissor[0], scissor[1], scissor[2], scissor[3]);
			}
			void GL3RenderStateManager::setScissorTest(bool enable, const Apoc3D::Math::Rectangle* r)
			{
				if (enable)
				{
					glEnable(GL_SCISSOR_TEST);
					
					Apoc3D::Math::Rectangle rect = *r;

					// normalize rect if the width or height is negative, meaning that the actual X, Y should be added by the Width/Height
					// 
					if (rect.X > rect.getRight())
					{
						rect.X = rect.X + rect.Width;
						rect.Width = -rect.Width;
					}
					if (rect.Y > rect.getBottom())
					{
						rect.Y = rect.Y + rect.Height;
						rect.Height = -rect.Height;
					}

					glScissor(r->X, r->Y, r->Width, r->Height);
				}
				else
				{
					glDisable(GL_SCISSOR_TEST);
					// GL requires resetting the scissor when disabling
					GLint viewport[4];
					glGetIntegerv(GL_VIEWPORT, viewport);
					glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
				}
			}

			ColorWriteMasks GL3RenderStateManager::getColorWriteMasks(uint32 rtIndex)
			{
				return m_stMgr->GetColorWriteMasks(rtIndex);
			}

			void GL3RenderStateManager::setColorWriteMasks(uint32 rtIndex, ColorWriteMasks masks)
			{
				m_stMgr->SetColorWriteMasks(rtIndex, masks);
			}
		}
	}
}