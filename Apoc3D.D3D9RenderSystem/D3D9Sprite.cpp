
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
				: Sprite(device)
			{
				HRESULT hr = D3DXCreateSprite(device->getDevice(), &m_sprite);
				assert(SUCCEEDED(hr));
			}

			void D3D9Sprite::Begin(bool alphabled)
			{
				m_sprite->Begin(alphabled ? D3DXSPRITE_ALPHABLEND : 0);
			}
			void D3D9Sprite::End()
			{
				m_sprite->End();
			}

			void D3D9Sprite::Draw(Texture* texture, Vector2 pos, uint color)
			{
				D3DVector3 position;
				position.x = Vector2Utils::GetX(pos);
				position.y = Vector2Utils::GetY(pos);
				position.z = 0;
				m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
					0, 0, &position, color);
			}
			void D3D9Sprite::Draw(Texture* texture, const PointF& pos, uint color)
			{
				D3DVector3 position;
				position.x = pos.X;
				position.y = pos.Y;
				position.z = 0;
				m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
					0, 0, &position, color);
			}
			void D3D9Sprite::Draw(Texture* texture, int x, int y, uint color)
			{
				D3DVector3 position;
				position.x = static_cast<float>(x);
				position.y = static_cast<float>(y);
				position.z = 0;
				m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
					0, 0, &position, color);
			}
			void D3D9Sprite::Draw(Texture* texture, 
				const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, uint color)
			{

				D3DVector3 position;
				position.x = static_cast<float>(dstRect.X);
				position.y = static_cast<float>(dstRect.Y);
				position.z = 0;

				D3DVector3 center;
				center.x = static_cast<float>(position.x + dstRect.Width * 0.5f);
				center.y = static_cast<float>(position.y + dstRect.Height * 0.5f);
				center.z = 0;

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
				
					m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						&r, &center, &position, color);
				}
				else
				{
					m_sprite->Draw(static_cast<D3D9Texture*>(texture)->getInternal2D(),
						0, &center, &position, color);
				}
			}

			void D3D9Sprite::SetTransform(const Matrix& matrix)
			{
				m_sprite->SetTransform(reinterpret_cast<const D3DMatrix*>(&matrix));
			}

			void D3D9Sprite::ReleaseVolatileResource()
			{
				m_sprite->OnLostDevice();
			}
			void D3D9Sprite::ReloadVolatileResource()
			{
				m_sprite->OnResetDevice();
			}
		}
	}
}