#pragma once
#ifndef APOC3D_FONTMANAGER_H
#define APOC3D_FONTMANAGER_H

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

#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Math/Rectangle.h"

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
		/**
		 *  This class contains the glyphs of a font. When rendering font, it pack
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
		 *  Fonts, containing all needed glyphs, are created from font files which is generated by APBuild
		 */
		class APAPI Font
		{
			friend class FontManager;
		public:
			enum CustomGlyphAlignment
			{
				CGA_Center,
				CGA_Top,
				CGA_Bottom
			};

			Font(RenderDevice* device, const ResourceLocation& fl);
			~Font();

			bool ChangeCharacterSetting(int32 ch, short left, short top, float adcanceX);
			bool LookupCharacterSetting(int32 ch, short& left, short& top, float& adcanceX);

			void RegisterCustomGlyph(int32 charCode, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect, short left, short top, float advanceX);
			void RegisterCustomGlyph(int32 charCode, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect) 
			{
				RegisterCustomGlyph(charCode, graphic, srcRect, 0, 0, static_cast<float>(srcRect.Width));
			}

			void RegisterCustomGlyphAligned(int32 charCode, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect, int32 padLeft, int32 padRight, CustomGlyphAlignment vertAlignment, int32 vaValue) 
			{
				short left = -padLeft;
				float advX = static_cast<float>(srcRect.Width - padLeft - padRight);

				if (vertAlignment == CGA_Center)
				{
					int32 top = (getLineHeightInt() - static_cast<int32>(m_descender) - srcRect.Height) / 2 + vaValue;
					RegisterCustomGlyph(charCode, graphic, srcRect, left, top, advX);
				}
				else if (vertAlignment == CGA_Bottom)
				{
					// bottom to baseline
					int32 adjustedContentHeight = srcRect.Height - vaValue;
					int32 top = getLineHeightInt() - static_cast<int32>(m_descender) - adjustedContentHeight;
					RegisterCustomGlyph(charCode, graphic, srcRect, left, top, advX);
				}
				else if (vertAlignment == CGA_Top)
				{
					RegisterCustomGlyph(charCode, graphic, srcRect, left, vaValue, advX);
				}
			}

			/*void RegisterCustomGlyphYCenter(int32 charCode, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect, short left, float advanceX) 
			{
				int32 top = (getLineHeightInt() - m_descender - srcRect.Height) / 2;
				RegisterCustomGlyph(charCode, graphic, srcRect, left, top, advanceX);
			}
			void RegisterCustomGlyphYCenter(int32 charCode, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect, short left, int32 height, float advanceX) 
			{
				int32 top = (height - srcRect.Height) / 2;
				RegisterCustomGlyph(charCode, graphic, srcRect, left, top, advanceX);
			}*/



			void UnregisterCustomGlyph(int32 utf16code);
			void ClearCustomGlyph();

			void DrawStringDissolving(Sprite* sprite, const String& text, float x, float y, uint color, float length, int dissolvingCount, const Point& dissolvePatchSize, float maxDissolvingScale);
			void DrawStringDissolving(Sprite* sprite, const String& text, const Point& pos, uint color, float length, int dissolvingCount, const Point& dissolvePatchSize, float maxDissolvingScale)
			{
				DrawStringDissolving(sprite, text, static_cast<float>(pos.X), static_cast<float>(pos.Y), color, length, dissolvingCount, dissolvePatchSize, maxDissolvingScale);
			}

			void DrawStringEx(Sprite* sprite, const String& text, float x, float y, uint color, int length=-1, float lineSpace = 0, wchar_t suffix=0, float hozShrink = 0);
			void DrawString(Sprite* sprite, const String& text, float x, float y, int width, uint color);

			void DrawStringEx(Sprite* sprite, const String& text, int x, int y, uint color, int length=-1, int lineSpace = 0, wchar_t suffix=0, float hozShrink = 0);
			void DrawString(Sprite* sprite, const String& text, int x, int y, int width, uint color);

			void DrawString(Sprite* sprite, const String& text, const Point& pt, uint color, float hozShrink = 0)
			{
				DrawStringEx(sprite, text, pt.X, pt.Y, color, -1, 0, 0, hozShrink);
			}
			void DrawString(Sprite* sprite, const String& text, const PointF& pt, uint color, float hozShrink = 0)
			{
				DrawStringEx(sprite, text, pt.X, pt.Y, color, -1, 0.0f, 0, hozShrink);
			}

			void DrawStringGradient(Sprite* sprite, const String& text, int x, int y, uint startColor, uint endColor);



			String LineBreakString(const String& text, int width, bool byWord, int& lineCount);
			Point MeasureString(const String& text);
			int FitSingleLineString(const String& str, int width);

			int CalculateLineCount(const String& text, int width);

			float getLineHeight() const { return m_height; }
			int getLineHeightInt() const { return m_heightInt; }

			float getLineGap() const { return m_lineGap; }
			
			float getLineBackgroundHeight() const { return m_height + m_lineGap + m_descender; }
			float getTextBackgroundHeight(int lineCount) const { return m_descender + (m_height+m_lineGap) * lineCount; }

			Texture* getInternalTexture() const { return m_font; }

		private:
			static const int MaxFreq = 10;
			struct Character
			{
				wchar_t _Character;
				int GlyphIndex;
				short Left;
				short Top;
				float AdvanceX;
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

				/**
				 *  The number of consecutive buckets that the glyph is using.
				 */
				int NumberOfBucketsUsing;
				int StartingParentBucket;

			};

			struct CustomGlyph
			{
				wchar_t _Character;

				short Left;
				short Top;
				float AdvanceX;

				Apoc3D::Math::RectangleF SrcRect;
				Apoc3D::Math::RectangleF SrcRectF;
				Texture* Graphic;
			};

			struct Bucket
			{
				int CurrentGlyph;
				Apoc3D::Math::RectangleF SrcRect;
				int BucketIndex;
			};

			void DrawCharacter(Sprite* sprite, int32 ch, float& x, float& y, uint color, float hozShrink, float extLineSpace, float widthCap, float xOrig, bool pixelAligned);
			
			void DrawDisolvingCharacter(Sprite* sprite, float x, float y,
				int32 seed, const Apoc3D::Math::RectangleF& srcRect, int32 glyphLeft, int32 glyphTop, int32 glyphWidth, int32 glyphHeight, uint32 color,
				const Point& dissolvePatchSize, float progress);


			Texture* m_font;
			float m_height;
			float m_ascender;
			float m_descender;
			float m_lineGap;
			int m_heightInt;
			bool m_hasLuminance;

			bool m_hasDrawOffset;
			PointF m_drawOffset;

			int m_selectTextureSize;

			int m_maxGlyphWidth;
			int m_maxGlyphHeight;

			/**
			 *  Number of buckets on one edge
			 */
			int m_edgeCount;
			ResourceLocation* m_resource;

			HashMap<int32, Character> m_charTable;
			Glyph* m_glyphList;

			HashMap<int32, CustomGlyph> m_customCharacters;


			/** 
			 *  The entire texture area is divided into 2d plane of buckets(m_edgeCount*m_edgeCount), 
			 *  which records the use of every small texture region
			 */
			Bucket* m_buckets;
			
			/** 
			 *  A one to one array containing the frequency of each bucket.
			 *  The frequency here indicates that approximately how many time the 
			 *  bucket is used since the beginning of the last frame.
			 */
			int* m_currentFreqTable;
			/** 
			 *  A one to one array containing the last-frame frequency of each bucket.
			 *  This is statistical as the string is drawn.
			 */
			int* m_lastFreqTable;
			/** 
			 *  An array of m_edgeCount*MaxFreq, 
			 *  indicates how many buckets with specific freqs in each line.
			 */
			int* m_lineBucketsFreqClassificationCount;
			/** 
			 *  The back up copy of last-frame m_lineBucketsFreqClassificationCount
			 */
			int* m_lasttime_lineBucketsFreqClassificationCount;

			Random* m_bucketSearchRandomizer;
			bool m_isUsingCaching;
			bool m_usedInFrame;

			void LoadGlyphData(BinaryReader* br, Glyph& glyph);
			void EnsureGlyph(Glyph& glyph);
			/**
			 *  Marks a glyph and its buckets using by the given glyph or 
			 *  clears the use when the given glyph pointer is 0
			 */
			void UseBuckets(Glyph* g, int i, int j, int amount);
			
			void FrameStartReset();

			/**
			 *  Increase the frequency of use of the buckets used by the given glyph, by one
			 */
			void SetUseFreq(const Glyph& g);

			static bool IgnoreCharDrawing(int32 ch) { return ch == '\r'; }

			friend class FontManager;
		
		};

		/**
		 *  Manages font resources, creating them from font files.
		 */
		class APAPI FontManager
		{
			SINGLETON_DECL(FontManager);
		public:
			static int MaxTextureSize;

			FontManager();
			~FontManager();

			/**
			 *  Should be called at the beginning of each frame to 
			 *  reset every Font's internal frequency statistics
			 */
			void StartFrame();
			Font* LoadFont(RenderDevice* device, const String& name, const ResourceLocation& rl);

			void ReportComplexFonts();

			Font* getFont(const String& fontName);
			bool hasFont(const String& fontName);

		private:
			HashMap<String, Font*> m_fontTable;

		};
	}
}
#endif