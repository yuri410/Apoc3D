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
#include "Core/Singleton.h"
#include "Math/Rectangle.h"
#include "Math/Point.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Math;
using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace UI
	{
		/** This class contains the glyphs of a font. When rendering font, it pack
			required glyphs into a font texture, and calculate the texture coordinates
			used for locating them.
			Fonts are created from font files, generated by APBuild, which containing all needed glyphs.
		*/
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

				bool IsMapped;
				Apoc3D::Math::Rectangle MappedRect;

				float LastTimeUsed;

				bool operator <(const Glyph& other)
				{
					return LastTimeUsed<other.LastTimeUsed;
				}
			};
			Texture* m_font;
			int m_height;
			ResourceLocation* m_resource;

			FastMap<wchar_t, Character> m_charTable;
			Glyph* m_glyphList;

			list<Glyph*> m_activeGlyph;

			void LoadGlyphData(BinaryReader* br, Glyph& glyph);
			void EnsureGlyph(Glyph& glyph);

		public:
			int getLineHeight() const { return m_height; }
			Font(RenderDevice* device, ResourceLocation* fl);
			~Font();
			void DrawString(Sprite* sprite, const String& text, int x, int y, uint color, int length=-1, int lineSpace = -1, wchar_t suffix=0);
			void DrawString(Sprite* sprite, const String& text, const Point& pt, uint color);

			void DrawString(Sprite* sprite, const String& text, int x, int y, int width, uint color);
			Point MeasureString(const String& text, int width);
			Point MeasureString(const String& text);
		};

		/** Manages font resources, creating them from font files.
			
		*/
		class APAPI FontManager : public Singleton<FontManager>
		{
		public:
			static int TextureSize;
		private:
			FastMap<String, Font*> m_fontTable;

		public:
			FontManager();
			~FontManager();

			void LoadFont(RenderDevice* device, const String& name, ResourceLocation* rl);


			Font* getFont(const String& fontName);


			SINGLETON_DECL_HEARDER(FontManager);
		};
	}
}
#endif