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
#include "D3D9Sprite.h"
#include "D3D9RenderDevice.h"
#include "D3D9Texture.h"

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			D3D9Sprite::D3D9Sprite(D3D9RenderDevice* device)
				: Sprite(device), VolatileResource(device)
			{
				HRESULT hr = D3DXCreateSprite(device->getDevice(), &m_sprite);
				assert(SUCCEEDED(hr));
			}
			D3D9Sprite::~D3D9Sprite()
			{
				m_sprite->Release();
				m_sprite = 0;
			}
			void D3D9Sprite::Begin(bool alphabled, bool useStack)
			{
				HRESULT hr = m_sprite->Begin(alphabled ? D3DXSPRITE_ALPHABLEND : 0);
				assert(SUCCEEDED(hr));
				Sprite::Begin(alphabled, useStack);
			}
			void D3D9Sprite::End()
			{
				HRESULT hr = m_sprite->End();
				assert(SUCCEEDED(hr));
			}


			void D3D9Sprite::Draw(Texture* texture, Vector2 pos, uint color)
			{
				D3DVector3 position;
				position.x = Vector2Utils::GetX(pos);
				position.y = Vector2Utils::GetY(pos);
				position.z = 0;

				HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
					0, 0, &position, color);
				assert(SUCCEEDED(hr));
				
			}
			void D3D9Sprite::Draw(Texture* texture, const Point& pos, uint color)
			{
				D3DVector3 position;
				position.x = (float)pos.X;
				position.y = (float)pos.Y;
				position.z = 0;
				HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
					0, 0, &position, color);
				assert(SUCCEEDED(hr));
			}
			void D3D9Sprite::Draw(Texture* texture, const PointF& pos, uint color)
			{
				D3DVector3 position;
				position.x = pos.X;
				position.y = pos.Y;
				position.z = 0;
				HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
					0, 0, &position, color);
				assert(SUCCEEDED(hr));
			}
			void D3D9Sprite::Draw(Texture* texture, int x, int y, uint color)
			{
				Draw(texture, Point(x,y), color);
			}

			// Auto resizing to fit the target rectangle is implemented in this method.
			// 
			void D3D9Sprite::Draw(Texture* texture, 
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
					// RECT:
					// LONG    left;
					// LONG    top;
					// LONG    right;
					// LONG    bottom;
					RECT r = { 
						(LONG)srcRect->X, 
						(LONG)srcRect->Y,
						(LONG)srcRect->getRight(),
						(LONG)srcRect->getBottom()
					};

					// In some cases, the X,Y of the rect is not always the top-left corner,
					// when the Width or Height is negative. Standardize it.
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

					// scaling is required when the 2 rects' size do not match
					if (srcRect->Width != dstRect.Width || srcRect->Height != dstRect.Height)
					{
						// calculate a scaling and translation matrix
						D3DMatrix trans;
						D3DXMatrixTranslation(&trans, position.x, position.y, position.z);
						trans._11 = (float)dstRect.Width / (float)srcRect->Width;
						trans._22 = (float)dstRect.Height / (float)srcRect->Height;

						// add "trans" at the the beginning for the result
						D3DMatrix baseTrans = reinterpret_cast<const D3DMatrix&>(getTransform());
						D3DXMatrixMultiply(&trans, &trans,&baseTrans);
						
						m_sprite->SetTransform(&trans);

						// As the position have been added to the transform, 
						// draw the texture at the origin
						HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
							&r, NULL, NULL, color);
						assert(SUCCEEDED(hr));

						// restore the normal transform to keep others working well
						m_sprite->SetTransform(&baseTrans);
					}
					else
					{
						HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
							&r, NULL, &position, color);
						assert(SUCCEEDED(hr));
					}
				}
				else
				{
					// scaling is required when the dstRect's size do not match the texture's

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
						HRESULT hr = m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
							NULL, NULL, &position, color);
						assert(SUCCEEDED(hr));
						
					}
				}
			}

			void D3D9Sprite::Flush()
			{
				HRESULT hr = m_sprite->Flush();
				assert(SUCCEEDED(hr));
			}

			void D3D9Sprite::SetTransform(const Matrix& matrix)
			{
				Sprite::SetTransform(matrix);

				// This will set the transformation. 
				// When using stack, the top matrix will be used.
				HRESULT hr = m_sprite->SetTransform(reinterpret_cast<const D3DMatrix*>(&getTransform()));
				assert(SUCCEEDED(hr));
			}

			void D3D9Sprite::ReleaseVolatileResource()
			{
				HRESULT hr = m_sprite->OnLostDevice();
				assert(SUCCEEDED(hr));
			}
			void D3D9Sprite::ReloadVolatileResource()
			{
				HRESULT hr = m_sprite->OnResetDevice();
				assert(SUCCEEDED(hr));
			}
		}
	}
}