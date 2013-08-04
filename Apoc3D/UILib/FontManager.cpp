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
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Core/Logging.h"

using namespace Apoc3D::IO;

SINGLETON_DECL(Apoc3D::UI::FontManager);

namespace Apoc3D
{
	namespace UI
	{
		// compare Glyph by their width
		int Font::qsort_comparer(const void* a, const void* b)
		{
			const Glyph* ga = reinterpret_cast<const Glyph*>(b);
			const Glyph* gb = reinterpret_cast<const Glyph*>(a);

			if (ga->Width<gb->Width)
				return -1;
			if (ga->Width>gb->Width)
				return 1;
			return 0;
		}

		Font::Font(RenderDevice* device, ResourceLocation* fl)
			: m_charTable(255, WCharEqualityComparer::BuiltIn::Default), m_resource(fl), m_isUsingCaching(false)
		{
			m_selectTextureSize = FontManager::MaxTextureSize;

			ObjectFactory* fac = device->getObjectFactory();
			
			Stream* strm = fl->GetReadStream();
			BinaryReader* br = new BinaryReader(strm);

			int charCount = br->ReadInt32();
			
			bool newVersion = false;
			if (((uint)charCount & 0xffffff00) == 0xffffff00)
			{
				newVersion = true;

				charCount = br->ReadInt32();
			}

			if (newVersion)
			{
				m_height = br->ReadSingle();
				m_lineGap = br->ReadSingle();
				m_ascender = br->ReadSingle();
				m_descender = br->ReadSingle();

				for (int i=0;i<charCount;i++)
				{
					Character ch;
					ch._Character = static_cast<wchar_t>( br->ReadInt32());
					ch.GlyphIndex = br->ReadInt32();
					ch.Left = br->ReadInt16();
					ch.Top = br->ReadInt16();
					ch.AdcanceX = br->ReadSingle();
					m_charTable.Add(ch._Character, ch);
				}
			}
			else
			{
				m_lineGap = m_ascender = m_descender = 0;

				for (int i=0;i<charCount;i++)
				{
					Character ch;
					ch._Character = static_cast<wchar_t>( br->ReadInt32());
					ch.GlyphIndex = br->ReadInt32();
					ch.Left = ch.Top = 0;
					m_charTable.Add(ch._Character, ch);
				}
			}
			
			int maxWidth = 1;
			int maxHeight = 1;
			int glyphCount = br->ReadInt32();
			m_glyphList = new Glyph[glyphCount];
			for (int i=0;i<glyphCount;i++)
			{
				Glyph glyph;
				glyph.Index = br->ReadInt32();
				glyph.Width = br->ReadInt32();
				glyph.Height = br->ReadInt32();
				glyph.Offset = br->ReadInt64();
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
			if (!newVersion)
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
			}


			
			// now put the glyphs into the buckets. Initially the largest glyphs are inserted.
			{
				// this temp list is used to store sorting without changing the original one
				Glyph* tempList = new Glyph[glyphCount];
				memcpy(tempList, m_glyphList, sizeof(Glyph) * glyphCount);

				// sort glyphs from wider to thiner.
				std::qsort(tempList, glyphCount, sizeof(Glyph), qsort_comparer);
				
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

						LoadGlyphData(br, oglyph);

						buckX[buckY]+=bucketsNeeded;
					}
					buckY = (buckY+1) % m_edgeCount;
				}

				delete[] tempList;
				delete[] buckX;
			}


			br->Close();
			delete br;

