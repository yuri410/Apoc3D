#pragma once
#ifndef APOC3D_TEXTDATA_H
#define APOC3D_TEXTDATA_H

#include "apoc3d/Common.h"

namespace Apoc3D
{
	namespace IO
	{
		namespace Encoding
		{
			enum TextEncoding
			{
				TEC_Unknown,
				TEC_UTF8,
				TEC_UTF16LE,
				TEC_UTF16BE,
				TEC_UTF32LE,
				TEC_UTF32BE,
				TEC_ASCII
			};

			TextEncoding APAPI FindEncodingByBOM(const char* bomData, int32 length, int32* bomLength = nullptr);
			inline TextEncoding FindEncodingByBOM(const char* bomData, int32 length, TextEncoding defaultEnc, int32* bomLength = nullptr)
			{
				TextEncoding encoding = FindEncodingByBOM(bomData, length, bomLength);

				if (encoding == TEC_Unknown)
					encoding = defaultEnc;
				return encoding;
			}


			String APAPI ConvertRawData(const char* rawData, int32 length, TextEncoding encoding, bool checkBom);


		}


	}
}

#endif