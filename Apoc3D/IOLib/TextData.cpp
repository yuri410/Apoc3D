#include "TextData.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/IOUtils.h"
#include "apoc3d/Library/ConvertUTF.h"
#include "apoc3d/Utility/StringUtils.h"


using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace IO
	{
		namespace Encoding
		{
			// For more info on BOM, check:
			//  http://en.wikipedia.org/wiki/Byte_order_mark#Representations_of_byte_order_marks_by_encoding

			static const byte utf8BOM[] = { 0xEF, 0xBB, 0xBF };
			static const byte utf16leBOM[] = { 0xFF, 0xFE };
			static const byte utf16beBOM[] = { 0xFE, 0xFF };
			static const byte utf32leBOM[] = { 0xFF, 0xFE, 0x00, 0x00 };
			static const byte utf32beBOM[] = { 0x00, 0x00, 0xFE, 0xFF };

			struct BomType
			{
				const TextEncoding encodingType;
				const byte* bom;
				const int bomLength;

				template <int32 N>
				BomType(TextEncoding enc, const byte(&b)[N])
					: encodingType(enc), bom(b), bomLength(N) { }
			};

			static const BomType bomList[] =
			{
				{ TEC_UTF8, utf8BOM },
				{ TEC_UTF16LE, utf16leBOM },
				{ TEC_UTF16BE, utf16beBOM },
				{ TEC_UTF32LE, utf32leBOM },
				{ TEC_UTF32BE, utf32beBOM }
			};

			TextEncoding FindEncodingByBOM(const char* bomData, int32 length, int32* bomLength)
			{
				const byte* data = (const byte*)bomData;

				for (const BomType& bt : bomList)
				{
					if (bt.bomLength <= length && memcmp(bt.bom, data, bt.bomLength) == 0)
					{
						if (bomLength) *bomLength = bt.bomLength;
						return bt.encodingType;
					}
				}

				if (bomLength) *bomLength = 0;

				return TEC_Unknown;
			}

			TextEncoding FindEncodingByBOM(const char* bomData, int32 length, TextEncoding defaultEnc, int32* bomLength)
			{
				TextEncoding encoding = FindEncodingByBOM(bomData, length, bomLength);

				if (encoding == TEC_Unknown)
					encoding = defaultEnc;
				return encoding;
			}

			const char* GetEncodingBOM(TextEncoding enc, int32& length)
			{
				length = 0;

				switch (enc)
				{
				case Apoc3D::IO::Encoding::TEC_UTF8:
					{
						length = 3;
						return (char*)utf8BOM;
					}
				case Apoc3D::IO::Encoding::TEC_UTF16LE:
					{
						length = 2;
						return (char*)utf16leBOM;
					}
				case Apoc3D::IO::Encoding::TEC_UTF16BE:
					{
						length = 2;
						return (char*)utf16beBOM;
					}
				case Apoc3D::IO::Encoding::TEC_UTF32LE:
					{
						length = 4;
						return (char*)utf32leBOM;
					}
				case Apoc3D::IO::Encoding::TEC_UTF32BE:
					{
						length = 4;
						return (char*)utf32beBOM;
					}
				default:
					return "";
				}
			}



			String ConvertFromRawData(const char* rawData, int32 length, TextEncoding encoding, bool checkBom)
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

					int32 utf16Len = length/2;
					String result;
					result.reserve(utf16Len+1);

					for (int32 i=0;i<utf16Len;i++)
					{
						result.append(1,(wchar_t)_byteswap_ushort(utf16Text[i]));
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

					int32 utf32Len = length/4;
					String32 swapped;
					swapped.reserve(utf32Len+1);

					for (int32 i=0;i<utf32Len;i++)
					{
						swapped.append(1,(char32_t)_byteswap_ulong(utf32Text[i]));
					}

					return StringUtils::UTF32toUTF16(swapped);
				}

				String asciiResult;
				asciiResult.reserve(length);
				for (int32 i=0;i<length;i++)
					asciiResult.append(1,rawData[i]);
				return asciiResult;
			}

			void ConvertToRawData(const String& text, TextEncoding encoding, bool includeBom, char*& dest, int32& destLength)
			{
				MemoryOutStream dataBuffer(text.size() * 3);
				
				ConvertToRawData(text, encoding, includeBom, dataBuffer);

				destLength = (int32)dataBuffer.getLength();
				dest = new char[destLength];
				memcpy(dest, dataBuffer.getDataPointer(), destLength);
			}

			void ConvertToRawData(const String& text, TextEncoding encoding, bool includeBom, Stream& dest)
			{
				assert(encoding != TEC_Unknown);

				if (includeBom)
				{
					int32 bomLen;
					const char* bom = GetEncodingBOM(Encoding::TEC_UTF8, bomLen);

					if (bomLen > 0)
						dest.Write(bom, bomLen);
				}

				const bool platformLittleEndian = isPlatformLittleEndian();

				switch (encoding)
				{
					case TEC_UTF8:
					{
						std::string utf8str = StringUtils::UTF16toUTF8(text);
						dest.Write(utf8str.c_str(), utf8str.size());
						break;
					}
					case TEC_UTF16BE:
					case TEC_UTF16LE:
					{
						if ((platformLittleEndian && encoding == TEC_UTF16LE) || (!platformLittleEndian && encoding == TEC_UTF16BE))
						{
							dest.Write((const char*)text.c_str(), text.size() * sizeof(char16_t));
						}
						else
						{
							for (size_t i = 0; i < text.size(); i++)
							{
								char buf[sizeof(char16_t)];
								ui16tomb_le(text[i], buf);
								dest.Write(buf, sizeof(char16_t));
							}
						}
						break;
					}
					case TEC_UTF32LE:
					case TEC_UTF32BE:
					{
						String32 utf32Str = StringUtils::UTF16toUTF32(text);
						if ((platformLittleEndian && encoding == TEC_UTF32LE) || (!platformLittleEndian && encoding == TEC_UTF32BE))
						{
							dest.Write((const char*)utf32Str.c_str(), utf32Str.size() * sizeof(char32_t));
						}
						else
						{
							for (size_t i = 0; i < utf32Str.size(); i++)
							{
								char buf[sizeof(char32_t)];
								ui32tomb_le(text[i], buf);
								dest.Write(buf, sizeof(char32_t));
							}
						}
						break;
					}
					case TEC_ASCII:
					{
						for (size_t i = 0; i < text.size(); i++)
							dest.WriteByte((char)text[i]);
						break;
					}
					default:
						assert(0);
						break;
				}
			}


			String ReadAllText(const VFS::ResourceLocation& rl, TextEncoding encoding)
			{
				BinaryReader br(rl);

				int32 length = (int32)br.getBaseStream()->getLength();
				char* rawBytes = new char[length + 1];
				rawBytes[length] = 0;
				br.ReadBytes(rawBytes, length);

				String alltext = ConvertFromRawData(rawBytes, length, encoding, true);

				delete[] rawBytes;
				br.Close();

				return alltext;
			}
			void WriteAllText(Stream& strm, const String& text, TextEncoding encoding)
			{
				assert(encoding != TEC_Unknown);

				ConvertToRawData(text, encoding, true, strm);
			}
		}
	}
}