			if (!newVersion)
			{
				for (HashMap<wchar_t, Character>::Enumerator i=m_charTable.GetEnumerator();i.MoveNext();)
				{
					Character* ch = i.getCurrentValue();
					Glyph& g = m_glyphList[ch->GlyphIndex];
					ch->AdcanceX = static_cast<float>(g.Width);
				}
			}
			
		}
		Font::~Font()
		{
			delete[] m_lineBucketsFreqClassificationCount;
			delete[] m_buckets;
			delete m_font;
			delete[] m_glyphList;
			delete m_resource;
		}

		void Font::DrawStringEx(Sprite* sprite, const String& text, float _x, float y, uint color, int length, float extLineSpace, wchar_t suffix, float hozShrink)
		{
			float std = _x;
			float x = std;

			y += m_descender;

			size_t len = text.length();
			if (length !=-1)
			{
				if ((size_t)length<len)
					len = (size_t)length;
			}

			float lineSpacing = m_height + m_lineGap;
			if (extLineSpace!=-1)
				lineSpacing=extLineSpace;

			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					if (IgnoreCharDrawing(ch))
						continue;

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						Glyph& glyph = m_glyphList[chdef.GlyphIndex];
						
						if (glyph.Width == 0 || glyph.Height == 0)
						{
							x += chdef.AdcanceX;// + chdef.Left;
							continue;
						}

						if (!glyph.IsMapped)
						{
							EnsureGlyph(glyph);
						}

						Apoc3D::Math::RectangleF rect;
						rect.X = x + chdef.Left;
						rect.Y = y + chdef.Top;
						rect.Width = static_cast<float>(glyph.Width);
						rect.Height = static_cast<float>(glyph.Height);
						//glyph.LastTimeUsed = (float)time(0);
						SetUseFreq(glyph);

						sprite->Draw(m_font, rect, &glyph.MappedRectF, color);

						x += chdef.AdcanceX + hozShrink;// glyph.Width - 1;
					}
				}
				else
				{
					x = std;
					y += lineSpacing;
				}
			}
			if (suffix)
			{
				wchar_t ch = suffix;
				
				Character chdef;
				if (m_charTable.TryGetValue(ch, chdef))
				{
					Glyph& glyph = m_glyphList[chdef.GlyphIndex];

					if (glyph.Width == 0 || glyph.Height == 0)
						return;

					if (!glyph.IsMapped)
					{
						EnsureGlyph(glyph);
					}

					Apoc3D::Math::RectangleF rect;
					rect.X = x + chdef.Left;
					rect.Y = y + chdef.Top;
					rect.Width = static_cast<float>(glyph.Width);
					rect.Height = static_cast<float>(glyph.Height);
					//glyph.LastTimeUsed = (float)time(0);
					SetUseFreq(glyph);

					sprite->Draw(m_font, rect, &glyph.MappedRectF, color);
				}
			}
		}
		void Font::DrawString(Sprite* sprite, const String& text, float _x, float y, int width, uint color)
		{
			float std = _x;
			float x = std;

			float lineSpacing = m_height + m_lineGap;
			
			y += m_descender;

			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					if (IgnoreCharDrawing(ch))
						continue;

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						if (glyph.Width == 0 || glyph.Height == 0)
						{
							x += chdef.AdcanceX;
							continue;
						}

						float nextX = x + chdef.AdcanceX;
						if (nextX>=width + std)
						{
							x = std;
							nextX = x + chdef.AdcanceX;
							y += lineSpacing;
						}

						if (!glyph.IsMapped)
						{
							EnsureGlyph(glyph);
						}

						Apoc3D::Math::RectangleF rect;
						rect.X = x + chdef.Left;
						rect.Y = y + chdef.Top;
						rect.Width = static_cast<float>(glyph.Width);
						rect.Height = static_cast<float>(glyph.Height);
						//glyph.LastTimeUsed = (float)time(0);
						SetUseFreq(glyph);

						sprite->Draw(m_font, rect, &glyph.MappedRectF, color);

						x = nextX;
					}

				}
				else
				{
					x = std;
					y += lineSpacing;
				}
			}
		}

		void Font::DrawStringEx(Sprite* sprite, const String& text, int _x, int y, uint color, int length, int extLineSpace, wchar_t suffix, float hozShrink)
		{
			float std = static_cast<float>(_x);
			float x = std;

			size_t len = text.length();
			if (length !=-1)
			{
				if ((size_t)length<len)
					len = (size_t)length;
			}

			int lineSpacing = static_cast<int>(m_height + m_lineGap+0.5f);
			if (extLineSpace!=-1)
				lineSpacing=extLineSpace;

			y += static_cast<int>(m_descender);

			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					if (IgnoreCharDrawing(ch))
						continue;

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						if (glyph.Width == 0 || glyph.Height == 0)
						{
							x += chdef.AdcanceX;// + chdef.Left;
							continue;
						}

						if (!glyph.IsMapped)
						{
							EnsureGlyph(glyph);
						}

						Apoc3D::Math::Rectangle rect;
						rect.X = static_cast<int32>(x+0.5f) + chdef.Left;
						rect.Y = y + chdef.Top;
						rect.Width = glyph.Width;
						rect.Height = glyph.Height;
						//glyph.LastTimeUsed = (float)time(0);
						SetUseFreq(glyph);

						sprite->Draw(m_font, rect, &glyph.MappedRect, color);

						x += chdef.AdcanceX + hozShrink;
					}

				}
				else
				{
					x = std;
					y += lineSpacing;
				}
			}
			if (suffix)
			{
				wchar_t ch = suffix;

				Character chdef;
				if (m_charTable.TryGetValue(ch, chdef))
				{
					Glyph& glyph = m_glyphList[chdef.GlyphIndex];

					if (glyph.Width == 0 || glyph.Height == 0)
						return;

					if (!glyph.IsMapped)
					{
						EnsureGlyph(glyph);
					}

					Apoc3D::Math::Rectangle rect;
					rect.X = static_cast<int32>(x+0.5f) + chdef.Left;
					rect.Y = y + chdef.Top;
					rect.Width = glyph.Width;
					rect.Height = glyph.Height;
					//glyph.LastTimeUsed = (float)time(0);
					SetUseFreq(glyph);

					sprite->Draw(m_font, rect, &glyph.MappedRect, color);
				}
			}
		}
		void Font::DrawString(Sprite* sprite, const String& text, int _x, int y, int width, uint color)
		{
			float std = static_cast<float>(_x);
			float x = std;

			int ls = static_cast<int>(m_height + m_lineGap+0.5f);

			y += static_cast<int>(m_descender);

			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					if (IgnoreCharDrawing(ch))
						continue;

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						if (glyph.Width == 0 || glyph.Height == 0)
						{
							x += chdef.AdcanceX;
							continue;
						}

						float nextX = x + chdef.AdcanceX;
						if (nextX>=width + std)
						{
							x = std;
							nextX = x + chdef.AdcanceX;
							y += ls;
						}


						if (!glyph.IsMapped)
						{
							EnsureGlyph(glyph);
						}

						Apoc3D::Math::Rectangle rect;
						rect.X = static_cast<int32>(x+0.5f) + chdef.Left;
						rect.Y = y + chdef.Top;
						rect.Width = glyph.Width;
						rect.Height = glyph.Height;
						//glyph.LastTimeUsed = (float)time(0);
						SetUseFreq(glyph);

						sprite->Draw(m_font, rect, &glyph.MappedRect, color);

						x = nextX;
					}

				}
				else
				{
					x = std;
					y += ls;
				}
			}
		}

		int Font::CalculateLineCount(const String& text, int width)
		{
			int lineCount=1;
			float x = 0;
			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						const Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						if (x>=width)
						{
							x =0;
							lineCount++;
						}

						x += chdef.AdcanceX;
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

			float x = 0.f;
			float y = m_height + m_lineGap + m_descender;
			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					if (IgnoreCharDrawing(ch))
						continue;

					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						//const Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						x += chdef.AdcanceX;
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
			
			int lineSpacing = static_cast<int>(m_height + m_lineGap+0.5f);

			float x = 0.f;
			float y = lineSpacing + m_descender;
			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				
				if (IgnoreCharDrawing(ch))
					continue;

				Character chdef;
				if (m_charTable.TryGetValue(ch, chdef))
				{
					x += chdef.AdcanceX;
					if (x > width)
						break;
					chCount++;
				}

				if (result.X < x)
					result.X = x;
				if (result.Y < y)
					result.Y = y;
			}
			return chCount;
		}

		void Font::FrameStartReset()
		{
			if (m_isUsingCaching)
			{
				// copy the data to its "last-frame" buffer
				memcpy(m_lasttime_lineBucketsFreqClassificationCount, m_lineBucketsFreqClassificationCount, sizeof(int)*m_edgeCount*MaxFreq);
				// the reset the current to zero for statistics in the current frame
				memset(m_lineBucketsFreqClassificationCount,0,sizeof(int)*m_edgeCount*MaxFreq);
				// set specific freq of all buckets in each line to the zero feq
				for (int i=0;i<m_edgeCount;i++)
				{
					m_lineBucketsFreqClassificationCount[i * MaxFreq + 0] = m_edgeCount;
				}

				memcpy(m_lastFreqTable, m_currentFreqTable, sizeof(int)*m_edgeCount*m_edgeCount);
				memset(m_currentFreqTable,0,sizeof(int)*m_edgeCount*m_edgeCount);
			
			}

		}

		void Font::LoadGlyphData(BinaryReader* br, Glyph& glyph)
		{
			if (glyph.Width == 0 || glyph.Height == 0)
				return;

			br->getBaseStream()->Seek(glyph.Offset, SEEK_Begin);

			Apoc3D::Math::Rectangle lockRect(static_cast<int32>(glyph.MappedRect.X), static_cast<int32>(glyph.MappedRect.Y),
				static_cast<int32>(glyph.MappedRect.Width), static_cast<int32>(glyph.MappedRect.Height));
			DataRectangle dataRect = m_font->Lock(0, LOCK_None, lockRect);

			char* buf = new char[glyph.Width * glyph.Height];
			assert(glyph.Width <= m_selectTextureSize && glyph.Width >=0);
			assert(glyph.Height <= m_selectTextureSize && glyph.Height >=0);

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


			m_font->Unlock(0);
			delete[] buf;
		}
		void Font::EnsureGlyph(Glyph& glyph)
		{
			int bucketsNeeded = (int)ceil((double)glyph.Width/m_height);

			bool done = false;
			// find enough space for the glyph, testing from lower use time to higher use time
			for (int freq = 0; freq<MaxFreq && !done; freq++)
			{
				for (int i=0;i<m_edgeCount && !done;i++)
				{
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
							BinaryReader* br = new BinaryReader(strm);

							// 2. Tag the buckets
							UseBuckets(&glyph, i, bucketPosition, bucketsNeeded);

							// 3. Load the data
							LoadGlyphData(br, glyph);

							br->Close();
							delete br;

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
				for (int i=0;i<glyph.NumberOfBucketsUsing;i++)
				{
					const Bucket& buk = m_buckets[glyph.StartingParentBucket+i];
					int& freq = m_currentFreqTable[buk.BucketIndex];
					int lineIndex = buk.BucketIndex / m_edgeCount;
					int spFreqLineIdx = lineIndex * MaxFreq;
					m_lineBucketsFreqClassificationCount[freq + spFreqLineIdx]--;

					/*if (max)
					{
						freq = MaxFreq -1;
					}
					else*/
					{
						freq++;
						if (freq >= MaxFreq)
							freq = MaxFreq-1;
					}

					m_lineBucketsFreqClassificationCount[freq + spFreqLineIdx]++;
				}
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
			for (HashMap<String, Font*>::Enumerator iter = m_fontTable.GetEnumerator(); iter.MoveNext();)
			{
				delete *iter.getCurrentValue();
			}
		}

		Font* FontManager::getFont(const String& fontName)
		{
			return m_fontTable[fontName];
		}

		void FontManager::LoadFont(RenderDevice* device, const String& name, ResourceLocation* rl)
		{
			Font* font = new Font(device, rl);
			m_fontTable.Add(name, font);

		}
		void FontManager::StartFrame()
		{
			for (HashMap<String, Font*>::Enumerator iter = m_fontTable.GetEnumerator(); iter.MoveNext();)
			{
				Font* fnt = *iter.getCurrentValue();
				fnt->FrameStartReset();
			}
		}
	}
}