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
#include "UICommon.h"

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
#include "apoc3d/Math/PerlinNoise.h"
#include "apoc3d/Library/lz4.h"

using namespace Apoc3D::IO;
using namespace Apoc3D::Utility;

namespace
{
	bool IsLetter(int ch)
	{
		return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
	}
	bool IsChinese(int ch)
	{
		return ch >= 0x4E00 && ch <= 0x9fff;
	}
}

namespace Apoc3D
{
	namespace UI
	{
		enum FontFlags
		{
			FF_None,
			FF_HasLuminance = 1,	// Some fonts has luminance in addition to alpha
			FF_HasDrawOffset = 2,
			FF_CompressedGlyph32 = 4
		};

		Font::Font(RenderDevice* device, const ResourceLocation& rl)
			: m_charTable(255), m_resource(rl.Clone())
		{
			m_selectTextureSize = FontManager::MaxTextureSize;

			ObjectFactory* fac = device->getObjectFactory();
			
			BinaryReader br(rl);

			int32 fileID = br.ReadInt32();
			int32 charCount;

			bool compressedGlyph32 = false;
			bool hasMetrics = false;
			if (((uint32)fileID & 0xffffff00) == 0xffffff00)
			{
				// new font format version

				hasMetrics = true;

				int32 ver = (fileID & 0xff);
				if (ver == 2)
				{
					uint32 flags = br.ReadUInt32();
					if ((flags & FF_HasLuminance))
						m_hasLuminance = true;
					if ((flags & FF_HasDrawOffset))
						m_hasDrawOffset = true;
					if (flags & FF_CompressedGlyph32)
						m_compressedGlyph = compressedGlyph32 = true;
				}

				charCount = br.ReadInt32();
			}
			else
			{
				charCount = fileID;
			}

			if (hasMetrics)
			{
				m_glyphHeight = br.ReadSingle();
				m_lineGap = br.ReadSingle();
				m_ascender = br.ReadSingle();
				m_descender = br.ReadSingle();

				if (m_hasDrawOffset)
				{
					m_drawOffset.X = br.ReadSingle();
					m_drawOffset.Y = br.ReadSingle();
				}

				for (int32 i = 0; i < charCount; i++)
				{
					Character ch;
					ch._Character = (char16_t)(br.ReadInt32());
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

				for (int32 i = 0; i < charCount; i++)
				{
					Character ch;
					ch._Character = (char16_t)br.ReadInt32();
					ch.GlyphIndex = br.ReadInt32();
					m_charTable.Add(ch._Character, ch);
				}
			}
			
			int32 maxWidth = 1;
			int32 maxHeight = 1;
			int32 glyphCount = br.ReadInt32();
			m_glyphList = new Glyph[glyphCount];
			for (int32 i = 0; i < glyphCount; i++)
			{
				Glyph glyph;
				if (compressedGlyph32)
				{
					glyph.Index = br.ReadUInt16();
					glyph.Width = br.ReadUInt16();
					glyph.Height = br.ReadUInt16();
					glyph.Offset = br.ReadUInt32();
					glyph.CompressedSize = br.ReadUInt32();
				}
				else
				{
					glyph.Index = br.ReadInt32();
					glyph.Width = br.ReadInt32();
					glyph.Height = br.ReadInt32();
					glyph.Offset = br.ReadInt64();
				}

				assert(glyph.Index < glyphCount);
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
				m_glyphHeight = (float)maxHeight;
			}
			m_glyphHeightInt = (int32)(m_glyphHeight + 0.5f);

			m_maxGlyphWidth = maxWidth;
			m_maxGlyphHeight = maxHeight;

			// estimate suitable texture size
			int32 estEdgeCount;
			const int32 MinTextureSize = 8;
			do 
			{
				m_selectTextureSize /= 2;
				estEdgeCount = m_selectTextureSize / maxHeight;
			} while (estEdgeCount * estEdgeCount > glyphCount * 1.5f && m_selectTextureSize > MinTextureSize);
			m_selectTextureSize *= 2;

			m_fontPack = fac->CreateTexture(m_selectTextureSize, m_selectTextureSize, 1, TU_Static, FMT_A8L8);

			m_edgeCount = m_selectTextureSize / maxHeight;
			m_grids = new Grid[m_edgeCount*m_edgeCount];
			m_currentFreqTable = new int32[m_edgeCount*m_edgeCount]();
			m_lastFreqTable = new int32[m_edgeCount*m_edgeCount]();

			// initializes the grids
			for (int32 i = 0; i < m_edgeCount; i++)
			{
				for (int32 j = 0; j < m_edgeCount; j++)
				{
					Grid& grd = m_grids[i*m_edgeCount + j];

					grd.GridIndex = i * m_edgeCount + j;
					grd.SrcRect = RectangleF((float)(j*maxHeight), (float)(i*maxHeight), (float)maxHeight, (float)maxHeight);
				}
			}
			m_rowGridsFreqClassificationCount = new int32[m_edgeCount*MaxFreq]();
			m_lasttime_rowGridsFreqClassificationCount = new int32[m_edgeCount*MaxFreq]();

			// set the ClassificationCount to max for each row for Freq 0
			for (int32 i = 0; i < m_edgeCount; i++)
			{
				m_rowGridsFreqClassificationCount[i * MaxFreq + 0] = m_edgeCount;
				m_lasttime_rowGridsFreqClassificationCount[i * MaxFreq + 0] = m_edgeCount;
			}


			
			// now put the glyphs into the grids. Initially the largest glyphs are inserted.
			{
				// this temp list is used to store sorting without changing the original one
				Glyph* tempList = new Glyph[glyphCount];
				memcpy(tempList, m_glyphList, sizeof(Glyph) * glyphCount);

				// sort glyphs from wider to thiner in width.
				if (glyphCount > 0)
				{
					QuickSort(tempList, 0, glyphCount - 1,
						[](const Glyph& ga, const Glyph& gb)->int32
					{
						return Apoc3D::Collections::OrderComparer(ga.Width, gb.Width);
					});
				}

				m_isUsingCaching = false;

				int32 gridY = 0;
				int32* gridX = new int32[m_edgeCount]();
				
				// use up the grids from column to column.
				// The first column will have the fattest glyph, while the latter ones will have thiner ones.
				for (int32 i = 0; i < glyphCount; i++)
				{
					if (gridX[gridY] >= m_edgeCount)
					{
						m_isUsingCaching = true;
					}

					int32 startX = gridX[gridY];

					Glyph& glyph = tempList[i];
					int32 gridsNeeded = (int32)ceil((double)glyph.Width / maxHeight);

					if (startX + gridsNeeded - 1 < m_edgeCount)
					{
						Glyph& oglyph = m_glyphList[glyph.Index];

						UseGrids(&oglyph, gridY, startX, gridsNeeded);

						LoadGlyphData(&br, oglyph);

						gridX[gridY] += gridsNeeded;
					}
					gridY = (gridY + 1) % m_edgeCount;
				}

				delete[] tempList;
				delete[] gridX;
			}



			if (!hasMetrics)
			{
				for (Character& ch : m_charTable.getValueAccessor())
				{
					Glyph& g = m_glyphList[ch.GlyphIndex];
					ch.AdvanceX = (float)g.Width;
				}
			}
		}

		Font::~Font()
		{
			delete[] m_currentFreqTable;
			delete[] m_lastFreqTable;

			delete[] m_lasttime_rowGridsFreqClassificationCount;
			delete[] m_rowGridsFreqClassificationCount;

			delete[] m_grids;
			delete[] m_glyphList;

			delete m_fontPack;
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

		void Font::RegisterBaseFontAsCustomGlyphs(Font* fnt)
		{
			assert(!fnt->m_isUsingCaching);
			if (!fnt->m_isUsingCaching)
			{
				for (auto& ch : fnt->m_charTable.getValueAccessor())
				{
					Glyph& g = fnt->m_glyphList[ch.GlyphIndex];
					RegisterCustomGlyph(ch._Character, fnt->m_fontPack, g.MappedRect, ch.Left, ch.Top, ch.AdvanceX);
				}
			}
		}

		void Font::RegisterCustomGlyph(int32 code, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect, short left, short top, float advanceX)
		{
			assert(!ControlCodes::isControlCode(code));

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
			RegisterCustomGlyph(charCode, graphic, srcRect, 0, 0, (float)srcRect.Width);
		}

		void Font::RegisterCustomGlyphAligned(int32 charCode, Texture* graphic, const Apoc3D::Math::Rectangle& srcRect,
			int32 padLeft, int32 padRight, CustomGlyphAlignment vertAlignment, int32 vaValue)
		{
			short left = -padLeft;
			float advX = (float)(srcRect.Width - padLeft - padRight);

			int32 top;
			if (vertAlignment == CGA_Center)
			{
				top = (getLineHeightInt() - (int32)m_descender - srcRect.Height) / 2 + vaValue;
			}
			else if (vertAlignment == CGA_Bottom)
			{
				// bottom to baseline
				int32 adjustedContentHeight = srcRect.Height - vaValue;
				top = getLineHeightInt() - (int32)m_descender - adjustedContentHeight;
			}
			else if (vertAlignment == CGA_Top)
			{
				top = vaValue;
			}

			RegisterCustomGlyph(charCode, graphic, srcRect, left, top, advX);
		}

		void Font::UnregisterCustomGlyph(int32 utf16code) { m_customCharacters.Remove(utf16code); }
		void Font::ClearCustomGlyph() { m_customCharacters.Clear(); }
		
		void Font::DrawDisolvingCharacter(Sprite* sprite, Texture* fontPack, float x, float y,
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
					srcRect.Width = (float)patchWidth;
					srcRect.Height = (float)patchHeight;

					Apoc3D::Math::RectangleF rect;
					rect.X = x + glyphLeft + offX;
					rect.Y = y + glyphTop + offY;
					rect.Width = (float)patchWidth;
					rect.Height = (float)patchHeight;

					rect.X += rndDir.X * _srcRect.Width * progress;
					rect.Y += rndDir.Y * _srcRect.Height * progress;

					sprite->Draw(fontPack, rect, &srcRect, color);
				}
			}
		}

