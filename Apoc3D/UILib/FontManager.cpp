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

#include "FontManager.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/Color.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Math/PerlinNoise.h"

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace UI
	{
		enum FontFlags
		{
			FF_None,
			FF_HasLuminance = 1,
			FF_HasDrawOffset = 2
		};

		Font::Font(RenderDevice* device, const ResourceLocation& rl)
			: m_charTable(255), m_resource(rl.Clone()), 
			m_bucketSearchRandomizer(nullptr), m_isUsingCaching(false),m_usedInFrame(false),
			m_hasLuminance(false), m_hasDrawOffset(false),
			m_drawOffset(0,0)
		{
			m_bucketSearchRandomizer = new Random();

			m_selectTextureSize = FontManager::MaxTextureSize;

			ObjectFactory* fac = device->getObjectFactory();
			
			BinaryReader br(rl);

			int fileID = br.ReadInt32();
			int charCount;

			bool hasMetrics = false;
			if (((uint)fileID & 0xffffff00) == 0xffffff00)
			{
				hasMetrics = true;

				int32 ver = (fileID & 0xff);
				if (ver == 2)
				{
					uint32 flags = br.ReadUInt32();
					if ((flags & FF_HasLuminance))
						m_hasLuminance = true;
					if ((flags & FF_HasDrawOffset))
						m_hasDrawOffset = true;
				}

				charCount = br.ReadInt32();
			}
			else
			{
				charCount = fileID;
			}

			if (hasMetrics)
			{
				m_height = br.ReadSingle();
				m_lineGap = br.ReadSingle();
				m_ascender = br.ReadSingle();
				m_descender = br.ReadSingle();

				if (m_hasDrawOffset)
				{
					m_drawOffset.X = br.ReadSingle();
					m_drawOffset.Y = br.ReadSingle();
				}

				for (int i=0;i<charCount;i++)
				{
					Character ch;
					ch._Character = static_cast<wchar_t>( br.ReadInt32());
					ch.GlyphIndex = br.ReadInt32();
					ch.Left = br.ReadInt16();
					ch.Top = br.ReadInt16();
					ch.AdvanceX = br.ReadSingle();
					m_charTable.Add(ch._Character, ch);
				}
			}
			else
			{
				m_lineGap = m_ascender = m_descender = 0;

				for (int i=0;i<charCount;i++)
				{
					Character ch;
					ch._Character = static_cast<wchar_t>( br.ReadInt32());
					ch.GlyphIndex = br.ReadInt32();
					ch.Left = ch.Top = 0;
					m_charTable.Add(ch._Character, ch);
				}
			}
			
			int maxWidth = 1;
			int maxHeight = 1;
			int glyphCount = br.ReadInt32();
			m_glyphList = new Glyph[glyphCount];
			for (int i=0;i<glyphCount;i++)
			{
				Glyph glyph;
				glyph.Index = br.ReadInt32();
				glyph.Width = br.ReadInt32();
				glyph.Height = br.ReadInt32();
				glyph.Offset = br.ReadInt64();
				glyph.IsMapped = false;
				glyph.NumberOfBucketsUsing = 0;
				glyph.StartingParentBucket = -1;

				assert(glyph.Index<glyphCount);
				m_glyphList[glyph.Index] = glyph;

				if (glyph.Width > maxWidth)
				{
					maxWidth = glyph.Width;
				}
				if (glyph.Height > maxHeight)
				{
					maxHeight = glyph.Height;
				}
			}
			if (!hasMetrics)
			{
				m_height = static_cast<float>(maxHeight);
			}
			m_heightInt = static_cast<int>(m_height+0.5f);

			m_maxGlyphWidth = maxWidth;
			m_maxGlyphHeight = maxHeight;

			// estimate suitable texture size
			int estEdgeCount;
			const int32 MinTextureSize = 8;
			do 
			{
				m_selectTextureSize/=2;
				estEdgeCount = m_selectTextureSize / maxHeight;
			} while (estEdgeCount * estEdgeCount > glyphCount * 1.5f && m_selectTextureSize > MinTextureSize);
			m_selectTextureSize *= 2;

			m_font = fac->CreateTexture(m_selectTextureSize, m_selectTextureSize, 1, TU_Static, FMT_A8L8);

			m_edgeCount = m_selectTextureSize / maxHeight;
			m_buckets = new Bucket[m_edgeCount*m_edgeCount];
			m_currentFreqTable = new int[m_edgeCount*m_edgeCount];
			m_lastFreqTable = new int[m_edgeCount*m_edgeCount];

			// initializes the buckets
			for (int i=0;i<m_edgeCount;i++)
			{
				for (int j=0;j<m_edgeCount;j++)
				{
					Bucket& bk = m_buckets[i*m_edgeCount+j];
					
					bk.BucketIndex = i * m_edgeCount+j;
					bk.CurrentGlyph = -1;
					bk.SrcRect = Apoc3D::Math::RectangleF(static_cast<float>(j*maxHeight), static_cast<float>(i*maxHeight), static_cast<float>(maxHeight), static_cast<float>(maxHeight));
				}
			}
			m_lineBucketsFreqClassificationCount = new int[m_edgeCount*MaxFreq];
			m_lasttime_lineBucketsFreqClassificationCount = new int[m_edgeCount*MaxFreq];

			memset(m_lasttime_lineBucketsFreqClassificationCount, 0, sizeof(int)*m_edgeCount*MaxFreq);
			memset(m_lineBucketsFreqClassificationCount,0,sizeof(int)*m_edgeCount*MaxFreq);
			memset(m_currentFreqTable,0,sizeof(int)*m_edgeCount*m_edgeCount);
			memset(m_lastFreqTable,0,sizeof(int)*m_edgeCount*m_edgeCount);

			for (int i=0;i<m_edgeCount;i++)
			{
				m_lineBucketsFreqClassificationCount[i * MaxFreq + 0] = m_edgeCount;
				m_lasttime_lineBucketsFreqClassificationCount[i * MaxFreq + 0] = m_edgeCount;
			}


			
			// now put the glyphs into the buckets. Initially the largest glyphs are inserted.
			{
				// this temp list is used to store sorting without changing the original one
				Glyph* tempList = new Glyph[glyphCount];
				memcpy(tempList, m_glyphList, sizeof(Glyph) * glyphCount);

				// sort glyphs from wider to thiner.
				if (glyphCount>0)
				{
					QuickSort(tempList, 0, glyphCount - 1,
						[](const Glyph& ga, const Glyph& gb)->int
					{
						return Apoc3D::Collections::OrderComparer(ga.Width, gb.Width);
					});
				}

				m_isUsingCaching = false;

				int buckY = 0;
				int* buckX = new int[m_edgeCount];
				memset(buckX,0,sizeof(int)*m_edgeCount);

				// use up the buckets from column to column.
				// The first column will have the most fat glyph, while the latter ones will have thiner ones.
				for (int i=0;i<glyphCount;i++)
				{
					if (buckX[buckY]>=m_edgeCount)
					{
						m_isUsingCaching = true;
					}

					int startX = buckX[buckY];

					Glyph& glyph = tempList[i];
					int bucketsNeeded = (int)ceil((double)glyph.Width/maxHeight);

					if (startX+bucketsNeeded-1<m_edgeCount)
					{
						Glyph& oglyph = m_glyphList[glyph.Index];

						UseBuckets(&oglyph, buckY, startX, bucketsNeeded);

						LoadGlyphData(&br, oglyph);

						buckX[buckY]+=bucketsNeeded;
					}
					buckY = (buckY+1) % m_edgeCount;
				}

				delete[] tempList;
				delete[] buckX;
			}



			if (!hasMetrics)
			{
				for (Character& ch : m_charTable.getValueAccessor())
				{
					Glyph& g = m_glyphList[ch.GlyphIndex];
					ch.AdvanceX = static_cast<float>(g.Width);
				}
			}
				
		}
		Font::~Font()
		{
			delete[] m_lineBucketsFreqClassificationCount;
			delete[] m_buckets;
			delete[] m_glyphList;
			delete m_bucketSearchRandomizer;

			delete m_font;
			delete m_resource;
		}

		bool Font::ChangeCharacterSetting(int32 ch, short left, short top, float adcanceX)
		{
			Character* c = m_charTable.TryGetValue(ch);
			if (c)
			{
				c->Left = left;
				c->Top = top;
				c->AdvanceX = adcanceX;
				return true;
			}
			return false;
		}
		bool Font::LookupCharacterSetting(int32 ch, short& left, short& top, float& adcanceX)
		{
			Character* c = m_charTable.TryGetValue(ch);
			if (c)
			{
				left = c->Left;
				top = c->Top;
				adcanceX = c->AdvanceX;
				return true;
			}
			return false;
		}

		// UTF-16 BMP(0) private range: U+E000..U+F8FF
		// U+F8E0..U+F8F0 for font system control codes
		const uint16 ControlCode_Color = 0xF8E0;
		const uint16 ControlCode_Font = 0xF8E1;
		const uint16 ControlCode_Move = 0xF8E2;		// move drawing position

		static bool isControlCode(int32 code) { return code >= 0xF8E0 && code <= 0xF8F0; }

		String Font::MakeColorControl(uint32 cv)
		{
			String r;
			r.append(1, ControlCode_Color);
			char16_t ar = (char16_t)((cv >> 16) & 0xffff);
			char16_t gb = (char16_t)(cv & 0xffff);
			r.append(1, ar);
			r.append(1, gb);
			return r;
		}
		String Font::MakeMoveControl(const Point& position, bool passConditionCheck, bool relative)
		{
			String r;
			r.append(1, ControlCode_Move);

			uint32 flags = passConditionCheck ? 1 : 0;
			flags |= (relative ? 1 : 0) << 1;

			uint16 x = (uint16)position.X;
			uint16 y = (uint16)position.Y;

			// 15 bit
			x &= 0x7FFF;
			y &= 0x7FFF;

			uint32 data = (flags << 30) | (x << 15) | y;

			char16_t ch1 = (char16_t)((data >> 16) & 0xffff);
			char16_t ch2 = (char16_t)(data & 0xffff);
			r.append(1, ch1);
			r.append(1, ch2);
			return r;
		}

		void Font::RegisterCustomGlyph(int32 code, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect, short left, short top, float advanceX)
		{
			assert(!isControlCode(code));

			CustomGlyph cg;
			cg._Character = code;
			cg.Graphic = graphic;
			cg.AdvanceX = advanceX;
			cg.Left = left;
			cg.Top = top;
			cg.SrcRect = srcRect;
			cg.SrcRectF = srcRect;

			m_customCharacters.Add(code, cg);
		}
		void Font::RegisterCustomGlyph(int32 charCode, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect)
		{
			RegisterCustomGlyph(charCode, graphic, srcRect, 0, 0, static_cast<float>(srcRect.Width));
		}

		void Font::RegisterCustomGlyphAligned(int32 charCode, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect,
			int32 padLeft, int32 padRight, CustomGlyphAlignment vertAlignment, int32 vaValue)
		{
			short left = -padLeft;
			float advX = static_cast<float>(srcRect.Width - padLeft - padRight);

			if (vertAlignment == CGA_Center)
			{
				int32 top = (getLineHeightInt() - static_cast<int32>(m_descender)-srcRect.Height) / 2 + vaValue;
				RegisterCustomGlyph(charCode, graphic, srcRect, left, top, advX);
			}
			else if (vertAlignment == CGA_Bottom)
			{
				// bottom to baseline
				int32 adjustedContentHeight = srcRect.Height - vaValue;
				int32 top = getLineHeightInt() - static_cast<int32>(m_descender)-adjustedContentHeight;
				RegisterCustomGlyph(charCode, graphic, srcRect, left, top, advX);
			}
			else if (vertAlignment == CGA_Top)
			{
				RegisterCustomGlyph(charCode, graphic, srcRect, left, vaValue, advX);
			}
		}

		void Font::UnregisterCustomGlyph(int32 utf16code) { m_customCharacters.Remove(utf16code); }
		void Font::ClearCustomGlyph() { m_customCharacters.Clear(); }
		
		void Font::DrawDisolvingCharacter(Sprite* sprite, float x, float y,
			int32 seed, const Apoc3D::Math::RectangleF& _srcRect, int32 glyphLeft, int32 glyphTop, int32 glyphWidth, int32 glyphHeight, uint32 color,
			const Point& dissolvePatchSize, float progress)
		{
			int32 xDPCount = (glyphWidth + dissolvePatchSize.X - 1) / dissolvePatchSize.X;
			int32 yDPCount = (glyphHeight + dissolvePatchSize.Y - 1) / dissolvePatchSize.Y;

			for (int32 dy = 0; dy < yDPCount; dy++)
			{
				for (int32 dx = 0; dx < xDPCount; dx++)
				{
					float ang = (float)PerlinNoise::Noise1D(seed + dx*yDPCount + dy) * Math::PI;
					Vector2 rndDir(cosf(ang), sinf(ang));
					if (xDPCount>1)
						rndDir.X += (float)dx / (xDPCount - 1) - 0.5f;
					if (yDPCount>1)
						rndDir.Y += (float)dy / (yDPCount - 1) - 0.5f;
					rndDir.NormalizeInPlace();


					int32 offX = dx * dissolvePatchSize.X;
					int32 offY = dy * dissolvePatchSize.Y;
					int32 patchWidth = Math::Min(dissolvePatchSize.X, glyphWidth - dx*dissolvePatchSize.X);
					int32 patchHeight = Math::Min(dissolvePatchSize.Y, glyphHeight - dy*dissolvePatchSize.Y);

					Apoc3D::Math::RectangleF srcRect = _srcRect;

					srcRect.X += offX;
					srcRect.Y += offY;
					srcRect.Width = static_cast<float>(patchWidth);
					srcRect.Height = static_cast<float>(patchHeight);

					Apoc3D::Math::RectangleF rect;
					rect.X = x + glyphLeft + offX;
					rect.Y = y + glyphTop + offY;
					rect.Width = static_cast<float>(patchWidth);
					rect.Height = static_cast<float>(patchHeight);

					rect.X += rndDir.X * _srcRect.Width * progress;
					rect.Y += rndDir.Y * _srcRect.Height * progress;

					sprite->Draw(m_font, rect, &srcRect, color);
				}
			}
		}

		void Font::DrawStringDissolving(Sprite* sprite, const String& text, float x, float y, uint color, float length,
			int dissolvingLength, const Point& dissolvePatchSize, float maxDissolvingScale)
		{
			const PointF origin = GetOrigin(x, y);

			x = origin.X;
			y = origin.Y;

			size_t maxLen = text.length();
			int32 loopCount;
			
			if (dissolvingLength <0)
			{
				// by word

				bool wasSpace = false;
				int32 wordCount = 0;
				int32 maxWord = (int32)ceilf(length);
				size_t i = 0;
				for (i = 0; i < text.size() && wordCount < maxWord; i++)
				{
					wchar_t ch = text[i];
					if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
					{
						if (!wasSpace)
						{
							wordCount++;
							wasSpace = true;
						}
					}
					else
					{
						wasSpace = false;
					}
				}

				loopCount = Math::Min((int32)text.length(), (int32)i);
			}
			else if (dissolvingLength == 0)
			{
				// together
				loopCount = (int32)text.length();
			}
			else
			{
				loopCount = Math::Min((int32)text.length(), (int32)(length+0.5f));
			}

			float lineSpacing = m_height + m_lineGap;
			int32 wordIndex = 0;
			bool wasSpace = false;

			int32 concurrentWords = 1;
			if (dissolvingLength<-1)
			{
				concurrentWords = -dissolvingLength;
			}


			for (int32 i = 0; i < loopCount; i++)
			{
				wchar_t ch = text[i];

				if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
				{
					if (!wasSpace)
					{
						wordIndex++;
						wasSpace = true;
					}
				}
				else
				{
					wasSpace = false;
				}

				if (ch != '\n')
				{
					if (IgnoreCharDrawing(ch))
						continue;

					bool shouldDissolve;

					if (dissolvingLength > 0)
					{
						shouldDissolve = i > (length - dissolvingLength);
					}
					else if (dissolvingLength == 0)
					{
						shouldDissolve = length < 1;
					}
					else
					{
						shouldDissolve = length - concurrentWords < wordIndex;
					}

					float dissolveProgress = 0;
					if (shouldDissolve)
					{
						if (dissolvingLength > 0)
							dissolveProgress = maxDissolvingScale * (float)(i-length+dissolvingLength) / dissolvingLength;
						else if (dissolvingLength == 0)
							dissolveProgress = maxDissolvingScale * length;
						else
							dissolveProgress = maxDissolvingScale * (1 - Math::Saturate((length - wordIndex)/concurrentWords));
					}

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						if (glyph.Width == 0 || glyph.Height == 0)
						{
							x += chdef.AdvanceX ;
							continue;
						}

						if (!glyph.IsMapped)
						{
							EnsureGlyph(glyph);
						}
						SetUseFreq(glyph);

						if (shouldDissolve)
						{
							DrawDisolvingCharacter(sprite, x, y, i, 
								glyph.MappedRectF, chdef.Left, chdef.Top, glyph.Width, glyph.Height, color,
								dissolvePatchSize, dissolveProgress);
						}
						else
						{
							Apoc3D::Math::RectangleF rect;
							rect.X = x + chdef.Left;
							rect.Y = y + chdef.Top;
							rect.Width = static_cast<float>(glyph.Width);
							rect.Height = static_cast<float>(glyph.Height);
							
							sprite->Draw(m_font, rect, &glyph.MappedRectF, color);
						}
						
						x += chdef.AdvanceX ;
					}
					else
					{
						CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
						if (cgdef)
						{
							if (shouldDissolve)
							{
								DrawDisolvingCharacter(sprite, x, y, i, 
									cgdef->SrcRectF, cgdef->Left, cgdef->Top, (int32)cgdef->SrcRectF.Width, (int32)cgdef->SrcRectF.Height, color,
									dissolvePatchSize, dissolveProgress);
							}
							else
							{
								Apoc3D::Math::RectangleF rect;
								rect.X = x + cgdef->Left;
								rect.Y = y + cgdef->Top;
								rect.Width = static_cast<float>(cgdef->SrcRectF.Width);
								rect.Height = static_cast<float>(cgdef->SrcRectF.Height);

								sprite->Draw(cgdef->Graphic, rect, &cgdef->SrcRectF, color);
							}

							x += cgdef->AdvanceX ;
						}
					}
				}
				else
				{
					x = origin.X;
					y += lineSpacing;
				}

				
				if (dissolvingLength<0 && wordIndex > length)
					break;
			}
		}

		void Font::DrawStringDissolving(Sprite* sprite, const String& text, const Point& pos, uint color, float length, 
			int dissolvingCount, const Point& dissolvePatchSize, float maxDissolvingScale)
		{
			DrawStringDissolving(sprite, text, static_cast<float>(pos.X), static_cast<float>(pos.Y), color, length, 
				dissolvingCount, dissolvePatchSize, maxDissolvingScale);
		}

		static size_t GetLength(const String& text, int32 length)
		{
			return length != -1 ? Math::Min((size_t)length, text.length()) : text.length();
		}

		void Font::DrawStringEx(Sprite* sprite, const String& text, float x, float y, uint color, int length, float extLineSpace, wchar_t suffix, float hozShrink)
		{
			DrawStringExT(sprite, text, x, y, color, 0, length, extLineSpace, suffix, hozShrink);
		}
		void Font::DrawStringEx(Sprite* sprite, const String& text, int x, int y, uint color, int length, int extLineSpace, wchar_t suffix, float hozShrink)
		{
			DrawStringExT(sprite, text, x, y, color, 0, length, extLineSpace, suffix, hozShrink);
		}

		void Font::DrawString(Sprite* sprite, const String& text, float x, float y, int width, uint color)
		{
			DrawStringExT(sprite, text, x, y, color, width);
		}
		void Font::DrawString(Sprite* sprite, const String& text, int x, int y, int width, uint color)
		{
			DrawStringExT(sprite, text, x, y, color, width);
		}

		void Font::DrawString(Sprite* sprite, const String& text, const Point& pt, uint color, float hozShrink)
		{
			DrawStringEx(sprite, text, pt.X, pt.Y, color, -1, 0, 0, hozShrink);
		}
		void Font::DrawString(Sprite* sprite, const String& text, const PointF& pt, uint color, float hozShrink)
		{
			DrawStringEx(sprite, text, pt.X, pt.Y, color, -1, 0.0f, 0, hozShrink);
		}

		template <typename UnitType>
		void Font::DrawStringExT(Sprite* sprite, const String& text, UnitType x, UnitType y, uint color, 
			int _width, int length, UnitType _extLineSpace, wchar_t suffix, float hozShrink)
		{
			const float extLineSpace = static_cast<float>(_extLineSpace);
			const float width = static_cast<float>(_width);

			const size_t len = GetLength(text, length);
			const PointF orig = GetOrigin(x, y);
			PointF pos = orig;

			for (size_t i = 0; i < len; i++)
			{
				wchar_t ch = text[i];
				ScanColorControlCodes(text, ch, i, len, &color);
				ScanMoveControlCode(text, ch, i, len, &orig, &pos);

				DrawCharacter(sprite, ch, pos, color, hozShrink, extLineSpace, width, orig.X, true);
			}

			if (suffix)
				DrawCharacter(sprite, suffix, pos, color, hozShrink, extLineSpace, width, orig.X, true);
		}

		template void Font::DrawStringExT<int32>(Sprite* sprite, const String& text, int32 x, int32 y, uint color, 
			int width, int length, int32 extLineSpace, wchar_t suffix, float hozShrink);
		template void Font::DrawStringExT<float>(Sprite* sprite, const String& text, float x, float y, uint color, 
			int width, int length, float extLineSpace, wchar_t suffix, float hozShrink);


		void Font::DrawStringGradient(Sprite* sprite, const String& text, int _x, int _y, uint _startColor, uint _endColor)
		{
			const size_t len = text.length();

			const Color4 startColor(_startColor);
			const Color4 endColor(_endColor);

			const PointF orig = GetOrigin(_x, _y);
			PointF pos = orig;

			for (size_t i = 0; i < len; i++)
			{
				float lerpAmount = len > 1 ? (i / (float)(len - 1)) : 0;

				Color4 curColor = Color4::Lerp(startColor, endColor, lerpAmount);

				wchar_t ch = text[i];
				ScanMoveControlCode(text, ch, i, len, &orig, &pos);

				DrawCharacter(sprite, ch, pos, curColor.ToArgb(), 0, 0, 0, orig.X, true);
			}
		}

		FORCE_INLINE void Font::DrawCharacter(Sprite* sprite, int32 ch, PointF& pos, uint color, float hozShrink, float extLineSpace, float widthCap, float xOrig, bool pixelAligned)
		{
			const float lineSpacing = extLineSpace != 0 ? 
				(pixelAligned ? floorf(extLineSpace) : extLineSpace) : 
				(pixelAligned ? floorf(m_height + m_lineGap) : (m_height + m_lineGap));
		
			float& x = pos.X;
			float& y = pos.Y;

			if (ch != '\n')
			{
				if (IgnoreCharDrawing(ch))
					return;

				Character chdef;
				if (m_charTable.TryGetValue(ch, chdef))
				{
					Glyph& glyph = m_glyphList[chdef.GlyphIndex];

					if (glyph.Width == 0 || glyph.Height == 0)
					{
						x += chdef.AdvanceX + hozShrink;
						return;
					}

					if (widthCap)
					{
						float nextX = x + chdef.AdvanceX + hozShrink;
						if (nextX>=widthCap + xOrig)
						{
							x = xOrig;
							nextX = x + chdef.AdvanceX;
							y += lineSpacing;
						}
					}


					if (!glyph.IsMapped)
					{
						EnsureGlyph(glyph);
					}

					if (pixelAligned)
					{
						Apoc3D::Math::Rectangle rect;
						rect.X = static_cast<int32>(x+0.5f) + chdef.Left;
						rect.Y = static_cast<int32>(y+0.5f) + chdef.Top;
						rect.Width = glyph.Width;
						rect.Height = glyph.Height;
						SetUseFreq(glyph);

						sprite->Draw(m_font, rect, &glyph.MappedRect, color);
					}
					else
					{
						Apoc3D::Math::RectangleF rect;
						rect.X = x + chdef.Left;
						rect.Y = y + chdef.Top;
						rect.Width = static_cast<float>(glyph.Width);
						rect.Height = static_cast<float>(glyph.Height);
						SetUseFreq(glyph);

						sprite->Draw(m_font, rect, &glyph.MappedRectF, color);
					}

					x += chdef.AdvanceX + hozShrink;
				}
				else
				{
					// draw custom characters
					CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
					if (cgdef)
					{
						if (widthCap)
						{
							float nextX = x + cgdef->AdvanceX;
							if (nextX>=widthCap + xOrig)
							{
								x = xOrig;
								nextX = x + cgdef->AdvanceX;
								y += lineSpacing;
							}
						}

						if (pixelAligned)
						{
							Apoc3D::Math::Rectangle rect;
							rect.X = static_cast<int32>(x+0.5f) + cgdef->Left;
							rect.Y = static_cast<int32>(y+0.5f) + cgdef->Top;
							rect.Width = static_cast<int32>(cgdef->SrcRectF.Width);
							rect.Height = static_cast<int32>(cgdef->SrcRectF.Height);

							sprite->Draw(cgdef->Graphic, rect, &cgdef->SrcRect, color);
						}
						else
						{
							Apoc3D::Math::RectangleF rect;
							rect.X = x + static_cast<float>(cgdef->Left);
							rect.Y = y + static_cast<float>(cgdef->Top);
							rect.Width = cgdef->SrcRectF.Width;
							rect.Height = cgdef->SrcRectF.Height;

							sprite->Draw(cgdef->Graphic, rect, &cgdef->SrcRectF, color);
						}

						x += cgdef->AdvanceX + hozShrink;
					}
				}
			}
			else
			{
				x = xOrig;
				y += lineSpacing;
			}
		}
		
		template <typename SizeType>
		FORCE_INLINE bool Font::ScanColorControlCodes(const String& str, wchar_t& cur, SizeType& i, SizeType len, uint* color)
		{
			if (cur == ControlCode_Color)
			{
				if (i + 3 < len)
				{
					if (color)
					{
						uint32 ar = str[i + 1];
						uint32 gb = str[i + 2];

						*color = (ar << 16) | gb;
					}
					i += 3;
					cur = str[i];
				}
				return true;
			}
			return false;
		}

		template bool Font::ScanColorControlCodes<size_t>(const String& str, wchar_t& cur, size_t& i, size_t len, uint* color);
		template bool Font::ScanColorControlCodes<int32>(const String& str, wchar_t& cur, int32& i, int32 len, uint* color);

		template <typename SizeType>
		FORCE_INLINE bool Font::ScanMoveControlCode(const String& str, wchar_t& cur, SizeType& i, SizeType len, const PointF* orig, PointF* pos)
		{
			if (cur == ControlCode_Move)
			{
				if (i + 3 < len)
				{
					if (orig && pos)
					{
						uint32 ch1 = str[i + 1];
						uint32 ch2 = str[i + 2];

						uint32 data = ch1 << 16 | ch2;

						uint32 flags = data >> 30;

						uint16 dstxi = (data >> 15) & 0x7FFF;
						uint16 dstyi = data & 0x7FFF;

						float dstx = static_cast<float>(dstxi);
						float dsty = static_cast<float>(dstyi);

						bool passConditionCheck = flags & 1;
						bool relative = (flags & 2) != 0;

						if (relative)
						{
							pos->X += dstx;
							pos->Y += dsty;
						}
						else
						{
							if (passConditionCheck)
							{
								float curx = pos->X - orig->X;
								float cury = pos->Y - orig->Y;

								if (curx * (dstx - curx) < 0)
									dstx = curx;
								if (cury * (dsty - cury) < 0)
									dsty = cury;
							}

							pos->X = orig->X + dstx;
							pos->Y = orig->Y + dsty;
						}
					}
					i += 3;
					cur = str[i];
				}
				return true;
			}
			return false;
		}

		template bool Font::ScanMoveControlCode<size_t>(const String& str, wchar_t& cur, size_t& i, size_t len, const PointF* orig, PointF* pos);
		template bool Font::ScanMoveControlCode<int32>(const String& str, wchar_t& cur, int32& i, int32 len, const PointF* orig, PointF* pos);


		PointF Font::GetOrigin(int32 x, int32 y) const
		{
			PointF r = { static_cast<float>(x), static_cast<float>(y) + m_descender };
			if (m_hasDrawOffset)
			{
				r.X -= static_cast<int32>(m_drawOffset.X);
				r.Y -= static_cast<int32>(m_drawOffset.Y);
			}
			return r;
		}
		PointF Font::GetOrigin(float x, float y) const
		{
			PointF r = { x, y + m_descender };
			if (m_hasDrawOffset)
				r -= m_drawOffset;
			return r;
		}


		int Font::CalculateLineCount(const String& text, int width)
		{
			const size_t len = text.length();

			int lineCount = 1;
			float x = 0;
			for (size_t i = 0; i < len; i++)
			{
				wchar_t ch = text[i]; 
				ScanColorControlCodes(text, ch, i, len, nullptr);
				ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);

				if (ch != '\n')
				{
					if (x>=width)
					{
						x =0;
						lineCount++;
					}

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						x += chdef.AdvanceX;
					}
					else
					{
						CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
						if (cgdef)
						{
							x += cgdef->AdvanceX;
						}
					}
				}
				else
				{
					x = 0;
					lineCount++;
				}
			}
			return lineCount;
		}
		Point Font::MeasureString(const String& text)
		{
			PointF result = PointF(0, m_height);
			const size_t len = text.length();

			float x = 0.f;
			float y = m_height + m_lineGap + m_descender;
			for (size_t i = 0; i < len; i++)
			{
				wchar_t ch = text[i];
				ScanColorControlCodes(text, ch, i, len, nullptr);
				ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);

				if (ch != '\n')
				{
					if (IgnoreCharDrawing(ch))
						continue;

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						x += chdef.AdvanceX;
					}
					else
					{
						CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
						if (cgdef)
						{
							x += cgdef->AdvanceX;
						}
					}
				}
				else
				{
					x = 0;
					y += m_height;
				}

				if (result.X < x)
					result.X = x;
				if (result.Y < y)
					result.Y = y;
			}

			return Point(static_cast<int32>(result.X), static_cast<int32>(result.Y));
		}
		int Font::FitSingleLineString(const String& text, int width)
		{
			int chCount = 0;

			PointF result = PointF(0, m_height);
			const size_t len = text.length();

			int lineSpacing = static_cast<int>(m_height + m_lineGap+0.5f);

			float x = 0.f;
			float y = lineSpacing + m_descender;
			for (size_t i = 0; i < len; i++)
			{
				wchar_t ch = text[i];
				ScanColorControlCodes(text, ch, i, len, nullptr);
				ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);

				if (IgnoreCharDrawing(ch))
					continue;

				Character chdef;
				if (m_charTable.TryGetValue(ch, chdef))
				{
					x += chdef.AdvanceX;
				}
				else
				{
					CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
					if (cgdef)
					{
						x += cgdef->AdvanceX;
					}
				}

				if (x > width) break;
				chCount++;

				if (result.X < x)
					result.X = x;
				if (result.Y < y)
					result.Y = y;
			}
			return chCount;
		}
		
		String Font::LineBreakString(const String& text, int width, bool byWord, int& lineCount)
		{
			String result;
			result.reserve(text.size() + 5);
			
			lineCount = 1;
			int32 lineBreakCount = 0;

			if (byWord)
			{
				float x = 0;
				int32 len = (int32)text.length();

				int32 prevWordBegin = 0;
				float prevWordBeginAdvX = 0;
				for (int32 i = 0; i < len; i++)
				{
					wchar_t ch = text[i];
					ScanColorControlCodes(text, ch, i, len, nullptr);
					ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);

					bool isBlankCh = ch == ' ' || ch == '\t';
					if (isBlankCh)
					{
						prevWordBegin = i;
						prevWordBeginAdvX = x;
					}

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						const Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						x += chdef.AdvanceX;
					}
					else
					{
						CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
						if (cgdef)
						{
							x += cgdef->AdvanceX;
						}
					}

					if (x > width && !isBlankCh)
					{
						int32 insertPos = prevWordBegin + lineBreakCount + 1;
						if (insertPos < (int32)result.size())
						{
							result.insert(insertPos, 1, '\n');
							x -= prevWordBeginAdvX;
						}
						else
						{
							result.append(1, '\n');
							x = 0;
						}

						lineBreakCount++;
						lineCount++;
					}

					result.append(1, text[i]);

					if (ch == '\n')
					{
						x = 0;
						lineCount++;
					}
				}
			}
			else
			{
				float x = 0;
				int32 len = (int32)text.length() - 1;

				if (text.length())
					result.append(1, text[0]);

				for (int32 i = 0; i < len; i++)
				{
					wchar_t ch = text[i]; 
					ScanColorControlCodes(text, ch, i, len, nullptr);
					ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);

					wchar_t nch = text[i+1];

					Character chdef;
					if (m_charTable.TryGetValue(nch, chdef))
					{
						const Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						x += chdef.AdvanceX;
					}
					else
					{
						CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
						if (cgdef)
						{
							x += cgdef->AdvanceX;
						}
					}

					if (x > width)
					{
						if (nch == ' ' || nch == '\t' || ch == ' ' || ch == '\t')
							result.append(1, nch);
						else
							result.append(1, '-');

						result.append(1, '\n');
						lineCount++;
						x = 0;
					}
					else result.append(1, nch);


					if (ch == '\n')
					{
						x = 0;
						lineCount++;
					}
				}
			}

			return result;
		}

		void Font::FrameStartReset()
		{
			if (m_isUsingCaching)
			{
				// copy the data to its "last-frame" buffer
				memcpy(m_lasttime_lineBucketsFreqClassificationCount, m_lineBucketsFreqClassificationCount, sizeof(int)*m_edgeCount*MaxFreq);

				memcpy(m_lastFreqTable, m_currentFreqTable, sizeof(int)*m_edgeCount*m_edgeCount);


				// then reset the current to zero for statistics in the current frame
				memset(m_lineBucketsFreqClassificationCount,0,sizeof(int)*m_edgeCount*MaxFreq);

				if (m_usedInFrame)
				{
					//memset(m_currentFreqTable,0,sizeof(int)*m_edgeCount*m_edgeCount);
					for (int i=0;i<m_edgeCount;i++)
					{
						for (int j=0;j<m_edgeCount;j++)
						{
							int& f = m_currentFreqTable[i*m_edgeCount+j];
							if (f>0)
							{
								f--;
								if (f<0) f = 0;
							}
							m_lineBucketsFreqClassificationCount[i*MaxFreq + f]++;
						}
					}
					m_usedInFrame = false;
				}
			}

		}

		void Font::LoadGlyphData(BinaryReader* br, Glyph& glyph)
		{
			if (glyph.Width == 0 || glyph.Height == 0)
				return;

			br->getBaseStream()->Seek(glyph.Offset, SeekMode::Begin);

			assert(glyph.Width <= m_selectTextureSize && glyph.Width >=0);
			assert(glyph.Height <= m_selectTextureSize && glyph.Height >=0);


			Apoc3D::Math::Rectangle lockRect(static_cast<int32>(glyph.MappedRect.X), static_cast<int32>(glyph.MappedRect.Y),
				static_cast<int32>(glyph.MappedRect.Width), static_cast<int32>(glyph.MappedRect.Height));
			DataRectangle dataRect = m_font->Lock(0, LOCK_None, lockRect);

			if (m_hasLuminance)
			{
				uint16* buf = new uint16[glyph.Width * glyph.Height];
				br->ReadBytes((char*)buf, glyph.Width * glyph.Height * sizeof(uint16));

				for (int j=0;j<dataRect.getHeight();j++)
				{
					uint16* src= buf+j*glyph.Width;
					char* dest = (char*)dataRect.getDataPointer()+j*dataRect.getPitch();
					for (int i=0;i<dataRect.getWidth();i++)
					{
						uint16* pix = (uint16*)(dest)+i;

						*pix = *(src + i);
					}
				}
				delete[] buf;
			}
			else
			{
				char* buf = new char[glyph.Width * glyph.Height];
				br->ReadBytes(buf, glyph.Width * glyph.Height);

				for (int j=0;j<dataRect.getHeight();j++)
				{
					char* src=buf+j*glyph.Width;
					char* dest = (char*)dataRect.getDataPointer()+j*dataRect.getPitch();
					for (int i=0;i<dataRect.getWidth();i++)
					{
						uint16* pix = (uint16*)(dest)+i;

						ushort highA = (byte)*(src+i);

						*pix = highA<<8 | 0xff;
					}
				}
				delete[] buf;
			}
			
			m_font->Unlock(0);
		}
		void Font::EnsureGlyph(Glyph& glyph)
		{
			int bucketsNeeded = (int)ceil((double)glyph.Width/m_height);

			bool done = false;

			// find enough space for the glyph, testing from lower use time to higher use time
			int32 startEdgeOffset = m_bucketSearchRandomizer->NextExclusive(m_edgeCount);
			for (int freq = 0; freq<MaxFreq && !done; freq++)
			{
				for (int ii=0;ii<m_edgeCount && !done;ii++)
				{
					// some randomness to be at some chances finding unused glyph that is out of the range of use freq stat
					int32 i = (startEdgeOffset + ii) % m_edgeCount;

					int32 freqIdx = freq + i * MaxFreq;
					if (m_lineBucketsFreqClassificationCount[freqIdx]>=bucketsNeeded &&
						m_lasttime_lineBucketsFreqClassificationCount[freqIdx]>=bucketsNeeded)
					{
						// even though the number of buckets in line is sufficient enough
						// whether they are consecutive still needed to be checked.
						int numOfConsqBuckets = 0;
						int bucketPosition = -1;
						for (int j=0;j<m_edgeCount && numOfConsqBuckets<bucketsNeeded;j++)
						{
							int32 cellIdx = i*m_edgeCount+j;
							if (m_currentFreqTable[cellIdx]<=freq && m_lastFreqTable[cellIdx]<=freq)
							{
								numOfConsqBuckets++;
								if (bucketPosition==-1)
									bucketPosition = j;
							}
							else
							{
								numOfConsqBuckets=0; // resets if it meets any obstacle
							}
						}

						if (numOfConsqBuckets==bucketsNeeded)
						{
							// once the space has been located,
							// load the glyph

							// 1. Clears the buckets
							UseBuckets(nullptr, i, bucketPosition, bucketsNeeded);

							Stream* strm = m_resource->GetReadStream();
							BinaryReader br(strm, true);

							// 2. Tag the buckets
							UseBuckets(&glyph, i, bucketPosition, bucketsNeeded);

							// 3. Load the data
							LoadGlyphData(&br, glyph);


							done = true;

							break;
						}

					}
				}
			}
		}
		void Font::SetUseFreq(const Glyph& glyph)
		{
			if (m_isUsingCaching)
			{
				m_usedInFrame = true;

				for (int i=0;i<glyph.NumberOfBucketsUsing;i++)
				{
					const Bucket& buk = m_buckets[glyph.StartingParentBucket+i];
					int& freq = m_currentFreqTable[buk.BucketIndex];
					int lineIndex = buk.BucketIndex / m_edgeCount;
					int spFreqLineIdx = lineIndex*MaxFreq;

					m_lineBucketsFreqClassificationCount[freq+spFreqLineIdx]--;

					freq = MaxFreq-1;
					//freq++;
					//if (freq >= MaxFreq)
						//freq = MaxFreq-1;
					m_lineBucketsFreqClassificationCount[freq+spFreqLineIdx]++;
				}

				//for (int i=0;i<glyph.NumberOfBucketsUsing;i++)
				//{
				//	const Bucket& buk = m_buckets[glyph.StartingParentBucket+i];
				//	int& freq = m_currentFreqTable[buk.BucketIndex];
				//	int lineIndex = buk.BucketIndex / m_edgeCount;
				//	int spFreqLineIdx = lineIndex * MaxFreq;
				//	m_lineBucketsFreqClassificationCount[freq + spFreqLineIdx]--;
				//
				//	/*if (max)
				//	{
				//		freq = MaxFreq -1;
				//	}
				//	else*/
				//	{
				//		freq++;
				//		if (freq >= MaxFreq)
				//			freq = MaxFreq-1;
				//	}
				//
				//	m_lineBucketsFreqClassificationCount[freq + spFreqLineIdx]++;
				//}
			}
		}
		void Font::UseBuckets(Glyph* g, int i, int j, int amount)
		{
			if (g)
			{
				// use buckets
				const Apoc3D::Math::RectangleF& bukRect = m_buckets[i*m_edgeCount+j].SrcRect;

				g->NumberOfBucketsUsing = amount;
				g->StartingParentBucket = i*m_edgeCount+j;
				for (int k=0;k<amount;k++)
				{
					m_buckets[i*m_edgeCount+j+k].CurrentGlyph = g->Index;
				}
				g->MappedRect = Apoc3D::Math::Rectangle(static_cast<int>(bukRect.X), static_cast<int>(bukRect.Y), g->Width, g->Height);
				g->MappedRectF = Apoc3D::Math::RectangleF(bukRect.X, bukRect.Y, static_cast<float>(g->Width), static_cast<float>(g->Height));
				g->IsMapped = true;
			}
			else
			{
				// clear the requested amount of buckets one by one
				for (int s = 0; s<amount; s++)
				{
					int index = m_buckets[i*m_edgeCount+j + s].CurrentGlyph;
					if (index != -1)
					{
						Glyph& oglyph = m_glyphList[index];
						oglyph.IsMapped = false;
						
						for (int k=0;k<oglyph.NumberOfBucketsUsing;k++)
						{
							m_buckets[oglyph.StartingParentBucket+k].CurrentGlyph = -1;
						}
						oglyph.StartingParentBucket = -1;
						oglyph.NumberOfBucketsUsing = 0;
					}
				}
			}
		}
		
		/************************************************************************/
		/*  FontManager                                                         */
		/************************************************************************/

		SINGLETON_IMPL(FontManager);

		int FontManager::MaxTextureSize = 1024;

		FontManager::FontManager()
			: m_fontTable()
		{
			LogManager::getSingleton().Write(LOG_System, 
				L"Font manager initialized", 
				LOGLVL_Infomation);
		}
		FontManager::~FontManager()
		{
			m_fontTable.DeleteValuesAndClear();
		}

		Font* FontManager::getFont(const String& fontName)
		{
			return m_fontTable[fontName];
		}
		bool FontManager::hasFont(const String& fontName)
		{
			return m_fontTable.Contains(fontName);
		}

		Font* FontManager::LoadFont(RenderDevice* device, const String& name, const ResourceLocation& rl)
		{
			Font* font = new Font(device, rl);
			m_fontTable.Add(name, font);
			return font;
		}
		void FontManager::StartFrame()
		{
			for (Font* fnt : m_fontTable.getValueAccessor())
			{
				fnt->FrameStartReset();
			}
		}

		void FontManager::ReportComplexFonts()
		{
			int32 bytesUsed = 0;
			int32 numCmpFont = 0;
			int32 numLargeFont = 0;
			for (Font* fnt : m_fontTable.getValueAccessor())
			{
				bytesUsed += fnt->m_selectTextureSize * fnt->m_selectTextureSize * 2;

				if (fnt->m_isUsingCaching)
				{
					numCmpFont++;
				}
				else if (fnt->m_selectTextureSize == MaxTextureSize)
				{
					numLargeFont++;
				}
			}
			bytesUsed /= 1048576;

			String msg = L"[FontManager] ";
			msg.append(StringUtils::IntToString(m_fontTable.getCount()));
			msg.append(L" fonts currently loaded using ");
			msg.append(StringUtils::IntToString(bytesUsed));
			msg.append(L"MB");

			if (numCmpFont>0)
			{
				msg.append(L" including ");
				msg.append(StringUtils::IntToString(numCmpFont));
				msg.append(L" complex fonts.");
			}
			else
			{
				msg.append(L".");
			}
			
			ApocLog(LOG_System, msg);

			if (numCmpFont > 0)
			{
				msg.clear();
				msg = L"[FontManager] Complex fonts: ";

				int32 counter = 0;
				
				for (Font* fnt : m_fontTable.getValueAccessor())
				{
					if (fnt->m_isUsingCaching)
					{
						FileLocation* fl = up_cast<FileLocation*>(fnt->m_resource);
						String name = fl ? PathUtils::GetFileNameNoExt(fl->getName()) : fnt->m_resource->getName();
						msg.append(name);
						counter++;

						if (counter != numCmpFont)
						{
							msg.append(L", ");
						}
					}
				} 

				ApocLog(LOG_System, msg);
			}

			if (numLargeFont > 0)
			{
				msg.clear();
				msg = L"[FontManager] Large fonts: ";

				int32 counter = 0;
				for (Font* fnt : m_fontTable.getValueAccessor())
				{
					if (!fnt->m_isUsingCaching && fnt->m_selectTextureSize == MaxTextureSize)
					{
						FileLocation* fl = up_cast<FileLocation*>(fnt->m_resource);
						String name = fl ? PathUtils::GetFileNameNoExt(fl->getName()) : fnt->m_resource->getName();
						msg.append(name);
						msg.append(L"(");
						msg.append(StringUtils::IntToString(fnt->m_selectTextureSize));
						msg.append(L")");
						counter++;

						if (counter != numLargeFont)
						{
							msg.append(L", ");
						}
					}
				} 

				ApocLog(LOG_System, msg);
			}

		}
	}
}