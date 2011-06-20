#include "Common.h"

#include "Core/Singleton.h"
#include "Collections/FastMap.h"
#include "Math/Rectangle.h"
#include "Math/Point.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Collections;
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
#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "Common.h"
#include "Collections/FastMap.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI Font
		{
		private:
			struct Character
			{
				wchar_t _Character;
				int GlyphIndex;
			};
			struct Glyph
			{
				int Index;
				int Width;
				int Height;
				int64 Offset;
			};
			Texture* m_font;
			int m_height;

			FastMap<wchar_t, Character> m_charTable;
		public:

			Font(RenderDevice* device, const ResourceLocation* fl);
			~Font();
			void DrawString(Sprite* sprite, const String& text, int x, int y, uint color);
			void DrawString(Sprite* sprite, const String& text, const Point& pt, uint color);

			Point MeasureString(const String& text);
		};

		class APAPI FontManager : public Singleton<FontManager>
		{
		public:
			static int TextureSize;
		private:
			FastMap<String, Font*> m_fontTable;

		public:
			FontManager();
			~FontManager();


			Font* getFont(const String& fontName);


			SINGLETON_DECL_HEARDER(FontManager);
		};
	}
}
#endif