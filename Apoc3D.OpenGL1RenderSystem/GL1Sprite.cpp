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
#include "GL1Sprite.h"
#include "GL1RenderDevice.h"
#include "GL1Texture.h"
#include "GL1RenderStateManager.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			GL1Sprite::GL1Sprite(GL1RenderDevice* device)
				: Sprite(device), m_gl1device(device), m_alphaEnabled(false),
				m_vboSupported(false)
			{

			}
			GL1Sprite::~GL1Sprite()
			{

			}
			void GL1Sprite::Begin(SpriteSettings settings)
			{
				m_alphaEnabled = (settings & Sprite::SPR_AlphaBlended) == Sprite::SPR_AlphaBlended;

				// disable all shaders
				m_gl1device->BindPixelShader(0);
				m_gl1device->BindVertexShader(0);
				
				
				if (m_alphaEnabled)
				{
					NativeGL1StateManager* mgr = m_gl1device->getNativeState();

					// back up the current blending settings
					// cant use glPushAttribe because the state manager can not keep up the change
					m_oldAlphaBlendEnabled = mgr->getAlphaBlendEnable();
					m_oldBlendConst = mgr->getAlphaBlendFactor();
					m_oldBlendFunc = mgr->getAlphaBlendOperation();
					m_oldSrcBlend = mgr->getAlphaSourceBlend();
					m_oldDstBlend = mgr->getAlphaDestinationBlend();

					m_oldSepAlphaBlendEnabled = mgr->getSeparateAlphaBlendEnable();
					m_oldSepDstBlend = mgr->getSeparateAlphaDestinationBlend();
					m_oldSepSrcBlend = mgr->getSeparateAlphaSourceBlend();
					m_oldSepBlendFunc = mgr->getSeparateAlphaBlendOperation();

					mgr->SetAlphaBlend(true, BlendFunction::Add, Blend::SourceAlpha, Blend::InverseSourceAlpha, 0);
					mgr->SetSeparateAlphaBlend(false, m_oldSepBlendFunc, m_oldSepSrcBlend, m_oldSepDstBlend);
				}
				Sprite::Begin(settings);

				// transform matrices and be push on the stack as no other ways can be used access
				// them in the engine while drawing sprites in fixed-function pipeline
				
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				// NB: must load the identity because the glOrtho will multiply a new one on this
				glLoadIdentity(); 
				GLint viewport[4];
				glGetIntegerv(GL_VIEWPORT, viewport);
				glOrtho(viewport[0], viewport[2], viewport[3], viewport[1], 0, 1);

				glMatrixMode(GL_MODELVIEW);

				glPushMatrix();
				glLoadIdentity();

			}
			void GL1Sprite::End()
			{
				if (m_alphaEnabled)
				{
					// restore blending states
					NativeGL1StateManager* mgr = m_gl1device->getNativeState();

					mgr->SetAlphaBlend(m_oldAlphaBlendEnabled, m_oldBlendFunc, m_oldSrcBlend, m_oldDstBlend, m_oldBlendConst);
					mgr->SetSeparateAlphaBlend(m_oldSepAlphaBlendEnabled, m_oldSepBlendFunc, m_oldSepSrcBlend, m_oldSepDstBlend);
				}

				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();

			}

			// the sprite's drawing is emulated by either glVertex immediate drawing, or 
			// VBO if the extension is available

			// the transformation is done by OpenGL. 

			void GL1Sprite::Draw(Texture* texture, Vector2 pos, uint color)
			{
				if (m_vboSupported)
				{

				}
				else
				{
					glColor4ub(CV_GetColorR(color), CV_GetColorG(color), CV_GetColorB(color), CV_GetColorA(color));

					//glBindTexture()
					glBegin(GL_QUADS);

					glTexCoord2f(0,0);
					glVertex4f(0, 0, 0, 1);
					glTexCoord2f(0,1);
					glVertex4f(0, pos.Y, 0, 1);
					glTexCoord2f(1,1);
					glVertex4f(pos.X, pos.Y, 0, 1);
					glTexCoord2f(1,0);
					glVertex4f(pos.X, 0, 0, 1);

					glEnd();
				}
			}
			void GL1Sprite::Draw(Texture* texture, const Point& pos, uint color)
			{
				if (m_vboSupported)
				{

				}
				else
				{
					glColor4ub(CV_GetColorR(color), CV_GetColorG(color), CV_GetColorB(color), CV_GetColorA(color));

					//glBindTexture()
					glBegin(GL_QUADS);

					glTexCoord2f(0,0);
					glVertex4f(0, 0, 0, 1);
					glTexCoord2f(0,1);
					glVertex4f(0, (float)pos.Y, 0, 1);
					glTexCoord2f(1,1);
					glVertex4f((float)pos.X, (float)pos.Y, 0, 1);
					glTexCoord2f(1,0);
					glVertex4f((float)pos.X, 0, 0, 1);

					glEnd();
				}
			}
			void GL1Sprite::Draw(Texture* texture, const PointF& pos, uint color)
			{
				if (m_vboSupported)
				{

				}
				else
				{
					glColor4ub(CV_GetColorR(color), CV_GetColorG(color), CV_GetColorB(color), CV_GetColorA(color));

					//glBindTexture()
					glBegin(GL_QUADS);

					glTexCoord2f(0,0);
					glVertex4f(0, 0, 0, 1);
					glTexCoord2f(0,1);
					glVertex4f(0, pos.Y, 0, 1);
					glTexCoord2f(1,1);
					glVertex4f(pos.X, pos.Y, 0, 1);
					glTexCoord2f(1,0);
					glVertex4f(pos.X, 0, 0, 1);

					glEnd();
				}
			}
			void GL1Sprite::Draw(Texture* texture, int x, int y, uint color)
			{
				Draw(texture, Point(x,y), color);
			}
			void GL1Sprite::Draw(Texture* texture, 
				const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, uint color)
			{
				
				float position[3];
				position[0] = static_cast<float>(dstRect.X);
				position[1] = static_cast<float>(dstRect.Y);
				position[2] = 0;



				if (srcRect)
				{
					// util for standardizing rect
					struct TempRect
					{
						int    left;
						int    top;
						int    right;
						int    bottom;
					};
					TempRect r = { 
						srcRect->X, 
						srcRect->Y,
						srcRect->getRight(),
						srcRect->getBottom()
					};

					// In some cases, the X,Y of the rect is not always the top-left corner,
					// when the Width or Height is negative. Standardize it.
					if (r.left > r.right)
					{
						int temp = r.right;
						r.right = r.left;
						r.left = temp;
						position[0] -= (float)( r.left - r.right);
					}
					if (r.top>r.bottom)
					{
						int temp = r.bottom;
						r.bottom = r.top;
						r.top = temp;
						position[1] -= (float)( r.top - r.bottom);
					}

					// scaling is required when the 2 rects' size do not match
					if (srcRect->Width != dstRect.Width || srcRect->Height != dstRect.Height)
					{
						// calculate a scaling and translation matrix
						Matrix trans;
						Matrix::CreateTranslation(trans, position[0], position[1], position[2]);
						
						trans.M11 = (float)dstRect.Width / (float)srcRect->Width;
						trans.M22 = (float)dstRect.Height / (float)srcRect->Height;

						//  "trans" * transform for the result
						Matrix result;
						Matrix::Multiply(result, trans, getTransform());

						glLoadMatrixf(reinterpret_cast<const float*>(&trans));
						
						// As the position have been added to the transform, 
						// draw the texture at the origin
						Draw(texture, 0,0, color);

						// restore the normal transform to keep others working well
						glLoadMatrixf(reinterpret_cast<const float*>(&getTransform()));
					}
					else
					{
						Draw(texture, PointF(position[0], position[1]), color);
					}
					
				}
				else
				{
					// scaling is required when the dstRect's size do not match the texture's
					if (texture->getWidth() != dstRect.Width || texture->getHeight() != dstRect.Height)
					{
						Matrix trans;
						Matrix::CreateTranslation(trans, position[0], position[1], position[2]);

						trans.M11 = (float)dstRect.Width / (float)texture->getWidth();
						trans.M22 = (float)dstRect.Height / (float)texture->getHeight();
						
						Matrix result;
						Matrix::Multiply(result, trans, getTransform());

						glLoadMatrixf(reinterpret_cast<const float*>(&trans));

						Draw(texture, 0,0, color);

						glLoadMatrixf(reinterpret_cast<const float*>(&getTransform()));
					}
					else
					{
						Draw(texture, PointF(position[0], position[1]), color);
					}
				}
			}

			void GL1Sprite::Flush()
			{
				// When using VBO, draws the quads in buffer then clears it

			}

			void GL1Sprite::SetTransform(const Matrix& matrix)
			{
				if (m_vboSupported)
					Flush();

				Sprite::SetTransform(matrix);

				// This will set the transformation. 
				// When using stack, the top matrix will be used.
				glLoadMatrixf(reinterpret_cast<const float*>(&getTransform()));
			}


		}
	}
}