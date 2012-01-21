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
#include "Vfs/File.h"
#include "Vfs/PathUtils.h"
#include "BuildConfig.h"
#include "BuildEngine.h"
#include "CompileLog.h"


using namespace Apoc3D;
using namespace Apoc3D::IO;
using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Gdiplus;

#pragma comment (lib,"Gdiplus.lib")



namespace APBuild
{
	struct GlyphBitmap
	{
		int Index;
		int Width;
		int Height;
		int HashCode;
		const char* PixelData;

		GlyphBitmap() { }
		GlyphBitmap(int width, int height, const char* data)
			: Width(width), Height(height), PixelData(data)
		{
			uint even = 0x15051505;
			uint odd = even;
			const uint* numPtr = reinterpret_cast<const uint*>(data);
			for (int i = width*height; i > 0; i -= 8)
			{
				even = ((even << 5) + even + (even >> 0x1b)) ^ numPtr[0];
				if (i < 8)
				{
					break;
				}
				odd = ((odd << 5) + odd + (odd >> 0x1b)) ^ numPtr[1];
				numPtr += (sizeof(wchar_t) * 4) / sizeof(uint);
			}
			HashCode = (int)( even + odd * 0x5d588b65);
		}

		friend bool operator ==(const GlyphBitmap& x, const GlyphBitmap& y)
		{
			if (x.Width == y.Width && x.Height == y.Height && x.HashCode == y.HashCode)
			{
				for (int i=0;i<x.Width*x.Height;i++)
				{
					if (x.PixelData[i] != y.PixelData[i])
					{
						return false;
					}
				}
				return true;
			}
				
			//	memcmp(x.PixelData, y.PixelData, x.Width * x.Height) == 0;
			return false;
		}
	};

	template class IEqualityComparer<GlyphBitmap>;

	class GlyphBitmapEqualityComparer : public IEqualityComparer<GlyphBitmap>
	{
	public:
		GlyphBitmapEqualityComparer() { }

		virtual bool Equals(const GlyphBitmap& x, const GlyphBitmap& y) const
		{
			if (x==y)
				return true;
			return false;
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


	bool IsColEmpty(const Gdiplus::BitmapData* bmp, int x)
	{
		const char* data = reinterpret_cast<const char*>(bmp->Scan0);

		for (uint y = 0; y < bmp->Height; y++)
		{
			if (data[y* bmp->Stride + x * sizeof(uint)] & 0xff000000)
				return false;
		}

		return true;
	}
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		ImageCodecInfo* pImageCodecInfo = NULL;

		GetImageEncodersSize(&num, &size);
		if(size == 0)
			return -1;  // Failure

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
		if(pImageCodecInfo == NULL)
			return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);

		for(UINT j = 0; j < num; ++j)
		{
			if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}    
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}