		void Font::DrawStringDissolving(Sprite* sprite, const String& text, float x, float y, uint color, float length,
			int32 dissolvingLength, const Point& dissolvePatchSize, float maxDissolvingScale)
		{
			const PointF origin = GetOrigin(x, y);

			x = origin.X;
			y = origin.Y;

			size_t maxLen = text.length();
			int32 loopCount;
			
			if (dissolvingLength < 0)
			{
				// by word

				bool wasSpace = false;
				int32 wordCount = 0;
				int32 maxWord = (int32)ceilf(length);
				size_t i = 0;
				for (i = 0; i < text.size() && wordCount < maxWord; i++)
				{
					char16_t ch = text[i];
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

			float lineSpacing = m_glyphHeight + m_lineGap;
			int32 wordIndex = 0;
			bool wasSpace = false;

			int32 concurrentWords = 1;
			if (dissolvingLength < -1)
			{
				concurrentWords = -dissolvingLength;
			}


			for (int32 i = 0; i < loopCount; i++)
			{
				char16_t ch = text[i];

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
							dissolveProgress = maxDissolvingScale * (float)(i - length + dissolvingLength) / dissolvingLength;
						else if (dissolvingLength == 0)
							dissolveProgress = maxDissolvingScale * length;
						else
							dissolveProgress = maxDissolvingScale * (1 - Math::Saturate((length - wordIndex) / concurrentWords));
					}

					CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
					if (cgdef)
					{
						if (shouldDissolve)
						{
							DrawDisolvingCharacter(sprite, cgdef->Graphic, x, y, i,
								cgdef->SrcRectF, cgdef->Left, cgdef->Top, (int32)cgdef->SrcRectF.Width, (int32)cgdef->SrcRectF.Height, color,
								dissolvePatchSize, dissolveProgress);
						}
						else
						{
							Apoc3D::Math::RectangleF rect;
							rect.X = x + cgdef->Left;
							rect.Y = y + cgdef->Top;
							rect.Width = (float)cgdef->SrcRectF.Width;
							rect.Height = (float)cgdef->SrcRectF.Height;

							sprite->Draw(cgdef->Graphic, rect, &cgdef->SrcRectF, color);
						}

						x += cgdef->AdvanceX;
					}
					else
					{
						Character chdef;
						if (m_charTable.TryGetValue(ch, chdef))
						{
							Glyph& glyph = m_glyphList[chdef.GlyphIndex];

							if (glyph.Width == 0 || glyph.Height == 0)
							{
								x += chdef.AdvanceX;
								continue;
							}

							if (!glyph.IsMapped)
							{
								EnsureGlyph(glyph);
							}
							SetUseFreqToMax(glyph);

							if (shouldDissolve)
							{
								DrawDisolvingCharacter(sprite, m_fontPack, x, y, i,
									glyph.MappedRectF, chdef.Left, chdef.Top, glyph.Width, glyph.Height, color,
									dissolvePatchSize, dissolveProgress);
							}
							else
							{
								Apoc3D::Math::RectangleF rect;
								rect.X = x + chdef.Left;
								rect.Y = y + chdef.Top;
								rect.Width = (float)glyph.Width;
								rect.Height = (float)glyph.Height;

								sprite->Draw(m_fontPack, rect, &glyph.MappedRectF, color);
							}

							x += chdef.AdvanceX;
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
			int32 dissolvingCount, const Point& dissolvePatchSize, float maxDissolvingScale)
		{
			DrawStringDissolving(sprite, text, (float)pos.X, (float)pos.Y, color, length,
				dissolvingCount, dissolvePatchSize, maxDissolvingScale);
		}

		static int32 GetLength(const String& text, int32 length)
		{
			return length != -1 ? Math::Min(length, (int32)text.length()) : (int32)text.length();
		}

		void Font::DrawStringEx(Sprite* sprite, const String& text, float x, float y, uint color, int32 length, float extLineSpace, char16_t suffix, float hozShrink)
		{
			DrawStringExT(sprite, text, x, y, color, 0, length, extLineSpace, suffix, hozShrink);
		}
		void Font::DrawStringEx(Sprite* sprite, const String& text, int32 x, int32 y, uint color, int32 length, int32 extLineSpace, char16_t suffix, float hozShrink)
		{
			DrawStringExT(sprite, text, x, y, color, 0, length, extLineSpace, suffix, hozShrink);
		}

		void Font::DrawString(Sprite* sprite, const String& text, float x, float y, int32 width, uint color)
		{
			DrawStringExT(sprite, text, x, y, color, width);
		}
		void Font::DrawString(Sprite* sprite, const String& text, int32 x, int32 y, int32 width, uint color)
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
			int32 _width, int32 length, UnitType _extLineSpace, char16_t suffix, float hozShrink)
		{
			const float extLineSpace = (float)_extLineSpace;
			const float width = (float)_width;

			const int32 len = GetLength(text, length);
			const PointF orig = GetOrigin(x, y);
			PointF pos = orig;

			for (int32 i = 0; i < len; i++)
			{
				char16_t ch = text[i];
				ScanColorControlCodes(text, ch, i, len, &color);
				ScanMoveControlCode(text, ch, i, len, &orig, &pos);
				ScanOtherControlCodes(text, ch, i, len);

				DrawCharacter(sprite, ch, pos, color, hozShrink, extLineSpace, width, orig.X, true);
			}

			if (suffix)
				DrawCharacter(sprite, suffix, pos, color, hozShrink, extLineSpace, width, orig.X, true);
		}

		template void Font::DrawStringExT<int32>(Sprite* sprite, const String& text, int32 x, int32 y, uint color, 
			int32 width, int32 length, int32 extLineSpace, char16_t suffix, float hozShrink);
		template void Font::DrawStringExT<float>(Sprite* sprite, const String& text, float x, float y, uint color, 
			int32 width, int32 length, float extLineSpace, char16_t suffix, float hozShrink);


		void Font::DrawStringGradient(Sprite* sprite, const String& text, int32 _x, int32 _y, uint _startColor, uint _endColor)
		{
			const int32 len = (int32)text.length();

			const Color4 startColor(_startColor);
			const Color4 endColor(_endColor);

			const PointF orig = GetOrigin(_x, _y);
			PointF pos = orig;

			for (int32 i = 0; i < len; i++)
			{
				float lerpAmount = len > 1 ? (i / (float)(len - 1)) : 0;

				Color4 curColor = Color4::Lerp(startColor, endColor, lerpAmount);

				char16_t ch = text[i];
				ScanMoveControlCode(text, ch, i, len, &orig, &pos);

				DrawCharacter(sprite, ch, pos, curColor.ToArgb(), 0, 0, 0, orig.X, true);
			}
		}

		FORCE_INLINE void Font::DrawCharacter(Sprite* sprite, int32 ch, PointF& pos, uint color, float horizShrink, float extLineSpace, float widthCap, float xOrig, bool pixelAligned)
		{
			const float lineSpacing = extLineSpace != 0 ? 
				(pixelAligned ? floorf(extLineSpace) : extLineSpace) : 
				(pixelAligned ? floorf(m_glyphHeight + m_lineGap) : (m_glyphHeight + m_lineGap));
		
			float& x = pos.X;
			float& y = pos.Y;

			if (ch != '\n')
			{
				if (IgnoreCharDrawing(ch))
					return;

				// draw custom characters
				CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
				if (cgdef)
				{
					if (widthCap)
					{
						float nextX = x + cgdef->AdvanceX;
						if (nextX >= widthCap + xOrig)
						{
							x = xOrig;
							nextX = x + cgdef->AdvanceX;
							y += lineSpacing;
						}
					}

					if (pixelAligned)
					{
						Apoc3D::Math::Rectangle rect;
						rect.X = (int32)(x + 0.5f) + cgdef->Left;
						rect.Y = (int32)(y + 0.5f) + cgdef->Top;
						rect.Width = (int32)cgdef->SrcRectF.Width;
						rect.Height = (int32)cgdef->SrcRectF.Height;

						sprite->Draw(cgdef->Graphic, rect, &cgdef->SrcRect, color);
					}
					else
					{
						Apoc3D::Math::RectangleF rect;
						rect.X = x + (float)cgdef->Left;
						rect.Y = y + (float)cgdef->Top;
						rect.Width = cgdef->SrcRectF.Width;
						rect.Height = cgdef->SrcRectF.Height;

						sprite->Draw(cgdef->Graphic, rect, &cgdef->SrcRectF, color);
					}

					x += cgdef->AdvanceX + horizShrink;
				}
				else
				{
					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						// draw characters that are part of the font

						Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						if (glyph.Width == 0 || glyph.Height == 0)
						{
							x += chdef.AdvanceX + horizShrink;
							return;
						}

						if (widthCap)
						{
							// change line if a width cap is present
							float nextX = x + chdef.AdvanceX + horizShrink;
							if (nextX >= widthCap + xOrig)
							{
								x = xOrig;
								nextX = x + chdef.AdvanceX;
								y += lineSpacing;
							}
						}

						if (!glyph.IsMapped)
						{
							// load glyph bitmap if not loaded
							EnsureGlyph(glyph);
						}

						SetUseFreqToMax(glyph);

						if (pixelAligned)
						{
							Apoc3D::Math::Rectangle rect;
							rect.X = (int32)(x + 0.5f) + chdef.Left;
							rect.Y = (int32)(y + 0.5f) + chdef.Top;
							rect.Width = glyph.Width;
							rect.Height = glyph.Height;

							sprite->Draw(m_fontPack, rect, &glyph.MappedRect, color);
						}
						else
						{
							Apoc3D::Math::RectangleF rect;
							rect.X = x + chdef.Left;
							rect.Y = y + chdef.Top;
							rect.Width = (float)glyph.Width;
							rect.Height = (float)glyph.Height;

							sprite->Draw(m_fontPack, rect, &glyph.MappedRectF, color);
						}

						x += chdef.AdvanceX + horizShrink;
					}
				}
			}
			else
			{
				x = xOrig;
				y += lineSpacing;
			}
		}
		

		int32 Font::CalculateLineCount(const String& text, int32 width)
		{
			const int32 len = (int32)text.length();

			int32 lineCount = 1;
			float x = 0;
			for (int32 i = 0; i < len; i++)
			{
				char16_t ch = text[i];
				ScanColorControlCodes(text, ch, i, len, nullptr);
				ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);
				ScanOtherControlCodes(text, ch, i, len);

				if (ch != '\n')
				{
					if (x >= width)
					{
						x = 0;
						lineCount++;
					}

					AdvanceXSimple(x, ch);
				}
				else
				{
					x = 0;
					lineCount++;
				}

				if (ch == '\n')
					lineCount++;
			}
			return lineCount;
		}
		Point Font::MeasureString(const String& text, int32 start, int32 end)
		{
			PointF result = PointF(0, m_glyphHeight);
			
			if (start < 0) start = 0;
			if (end >= (int32)text.size()) end = (int32)text.size() - 1;

			const int32 len = end + 1;

			float x = 0.f;
			float y = m_glyphHeight + m_lineGap + m_descender;
			for (int32 i = start; i <= end; i++)
			{
				char16_t ch = text[i];
				ScanColorControlCodes(text, ch, i, len, nullptr);
				ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);
				ScanOtherControlCodes(text, ch, i, len);

				if (ch != '\n')
				{
					if (IgnoreCharDrawing(ch))
						continue;

					AdvanceXSimple(x, ch);
				}
				else
				{
					x = 0;
					y += m_glyphHeight;
				}

				if (result.X < x)
					result.X = x;
				if (result.Y < y)
					result.Y = y;
			}

			return Point((int32)result.X, (int32)result.Y);
		}
		Point Font::MeasureString(const String& text)
		{
			return MeasureString(text, 0, (int32)text.size() - 1);
		}
		int32 Font::FitSingleLineString(const String& text, int32 width)
		{
			int32 chCount = 0;

			PointF result = PointF(0, m_glyphHeight);
			const int32 len = (int32)text.length();

			int32 lineSpacing = (int32)(m_glyphHeight + m_lineGap + 0.5f);

			float x = 0.f;
			float y = lineSpacing + m_descender;
			for (int32 i = 0; i < len; i++)
			{
				char16_t ch = text[i];
				ScanColorControlCodes(text, ch, i, len, nullptr);
				ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);
				ScanOtherControlCodes(text, ch, i, len);

				if (IgnoreCharDrawing(ch))
					continue;

				AdvanceXSimple(x, ch);

				if (x > width) break;
				chCount++;

				if (result.X < x)
					result.X = x;
				if (result.Y < y)
					result.Y = y;
			}
			return chCount;
		}
		
