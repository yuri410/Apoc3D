/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

#include "GL1RenderStateManager.h"

#include "GL1RenderDevice.h"
#include "GL1Utils.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{

			NativeGL1StateManager::NativeGL1StateManager(GL1RenderDevice* device)
				: m_device(device)
			{
				InitializeDefaultState();
			}
			NativeGL1StateManager::~NativeGL1StateManager()
			{

			}

			void NativeGL1StateManager::SetCullMode(CullMode mode)
			{
				m_cachedCullMode = mode;

				switch( mode )
				{
				case CULL_None:
					glDisable( GL_CULL_FACE );
					return;
				default:
				case CULL_Clockwise:
					glCullFace( GL_BACK );
					break;
				case CULL_CounterClockwise:
					glCullFace( GL_FRONT );
					break;
				}

				glEnable( GL_CULL_FACE );
				glFrontFace(GL_CCW);

			}
			void NativeGL1StateManager::SetFillMode(FillMode mode)
			{
				m_cachedFillMode = mode;
				GLenum pm = 0;
				switch (mode)
				{
				case FILL_Point:
					pm = GL_POINT;
					break;
				case FILL_Solid:
					pm = GL_FILL;
					break;
				case FILL_WireFrame:
					pm = GL_LINE;
					break;
				}
				glPolygonMode(GL_FRONT_AND_BACK, pm);
			}
			void NativeGL1StateManager::SetAlphaTestParameters(bool enable, uint32 reference)
			{
				m_cachedAlphaTestEnable = enable;
				m_cachedAlphaReference = reference;

				if (enable)
				{
					glEnable(GL_ALPHA_TEST);
				}
				else
				{
					glDisable(GL_ALPHA_TEST);
				}

				// the assumption here is that reference ranges form 0 to 255
				// this is true for this RS and the D3D9 RS.
				glAlphaFunc(GLUtils::ConvertCompare(m_cachedAlphaTestFunction), reference/255.0f);
			}
			void NativeGL1StateManager::SetAlphaTestParameters(bool enable, CompareFunction func, uint32 reference)
			{
				m_cachedAlphaTestEnable = enable;
				m_cachedAlphaReference = reference;
				m_cachedAlphaTestFunction = func;

				if (enable)
				{
					glEnable(GL_ALPHA_TEST);
				}
				else
				{
					glDisable(GL_ALPHA_TEST);
				}

				// the assumption here is that reference ranges form 0 to 255.0
				// this is true for this RS and the D3D9 RS.
				glAlphaFunc(GLUtils::ConvertCompare(func), reference/255.0f);
				
			}
			void NativeGL1StateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
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

				if (GLEE_ARB_imaging || GLEE_VERSION_1_3)
				{
					glBlendEquation(GLUtils::ConvertBlendFunction(func));
				}
			}
			void NativeGL1StateManager::SetAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend, uint32 factor)
			{
				m_cachedAlphaBlendEnable = enable;
				m_cachedAlphaBlendFunction = func;
				m_cachedAlphaSourceBlend = srcBlend;
				m_cachedAlphaDestBlend = dstBlend;
				m_cachedAlphaBlendFactor = factor;

				if (enable)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);

				glBlendFunc(GLUtils::ConvertBlend(srcBlend), GLUtils::ConvertBlend(dstBlend));

				if (GLEE_ARB_imaging)
				{
					glBlendEquation(GLUtils::ConvertBlendFunction(func));
					glBlendColor(GetColorR(factor)/255.0f,GetColorG(factor)/255.0f,GetColorB(factor)/255.0f,GetColorA(factor)/255.0f);
				}

			}

			void NativeGL1StateManager::setAlphaBlendEnable(bool enable)
			{
				m_cachedAlphaBlendEnable = enable;

				if (enable)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);
			}
			void NativeGL1StateManager::setAlphaBlendOperation(BlendFunction func)
			{
				m_cachedAlphaBlendFunction = func;
				if (GLEE_ARB_imaging)
				{
					glBlendEquation(GLUtils::ConvertBlendFunction(func));
				}
			}
			void NativeGL1StateManager::setAlphaSourceBlend(Blend srcBlend)
			{
				m_cachedAlphaSourceBlend = srcBlend;

				glBlendFunc(GLUtils::ConvertBlend(srcBlend), GLUtils::ConvertBlend(m_cachedAlphaDestBlend));

			}
			void NativeGL1StateManager::setAlphaDestinationBlend(Blend dstBlend)
			{
				m_cachedAlphaDestBlend = dstBlend;

				glBlendFunc(GLUtils::ConvertBlend(m_cachedAlphaSourceBlend), GLUtils::ConvertBlend(dstBlend));
			}

			void NativeGL1StateManager::SetSeparateAlphaBlend(bool enable, BlendFunction func, Blend srcBlend, Blend dstBlend)
			{
				m_cachedSepAlphaBlendEnable = enable;
				m_cachedSepAlphaBlendFunction = func;
				m_cachedSepAlphaSourceBlend = srcBlend;
				m_cachedSepAlphaDestBlend = dstBlend;

				if (GLEE_ARB_imaging || GLEE_VERSION_1_3)
				{

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
			}

			void NativeGL1StateManager::SetDepth(bool enable, bool writeEnable)
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
			void NativeGL1StateManager::SetDepth(bool enable, bool writeEnable, float bias, float slopebias, CompareFunction compare)
			{
				m_cachedDepthBufferEnabled = enable;
				m_cachedDepthBufferWriteEnabled = writeEnable;
				m_cachedDepthBias = bias;
				m_cachedSlopeScaleDepthBias = slopebias;
				m_cachedDepthBufferFunction = compare;

				if (enable)
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);

				if (writeEnable)
					glDepthMask(GL_TRUE);
				else
					glDepthMask(GL_FALSE);

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
			void NativeGL1StateManager::SetPointParameters(float size, float maxSize, float minSize, bool pointSprite)
			{
				m_cachedPointSize = size;
				m_cachedPointSizeMax = maxSize;
				m_cachedPointSizeMin = minSize;
				m_cachedPointSpriteEnabled = pointSprite;

				glPointSize(size);

				if (GLEE_VERSION_1_4)
				{
					glPointParameterf(GL_POINT_SIZE_MIN, minSize);
					glPointParameterf(GL_POINT_SIZE_MAX, maxSize);
				} 
				else if (GLEE_ARB_point_parameters)
				{
					glPointParameterfARB(GL_POINT_SIZE_MIN, minSize);
					glPointParameterfARB(GL_POINT_SIZE_MAX, maxSize);
				} 
				else if (GLEE_EXT_point_parameters)
				{
					glPointParameterfEXT(GL_POINT_SIZE_MIN, minSize);
					glPointParameterfEXT(GL_POINT_SIZE_MAX, maxSize);
				}

				if (GLEE_VERSION_2_0 ||	GLEE_ARB_point_sprite)
				{
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
					if (GLEE_VERSION_1_3 || 
						GLEE_ARB_multitexture)
					{
						GLint units;
						glGetIntegerv( GL_MAX_TEXTURE_UNITS, &units );

						for (int i=0;i<units;i++)
						{
							glActiveTextureARB(GL_TEXTURE0 + i);
							glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, pointSprite ? GL_TRUE : GL_FALSE);
						}
						glActiveTextureARB(GL_TEXTURE0);
					}
				}
				

			}
			void NativeGL1StateManager::SetStencil(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, 
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
				
				if (GLEE_EXT_stencil_two_side)
					glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
				
				glStencilMask(mask);
				glStencilFunc(GLUtils::ConvertCompare(func), ref, mask);
				glStencilOp(
					GLUtils::ConvertStencilOperation(fail),
					GLUtils::ConvertStencilOperation(depthFail), 
					GLUtils::ConvertStencilOperation(pass));

			}
			void NativeGL1StateManager::SetStencilTwoSide(bool enabled, StencilOperation fail, StencilOperation depthFail, StencilOperation pass, CompareFunction func)
			{
				m_cachedTwoSidedStencilMode = enabled;
				m_cachedCounterClockwiseStencilFail = fail;
				m_cachedCounterClockwiseStencilDepthBufferFail = depthFail;
				m_cachedCounterClockwiseStencilPass = pass;
				m_cachedCounterClockwiseStencilFunction = func;

				// Front is the 2nd side
				if(GLEE_VERSION_2_0) 
				{
					glStencilMaskSeparate(GL_FRONT, m_cachedStencilMask);
					glStencilFuncSeparate(GL_FRONT, GLUtils::ConvertCompare(func), m_cachedRefrenceStencil, m_cachedStencilMask);
					glStencilOpSeparate(GL_FRONT, 
						GLUtils::ConvertStencilOperation(fail, true),
						GLUtils::ConvertStencilOperation(depthFail, true), 
						GLUtils::ConvertStencilOperation(pass, true));
				}
				else if (GLEE_EXT_stencil_two_side)
				{
					glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);

					glActiveStencilFaceEXT(GL_FRONT);
					glStencilMask(m_cachedStencilMask);
					glStencilFunc(GLUtils::ConvertCompare(func), m_cachedRefrenceStencil, m_cachedStencilMask);
					glStencilOp(
						GLUtils::ConvertStencilOperation(fail, true),
						GLUtils::ConvertStencilOperation(depthFail, true), 
						GLUtils::ConvertStencilOperation(pass, true));
				}
			}


			void NativeGL1StateManager::InitializeDefaultState()
			{
				GLboolean zEnabled = glIsEnabled(GL_DEPTH_TEST);

				SetAlphaTestParameters(false, COMFUN_Always, 0);
				SetAlphaBlend(false, BLFUN_Add, BLEND_One, BLEND_Zero, 0xffffffff);
				SetSeparateAlphaBlend(false, BLFUN_Add, BLEND_One, BLEND_Zero);
				SetDepth(!!zEnabled, !!zEnabled, 0, 0, COMFUN_LessEqual);

				float psize;
				glGetFloatv(GL_POINT_SIZE, &psize);
				SetPointParameters(psize, 1, 64, false);

				SetStencil(false, STOP_Keep, STOP_Keep, STOP_Keep, 0, COMFUN_Always, 0xFFFFFFFF, 0xFFFFFFFF);
				SetStencilTwoSide(false, STOP_Keep, STOP_Keep, STOP_Keep, COMFUN_Always);
			}



			
			GL1RenderStateManager::GL1RenderStateManager(GL1RenderDevice* device, NativeGL1StateManager* nsmgr)
				: RenderStateManager(device), m_device(device), clipPlaneEnable(0), m_stMgr(nsmgr)
			{
			}
			GL1RenderStateManager::~GL1RenderStateManager()
			{

			}

			bool GL1RenderStateManager::getScissorTestEnabled()
			{
				GLboolean scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);
				return !!scissorTestEnabled;
			}
			Apoc3D::Math::Rectangle GL1RenderStateManager::getScissorTestRect()
			{
				GLint scissor[4];
				glGetIntegerv(GL_SCISSOR_BOX, scissor);

				return Apoc3D::Math::Rectangle(scissor[0], scissor[1], scissor[2], scissor[3]);
			}
			void GL1RenderStateManager::setScissorTest(bool enable, const Apoc3D::Math::Rectangle* r)
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

		}
	}
}