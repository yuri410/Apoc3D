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

#include "FontManager.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Vfs/ResourceLocation.h"
#include "IOLib/BinaryReader.h"
#include "IOLib/Streams.h"
#include "Math/Point.h"
#include "Core/Logging.h"


using namespace Apoc3D::IO;

SINGLETON_DECL(Apoc3D::UI::FontManager);

namespace Apoc3D
{
	namespace UI
	{
		//int qsort_comparer(const void* a, const void* b)
		//{
		//	return Font::qsort_comparer(b,a);
		//}
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
			: m_charTable(255, WCharEqualityComparer::BuiltIn::Default), m_resource(fl)
		{
			ObjectFactory* fac = device->getObjectFactory();
			m_font = fac->CreateTexture(FontManager::TextureSize, FontManager::TextureSize, 1, TU_Static, FMT_A8L8);

			Stream* strm = fl->GetReadStream();
			BinaryReader* br = new BinaryReader(strm);

			int charCount = br->ReadInt32();
			
			for (int i=0;i<charCount;i++)
			{
				Character ch;
				ch._Character = static_cast<wchar_t>( br->ReadInt32());
				ch.GlyphIndex = br->ReadInt32();
				m_charTable.Add(ch._Character, ch);
			}

			int maxHeight = 1;
			int maxWidth = 1;

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
				glyph.ParentBuckets[0] = glyph.ParentBuckets[1] = glyph.ParentBuckets[2] = glyph.ParentBuckets[3] = glyph.ParentBuckets[4]=-1;
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
			m_height = maxHeight;
			m_maxWidth = maxWidth;

			m_edgeCount = FontManager::TextureSize / maxHeight;
			m_buckets = new Bucket[m_edgeCount*m_edgeCount];
			m_currentFreqTable = new int[m_edgeCount*m_edgeCount];
			m_lastFreqTable = new int[m_edgeCount*m_edgeCount];

			for (int i=0;i<m_edgeCount;i++)
			{
				for (int j=0;j<m_edgeCount;j++)
				{
					Bucket& bk = m_buckets[i*m_edgeCount+j];
					//bk.Freq = 0;
					//bk.X = j;
					//bk.Y = i;
					bk.BucketIndex = i * m_edgeCount+j;
					bk.CurrentGlyph = -1;
					bk.SrcRect = Apoc3D::Math::Rectangle(j*m_height, i*m_height, m_height, m_height);
				}
			}
			m_lineBucketsFreqClassificationCount = new int[m_edgeCount*MaxFreq];
			
			memset(m_lineBucketsFreqClassificationCount,0,sizeof(int)*m_edgeCount*MaxFreq);
			memset(m_currentFreqTable,0,sizeof(int)*m_edgeCount*m_edgeCount);
			memset(m_lastFreqTable,0,sizeof(int)*m_edgeCount*m_edgeCount);


			if (glyphCount<=256)
			{
				Glyph* tempList = new Glyph[glyphCount];
				memcpy(tempList, m_glyphList, sizeof(Glyph) * glyphCount);

				std::qsort(tempList, glyphCount, sizeof(Glyph), qsort_comparer);

				int buckY = 0;
				int* buckX = new int[m_edgeCount];
				memset(buckX,0,sizeof(int)*m_edgeCount);
				for (int i=0;i<glyphCount && buckX[buckY]<m_edgeCount;i++)
				{
					int startX = buckX[buckY];
					//// first find the 1st available bucket in this line
					//for (int k=0;k<m_edgeCount;k++)
					//{
					//	if (m_buckets[k+buckY*m_edgeCount].BucketIndex!=-1) buckX++;
					//}


					Glyph& glyph = tempList[i];
					int bucketsNeeded = (int)ceil((double)glyph.Width/m_height);

					if (startX+bucketsNeeded-1<m_edgeCount)
					{
						for (int k=0;k<bucketsNeeded;k++)
						{
							glyph.ParentBuckets[k] = buckY*m_edgeCount+startX+k;
							m_buckets[buckY*m_edgeCount+startX+k].CurrentGlyph = glyph.Index;
						}

						Glyph& oglyph = m_glyphList[glyph.Index];
						const Apoc3D::Math::Rectangle& bukRect = m_buckets[buckY*m_edgeCount+startX].SrcRect;
						oglyph.MappedRect = Apoc3D::Math::Rectangle(bukRect.X, bukRect.Y, glyph.Width, glyph.Height);
						oglyph.IsMapped = true;

						LoadGlyphData(br, oglyph);

						buckX[buckY]+=bucketsNeeded;
					}
					buckY = (buckY+1) % m_edgeCount;
				}

				delete[] tempList;
				delete[] buckX;
			}
			
			//// if the texture can hold all glyphs, just load them all at once
			////if (m_edgeCount*m_edgeCount>glyphCount)
			//{
			//	int remainingBuckets = m_edgeCount*m_edgeCount;
			//	//int stx = 0;
			//	//int sty = 0;
			//	//int holdableCount = 0;//(FontManager::TextureSize / maxHeight) *  (FontManager::TextureSize/maxWidth);
			//	for (int i=0;i<glyphCount;i++)
			//	{
			//		const Glyph& glyph = m_glyphList[i];

			//		int w = glyph.Width;

			//		//stx += glyph.Width+1;
			//		//
			//		//if (stx>=FontManager::TextureSize)
			//		//{
			//		//	stx = glyph.Width+1;
			//		//	sty += maxHeight+1;
			//		//	if (sty>=FontManager::TextureSize)
			//		//	{
			//		//		break;
			//		//	}
			//		//}
			//		//holdableCount++;
			//	}


			//	stx = 0;
			//	sty = 0;
			//	//int lineHeight = 0;

			//	int loopMax = min(holdableCount, glyphCount);
			//	//if (holdableCount>=glyphCount)

			//	//m_buckets.ResizeDiscard(FontManager::TextureSize/maxHeight);


			//	{
			//		//Bucket* bukk = 0;

			//		for (int i=0;i<loopMax;i++)
			//		{
			//			Glyph& glyph = m_glyphList[i];
			//			
			//			glyph.IsMapped = true;
			//			

			//			glyph.MappedRect = Apoc3D::Math::Rectangle(stx, sty, glyph.Width, glyph.Height);

			//			stx += glyph.Width+1;
			//			//if (lineHeight<glyph.Height)
			//			//{
			//				//lineHeight = glyph.Height;
			//			//}

			//			if (stx>=FontManager::TextureSize)
			//			{
			//				stx = glyph.Width+1;
			//				sty += m_height+1;
			//				//lineHeight = 0;
			//				glyph.MappedRect = Apoc3D::Math::Rectangle(0, sty, glyph.Width, glyph.Height);

			//				
			//				bukk = 0;
			//			}

			//			
			//			LoadGlyphData(br, glyph);

			//			//if (bukk==0)
			//			//{
			//			//	bukk = new Bucket();
			//			//	bukk->CurrentGlyph = &glyph;
			//			//	bukk->SrcRect = glyph.MappedRect;
			//			//	bukk->Next = 0;
			//			//	m_buckets.Add(bukk);
			//			//}
			//			//else
			//			//{
			//			//	bukk->Next = new Bucket();
			//			//	bukk->Next->CurrentGlyph = &glyph;
			//			//	bukk->Next->SrcRect = glyph.MappedRect;
			//			//	bukk->Next->Next = 0;
			//			//}
			//		}
			//	}
			//}
			//

			br->Close();
			delete br;
		}
		Font::~Font()
		{
			delete[] m_lineBucketsFreqClassificationCount;
			delete[] m_buckets;
			delete m_font;
			delete[] m_glyphList;
			delete m_resource;
		}


