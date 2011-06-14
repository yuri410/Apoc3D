#include "FontBuild.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include "Config/ConfigurationSection.h"
#include "Collections/FastList.h"
#include "IOLib/Streams.h"
#include "IOLib/BinaryWriter.h"

using namespace Apoc3D;
using namespace Apoc3D::IO;
using namespace Apoc3D::Collections;
using namespace Gdiplus;

#pragma comment (lib,"Gdiplus.lib")

namespace APBuild
{
	struct CharRange
	{
		int MinChar;
		int MaxChar;

	};
	struct FontConfig
	{
		FastList<CharRange> Ranges;
		FontStyle Style;
		String Name;
		float Size;

		String DestFile;

		void Parse(const ConfigurationSection* sect)
		{

		}
	};

	struct GlyphBitmap
	{
		int Width;
		int Height;
		int64 HashCode;
		char* PixelData;
	};

	void FontBuild::Build(const ConfigurationSection* sect)
	{
		FontConfig config;
		config.Parse(sect);

		

		pair<int, int>* sizeTable = new pair<int, int>[0xffff];
		memset(sizeTable,0,0xffff*sizeof(pair<int,int>));

		FileOutStream* fs = new FileOutStream(config.DestFile);
		BinaryWriter* bw = new BinaryWriter(fs);

		Bitmap globalBmp(1,1, PixelFormat32bppARGB);
		Gdiplus::Graphics* gg = Gdiplus::Graphics::FromImage(&globalBmp);

		Font font(config.Name.c_str(), config.Size, config.Style);

		int count = 0;
		bw->Write(count);
		int64 offset = 0;
		

		for (int i=0;i<config.Ranges.getCount();i++)
		{
			for (wchar_t ch = config.Ranges[i].MinChar; ch<=config.Ranges[i].MaxChar; ch++)
			{
				RectF size;
				gg->MeasureString(&ch, 1, &font, Gdiplus::PointF(0,0), &size);

				int width = ceilf(size.Width);
				int height = ceilf(size.Height);

				sizeTable[ch].first = width;
				sizeTable[ch].second = height;

				bw->Write(ch);
				bw->Write((int32)width);
				bw->Write((int32)height);
				bw->Write(offset);

				count++;
				offset+= width * height;
			}
		}

		offset = fs->getPosition();
		fs->Seek(sizeof(count), SEEK_Begin);
		for (int i=0;i<config.Ranges.getCount();i++)
		{
			for (wchar_t ch = config.Ranges[i].MinChar; ch<=config.Ranges[i].MaxChar; ch++)
			{
				int width = sizeTable[ch].first;
				int height = sizeTable[ch].second;

				bw->Write(ch);
				bw->Write((int32)width);
				bw->Write((int32)height);
				bw->Write(offset);

				offset += width * height;
			}
		}

		fs->Seek(0, SEEK_Begin);
		bw->Write(count);

		for (int i=0;i<config.Ranges.getCount();i++)
		{
			for (wchar_t ch = config.Ranges[i].MinChar; ch<=config.Ranges[i].MaxChar; ch++)
			{
				int width = sizeTable[ch].first;
				int height = sizeTable[ch].second;

				Bitmap bitmap((INT)width, (INT)height, PixelFormat32bppARGB);

				Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(&bitmap);
				g->SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
				g->Clear(Gdiplus::Color(0));


				Brush* brush = new SolidBrush(Gdiplus::Color::White);
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

				const char* data = reinterpret_cast<const char*>(bmpData.Scan0);
				
				bw->Write(data, width * height);

				bitmap.UnlockBits(&bmpData);

			}
		}

		bw->Write(count);


		bw->Close();
		delete bw;
		delete fs;
	}
}