		String Font::LineBreakString(const String& text, int32 width, bool byWord, int32& lineCount)
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
					char16_t ch = text[i];
					//ScanColorControlCodes(text, ch, i, len, nullptr);
					//ScanMoveControlCode(text, ch, i, len, nullptr, nullptr);
					//ScanUselessControlCodes(text, ch, i, len);

					bool isChinese = IsChinese(ch);
					bool isLetterCh = IsLetter(ch);
					if (!isLetterCh)
					{
						prevWordBegin = i;
						prevWordBeginAdvX = x;
					}

					AdvanceXSimple(x, ch);

					if (x > width)
					{
						if (isLetterCh)
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
						else if (isChinese || x >= width + m_maxGlyphWidth)
						{
							// new line as Chinese or too much overage
							result.append(1, '\n');
							lineCount++;
							x = 0;
						}
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
					char16_t ch = text[i];
					char16_t nch = text[i + 1];

					//ScanColorControlCodes(text, ch, i, len, nullptr);
					//ScanMoveControlCodeNoSkip(text, ch, i, len, nullptr, nullptr);
					//ScanUselessControlCodes(text, ch, i, len);

					AdvanceXSimple(x, ch);

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
				memcpy(m_lasttime_rowGridsFreqClassificationCount, m_rowGridsFreqClassificationCount, sizeof(int32)*m_edgeCount*MaxFreq);
				memcpy(m_lastFreqTable, m_currentFreqTable, sizeof(int32)*m_edgeCount*m_edgeCount);


				// then reset the current to zero for statistics in the current frame
				// this will be recalcualted below
				memset(m_rowGridsFreqClassificationCount, 0, sizeof(int32)*m_edgeCount*MaxFreq);

				if (m_usedInFrame)
				{
					// decay used frequency in m_currentFreqTable
					// and calculate current ClassificationCount based on current Freq table

					for (int32 i = 0; i < m_edgeCount; i++)
					{
						for (int32 j = 0; j < m_edgeCount; j++)
						{
							int32& f = m_currentFreqTable[i*m_edgeCount + j];
							if (f > 0)
							{
								f--;
								if (f < 0) f = 0;
							}
							m_rowGridsFreqClassificationCount[i*MaxFreq + f]++;
						}
					}
					m_usedInFrame = false;
				}
			}

		}

		void Font::ReadGlyphData(BinaryReader* br, void* buf, int32 pixelSize, Glyph& glyph)
		{
			if (m_compressedGlyph)
			{
				if (glyph.CompressedSize)
				{
					char* compressedBuf = new char[glyph.CompressedSize];
					br->ReadBytes(compressedBuf, glyph.CompressedSize);
					LZ4_decompress_safe(compressedBuf, (char*)buf, glyph.CompressedSize, glyph.Width * glyph.Height * pixelSize);
					delete[] compressedBuf;
				}
				else
				{
					memset(buf, 0, glyph.Width * glyph.Height * pixelSize);
				}
			}
			else
			{
				br->ReadBytes((char*)buf, glyph.Width * glyph.Height * pixelSize);
			}
		}

		void Font::LoadGlyphData(BinaryReader* br, Glyph& glyph)
		{
			if (glyph.Width == 0 || glyph.Height == 0)
				return;

			br->getBaseStream()->Seek(glyph.Offset, SeekMode::Begin);

			assert(glyph.Width <= m_selectTextureSize && glyph.Width >=0);
			assert(glyph.Height <= m_selectTextureSize && glyph.Height >=0);


			Apoc3D::Math::Rectangle lockRect((int32)glyph.MappedRect.X, (int32)glyph.MappedRect.Y,
				(int32)glyph.MappedRect.Width, (int32)glyph.MappedRect.Height);
			DataRectangle dataRect = m_fontPack->Lock(0, LOCK_None, lockRect);

			if (m_hasLuminance)
			{
				uint16* buf = new uint16[glyph.Width * glyph.Height];
				ReadGlyphData(br, buf, sizeof(*buf), glyph);

				for (int32 j = 0; j < dataRect.getHeight(); j++)
				{
					uint16* src = buf + j*glyph.Width;
					char* dest = (char*)dataRect.getDataPointer() + j*dataRect.getPitch();
					for (int32 i = 0; i < dataRect.getWidth(); i++)
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
				ReadGlyphData(br, buf, sizeof(*buf), glyph);

				for (int32 j = 0; j < dataRect.getHeight(); j++)
				{
					char* src = buf + j*glyph.Width;
					char* dest = (char*)dataRect.getDataPointer() + j*dataRect.getPitch();
					for (int32 i = 0; i < dataRect.getWidth(); i++)
					{
						uint16* pix = (uint16*)(dest)+i;

						ushort highA = (byte)*(src + i);

						*pix = highA << 8 | 0xff;
					}
				}
				delete[] buf;
			}
			
			m_fontPack->Unlock(0);
		}
		void Font::EnsureGlyph(Glyph& glyph)
		{
			// grids are squares. m_maxGlyphHeight is the edge length in pixels of a grid
			int32 gridsNeeded = (int32)ceil((double)glyph.Width / m_maxGlyphHeight);

			bool done = false;

			// find enough space for the glyph, testing from lower use time to higher use time
			int32 startColumnffset = m_gridSearchRandomizer.NextExclusive(m_edgeCount);
			for (int32 freq = 0; freq < MaxFreq && !done; freq++)
			{
				// search by row
				for (int32 ii = 0; ii < m_edgeCount && !done; ii++)
				{
					// some randomness to be at some chances finding unused glyph that is out of the range of use freq stat
					int32 i = (startColumnffset + ii) % m_edgeCount;

					int32 freqIdx = freq + i * MaxFreq;
					if (m_rowGridsFreqClassificationCount[freqIdx] >= gridsNeeded &&
						m_lasttime_rowGridsFreqClassificationCount[freqIdx] >= gridsNeeded)
					{
						// even though the number of grids in the row is sufficient enough
						// whether they are consecutive still needed to be checked here.
						int32 numOfConsqGrids = 0;
						int32 gridPosition = -1;
						for (int32 j = 0; j < m_edgeCount && numOfConsqGrids < gridsNeeded; j++)
						{
							int32 cellIdx = i*m_edgeCount + j;
							if (m_currentFreqTable[cellIdx] <= freq && m_lastFreqTable[cellIdx] <= freq)
							{
								numOfConsqGrids++;
								if (gridPosition == -1)
									gridPosition = j;
							}
							else
							{
								numOfConsqGrids = 0; // resets if it meets any obstacle
							}
						}

						if (numOfConsqGrids == gridsNeeded)
						{
							// once the space has been located,
							// load the glyph

							// 1. Clears the grids
							UseGrids(nullptr, i, gridPosition, gridsNeeded);

							Stream* strm = m_resource->GetReadStream();
							BinaryReader br(strm, true);

							// 2. Tag the grids
							UseGrids(&glyph, i, gridPosition, gridsNeeded);

							// 3. Load the data
							LoadGlyphData(&br, glyph);


							done = true;

							break;
						}

					}
				}
			}
		}
		void Font::SetUseFreqToMax(const Glyph& glyph)
		{
			if (m_isUsingCaching)
			{
				m_usedInFrame = true;

				for (int32 i = 0; i < glyph.NumberOfGridsUsing; i++)
				{
					const Grid& grd = m_grids[glyph.StartingParentGrid + i];
					int32& freq = m_currentFreqTable[grd.GridIndex];
					int32 rowIndex = grd.GridIndex / m_edgeCount;
					int32 spFreqRowIdx = rowIndex*MaxFreq;

					m_rowGridsFreqClassificationCount[freq + spFreqRowIdx]--;

					freq = MaxFreq - 1;

					m_rowGridsFreqClassificationCount[freq + spFreqRowIdx]++;
				}
			}
		}
		void Font::UseGrids(Glyph* g, int32 i, int32 j, int32 amount)
		{
			if (g)
			{
				// use grids
				const Apoc3D::Math::RectangleF& bukRect = m_grids[i*m_edgeCount + j].SrcRect;

				g->NumberOfGridsUsing = amount;
				g->StartingParentGrid = i*m_edgeCount + j;
				for (int32 k = 0; k < amount; k++)
				{
					m_grids[i*m_edgeCount + j + k].CurrentGlyph = g->Index;
				}
				g->MappedRect = Apoc3D::Math::Rectangle((int32)bukRect.X, (int32)bukRect.Y, g->Width, g->Height);
				g->MappedRectF = Apoc3D::Math::RectangleF(bukRect.X, bukRect.Y, (float)g->Width, (float)g->Height);
				g->IsMapped = true;
			}
			else
			{
				// clear the requested amount of grids one by one
				for (int32 s = 0; s < amount; s++)
				{
					int32 index = m_grids[i*m_edgeCount + j + s].CurrentGlyph;
					if (index != -1)
					{
						Glyph& oglyph = m_glyphList[index];
						oglyph.IsMapped = false;

						for (int32 k = 0; k < oglyph.NumberOfGridsUsing; k++)
						{
							m_grids[oglyph.StartingParentGrid + k].CurrentGlyph = -1;
						}
						oglyph.StartingParentGrid = -1;
						oglyph.NumberOfGridsUsing = 0;
					}
				}
			}
		}
		
		void Font::AdvanceXSimple(float& x, char16_t ch)
		{
			CustomGlyph* cgdef = m_customCharacters.TryGetValue(ch);
			if (cgdef)
			{
				x += cgdef->AdvanceX;
			}
			else
			{
				Character chdef;
				if (m_charTable.TryGetValue(ch, chdef))
				{
					x += chdef.AdvanceX;
				}
			}
		}

		PointF Font::GetOrigin(int32 x, int32 y) const
		{
			PointF r = { (float)x, (float)y + m_descender };
			if (m_hasDrawOffset)
			{
				r.X -= (int32)m_drawOffset.X;
				r.Y -= (int32)m_drawOffset.Y;
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


		FORCE_INLINE bool Font::ScanColorControlCodes(const String& str, char16_t& cur, int32& i, int32 len, uint* color)
		{
			if (cur == ControlCodes::Font_Color)
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

		FORCE_INLINE bool Font::ScanMoveControlCode(const String& str, char16_t& cur, int32& i, int32 len, const PointF* orig, PointF* pos)
		{
			if (cur == ControlCodes::Font_Move)
			{
				if (i + 3 < len)
				{
					if (orig && pos)
					{
						uint32 ch1 = str[i + 1];
						uint32 ch2 = str[i + 2];

						uint32 data = ch1 << 16 | ch2;

						uint32 flags = data >> 30;

						uint16 dstxi = (data >> 15) & 0xffff;
						uint16 dstyi = data & 0xffff;

						dstxi = ~dstxi;
						dstyi = ~dstyi;

						dstxi &= 0x7FFF;
						dstyi &= 0x7FFF;

						float dstx = (float)dstxi;
						float dsty = (float)dstyi;

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

		FORCE_INLINE void Font::ScanOtherControlCodes(const String& str, char16_t& cur, int32& i, int32 len)
		{
			if (cur == ControlCodes::Label_HyperLinkStart)
			{
				if (i + 2 < len)
				{
					i += 2;
					cur = str[i];
				}
			}
			else if (cur == ControlCodes::Label_HyperLinkEnd)
			{
				if (i + 1 < len)
				{
					i++;
					cur = str[i];
				}
			}
		}




		/************************************************************************/
		/*  FontManager                                                         */
		/************************************************************************/

		SINGLETON_IMPL(FontManager);

		int32 FontManager::MaxTextureSize = 1024;

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
			msg += StringUtils::IntToString(m_fontTable.getCount());
			msg.append(L" fonts currently loaded using ");
			msg += StringUtils::IntToString(bytesUsed);
			msg.append(L"MB");

			if (numCmpFont>0)
			{
				msg.append(L" including ");
				msg += StringUtils::IntToString(numCmpFont);
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
						msg += name;
						msg.append(L"(");
						msg += StringUtils::IntToString(fnt->m_selectTextureSize);
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