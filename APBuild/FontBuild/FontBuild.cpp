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
#include "FontBuild.h"

#include "../BuildConfig.h"
#include "../BuildEngine.h"
#include "../CompileLog.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#include "freetype/fttrigon.h"
#include "freetype/ftbitmap.h"

#include "apoc3d/Collections/CollectionsCommon.h"
#include "apoc3d/Config/ConfigurationSection.h"
#include "apoc3d/Config/Configuration.h"
#include "apoc3d/Config/XmlConfigurationFormat.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/ExistTable.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/Vfs/File.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Utility/Hash.h"

using namespace Apoc3D;
using namespace Apoc3D::IO;
using namespace Apoc3D::Collections;
using namespace Apoc3D::VFS;
using namespace Apoc3D::Utility;

#pragma comment (lib, "Gdiplus.lib")

#if _DEBUG
#if APOC3D_STATIC_RT
#pragma comment (lib, "freetype2411MT_D.lib")
#else
#pragma comment (lib, "freetype2411_D.lib")
#endif
#else
#if APOC3D_STATIC_RT
#pragma comment (lib, "freetype2411MT.lib")
#else
#pragma comment (lib, "freetype2411.lib")
#endif
#endif

namespace APBuild
{
	/** 
	 * The font file is made up of the following
	 *	 0. Font metrics
	 *   1. Mapping from each character to the glyphs
	 *   2. Glyph offset info & bitmap data
	 */


	struct GlyphBitmap
	{
		int Index;
		int Width;
		int Height;
		int HashCode;
		const char* PixelData;
		bool HasLuminance;