		void Font::DrawString(Sprite* sprite, const String& text, const Point& pt, uint color)
		{
			DrawStringEx(sprite, text, pt.X, pt.Y, color);
		}
		void Font::DrawStringEx(Sprite* sprite, const String& text, int x, int y, uint color, int length, int lineSpace,wchar_t suffix)
		{
			int std = x;
			size_t len = text.length();
			if (length !=-1)
			{
				if ((size_t)length<len)
					len = (size_t)length;
			}
			int ls = m_height;
			if (lineSpace!=-1)
				ls=lineSpace;
			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						Glyph& glyph = m_glyphList[chdef.GlyphIndex];


						if (!glyph.IsMapped)
						{
							EnsureGlyph(glyph);
						}

						Apoc3D::Math::Rectangle rect;
						rect.X = x;
						rect.Y = y;
						rect.Width = glyph.Width;
						rect.Height = glyph.Height;
						//glyph.LastTimeUsed = (float)time(0);
						SetUseFreq(glyph);

						sprite->Draw(m_font, rect, &glyph.MappedRect, color);

						x += glyph.Width - 1;
					}
					
				}
				else
				{
					x = std;
					y += ls;
				}
			}
			if (suffix)
			{
				wchar_t ch = suffix;
				
				Character chdef;
				if (m_charTable.TryGetValue(ch, chdef))
				{
					Glyph& glyph = m_glyphList[chdef.GlyphIndex];

					if (!glyph.IsMapped)
					{
						EnsureGlyph(glyph);
					}

					Apoc3D::Math::Rectangle rect;
					rect.X = x;
					rect.Y = y;
					rect.Width = glyph.Width;
					rect.Height = glyph.Height;
					//glyph.LastTimeUsed = (float)time(0);
					SetUseFreq(glyph);

					sprite->Draw(m_font, rect, &glyph.MappedRect, color);
				}
			}
		}
		void Font::DrawString(Sprite* sprite, const String& text, int x, int y, int width, uint color)
		{
			int stdY = y;
			int std = x;
			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						Glyph& glyph = m_glyphList[chdef.GlyphIndex];


						if (!glyph.IsMapped)
						{
							EnsureGlyph(glyph);
						}

						Apoc3D::Math::Rectangle rect;
						rect.X = x;
						rect.Y = y;
						rect.Width = glyph.Width;
						rect.Height = glyph.Height;
						//glyph.LastTimeUsed = (float)time(0);
						SetUseFreq(glyph);

						sprite->Draw(m_font, rect, &glyph.MappedRect, color);

						x += glyph.Width - 1;
						if (x>=width)
						{
							x=std;
							y+=m_height;
						}
					}

				}
				else
				{
					x = std;
					y += m_height;
				}
			}
		}
		Point Font::MeasureString(const String& text, int width)
		{
			int x =0;
			int y =0;
			int stdY = y;
			int std = x;
			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						const Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						x += glyph.Width - 1;
						if (x>=width)
						{
							x=std;
							y+=m_height;
						}
					}

				}
				else
				{
					x = std;
					y += m_height;
				}
			}
			return Point(width-std, y+m_height-stdY);
		}
		Point Font::MeasureString(const String& text)
		{
			Point result = Point(0, m_height);

			int x = 0, y = m_height;
			for (size_t i = 0; i < text.length(); i++)
			{
				wchar_t ch = text[i];
				if (ch != '\n')
				{
					Character chdef;
					if (m_charTable.TryGetValue(ch, chdef))
					{
						const Glyph& glyph = m_glyphList[chdef.GlyphIndex];

						x += glyph.Width - 1;
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

			return result;
		}

		void Font::FrameStartReset()
		{
			
			memset(m_lineBucketsFreqClassificationCount,0,sizeof(int)*m_edgeCount*MaxFreq);
			memcpy(m_lastFreqTable, m_currentFreqTable, sizeof(int)*m_edgeCount*m_edgeCount);
			memset(m_currentFreqTable,0,sizeof(int)*m_edgeCount*m_edgeCount);
			
		}

		void Font::LoadGlyphData(BinaryReader* br, Glyph& glyph)
		{
			//br->getBaseStream()->setPosition(glyph.Offset);
			br->getBaseStream()->Seek(glyph.Offset, SEEK_Begin);

			DataRectangle dataRect = m_font->Lock(0, LOCK_None, glyph.MappedRect);

			char* buf = new char[glyph.Width * glyph.Height];
			br->ReadBytes(buf, glyph.Width * glyph.Height);


			for (int j=0;j<dataRect.getHeight();j++)
			{
				char* src=buf+j*glyph.Width;
				char* dest = (char*)dataRect.getDataPointer()+j*dataRect.getPitch();
				for (int i=0;i<dataRect.getWidth();i++)
				{
					uint16* pix = (uint16*)(dest)+i;
					byte alpha = *(byte*)(src+i);
					*pix = alpha << 8 | 0xff;
				}
				//memcpy((char*)dataRect.getDataPointer()+j*dataRect.getPitch(),
				//buf+j*glyph.Width, glyph.Width);
			}
			m_font->Unlock(0);
			delete[] buf;
		}
		void Font::EnsureGlyph(Glyph& glyph)
		{

			//m_activeGlyph.sort();

			//int accum_width = 0;
			//list<list<Glyph*>::iterator> discards;
			//for (list<Glyph*>::iterator iter = m_activeGlyph.begin();iter != m_activeGlyph.end();iter++)
			//{
			//	Glyph* g = *iter;
			//	discards.push_back(iter);

			//	accum_width += g->Width;
			//	if (accum_width>=glyph.Width)
			//	{

			//	}
			//	//if (g->Height >= glyph.Height && g->Width >= glyph.Width)
			//	//{
			//	//	g->IsMapped = false;
			//	//	m_activeGlyph.erase(iter);


			//	//	Stream* strm = m_resource->GetReadStream();
			//	//	BinaryReader* br = new BinaryReader(strm);

			//	//	LoadGlyphData(br, glyph);
			//	//	br->Close();delete br;

			//	//	m_activeGlyph.push_back(&glyph);

			//	//	break;
			//	//}
			//}
		}
		void Font::SetUseFreq(const Glyph& glyph)
		{
			int k=0;
			while (k<5 && glyph.ParentBuckets[k]!=-1)
			{
				const Bucket& buk = m_buckets[glyph.ParentBuckets[k]];
				int& freq = m_currentFreqTable[buk.BucketIndex];
				freq++;
				if (freq >= MaxFreq)
					freq = MaxFreq-1;
				m_lineBucketsFreqClassificationCount[freq]++;
				k++;
			}
		}

		int FontManager::TextureSize = 1024;

		FontManager::FontManager()
			: m_fontTable()
		{
			LogManager::getSingleton().Write(LOG_System, 
				L"Font manager initialized", 
				LOGLVL_Infomation);
		}
		FontManager::~FontManager()
		{
			for (FastMap<String, Font*>::Enumerator iter = m_fontTable.GetEnumerator(); iter.MoveNext();)
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
			for (FastMap<String, Font*>::Enumerator iter = m_fontTable.GetEnumerator(); iter.MoveNext();)
			{
				Font* fnt = *iter.getCurrentValue();
				
			}
		}
	}
}