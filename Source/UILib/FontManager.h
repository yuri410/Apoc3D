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
#ifndef APOC3D_FONTMANAGER_H
#define APOC3D_FONTMANAGER_H

#include "Common.h"
#include "Collections/FastMap.h"
#include "Collections/FastList.h"
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
		 *  required glyphs into a font texture, and calculate the texture coordinates
		 *  used for locating them.
		 *
		 * @remarks
		 *  When the Font class can not hold the number of characters at once, the first glyphs will be 
		 *  loaded. When drawing a string, if any required glyph is not yet loaded, the ones seldom used 
		 *  will be unloaded to make room for the requested one.
		 *  This circumstances happens for big fonts or languages like Chinese, Korean, which has a number of
		 *  characters.
		 *
		 *  Fonts are created from font files, generated by APBuild, which containing all needed glyphs.
		 */
		class APAPI Font
		{
		public:
			
			Font(RenderDevice* device, ResourceLocation* fl);
			~Font();

			void DrawStringEx(Sprite* sprite, const String& text, float x, float y, uint color, int length=-1, int lineSpace = -1, wchar_t suffix=0, float hozShrink = 0);
			void DrawString(Sprite* sprite, const String& text, float x, float y, int width, uint color);

			void DrawStringEx(Sprite* sprite, const String& text, int x, int y, uint color, int length=-1, int lineSpace = -1, wchar_t suffix=0, float hozShrink = 0);
			void DrawString(Sprite* sprite, const String& text, int x, int y, int width, uint color);

			void DrawString(Sprite* sprite, const String& text, const Point& pt, uint color, float hozShrink = 0)
			{
				DrawStringEx(sprite, text, pt.X, pt.Y, color, -1, -1, 0, hozShrink);
			}
			void DrawString(Sprite* sprite, const String& text, const PointF& pt, uint color, float hozShrink = 0)
			{
				DrawStringEx(sprite, text, pt.X, pt.Y, color, -1, -1, 0, hozShrink);
			}


			Point MeasureString(const String& text, int width);
			Point MeasureString(const String& text);

			int getLineHeight() const { return m_height; }

			static int qsort_comparer(const void* a, const void* b);

		private:
			static const int MaxFreq = 5;
			struct Character
			{
				wchar_t _Character;
				int GlyphIndex;
				short Left;
				short Top;
				float AdcanceX;
			};
			struct Glyph
			{
				int Index;
				int Width;
				int Height;
				int64 Offset;

				bool IsMapped;
				Apoc3D::Math::Rectangle MappedRect;
				Apoc3D::Math::RectangleF MappedRectF;

				/** The number of consecutive buckets that the glyph is using.
				*/
				int NumberOfBucketUsing;
				int StartingParentBucket;

			};
			struct Bucket
			{
				int CurrentGlyph;
				Apoc3D::Math::RectangleF SrcRect;
				int BucketIndex;
			};


			Texture* m_font;
			int m_height;
			int m_maxWidth;
			int m_edgeCount;
			ResourceLocation* m_resource;

			FastMap<wchar_t, Character> m_charTable;
			Glyph* m_glyphList;

			//list<Glyph*> m_activeGlyph;
			//FastList<Bucket*> m_buckets;

			/** The entire texture area is divided into a 2x2 buckets, which records
			 *  the use of every small texture region
			 */
			Bucket* m_buckets;
			
			/** A one to one array containing the frequency of each bucket.
			 *  The frequency here indicates that approximately how many time the 
			 *  bucket is used since the beginning of the last frame.
			 */
			int* m_currentFreqTable;
			/** A one to one array containing the last-frame frequency of each bucket.
			 *  This is statistical as the string is drawn.
			 */
			int* m_lastFreqTable;
			/** An array of m_edgeCount*MaxFreq, 
			 *  indicates how many buckets with specific freqs in each line.
			 */
			int* m_lineBucketsFreqClassificationCount;
			/** The back up copy of last-frame m_lineBucketsFreqClassificationCount
			*/
			int* m_lasttime_lineBucketsFreqClassificationCount;

			void LoadGlyphData(BinaryReader* br, Glyph& glyph);
			void EnsureGlyph(Glyph& glyph);
			/** Marks a glyph and its buckets using by the given glyph or 
			 *  clears the use when the given glyph pointer is 0
			 */
			void UseBuckets(Glyph* g, int i, int j, int amount);
			
			void FrameStartReset();

			/** Increase the frequency of use of the buckets used by the given glyph, by one
			*/
			void SetUseFreq(const Glyph& g);
			
			friend class FontManager;
		
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

			/** Should be called at the beginning of each frame to 
			 *  reset every Font's internal frequency statistics
			 */
			void StartFrame();
			void LoadFont(RenderDevice* device, const String& name, ResourceLocation* rl);


			Font* getFont(const String& fontName);


			SINGLETON_DECL_HEARDER(FontManager);
		};
	}
}
#endif