		GlyphBitmap() { }
		GlyphBitmap(int width, int height, const char* data, bool hasLuminance)
			: Width(width), Height(height), PixelData(data), HasLuminance(hasLuminance)
		{
			FNVHash32 hasher;

			int32 size = width * height;
			if (HasLuminance)
			{
				size *= 2;
			}

			hasher.Accumulate(PixelData, size);

			HashCode = hasher.GetResult();
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


	const int32 FIDXFileID = 'FIDX';

	/** Establish a mapping between a character and a glyph
	*/
	struct CharMapping
	{
		wchar_t Character;
		int GlyphIndex;

		short Left;
		short Top;
		float AdvanceX;
	};
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	
	struct FontRenderInfo
	{
		float Height;
		float LineGap;
		float Ascender;
		float Descender;
		bool HasLuminance;

		bool HasDrawOffset;
		float DrawOffset[2];
	};

	typedef void (*GlyphRenderHandler)(const std::string& fontFile, float fontSize, const FastList<CharRange>& ranges, bool antiAlias,
		FastList<CharMapping>& charMap, HashMap<GlyphBitmap, GlyphBitmap>& glyphHashTable, FontRenderInfo& resultInfo);
	void BuildFont(const FontBuildConfig& config, GlyphRenderHandler renderer);
	void RenderGlyphsByFreeType(const std::string& fontFile, float fontSize, const FastList<CharRange>& ranges, bool antiAlias,
		FastList<CharMapping>& charMap, HashMap<GlyphBitmap, GlyphBitmap>& glyphHashTable, FontRenderInfo& resultInfo);
	void RenderGlyphsByFontMap(const std::string& fontFile, float fontSize, const FastList<CharRange>& ranges, bool antiAlias,
		FastList<CharMapping>& charMap, HashMap<GlyphBitmap, GlyphBitmap>& glyphHashTable, FontRenderInfo& resultInfo);


	void fbBuild(const ConfigurationSection* sect)
	{
		FontBuildConfig config;
		config.Parse(sect);

		EnsureDirectory(PathUtils::GetDirectory(config.DestFile));

		BuildFont(config, RenderGlyphsByFreeType);

		CompileLog::WriteInformation(config.Name, L">");
	}

	void fbBuildGlyphCheck(const ConfigurationSection* sect)
	{
		String srcFile = sect->getAttribute(L"SourceFile");
		String dstFile = sect->getAttribute(L"DestinationFile");

		EnsureDirectory(PathUtils::GetDirectory(dstFile));
		
		FT_Library library;
		if (FT_Init_FreeType( &library )) 
			throw std::runtime_error("FT_Init_FreeType failed");
		
		std::string name = StringUtils::toPlatformNarrowString(srcFile);
		FT_Face face;
		if (FT_New_Face( library, name.c_str(), 0, &face )) 
			throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

		FT_Select_Charmap(face, FT_ENCODING_UNICODE );

		List<std::pair<FT_UInt, FT_UInt>> regions;
		FT_UInt lastIdx = 0;
		for (uint i=0;i<65535;i++)
		{
			FT_UInt idx = FT_Get_Char_Index(face, i);
			
			if (idx == 0)
			{
				if (i - lastIdx > 1)
				{
					// put session
					regions.Add(std::make_pair(lastIdx+1, i-1));
				}
				lastIdx = i;
			}
		}


		FT_Done_Face(face);
		FT_Done_FreeType(library);

		FileOutStream* fs = new FileOutStream(dstFile);
		BinaryWriter* bw = new BinaryWriter(fs);
		
		bw->WriteInt32(regions.getCount());

		for (int i=0;i<regions.getCount();i++)
		{
			bw->WriteUInt32(regions[i].first);
			bw->WriteUInt32(regions[i].second);
		}

		bw->Close();
		delete bw;

		CompileLog::WriteInformation(srcFile, L">");
	}
	void fbBuildToFontMap(const ConfigurationSection* sect)
	{
		FontMapBuildConfig config;
		config.Parse(sect);

		EnsureDirectory(PathUtils::GetDirectory(config.DestFile));
		EnsureDirectory(PathUtils::GetDirectory(config.DestIndexFile));

		GlyphBitmapEqualityComparer comparer;
		FastList<CharMapping> charMap(0xffff);
		HashMap<GlyphBitmap, GlyphBitmap> glyphHashTable(0xffff, &comparer);

		FontRenderInfo info;
		RenderGlyphsByFreeType(StringUtils::toPlatformNarrowString(config.SourceFile), config.Size, config.Ranges, config.AntiAlias, charMap, glyphHashTable, info);

		FileOutStream* fs = new FileOutStream(config.DestIndexFile);
		BinaryWriter* bw = new BinaryWriter(fs);

		bw->WriteInt32(FIDXFileID);
		bw->WriteInt32(0); // version

		bw->WriteInt32(charMap.getCount());
		bw->WriteSingle(info.Height);
		bw->WriteSingle(info.LineGap);
		bw->WriteSingle(-info.Ascender);
		bw->WriteSingle(-info.Descender);
		bw->WriteSingle((float)config.GlyphMargin);
		bw->WriteSingle((float)config.GlyphMargin);

		// char to glyph mapping
		for (int i=0;i<charMap.getCount();i++)
		{
			bw->WriteInt32(charMap[i].Character);
			bw->WriteInt32(charMap[i].GlyphIndex);

			bw->WriteInt16(charMap[i].Left);
			bw->WriteInt16(charMap[i].Top);
			bw->WriteSingle(charMap[i].AdvanceX);
		}

		bw->WriteInt32(glyphHashTable.getCount());


		int maxGWidth = 0;
		int maxGHeight = 0;
		for (HashMap<GlyphBitmap, GlyphBitmap>::Enumerator i = glyphHashTable.GetEnumerator();i.MoveNext();)
		{
			const GlyphBitmap* g = i.getCurrentKey();
			if (g->Width > maxGWidth) maxGWidth = g->Width;
			if (g->Height > maxGHeight) maxGHeight = g->Height;
		}

		int32 cellWidth = maxGWidth + config.GlyphMargin*2;
		int32 cellHeight = maxGHeight + config.GlyphMargin*2;
		int32 edgeCount = (int32)(ceilf(sqrtf((float)glyphHashTable.getCount())) + 0.5f);
		int32 bitMapWdith = edgeCount * cellWidth;
		int32 bitMapHeight = edgeCount * cellHeight;

		Gdiplus::Bitmap packedMap(bitMapWdith, bitMapHeight, PixelFormat32bppARGB);
		
		{
			Gdiplus::BitmapData bmpData;
			Gdiplus::Rect lr( 0, 0, bitMapWdith, bitMapHeight );
			Gdiplus::Status ret = packedMap.LockBits(&lr, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

			int32 xPos = 0;
			int32 yPos = 0;
			for (HashMap<GlyphBitmap, GlyphBitmap>::Enumerator i = glyphHashTable.GetEnumerator();i.MoveNext();)
			{
				const GlyphBitmap* g = i.getCurrentKey();
				bw->WriteInt32(g->Index);
				bw->WriteInt32(g->Width + config.GlyphMargin*2);
				bw->WriteInt32(g->Height + config.GlyphMargin*2);

				int32 regionX = xPos * cellWidth;
				int32 regionY = yPos * cellHeight;
				bw->WriteInt32(regionX);
				bw->WriteInt32(regionY);

				regionX += config.GlyphMargin;
				regionY += config.GlyphMargin;

				char* pixelRowAddr = (char*)bmpData.Scan0 + bmpData.Stride * regionY + regionX * sizeof(uint32);

				for (int i=0;i<g->Height;i++)
				{
					for (int j=0;j<g->Width;j++)
					{
						int32 gray = *(g->PixelData + g->Width * i + j);
						uint32* pa = ((uint32*)pixelRowAddr) + j;

						*pa = (uint32)gray << 24 | 0xffffff;
					}

					pixelRowAddr += bmpData.Stride;
				}
				
				delete[] g->PixelData;

				xPos++;
				if (xPos >= edgeCount)
				{
					xPos = 0;
					yPos++;
				}
			}

			if (config.ShowGrid)
			{
				char* pixelRowAddr = (char*)bmpData.Scan0;

				for (int i=0;i<edgeCount;i++)
				{
					xPos = cellWidth * i - 1;
					yPos = cellHeight * i - 1;

					if (xPos>0)
					{
						uint32* pa = ((uint32*)pixelRowAddr) + xPos;
						for (int j=0;j<bitMapHeight;j++)
						{
							*pa = 0xff000000U;
							pa = (uint32*)(((char*)pa) + bmpData.Stride);
						}
					}
					if (yPos>0)
					{
						uint32* pa = (uint32*)(pixelRowAddr + yPos * bmpData.Stride);
						for (int j=0;j<bitMapWdith;j++)
						{
							*pa++ = 0xff000000U;
						}
					}
					
				}

			}

			packedMap.UnlockBits(&bmpData);
		}
		
		bw->Close();
		delete bw;

		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);
		packedMap.Save(config.DestFile.c_str(), &pngClsid);

		CompileLog::WriteInformation(config.SourceFile, L">");
	}
	void fbBuildFromFontMap(const ConfigurationSection* sect)
	{
		String name = sect->getAttribute(L"Name");
		String destFile = sect->getAttribute(L"DestinationFile");

		EnsureDirectory(PathUtils::GetDirectory(destFile));
		
		FontBuildConfig config;
		config.Name = name;
		config.DestFile = destFile;
		// other params ignored
		BuildFont(config, RenderGlyphsByFontMap);

		CompileLog::WriteInformation(name, L">");
	}

	void BuildFont(const FontBuildConfig& config, GlyphRenderHandler renderer)
	{
		GlyphBitmapEqualityComparer comparer;
		FastList<CharMapping> charMap(0xffff);
		HashMap<GlyphBitmap, GlyphBitmap> glyphHashTable(0xffff, &comparer);

		FontRenderInfo info;
		renderer(StringUtils::toPlatformNarrowString(config.Name), config.Size, config.Ranges, config.AntiAlias, charMap, glyphHashTable, info);

		FileOutStream* fs = new FileOutStream(config.DestFile);
		BinaryWriter* bw = new BinaryWriter(fs);

		bw->WriteUInt32(0xffffff02U); // new version id

		uint32 flags = 0;
		if (info.HasLuminance)
			flags |= 1;
		if (info.HasDrawOffset)
			flags |= 2;

		bw->WriteUInt32(flags);

		bw->WriteInt32(charMap.getCount());
		bw->WriteSingle(info.Height);
		bw->WriteSingle(info.LineGap);
		bw->WriteSingle(-info.Ascender);
		bw->WriteSingle(-info.Descender);
		if (info.HasDrawOffset)
		{
			bw->WriteSingle(info.DrawOffset[0]);
			bw->WriteSingle(info.DrawOffset[1]);
		}

		for (int i=0;i<charMap.getCount();i++)
		{
			bw->WriteInt32(charMap[i].Character);
			bw->WriteInt32(charMap[i].GlyphIndex);

			bw->WriteInt16(charMap[i].Left);
			bw->WriteInt16(charMap[i].Top);
			bw->WriteSingle(charMap[i].AdvanceX);
		}

		bw->WriteInt32(glyphHashTable.getCount());
		int64 glyRecPos = fs->getPosition();
		for (int i=0;i<glyphHashTable.getCount();i++)
		{
			bw->WriteInt32((int32)0);
			bw->WriteInt32((int32)0);
			bw->WriteInt32((int32)0);
			bw->WriteInt64((int64)0);
		}
		int64 baseOfs = fs->getPosition();

		for (HashMap<GlyphBitmap, GlyphBitmap>::Enumerator i = glyphHashTable.GetEnumerator();i.MoveNext();)
		{
			const GlyphBitmap* g = i.getCurrentKey();

			if (info.HasLuminance)
				bw->Write(g->PixelData, g->Width * g->Height * 2);
			else
				bw->Write(g->PixelData, g->Width * g->Height);
		}

		fs->Seek(glyRecPos, SEEK_Begin);

		for (HashMap<GlyphBitmap, GlyphBitmap>::Enumerator i = glyphHashTable.GetEnumerator();i.MoveNext();)
		{
			const GlyphBitmap* g = i.getCurrentKey();

			bw->WriteInt32((int32)g->Index);
			bw->WriteInt32((int32)g->Width);
			bw->WriteInt32((int32)g->Height);
			bw->WriteInt64((int64)baseOfs);

			if (info.HasLuminance)
				baseOfs += g->Width * g->Height * 2;
			else
				baseOfs += g->Width * g->Height;

			delete[] g->PixelData;
		}

		bw->Close();
		delete bw;
	}

	void RenderGlyphsByFreeType(const std::string& fontFile, float fontSize, const FastList<CharRange>& ranges, bool antiAlias,
		FastList<CharMapping>& charMap, HashMap<GlyphBitmap, GlyphBitmap>& glyphHashTable, FontRenderInfo& resultInfo)
	{
		//Create and initialize a freetype font library.
		FT_Library library;
		if (FT_Init_FreeType( &library )) 
			throw std::runtime_error("FT_Init_FreeType failed");

		//The object in which Freetype holds information on a given
		//font is called a "face".
		FT_Face face;

		//This is where we load in the font information from the file.
		//Of all the places where the code might die, this is the most likely,
		//as FT_New_Face will die if the font file does not exist or is somehow broken.
		if (FT_New_Face( library, fontFile.c_str(), 0, &face )) 
			throw std::runtime_error("FT_New_Face failed (there is probably a problem with your font file)");

		FT_Select_Charmap(face, FT_ENCODING_UNICODE );


		//For some twisted reason, Freetype measures font size
		//in terms of 1/64ths of pixels.  Thus, to make a font
		//h pixels high, we need to request a size of h*64.
		//(h << 6 is just a prettier way of writting h*64)
		FT_Set_Char_Size( face, static_cast<int>(fontSize * 64.0f), 0, 96, 96);

		FT_Size_Metrics metrics = face->size->metrics; 

		float ascender = metrics.ascender / 64.f; //(metrics.ascender >> 6) / 100.0f;
		float descender = metrics.descender / 64.f; //(metrics.descender >> 6) / 100.0f;
		float height = metrics.height / 64.f; //(metrics.height >> 6) / 100.0f;
		float lineGap = height - ascender + descender;

		for (int i=0;i<ranges.getCount();i++)
		{
			for (int ch = ranges[i].MinChar; 
				ch <= ranges[i].MaxChar; ch++)
			{
				//Load the Glyph for our character.
				if(FT_Load_Glyph( face, FT_Get_Char_Index( face, (FT_ULong)ch ), FT_LOAD_DEFAULT ))
					throw std::runtime_error("FT_Load_Glyph failed");

				//Move the face's glyph into a Glyph object.
				FT_Glyph glyph;
				if(FT_Get_Glyph( face->glyph, &glyph ))
					throw std::runtime_error("FT_Get_Glyph failed");

				//Convert the glyph to a bitmap.
				if (antiAlias)
					FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, nullptr, 1 );
				else
					FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_MONO, nullptr, 1 );

				FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

				FT_Bitmap tempbitmap;
				if (!antiAlias)
				{
					FT_Bitmap_New(&tempbitmap);
					FT_Error ret = FT_Bitmap_Convert( library, &bitmap_glyph->bitmap, &tempbitmap, 1);
					assert(ret == 0);
				}

				FT_Bitmap& readSource = antiAlias ? bitmap_glyph->bitmap : tempbitmap;

				int height = readSource.rows;
				int width = readSource.width;

				char* buffer = new char[width * height];
				memset(buffer, 0, width*height);


				//assert(sy>=0);

				int srcofs = 0;
				int dstofs = 0;
				for (int k=0;k<readSource.rows;k++)
				{
					for (int j=0;j<readSource.width;j++)
					{
						if (antiAlias)
							buffer[dstofs+j] = readSource.buffer[srcofs + j];
						else
							buffer[dstofs+j] = readSource.buffer[srcofs + j] ? 0xff : 0;
					}

					srcofs += readSource.width;
					dstofs += width;
				}

				if (!antiAlias)
				{
					FT_Bitmap_Done( library, &tempbitmap );
				}

				// check duplicated glyphs using hash table
				// use the previous glyph if a same one already exists
				GlyphBitmap result;
				GlyphBitmap glyphMap(width, height, buffer, false);
				if (!glyphHashTable.TryGetValue(glyphMap, result))
				{
					glyphMap.Index = glyphHashTable.getCount();// index++;
					glyphHashTable.Add(glyphMap, glyphMap);
				}
				else
				{
					glyphMap = result;
					delete[] buffer;
				}

				{
					CharMapping m = { (wchar_t)ch, glyphMap.Index, (short)bitmap_glyph->left, (short)(fontSize-bitmap_glyph->top), (face->glyph->advance.x / 64.0f) };
					charMap.Add(m);
				}
			}
		}


