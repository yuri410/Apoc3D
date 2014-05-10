#include "TextData.h"
#include "apoc3d/Library/ConvertUTF.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/IOUtils.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		namespace Encoding
		{
			TextEncoding FindEncodingByBOM(const char* bomData, int32 length, int32* bomLength)
			{
				// For more info, check:
				//  http://en.wikipedia.org/wiki/Byte_order_mark#Representations_of_byte_order_marks_by_encoding

				const byte* data = (const byte*)bomData;

				if (length >= 2)
				{
					if (data[0] == 0xFE && data[1] == 0xFF)
					{
						if (bomLength) *bomLength = 2;
						return TEC_UTF16BE;
					}

					if (data[1] == 0xFE && data[0] == 0xFF)
					{
						if (bomLength) *bomLength = 2;
						return TEC_UTF16LE;
					}
				}

				if (length >= 3)
				{
					if (data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
					{
						if (bomLength) *bomLength = 3;
						return TEC_UTF8;
					}
				}

				if (length >= 4)
				{
					if (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0xFE && data[3] == 0xFF)
					{
						if (bomLength) *bomLength = 4;
						return TEC_UTF32BE;
					}

					if (data[0] == 0xFF && data[1] == 0xFE && data[2] == 0x00 && data[3] == 0x00)
					{
						if (bomLength) *bomLength = 4;
						return TEC_UTF32LE;
					}
				}

				if (bomLength) *bomLength = 0;

				return TEC_Unknown;
			}

			String ConvertRawData(const char* rawData, int32 length, TextEncoding encoding, bool checkBom)
			{
				// skip BOM
				if (checkBom || encoding == TEC_Unknown)
				{
					int32 bomLength;
					encoding = FindEncodingByBOM(rawData, length, TEC_ASCII, &bomLength);

					rawData += bomLength;
					length -= bomLength;
				}


				const bool platformLittleEndian = isPlatformLittleEndian();


				if (encoding == TEC_UTF8)
				{
					return StringUtils::UTF8toUTF16(rawData);
				}
				else if (encoding == TEC_UTF16LE || encoding == TEC_UTF16BE)
				{
					const wchar_t* utf16Text = reinterpret_cast<const wchar_t*>(rawData);

					if ((platformLittleEndian && encoding == TEC_UTF16LE) || (!platformLittleEndian && encoding == TEC_UTF16BE))
					{
						return String(utf16Text);
					}

				
					String result;
					result.reserve(length/2+1);

					for (int32 i=0;i<length;i+=2)
					{
						result.append(1, (wchar_t)_byteswap_ushort(utf16Text[i]));
					}

					return result;
				}
				else if (encoding == TEC_UTF32LE || encoding == TEC_UTF32BE)
				{
					const char32_t* utf32Text = reinterpret_cast<const char32_t*>(rawData);

					if ((platformLittleEndian && encoding == TEC_UTF32LE) || (!platformLittleEndian && encoding == TEC_UTF32BE))
					{
						return StringUtils::UTF32toUTF16(utf32Text);
					}

					String32 swapped;
					swapped.reserve(length/4+1);

					for (int32 i=0;i<length;i+=2)
					{
						swapped.append(1, (char32_t)_byteswap_ulong(utf32Text[i]));
					}

					return StringUtils::UTF32toUTF16(swapped);
				}

				String asciiResult;
				asciiResult.reserve(length);
				for (int32 i=0;i<length;i++)
					asciiResult[i] = rawData[i];
				return asciiResult;
			}

		}
	}
}