	void FontBuild::Build(const ConfigurationSection* sect)
	{
		FontBuildConfig config;
		config.Parse(sect);

		EnsureDirectory(PathUtils::GetDirectory(config.DestFile));

		GlyphBitmapEqualityComparer* comparer = new GlyphBitmapEqualityComparer();
		FastList<CharMapping> charMap(0xffff);
		FastMap<GlyphBitmap, GlyphBitmap> glyphHashTable(0xffff, comparer);
		//bool* passCheck = new bool[0xffff];
		//memset(passCheck,0,sizeof(bool)*0xffff);
		

		Bitmap globalBmp(1,1, PixelFormat32bppARGB);
		Gdiplus::Graphics* gg = Gdiplus::Graphics::FromImage(&globalBmp);
		
		Font font(config.Name.c_str(), config.Size, config.Style);

		//int index = 0;
		for (int i=0;i<config.Ranges.getCount();i++)
		{
			for (wchar_t ch = (wchar_t)config.Ranges[i].MinChar; 
				ch <= (wchar_t)config.Ranges[i].MaxChar; ch++)
			{
				
				RectF size;
				gg->MeasureString(&ch, 1, &font, Gdiplus::PointF(0,0), &size);

				int width = static_cast<int>(ceilf(size.Width));
				int height = static_cast<int>(ceilf(size.Height));

				Bitmap* bitmap = new Bitmap((INT)width, (INT)height, PixelFormat32bppARGB);

				Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(bitmap);
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
				Gdiplus::Status ret = bitmap->LockBits(&lr, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

				const char* data = reinterpret_cast<const char*>(bmpData.Scan0);

				{
					int cropLeft = 0;
					int cropRight = width - 1;
					while ((cropLeft < cropRight) && IsColEmpty(&bmpData, cropLeft))
						cropLeft++;

					while ((cropRight > cropLeft) && IsColEmpty(&bmpData, cropRight))
						cropRight--;

					if (cropLeft != cropRight)
					{
						cropLeft = max(cropLeft-1,0);
						cropRight = min(cropRight+1, width-1);

						int width2 = cropRight-cropLeft+1;
						Gdiplus::Bitmap* altBmp = new Gdiplus::Bitmap(width2, height, bitmap->GetPixelFormat());

						{
							Gdiplus::BitmapData bmpData2;
							Gdiplus::Rect lr2 = Gdiplus::Rect( 0, 0, width2, height );
							Gdiplus::Status ret2 = altBmp->LockBits(&lr2, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpData2);

							char* data2 = reinterpret_cast<char*>(bmpData2.Scan0);
							int srcofs = 0;
							int dstofs = 0;
							for (int k=0;k<height;k++)
							{
								for (int j=cropLeft;j<=cropRight;j++)
								{
									*(uint*)(data2 + dstofs+(j-cropLeft)*sizeof(uint)) = *(uint*)(data + srcofs + j*sizeof(uint));
								}

								srcofs += bmpData.Stride;
								dstofs += bmpData2.Stride;
							}
							altBmp->UnlockBits(&bmpData2);
						}
					

						//Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(altBmp);
						
						//g->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
						//g->DrawImage(bitmap, 0,0, cropLeft, 0, width2, height, Gdiplus::UnitPixel);
						//g->Flush();
						//delete g;
						

						bitmap->UnlockBits(&bmpData);
						

						width = width2;

						delete bitmap;

						{
							//String testOut = String(L"E:\\Desktop\\fntb\\ss")+String(1,ch)+String(L".png");
							//if (ch >= 'A' && ch <= 'Z' && !File::FileExists(testOut))
							//{
							//	CLSID pngClsid;
							//	GetEncoderClsid(L"image/png", &pngClsid);
							//	altBmp->Save(testOut.c_str(), &pngClsid);

							//}
							
						}
						
						bitmap = altBmp;
						lr = Gdiplus::Rect( 0, 0, width, height );
						ret = bitmap->LockBits(&lr, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData);
					}
				}
				


				data = reinterpret_cast<const char*>(bmpData.Scan0);

				char* buffer = new char[width*height];
				int srcofs = 0;
				int dstofs = 0;
				for (int k=0;k<height;k++)
				{
					for (int j=0;j<width;j++)
					{
						buffer[dstofs+j] = data[srcofs + j*sizeof(uint)+3];
					}

					srcofs += bmpData.Stride;
					dstofs += width;
				}

				GlyphBitmap result;
				GlyphBitmap glyph(width, height, buffer);
				if (!glyphHashTable.TryGetValue(glyph, result))
				{
					glyph.Index = glyphHashTable.getCount();// index++;
					glyphHashTable.Add(glyph, glyph);
				}
				else
				{
					glyph = result;
					delete[] buffer;
				}



				//if (!passCheck[ch])
				{
					CharMapping m = { ch, glyph.Index };
					charMap.Add(m);
				}

				bitmap->UnlockBits(&bmpData);
				delete bitmap;
			}
		}

		FileOutStream* fs = new FileOutStream(config.DestFile);
		BinaryWriter* bw = new BinaryWriter(fs);

		bw->Write(charMap.getCount());
		for (int i=0;i<charMap.getCount();i++)
		{
			bw->Write((int32)charMap[i].Character);
			bw->Write((int32)charMap[i].GlyphIndex);
		}

		bw->Write(glyphHashTable.getCount());
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

			//int compLength;
			//char* compressed = CompressData(g->PixelData, g->Width, g->Height, compLength);
			bw->Write(g->PixelData, g->Width * g->Height);
			//delete[] compressed;
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
			delete[] g->PixelData;
		}


		bw->Close();
		delete bw;

		delete comparer;
		//delete[] passCheck;

		CompileLog::WriteInformation(config.Name, L">");

	}
}