		//We don't need the face information now that the display
		//lists have been created, so we free the assosiated resources.
		FT_Done_Face(face);

		//Ditto for the library.
		FT_Done_FreeType(library);

		resultInfo.Ascender = ascender;
		resultInfo.Descender = descender;
		resultInfo.Height = height;
		resultInfo.LineGap = lineGap;
		resultInfo.HasLuminance = false;
		resultInfo.HasDrawOffset = false;
	}
	
	void RenderGlyphsByFontMap(const std::string& fontFile, float fontSize, const FastList<CharRange>& ranges, bool antiAlias,
		FastList<CharMapping>& charMap, HashMap<GlyphBitmap, GlyphBitmap>& glyphHashTable, FontRenderInfo& resultInfo)
	{
		String mapFile = StringUtils::toPlatformWideString(fontFile) + L".png";
		String idxFile = StringUtils::toPlatformWideString(fontFile) + L".fid";

		Gdiplus::Bitmap* packedMap = Gdiplus::Bitmap::FromFile(mapFile.c_str());
		Gdiplus::BitmapData bmpData;
		Gdiplus::Rect lr( 0, 0, packedMap->GetWidth(), packedMap->GetHeight() );
		Gdiplus::Status ret = packedMap->LockBits(&lr, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

		BinaryReader* br = new BinaryReader(new FileStream(idxFile));

		int32 fid = br->ReadInt32();
		assert(fid == FIDXFileID);

		br->ReadInt32();

		int32 charCount = br->ReadInt32();
		charMap.ReserveDiscard(charCount);

		resultInfo.Height = br->ReadSingle();
		resultInfo.LineGap = br->ReadSingle();
		resultInfo.Ascender = -br->ReadSingle();
		resultInfo.Descender = -br->ReadSingle();
		resultInfo.DrawOffset[0] = br->ReadSingle();
		resultInfo.DrawOffset[1] = br->ReadSingle();

		resultInfo.HasLuminance = true;
		resultInfo.HasDrawOffset = true;

		for (int i=0;i<charCount;i++)
		{
			CharMapping& cm = charMap[i];
			cm.Character = br->ReadInt32();
			cm.GlyphIndex = br->ReadInt32();
			cm.Left = br->ReadInt16();
			cm.Top = br->ReadInt16();
			cm.AdvanceX = br->ReadSingle();
		}

		int32 glyphCount = br->ReadInt32();
		glyphHashTable.Resize(glyphCount);

		for (int k=0;k<glyphCount;k++)
		{
			int32 gbIndex = br->ReadInt32();
			int32 gbWidth = br->ReadInt32();
			int32 gbHeight = br->ReadInt32();
			
			int rx = br->ReadInt32();
			int ry = br->ReadInt32();

			char* pixelData = new char[gbWidth * gbHeight * 2];

			uint16* dest = (uint16*)pixelData;
			const char* pixelRowAddr = (char*)bmpData.Scan0 + bmpData.Stride * ry + rx * sizeof(uint32);

			for (int i=0;i<gbHeight;i++)
			{
				for (int j=0;j<gbWidth;j++)
				{
					uint32 pa = *(((uint32*)pixelRowAddr) + j);
					
					uint32 r = (pa >> 16) & 0xff;
					uint32 g = (pa >> 8) & 0xff;
					uint32 b = pa & 0xff;

					uint32 l = (r + g + b) / 3;

					*(dest + gbWidth * i + j) = (uint16)((pa >> 24)<<8) | (uint16)l;
				}

				pixelRowAddr += bmpData.Stride;
			}


			GlyphBitmap gb(gbWidth, gbHeight, pixelData, true);
			gb.Index = gbIndex;
			glyphHashTable.Add(gb, gb);
		}

		packedMap->UnlockBits(&bmpData);
		delete packedMap;
		delete br;
	}

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

		Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if(size == 0)
			return -1;  // Failure

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
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

	// this is useless
	void BuildByGDIPlus(const FontBuildConfig& config)
	{
		GlyphBitmapEqualityComparer comparer;
		FastList<CharMapping> charMap(0xffff);
		HashMap<GlyphBitmap, GlyphBitmap> glyphHashTable(0xffff, &comparer);
		
		Gdiplus::Bitmap globalBmp(1,1, PixelFormat32bppARGB);
		Gdiplus::Graphics* gg = Gdiplus::Graphics::FromImage(&globalBmp);

		Gdiplus::Font font(config.Name.c_str(), config.Size, config.Style);

		// generate the images for characters as specified in the ranges
		for (int i=0;i<config.Ranges.getCount();i++)
		{
			for (wchar_t ch = (wchar_t)config.Ranges[i].MinChar; 
				ch <= (wchar_t)config.Ranges[i].MaxChar; ch++)
			{
				Gdiplus::RectF size;
				gg->MeasureString(&ch, 1, &font, Gdiplus::PointF(0,0), &size);

				int width = static_cast<int>(ceilf(size.Width));
				int height = static_cast<int>(ceilf(size.Height));

				Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap((INT)width, (INT)height, PixelFormat32bppARGB);

				Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(bitmap);
				if (config.AntiAlias)
					g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
				else
					g->SetTextRenderingHint(Gdiplus::TextRenderingHintSingleBitPerPixel);
				g->Clear(Gdiplus::Color(0));


				Gdiplus::Brush* brush = new Gdiplus::SolidBrush((Gdiplus::ARGB)Gdiplus::Color::White);
				Gdiplus::StringFormat* strFmt = new Gdiplus::StringFormat();
				strFmt->SetAlignment(Gdiplus::StringAlignmentNear);
				strFmt->SetLineAlignment(Gdiplus::StringAlignmentNear);

				g->DrawString(&ch,1,&font,Gdiplus::PointF(0,0), strFmt, brush);


				g->Flush();

				delete g;
				delete brush;
				delete strFmt;

				Gdiplus::BitmapData bmpData;
				Gdiplus::Rect lr = Gdiplus::Rect( 0, 0, width, height );
				Gdiplus::Status ret = bitmap->LockBits(&lr, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

				const char* data = reinterpret_cast<const char*>(bmpData.Scan0);

				// clip the character
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
							altBmp->LockBits(&lr2, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpData2);

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

				// put the data in bitmap to a byte buffer
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

				// check duplicated glyphs using hash table
				// use the previous glyph if a same one already exists
				GlyphBitmap result;
				GlyphBitmap glyph(width, height, buffer, false);
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

		bw->WriteInt32(charMap.getCount());
		for (int i=0;i<charMap.getCount();i++)
		{
			bw->WriteInt32((int32)charMap[i].Character);
			bw->WriteInt32((int32)charMap[i].GlyphIndex);
		}

		bw->WriteInt32(glyphHashTable.getCount());
		int64 glyRecPos = fs->getPosition();
		for (int i=0;i<glyphHashTable.getCount();i++)
		{
			bw->WriteInt32((int32)0);
			bw->WriteInt32((int32)0);
			bw->WriteInt32((int32)0);
			bw->WriteInt64((int64)0);
		}
		int64 baseOfs = fs->getPosition();

		for (HashMap<GlyphBitmap, GlyphBitmap>::Enumerator i = glyphHashTable.GetEnumerator();i.MoveNext();)
		{
			const GlyphBitmap* g = i.getCurrentKey();

			bw->Write(g->PixelData, g->Width * g->Height);
		}

		fs->Seek(glyRecPos, SEEK_Begin);

		for (HashMap<GlyphBitmap, GlyphBitmap>::Enumerator i = glyphHashTable.GetEnumerator();i.MoveNext();)
		{
			const GlyphBitmap* g = i.getCurrentKey();

			bw->WriteInt32((int32)g->Index);
			bw->WriteInt32((int32)g->Width);
			bw->WriteInt32((int32)g->Height);
			bw->WriteInt64((int64)baseOfs);
			baseOfs += g->Width * g->Height;
			delete[] g->PixelData;
		}


		bw->Close();
		delete bw;

	}

}
