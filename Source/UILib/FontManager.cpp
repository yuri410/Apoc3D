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

			// if the texture can hold all glyphs, just load them all at once
			{

				int stx = 0;
				int sty = 0;
				int holdableCount = 0;//(FontManager::TextureSize / maxHeight) *  (FontManager::TextureSize/maxWidth);
				for (int i=0;i<glyphCount;i++)
				{
					const Glyph& glyph = m_glyphList[i];

					stx += glyph.Width+1;
					
					if (stx>=FontManager::TextureSize)
					{
						stx = glyph.Width+1;
						sty += maxHeight+1;
						if (sty>=FontManager::TextureSize)
						{
							break;
						}
					}
					holdableCount++;
				}


				stx = 0;
				sty = 0;
				//int lineHeight = 0;

				int loopMax = min(holdableCount, glyphCount);
				//if (holdableCount>=glyphCount)
				{
					for (int i=0;i<loopMax;i++)
					{
						Glyph& glyph = m_glyphList[i];
						
						glyph.IsMapped = true;
						

						glyph.MappedRect = Apoc3D::Math::Rectangle(stx, sty, glyph.Width, glyph.Height);

						stx += glyph.Width+1;
						//if (lineHeight<glyph.Height)
						//{
							//lineHeight = glyph.Height;
						//}

						if (stx>=FontManager::TextureSize)
						{
							stx = glyph.Width+1;
							sty += m_height+1;
							//lineHeight = 0;
							glyph.MappedRect = Apoc3D::Math::Rectangle(0, sty, glyph.Width, glyph.Height);
						}

						
						LoadGlyphData(br, glyph);
					}
				}
			}
			

			br->Close();
			delete br;
		}
		Font::~Font()
		{
			delete m_font;
			delete[] m_glyphList;
			delete m_resource;
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
			m_activeGlyph.sort();
			
			for (list<Glyph*>::iterator iter = m_activeGlyph.begin();iter != m_activeGlyph.end();iter++)
			{
				Glyph* g = *m_activeGlyph.begin();
				if (g->Height >= glyph.Height && g->Width >= glyph.Width)
				{
					g->IsMapped = false;
					m_activeGlyph.erase(iter);


					Stream* strm = m_resource->GetReadStream();
					BinaryReader* br = new BinaryReader(strm);

					LoadGlyphData(br, glyph);
					br->Close();delete br;

					m_activeGlyph.push_back(&glyph);

					break;
				}
			}
		}

		void Font::DrawString(Sprite* sprite, const String& text, const Point& pt, uint color)
		{
			DrawString(sprite, text, pt.X, pt.Y, color);
		}
		void Font::DrawString(Sprite* sprite, const String& text, int x, int y, uint color)
		{
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
						glyph.LastTimeUsed = (float)time(0);

						sprite->Draw(m_font, rect, &glyph.MappedRect, color);

						x += glyph.Width - 1;
					}
					
				}
				else
				{
					x = std;
					y += m_height;
				}
			}
		}
		Point Font::DrawString(Sprite* sprite, const String& text, int x, int y, int width, uint color)
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
						glyph.LastTimeUsed = (float)time(0);

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

		int FontManager::TextureSize = 512;

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
		
	}
}