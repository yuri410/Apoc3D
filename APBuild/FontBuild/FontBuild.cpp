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
#include "FontBuild.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include "Collections/CollectionsCommon.h"
#include "Config/ConfigurationSection.h"
#include "Collections/FastList.h"
#include "Collections/ExistTable.h"
#include "Collections/FastMap.h"
#include "IOLib/Streams.h"
#include "IOLib/BinaryWriter.h"
#include "BuildConfig.h"

using namespace Apoc3D;
using namespace Apoc3D::IO;
using namespace Apoc3D::Collections;
using namespace Gdiplus;

#pragma comment (lib,"Gdiplus.lib")



namespace APBuild
{
	struct GlyphBitmap
	{
		int Index;
		int Width;
		int Height;
		int64 HashCode;
		const char* PixelData;

		GlyphBitmap() { }
		GlyphBitmap(int width, int height, const char* data)
			: Width(width), Height(height), PixelData(data)
		{
			// TODO:compute hash code
		}

		friend bool operator ==(const GlyphBitmap& x, const GlyphBitmap& y)
		{
			return x.Width == y.Width && x.Height == y.Height && x.HashCode == y.HashCode &&
				!memcmp(x.PixelData, y.PixelData, x.Width * y.Height);
		}
	};

	template class IEqualityComparer<GlyphBitmap>;

	class GlyphBitmapEqualityComparer : public IEqualityComparer<GlyphBitmap>
	{
	public:
		GlyphBitmapEqualityComparer() { }

		virtual bool Equals(const GlyphBitmap& x, const GlyphBitmap& y) const
		{
			return x==y;
		}

		virtual int64 GetHashCode(const GlyphBitmap& obj) const
		{
			return obj.HashCode;
		}
	};

	

	struct CharMapping
	{
		wchar_t Character;
		int GlyphIndex;
	};

	char ConvertByte(char c)
	{
		byte b = reinterpret_cast<const byte&>(c);
		if(b == 0xff)
			b = 0xfe;
		return reinterpret_cast<const char&>(b);
	}

	char* CompressData(const char* src, int width, int height)
	{
		assert(width);
		assert(height);

		vector<char> data;
		data.reserve(width*height);

		for (int i=0;i<height;i++)
		{
			int counter = 0;
			
			char curByte = ConvertByte(src[i*width]);

			int startPos = 0;
			

			for (int j=1;j<width;j++)
			{
				char cc = ConvertByte(src[i*width + j]);
				if (curByte == cc && counter < 0xfe)
				{
					if (!counter)
					{
						startPos = j;
						curByte = ConvertByte(src[i*width]);
					}
					counter++;
				}
				else
				{
					if (counter > 3)
					{
						static const byte maxb = 0xff;
						data.push_back(reinterpret_cast<const char&>(maxb));
						data.push_back(reinterpret_cast<const char&>(counter));
						data.push_back(curByte);
						counter = 0;
					}
					else
					{
						for (int k=startPos;k<=j;k++)
						{
							data.push_back(ConvertByte(src[i*width + k]));
						}
						counter = 0;
					}
				}
			}
		}

		char* result = new char[data.size()];
		for (size_t i=0;i<data.size();i++)
			result[i] = data[i];
		return result;
	}

