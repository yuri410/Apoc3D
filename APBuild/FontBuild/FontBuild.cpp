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

#include "BuildConfig.h"
#include "BuildSystem.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#include "freetype/fttrigon.h"
#include "freetype/ftbitmap.h"
#include "apoc3d/Library/lz4.h"

#pragma comment (lib, "Gdiplus.lib")

#if _DEBUG
#if APOC3D_MT
#pragma comment (lib, "freetype2411MT_D.lib")
#else
#pragma comment (lib, "freetype2411_D.lib")
#endif
#else
#if APOC3D_MT
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

			HashCode = static_cast<int32>(hasher.getResult());
		}

		bool operator ==(const GlyphBitmap& y) const
		{
			if (Width == y.Width && Height == y.Height && HashCode == y.HashCode)
			{
				for (int i=0;i<Width*Height;i++)
				{
					if (PixelData[i] != y.PixelData[i])
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

	struct GlyphBitmapEqualityComparer
	{
		static bool Equals(const GlyphBitmap& x, const GlyphBitmap& y) { return (x==y); }
		static int64 GetHashCode(const GlyphBitmap& obj) { return obj.HashCode; }
	};

	typedef HashMap<GlyphBitmap, GlyphBitmap, GlyphBitmapEqualityComparer> GlyphBitmapTable;

	struct GlyphBitmapStoreState
	{
		const GlyphBitmap* GlyphRef;
		int64 Offset;
		int32 CompressedSize;
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

	typedef void (*GlyphRenderHandler)(const std::string& fontFile, float fontSize, const List<CharRange>& ranges, const List<CharRange>& invRanges, bool antiAlias,
		List<CharMapping>& charMap, GlyphBitmapTable& glyphHashTable, FontRenderInfo& resultInfo);
	void BuildFont(const FontBuildConfig& config, GlyphRenderHandler renderer);
	void RenderGlyphsByFreeType(const std::string& fontFile, float fontSize, const List<CharRange>& ranges, const List<CharRange>& invRanges, bool antiAlias,
		List<CharMapping>& charMap, GlyphBitmapTable& glyphHashTable, FontRenderInfo& resultInfo);
	void RenderGlyphsByFontMap(const std::string& fontFile, float fontSize, const List<CharRange>& ranges, const List<CharRange>& invRanges, bool antiAlias,
		List<CharMapping>& charMap, GlyphBitmapTable& glyphHashTable, FontRenderInfo& resultInfo);


	void FontBuild::Build(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		FontBuildConfig config;
		config.Parse(sect);

		if (!File::FileExists(config.SourceFile))
		{
			BuildSystem::LogError(config.SourceFile, L"Could not find source file.");
			return;
		}

		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DestFile));

		BuildFont(config, RenderGlyphsByFreeType);

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}

	void FontBuild::BuildToFontMap(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		FontMapBuildConfig config;
		config.Parse(sect);

		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DestFile));
		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(config.DestIndexFile));

		List<CharMapping> charMap(0xffff);
		GlyphBitmapTable glyphHashTable(0xffff);

		FontRenderInfo info;
		RenderGlyphsByFreeType(StringUtils::toPlatformNarrowString(config.SourceFile), config.Size, config.Ranges, {}, config.AntiAlias, charMap, glyphHashTable, info);

		FileOutStream fs(config.DestIndexFile);
		BinaryWriter bw(&fs, false);

		bw.WriteInt32(FIDXFileID);
		bw.WriteInt32(0); // version

		bw.WriteInt32(charMap.getCount());
		bw.WriteSingle(info.Height);
		bw.WriteSingle(info.LineGap);
		bw.WriteSingle(-info.Ascender);
		bw.WriteSingle(-info.Descender);
		bw.WriteSingle((float)config.GlyphMargin);
		bw.WriteSingle((float)config.GlyphMargin);

		// char to glyph mapping
		for (int i=0;i<charMap.getCount();i++)
		{
			bw.WriteInt32(charMap[i].Character);
			bw.WriteInt32(charMap[i].GlyphIndex);

			bw.WriteInt16(charMap[i].Left);
			bw.WriteInt16(charMap[i].Top);
			bw.WriteSingle(charMap[i].AdvanceX);
		}

		bw.WriteInt32(glyphHashTable.getCount());


		int maxGWidth = 0;
		int maxGHeight = 0;
		for (const GlyphBitmap& g : glyphHashTable.getKeyAccessor())
		{
			if (g.Width > maxGWidth) maxGWidth = g.Width;
			if (g.Height > maxGHeight) maxGHeight = g.Height;
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

			for (const GlyphBitmap& g : glyphHashTable.getKeyAccessor())
			{
				bw.WriteInt32(g.Index);
				bw.WriteInt32(g.Width + config.GlyphMargin*2);
				bw.WriteInt32(g.Height + config.GlyphMargin*2);

				int32 regionX = xPos * cellWidth;
				int32 regionY = yPos * cellHeight;
				bw.WriteInt32(regionX);
				bw.WriteInt32(regionY);

				regionX += config.GlyphMargin;
				regionY += config.GlyphMargin;

				char* pixelRowAddr = (char*)bmpData.Scan0 + bmpData.Stride * regionY + regionX * sizeof(uint32);

				for (int i = 0; i < g.Height; i++)
				{
					for (int j = 0; j < g.Width; j++)
					{
						int32 gray = *(g.PixelData + g.Width * i + j);
						uint32* pa = ((uint32*)pixelRowAddr) + j;

						*pa = (uint32)gray << 24 | 0xffffff;
					}

					pixelRowAddr += bmpData.Stride;
				}
				
				delete[] g.PixelData;

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

		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);
		packedMap.Save(config.DestFile.c_str(), &pngClsid);

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}
	void FontBuild::BuildFromFontMap(const String& hierarchyPath, const ConfigurationSection* sect)
	{
		String sourceFile = sect->getAttribute(L"SourceFile");
		String destFile = sect->getAttribute(L"DestinationFile");

		BuildSystem::EnsureDirectory(PathUtils::GetDirectory(destFile));
		
		FontBuildConfig config;
		config.SourceFile = sourceFile;
		config.DestFile = destFile;
		// other params ignored
		BuildFont(config, RenderGlyphsByFontMap);

		BuildSystem::LogEntryProcessed(config.DestFile, hierarchyPath);
	}

	void BuildFont(const FontBuildConfig& config, GlyphRenderHandler renderer)
	{
		List<CharMapping> charMap(0xffff);
		GlyphBitmapTable glyphHashTable(0xffff);

		FontRenderInfo info;
		renderer(StringUtils::toPlatformNarrowString(config.SourceFile), config.Size, config.Ranges, config.InvRanges, config.AntiAlias, charMap, glyphHashTable, info);

		FileOutStream fs(config.DestFile);
		BinaryWriter bw(&fs, false);

		bw.WriteUInt32(0xffffff02U); // new version id

		uint32 flags = 0;
		if (info.HasLuminance)
			flags |= 1;
		if (info.HasDrawOffset)
			flags |= 2;
		if (config.Compress)
			flags |= 4;

		bw.WriteUInt32(flags);

		bw.WriteInt32(charMap.getCount());
		bw.WriteSingle(info.Height);
		bw.WriteSingle(info.LineGap);
		bw.WriteSingle(-info.Ascender);
		bw.WriteSingle(-info.Descender);
		if (info.HasDrawOffset)
		{
			bw.WriteSingle(info.DrawOffset[0]);
			bw.WriteSingle(info.DrawOffset[1]);
		}

		for (const auto& ch : charMap)
		{
			bw.WriteInt32(ch.Character);
			bw.WriteInt32(ch.GlyphIndex);

			bw.WriteInt16(ch.Left);
			bw.WriteInt16(ch.Top);
			bw.WriteSingle(ch.AdvanceX);
		}

		bw.WriteInt32(glyphHashTable.getCount());
		int64 glyRecPos = fs.getPosition();
		for (int i=0;i<glyphHashTable.getCount();i++)
		{
			if (config.Compress)
			{
				bw.WriteUInt16(0);
				bw.WriteUInt16(0);
				bw.WriteUInt16(0);
				bw.WriteUInt32(0);
				bw.WriteUInt32(0);
			}
			else
			{
				bw.WriteInt32(0);
				bw.WriteInt32(0);
				bw.WriteInt32(0);
				bw.WriteInt64(0);
			}	
		}
		
		List<GlyphBitmapStoreState> glyphList(glyphHashTable.getCount());
		for (const GlyphBitmap& g : glyphHashTable.getKeyAccessor())
		{
			glyphList.Add({ &g, 0 });
		}

		for (GlyphBitmapStoreState& gr : glyphList)
		{
			gr.Offset = fs.getPosition();
			gr.CompressedSize = 0;

			const GlyphBitmap& g = *gr.GlyphRef;
			const int32 srcDataSize = g.Width * g.Height * (info.HasLuminance ? 2 : 1);

			if (config.Compress)
			{
				char* compressedData = new char[LZ4_compressBound(srcDataSize)];
				int32 compressedSize = LZ4_compressHC2(g.PixelData, compressedData, srcDataSize, 16);

				bw.WriteBytes(compressedData, compressedSize);

				delete[] compressedData;

				gr.CompressedSize = compressedSize;
			}
			else
			{
				bw.WriteBytes(g.PixelData, srcDataSize);
			}
		}

		fs.Seek(glyRecPos, SeekMode::Begin);

		for (GlyphBitmapStoreState& gr : glyphList)
		{
			const GlyphBitmap& g = *gr.GlyphRef;
			if (config.Compress)
			{
				bw.WriteUInt16(g.Index);
				bw.WriteUInt16(g.Width);
				bw.WriteUInt16(g.Height);
				bw.WriteUInt32((uint32)gr.Offset);
				bw.WriteUInt32(gr.CompressedSize);
			}
			else
			{
				bw.WriteInt32(g.Index);
				bw.WriteInt32(g.Width);
				bw.WriteInt32(g.Height);
				bw.WriteInt64(gr.Offset);
			}

			delete[] g.PixelData;
		}
	}

	void RenderGlyphsByFreeType(FT_Library& library, FT_Face& face, int ch, float fontSize, bool antiAlias,
		List<CharMapping>& charMap, GlyphBitmapTable& glyphHashTable)
	{
		//Load the Glyph for our character.
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, (FT_ULong)ch), FT_LOAD_DEFAULT))
			throw std::runtime_error("FT_Load_Glyph failed");

		//Move the face's glyph into a Glyph object.
		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph))
			throw std::runtime_error("FT_Get_Glyph failed");

		//Convert the glyph to a bitmap.
		if (antiAlias)
			FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1);
		else
			FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, nullptr, 1);

		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		FT_Bitmap tempbitmap;
		if (!antiAlias)
		{
			FT_Bitmap_New(&tempbitmap);
			FT_Error ret = FT_Bitmap_Convert(library, &bitmap_glyph->bitmap, &tempbitmap, 1);
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
		for (int k = 0; k < readSource.rows; k++)
		{
			for (int j = 0; j < readSource.width; j++)
			{
				if (antiAlias)
					buffer[dstofs + j] = readSource.buffer[srcofs + j];
				else
					buffer[dstofs + j] = readSource.buffer[srcofs + j] ? 0xff : 0;
			}

			srcofs += readSource.width;
			dstofs += width;
		}

		if (!antiAlias)
		{
			FT_Bitmap_Done(library, &tempbitmap);
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
			CharMapping m = { (wchar_t)ch, glyphMap.Index, (short)bitmap_glyph->left, (short)(fontSize - bitmap_glyph->top), (face->glyph->advance.x / 64.0f) };
			charMap.Add(m);
		}
	}

	void RenderGlyphsByFreeType(const std::string& fontFile, float fontSize, const List<CharRange>& ranges, const List<CharRange>& invRanges, bool antiAlias,
		List<CharMapping>& charMap, GlyphBitmapTable& glyphHashTable, FontRenderInfo& resultInfo)
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

		if (ranges.getCount())
		{
			for (const CharRange& cr : ranges)
			{
				for (uint32 ch = cr.MinChar; ch <= cr.MaxChar; ch++)
				{
					RenderGlyphsByFreeType(library, face, ch, fontSize, antiAlias, charMap, glyphHashTable);
				}
			}
		}
		else
		{
			uint counter = 0;
			FT_UInt lastIdx = 0;
			for (uint ch = 0; ch <= 65535; ch++)
			{
				bool isIgnored = false;
				for (const CharRange& cr : invRanges)
				{
					if (ch >= cr.MinChar && ch <= cr.MaxChar)
					{
						isIgnored = true;
						break;
					}
				}

				if (isIgnored)
					continue;

				FT_UInt idx = FT_Get_Char_Index(face, ch);
				if (idx)
				{ 
					RenderGlyphsByFreeType(library, face, ch, fontSize, antiAlias, charMap, glyphHashTable);
					counter++;
				}
			}
			BuildSystem::LogInformation(L"Automatically detected " + StringUtils::UIntToString(counter) 
				+ L" characters. Compiled to " + StringUtils::IntToString(glyphHashTable.getCount()) + L" glyphs.", L"");
		}


		//We don't need the face information now that the display
		//lists have been created, so we free the associated resources.
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
	
	void RenderGlyphsByFontMap(const std::string& fontFile, float fontSize, const List<CharRange>& ranges, const List<CharRange>& invRanges, bool antiAlias,
		List<CharMapping>& charMap, GlyphBitmapTable& glyphHashTable, FontRenderInfo& resultInfo)
	{
		String mapFile = StringUtils::toPlatformWideString(fontFile) + L".png";
		String idxFile = StringUtils::toPlatformWideString(fontFile) + L".fid";

		Gdiplus::Bitmap* packedMap = Gdiplus::Bitmap::FromFile(mapFile.c_str());
		Gdiplus::BitmapData bmpData;
		Gdiplus::Rect lr( 0, 0, packedMap->GetWidth(), packedMap->GetHeight() );
		Gdiplus::Status ret = packedMap->LockBits(&lr, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

		FileStream fs(idxFile);
		BinaryReader br(&fs, false);

		int32 fid = br.ReadInt32();
		assert(fid == FIDXFileID);

		br.ReadInt32();

		int32 charCount = br.ReadInt32();
		charMap.ReserveDiscard(charCount);

		resultInfo.Height = br.ReadSingle();
		resultInfo.LineGap = br.ReadSingle();
		resultInfo.Ascender = -br.ReadSingle();
		resultInfo.Descender = -br.ReadSingle();
		resultInfo.DrawOffset[0] = br.ReadSingle();
		resultInfo.DrawOffset[1] = br.ReadSingle();

		resultInfo.HasLuminance = true;
		resultInfo.HasDrawOffset = true;

		for (int i=0;i<charCount;i++)
		{
			CharMapping& cm = charMap[i];
			cm.Character = br.ReadInt32();
			cm.GlyphIndex = br.ReadInt32();
			cm.Left = br.ReadInt16();
			cm.Top = br.ReadInt16();
			cm.AdvanceX = br.ReadSingle();
		}

		int32 glyphCount = br.ReadInt32();
		glyphHashTable.Resize(glyphCount);

		for (int k=0;k<glyphCount;k++)
		{
			int32 gbIndex = br.ReadInt32();
			int32 gbWidth = br.ReadInt32();
			int32 gbHeight = br.ReadInt32();
			
			int rx = br.ReadInt32();
			int ry = br.ReadInt32();

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
}
