
/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "UICommon.h"

#include "Apoc3D/Graphics/RenderSystem/Sprite.h"

namespace Apoc3D
{
	namespace UI
	{
		void gui2DrawRegion9(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerRegion)
		{
			const int MarginWidth = srcRects[0].Width + srcRects[2].Width;
			const int MarginHeight = srcRects[0].Height + srcRects[6].Height;

			Apoc3D::Math::Rectangle destRects[9];
			for (int i=0;i<9;i++)
			{
				destRects[i] = srcRects[i];

				destRects[i].X += dstRect.X - srcRects[0].X;
				destRects[i].Y += dstRect.Y - srcRects[0].Y;
			}

			int centerWidth = dstRect.Width - MarginWidth;
			int centerHeight = dstRect.Height - MarginHeight;

			destRects[1].Width = centerWidth; // top
			destRects[4].Width = centerWidth; // mid
			destRects[7].Width = centerWidth; // bottom

			destRects[3].Height = centerHeight; // left
			destRects[4].Height = centerHeight; // mid
			destRects[5].Height = centerHeight; // right

			destRects[2].X = destRects[5].X = destRects[8].X = destRects[1].getRight();
			destRects[6].Y = destRects[7].Y = destRects[8].Y = destRects[3].getBottom();

			for (int i=0;i<9;i++)
			{
				if (destRects[i].Width > 0 && destRects[i].Height > 0)
					sprite->Draw(texture, destRects[i], &srcRects[i], cv);
			}

			if (centerRegion)
			{
				*centerRegion = destRects[4];
			}
		}


		bool guiDrawRegion9(Sprite* sprite, const Apoc3D::Math::Rectangle& rect, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerRegion)
		{
			const int MarginWidth = srcRects[0].Width + srcRects[2].Width;
			const int MarginHeight = srcRects[0].Height + srcRects[6].Height;

			// size limit
			if (rect.Width >= MarginWidth && rect.Height >= MarginHeight)
			{
				Apoc3D::Math::Rectangle destRect[9];
				for (int i=0;i<9;i++)
				{
					destRect[i] = srcRects[i];

					destRect[i].X += rect.X - srcRects[0].X;
					destRect[i].Y += rect.Y - srcRects[0].Y;
				}

				int eWidth = rect.Width - MarginWidth;
				int eHeight = rect.Height - MarginHeight;

				destRect[1].Width = eWidth; // top
				destRect[4].Width = eWidth; // mid
				destRect[7].Width = eWidth; // bottom

				destRect[3].Height = eHeight; // left
				destRect[4].Height = eHeight; // mid
				destRect[5].Height = eHeight; // right

				destRect[2].X = destRect[5].X = destRect[8].X = destRect[1].getRight();
				destRect[6].Y = destRect[7].Y = destRect[8].Y = destRect[3].getBottom();

				for (int i=0;i<9;i++)
				{
					if (destRect[i].Width > 0 && destRect[i].Height > 0)
						sprite->Draw(texture, destRect[i], &srcRects[i], cv);
				}

				if (centerRegion)
				{
					*centerRegion = destRect[4];
				}
				return true;
			}
			return false;
		}

		bool guiDrawRegion3(Sprite* sprite, const Point& pos, int width, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerRegion)
		{
			int headAndTailWidth = srcRects[0].Width + srcRects[2].Width;

			if (width >= headAndTailWidth)
			{
				int x = pos.X;
				int y = pos.Y;

				Apoc3D::Math::Rectangle dstRects[3] = 
				{
					srcRects[0],
					srcRects[1],
					srcRects[2]
				};

				dstRects[0].Y = dstRects[1].Y = dstRects[2].Y = y;

				dstRects[0].X = x;
				//dstRects[1].X -= srcRect[0].X;
				dstRects[1].X = dstRects[0].getRight();
				dstRects[1].Width = width - headAndTailWidth;
				dstRects[2].X = dstRects[1].getRight();

				for (int i=0;i<3;i++)
				{
					sprite->Draw(texture, dstRects[i], &srcRects[i], cv);
				}
				return true;
			}
			return false;
		}

		bool guiDrawRegion3Vert(Sprite* sprite, const Point& pos, int height, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, Apoc3D::Math::Rectangle* centerRegion)
		{
			int headAndTailHeight = srcRects[0].Height + srcRects[2].Height;

			if (height >= headAndTailHeight)
			{
				int x = pos.X;
				int y = pos.Y;

				Apoc3D::Math::Rectangle dstRects[3] = 
				{
					srcRects[0],
					srcRects[1],
					srcRects[2]
				};

				dstRects[0].X = dstRects[1].X = dstRects[2].X = x;

				dstRects[0].Y = y;
				//dstRects[1].Y -= srcRect[0].Y;
				dstRects[1].Y = dstRects[0].getBottom();
				dstRects[1].Height = height - headAndTailHeight;
				dstRects[2].Y = dstRects[1].getBottom();

				for (int i=0;i<3;i++)
				{
					sprite->Draw(texture, dstRects[i], &srcRects[i], cv);
				}
				return true;
			}
			return false;
		}

		void guiDrawRegion3Capped(Sprite* sprite, const Point& pos, int32 maxWidth, int32 capWidth, 
			ColorValue cv, Texture* texture, const Apoc3D::Math::Rectangle* srcRects)
		{
			int32 cx = pos.X;

			for (int32 i=0;i<3;i++)
			{
				Apoc3D::Math::Rectangle srcRect = srcRects[i];
				Apoc3D::Math::Rectangle dstRect = srcRect;

				dstRect.X = cx;
				dstRect.Y = pos.Y;

				int32 partWidth;

				if (i == 1)
				{
					partWidth = Math::Min(maxWidth - srcRects[0].Width - srcRects[2].Width, capWidth);
				}
				else
				{
					partWidth = Math::Min(srcRect.Width, capWidth);
					srcRect.Width = partWidth;
				}

				dstRect.Width = partWidth;
				sprite->Draw(texture, dstRect, &srcRect, cv);

				cx += partWidth;
				capWidth -= partWidth;

				if (capWidth<=0)
					break;
			}
		}



		void guiDrawRegion9Subbox(Sprite* sprite, const Apoc3D::Math::Rectangle& dstRect, ColorValue cv, 
			Texture* texture, const Apoc3D::Math::Rectangle* srcRects, uint32 subRegionFlags)
		{
			Apoc3D::Math::Rectangle srcRectsTable[9];

			const uint flags[9] = 
			{
				R9_TopLeft,
				R9_TopCenter,
				R9_TopRight,
				R9_MiddleLeft,
				R9_MiddleCenter,
				R9_MiddleRight,
				R9_BottomLeft,
				R9_BottomCenter,
				R9_BottomRight
			};

			for (int32 i=0;i<9;i++)
			{
				if (subRegionFlags & flags[i])
				{
					srcRectsTable[i] = srcRects[i];
				}
			}
			
			
		}

	}
}