	void FontBuild::Build(const ConfigurationSection* sect)
	{
		FontBuildConfig config;
		config.Parse(sect);

		GlyphBitmapEqualityComparer* comparer = new GlyphBitmapEqualityComparer();
		FastList<CharMapping> charMap(0xffff);
		FastMap<GlyphBitmap, GlyphBitmap> glyphHashTable(0xffff, comparer);
		bool* passCheck = new bool[0xffff];
		memset(passCheck,0,sizeof(bool)*0xffff);
		

		Bitmap globalBmp(1,1, PixelFormat32bppARGB);
		Gdiplus::Graphics* gg = Gdiplus::Graphics::FromImage(&globalBmp);
		
		Font font(config.Name.c_str(), config.Size, config.Style);

		int index = 0;
		for (int i=0;i<config.Ranges.getCount();i++)
		{
			for (wchar_t ch = (wchar_t)config.Ranges[i].MinChar; 
				ch <= (wchar_t)config.Ranges[i].MaxChar; ch++)
			{
				RectF size;
				gg->MeasureString(&ch, 1, &font, Gdiplus::PointF(0,0), &size);

				int width = static_cast<int>(ceilf(size.Width));
				int height = static_cast<int>(ceilf(size.Height));

				Bitmap bitmap((INT)width, (INT)height, PixelFormat32bppARGB);

				Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(&bitmap);
				g->SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
				g->Clear(Gdiplus::Color(0));


				Brush* brush = new SolidBrush((Gdiplus::ARGB)Gdiplus::Color::White);
				StringFormat* strFmt = new StringFormat();
				strFmt->SetAlignment(StringAlignmentNear);
				strFmt->SetLineAlignment(StringAlignmentNear);

				g->DrawString(&ch,1,&font,Gdiplus::PointF(0,0), strFmt, brush);


				g->Flush();
				delete g;
				delete brush;
				delete strFmt;

				Gdiplus::BitmapData bmpData;
				Gdiplus::Rect lr = Gdiplus::Rect( 0, 0, width, height );
				bitmap.LockBits(&lr, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

				const uint* data = reinterpret_cast<const uint*>(bmpData.Scan0);

				char* buffer = new char[width*height];
				int srcofs = 0;
				int dstofs = 0;
				for (int i=0;i<height;i++)
				{
					srcofs += bmpData.Stride;
					dstofs += width;
					for (int j=0;j<width;j++)
					{
						buffer[dstofs+j] = data[srcofs + j] >> 24;
					}
				}

				GlyphBitmap result;
				GlyphBitmap glyph(width, height, buffer);
				if (!glyphHashTable.TryGetValue(glyph, result))
				{
					glyph.Index = index++;
					glyphHashTable.Add(glyph, glyph);
				}
				else
				{
					delete[] buffer;
				}



				if (!passCheck[ch])
				{
					CharMapping m = { ch, glyph.Index };
					charMap.Add(m);
				}

				bitmap.UnlockBits(&bmpData);
			}
		}

		FileOutStream* fs = new FileOutStream(config.DestFile);
		BinaryWriter* bw = new BinaryWriter(fs);

		bw->Write(charMap.getCount());
		for (int i=0;i<charMap.getCount();i++)
		{
			bw->Write(charMap[i].Character);
			bw->Write(charMap[i].GlyphIndex);
		}

		int64 glyRecPos = fs->getPosition();
		for (int i=0;i<glyphHashTable.getCount();i++)
		{
			bw->Write((int32)0);
			bw->Write((int32)0);
			bw->Write((int32)0);
			bw->Write((int64)0);
		}
		int64 baseOfs = fs->getPosition();

		for (FastMap<GlyphBitmap, GlyphBitmap>::Enumerator i = glyphHashTable.GetEnumerator();i.MoveNext();)
		{
			const GlyphBitmap* g = i.getCurrentKey();

			bw->Write(g->PixelData, g->Width * g->Height);
		}

		fs->Seek(glyRecPos, SEEK_Begin);

		for (FastMap<GlyphBitmap, GlyphBitmap>::Enumerator i = glyphHashTable.GetEnumerator();i.MoveNext();)
		{
			const GlyphBitmap* g = i.getCurrentKey();

			bw->Write((int32)g->Index);
			bw->Write((int32)g->Width);
			bw->Write((int32)g->Height);
			bw->Write((int64)baseOfs);
			baseOfs += g->Width * g->Height;
		}


		bw->Close();
		delete bw;
		delete fs;

		delete comparer;
		delete[] passCheck;
	}
}
