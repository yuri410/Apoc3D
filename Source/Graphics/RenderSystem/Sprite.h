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
#ifndef SPRITE_H
#define SPRITE_H

#include "Common.h"
#include "Math/Vector.h"
#include "Math/Rectangle.h"
#include "Math/Matrix.h"

using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace RenderSystem
		{
			class APAPI Sprite
			{
			private:
				RenderDevice* m_renderDevice;
			protected:
				Sprite(RenderDevice* rd)
					m_renderDevice(rd)
				{
				}
			public:
				virtual void DrawQuad(const GeometryData* quad, PostEffect* effect) = 0;
				virtual void Draw(Texture* texture, const Rectangle &rect, uint color) = 0;
				virtual void Draw(Texture* texture, Vector pos, uint color) = 0;
				virtual void Draw(Texture* texture, int x, int y, uint color) = 0;
				virtual void Draw(Texture* texture, const Rectangle &dstRect, const Rectangle* srcRect, uint color) = 0;

				virtual void SetTransform(const Matrix &matrix);
			};
		}
	}
}
#endif