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

#ifndef GL1SPRITE_H
#define GL1SPRITE_H

#include "GL1Common.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace GL1RenderSystem
		{
			class GL1Sprite : public Sprite
			{
			public:
				GL1Sprite(GL1RenderDevice* device);
				~GL1Sprite();

				virtual void Begin(SpriteSettings settings);
				virtual void End();

				virtual void Draw(Texture* texture, const Apoc3D::Math::Rectangle &rect, uint color)
				{
					Draw(texture, rect, 0, color);
				}
				virtual void Draw(Texture* texture, Vector2 pos, uint color);
				virtual void Draw(Texture* texture, const PointF& pos, uint color);
				virtual void Draw(Texture* texture, const Point& pos, uint color);
				virtual void Draw(Texture* texture, int x, int y, uint color);
				virtual void Draw(Texture* texture, const Apoc3D::Math::Rectangle& dstRect, const Apoc3D::Math::Rectangle* srcRect, uint color);

				virtual void Flush();

				virtual void SetTransform(const Matrix& matrix);
			private:
				GL1RenderDevice* m_gl1device;
				bool m_vboSupported;

				bool m_alphaEnabled;

				bool m_oldAlphaBlendEnabled;
				BlendFunction m_oldBlendFunc;
				Blend m_oldSrcBlend;
				Blend m_oldDstBlend;
				uint m_oldBlendConst;


				bool m_oldSepAlphaBlendEnabled;
				Blend m_oldSepSrcBlend;
				Blend m_oldSepDstBlend;
				BlendFunction m_oldSepBlendFunc;
			};
		}
	}
}

#endif