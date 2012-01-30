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
				: Sprite(device), m_gl1device(device), m_alphaEnabled(false)
			{

			}
			GL1Sprite::~GL1Sprite()
			{

			}
			void GL1Sprite::Begin(bool alphabled, bool useStack)
			{
				m_alphaEnabled = alphabled;

				if (GLEE_ARB_vertex_program)
				{
					glDisable(GL_VERTEX_PROGRAM_ARB);
				}
				if (GLEE_ARB_fragment_program)
				{
					glDisable(GL_FRAGMENT_PROGRAM_ARB);
				}
				
				if (alphabled)
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

					mgr->SetAlphaBlend(true, BLFUN_Add, BLEND_SourceAlpha, BLEND_InverseSourceAlpha, 0);
					mgr->SetSeparateAlphaBlend(false, m_oldSepBlendFunc, m_oldSepSrcBlend, m_oldSepDstBlend);
				}
				Sprite::Begin(alphabled, useStack);

				// transform matrices and be push on the stack as no other ways can be used access
				// them in the engine while drawing sprites in fixed-function pipeline
				
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
					
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
				if (isUsingStack())
				{
					D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());

					D3DMatrix trans = baseTrans;
					trans._41 += Vector2Utils::GetX(pos);
					trans._42 += Vector2Utils::GetY(pos);

					m_sprite->SetTransform(&trans);

					HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						NULL, NULL, NULL, color);
					assert(SUCCEEDED(hr));

					m_sprite->SetTransform(&baseTrans);
				}
				else
				{
					D3DVector3 position;
					position.x = Vector2Utils::GetX(pos);
					position.y = Vector2Utils::GetY(pos);
					position.z = 0;
					HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						0, 0, &position, color);
					assert(SUCCEEDED(hr));
				}
			}
			void GL1Sprite::Draw(Texture* texture, const Point& pos, uint color)
			{
				if (isUsingStack())
				{
					D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());

					D3DMatrix trans = baseTrans;
					trans._41 += pos.X;
					trans._42 += pos.Y;

					m_sprite->SetTransform(&trans);

					HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						NULL, NULL, NULL, color);
					assert(SUCCEEDED(hr));

					m_sprite->SetTransform(&baseTrans);
				}
				else
				{
					D3DVector3 position;
					position.x = (float)pos.X;
					position.y = (float)pos.Y;
					position.z = 0;
					HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						0, 0, &position, color);
					assert(SUCCEEDED(hr));
				}
			}
			void GL1Sprite::Draw(Texture* texture, const PointF& pos, uint color)
			{
				if (isUsingStack())
				{
					D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());

					D3DMatrix trans = baseTrans;
					trans._41 += pos.X;
					trans._42 += pos.Y;

					m_sprite->SetTransform(&trans);

					HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						NULL, NULL, NULL, color);
					assert(SUCCEEDED(hr));

					m_sprite->SetTransform(&baseTrans);
				}
				else
				{
					D3DVector3 position;
					position.x = pos.X;
					position.y = pos.Y;
					position.z = 0;
					HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						0, 0, &position, color);
					assert(SUCCEEDED(hr));
				}
			}
			void GL1Sprite::Draw(Texture* texture, int x, int y, uint color)
			{
				if (isUsingStack())
				{
					D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());

					D3DMatrix trans = baseTrans;
					trans._41 += x;
					trans._42 += y;

					m_sprite->SetTransform(&trans);

					HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						NULL, NULL, NULL, color);
					assert(SUCCEEDED(hr));

					m_sprite->SetTransform(&baseTrans);
				}
				else
				{
					D3DVector3 position;
					position.x = static_cast<float>(x);
					position.y = static_cast<float>(y);
					position.z = 0;
					HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						0, 0, &position, color);
					assert(SUCCEEDED(hr));
				}
				
			}
			void GL1Sprite::Draw(Texture* texture, 
				const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, uint color)
			{
				
				D3DVector3 position;
				position.x = static_cast<float>(dstRect.X);
				position.y = static_cast<float>(dstRect.Y);
				position.z = 0;

				//D3DVector3 center;
				//center.x = static_cast<float>(position.x + dstRect.Width * 0.5f);
				//center.y = static_cast<float>(position.y + dstRect.Height * 0.5f);
				//center.z = 0;

				if (srcRect)
				{
					//LONG    left;
					//LONG    top;
					//LONG    right;
					//LONG    bottom;
					RECT r = { 
						(LONG)srcRect->X, 
						(LONG)srcRect->Y,
						(LONG)srcRect->getRight(),
						(LONG)srcRect->getBottom()
					};

					if (r.left > r.right)
					{
						LONG temp = r.right;
						r.right = r.left;
						r.left = temp;
						position.x -= (float)( r.left - r.right);
					}
					if (r.top>r.bottom)
					{
						LONG temp = r.bottom;
						r.bottom = r.top;
						r.top = temp;
						position.y -= (float)( r.top - r.bottom);
					}

					if (srcRect->Width != dstRect.Width || srcRect->Height != dstRect.Height)
					{
						D3DMatrix trans;
						D3DXMatrixTranslation(&trans, position.x, position.y, position.z);
						trans._11 = (float)dstRect.Width / (float)srcRect->Width;
						trans._22 = (float)dstRect.Height / (float)srcRect->Height;

						D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());
						D3DXMatrixMultiply(&trans, &trans,&baseTrans);
						
						m_sprite->SetTransform(&trans);

						HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
							&r, NULL, NULL, color);
						assert(SUCCEEDED(hr));

						m_sprite->SetTransform(&baseTrans);
					}
					else
					{
						if (isUsingStack())
						{
							D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());
							//D3DXMatrixMultiply(&trans, &trans,&baseTrans);
							D3DMatrix trans = baseTrans;
							trans._41 += position.x;
							trans._42 += position.y;
							trans._43 += position.z;

							m_sprite->SetTransform(&trans);

							HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
								&r, NULL, NULL, color);
							assert(SUCCEEDED(hr));

							m_sprite->SetTransform(&baseTrans);
						}
						else
						{
							HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
								&r, NULL, &position, color);
							assert(SUCCEEDED(hr));
						}
					}
					
				}
				else
				{
					if (texture->getWidth() != dstRect.Width || texture->getHeight() != dstRect.Height)
					{
						D3DMatrix trans;
						D3DXMatrixTranslation(&trans, position.x, position.y, position.z);
						trans._11 = (float)dstRect.Width / (float)texture->getWidth();
						trans._22 = (float)dstRect.Height / (float)texture->getHeight();
						
						D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());
						D3DXMatrixMultiply(&trans,  &trans, &baseTrans);

						m_sprite->SetTransform(&trans);

						HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
							NULL, NULL, NULL, color);
						assert(SUCCEEDED(hr));

						m_sprite->SetTransform(&baseTrans);
					}
					else
					{
						if (isUsingStack())
						{
							D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());
							D3DMatrix trans = baseTrans;
							trans._41 += position.x;
							trans._42 += position.y;
							trans._43 += position.z;

							m_sprite->SetTransform(&trans);

							HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
								NULL, NULL, NULL, color);
							assert(SUCCEEDED(hr));

							m_sprite->SetTransform(&baseTrans);
						}
						else
						{
							HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
								NULL, NULL, &position, color);
							assert(SUCCEEDED(hr));
						}
						
					}
				}
			}

			void GL1Sprite::Flush()
			{
				//HRESULT hr = m_sprite->Flush();
				//assert(SUCCEEDED(hr));
			}

			void GL1Sprite::SetTransform(const Matrix& matrix)
			{
				//HRESULT hr = m_sprite->SetTransform(reinterpret_cast<const D3DMatrix*>(&getTransform()));
				//assert(SUCCEEDED(hr));

				Sprite::SetTransform(matrix);
			}


		}
	}
}