/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D

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
#include "StringUtils.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Library/ConvertUTF.h"
#include "apoc3d/Math/Math.h"
#include <strstream>
#include <sstream>
#include <algorithm>

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace Utility
	{
		using nstring = std::string;
		using std::wstring;

		const String StringUtils::Empty;


		nstring StringUtils::toPlatformNarrowString(const String& str) { return toPlatformNarrowString(str.c_str()); }
		nstring StringUtils::toPlatformNarrowString(const wchar_t* str)
		{
			size_t bufSize = wcstombs(nullptr, str, 0);
			if (bufSize != static_cast<size_t>(-1))
			{
				char* buffer = new char[bufSize+1];
				buffer[bufSize] = 0;
				wcstombs(buffer, str, bufSize);

				nstring result = buffer;
				delete[] buffer;
				return result;
			}
			return nstring();
		}
		String StringUtils::toPlatformWideString(const nstring& str) { return toPlatformWideString(str.c_str()); }
		String StringUtils::toPlatformWideString(const char* str)
		{
			size_t bufSize = mbstowcs(nullptr, str, 0);
			if (bufSize != static_cast<size_t>(-1))
			{
				wchar_t* buffer = new wchar_t[bufSize+1];
				buffer[bufSize] = 0;
				mbstowcs(buffer, str, bufSize);

				String result = buffer;
				delete[] buffer;
				return result;
			}
			return L"";
		}

		nstring StringUtils::toASCIINarrowString(const String& str) { return nstring(str.begin(), str.end()); }
		String StringUtils::toASCIIWideString(const nstring& str) { return String(str.begin(), str.end()); }


		String StringUtils::UTF8toUTF16(const nstring& utf8)
		{
			size_t utf16MaxLength = utf8.length()+1;
			UTF16* resultBuffer = new UTF16[utf16MaxLength];
			memset(resultBuffer, 0, sizeof(UTF16) * utf16MaxLength);

			const UTF8* sourceStart = (const UTF8*)utf8.c_str();
			const UTF8* sourceEnd = sourceStart + utf8.length();
			UTF16* targetStart = resultBuffer;
			UTF16* targetEnd = targetStart + utf16MaxLength;

			ConvertUTF8toUTF16(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

			String result((const wchar_t*)resultBuffer);

			delete[] resultBuffer;

			return result;
		}
		nstring StringUtils::UTF16toUTF8(const String& utf16)
		{
			size_t utf8MaxLength = 4 * utf16.size()+1;
			UTF8* byteBuffer = new UTF8[utf8MaxLength];
			memset(byteBuffer, 0, sizeof(UTF8) * utf8MaxLength);

			const UTF16* sourcestart = reinterpret_cast<const UTF16*>(utf16.c_str());
			const UTF16* sourceend = sourcestart + utf16.size();
			UTF8* targetstart = byteBuffer;
			UTF8* targetend = targetstart + utf8MaxLength;

			ConvertUTF16toUTF8(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);
			
			nstring result((const char*)byteBuffer);

			delete[] byteBuffer;

			return result;
		}

		String32 StringUtils::UTF8toUTF32(const nstring& utf8)
		{
			size_t utf32MaxLength = utf8.length()+1;
			UTF32* resultBuffer = new UTF32[utf32MaxLength];
			memset(resultBuffer, 0, sizeof(UTF32) * utf32MaxLength);

			const UTF8* sourceStart = (const UTF8*)utf8.c_str();
			const UTF8* sourceEnd = sourceStart + utf8.length();
			UTF32* targetStart = resultBuffer;
			UTF32* targetEnd = targetStart + utf32MaxLength;

			ConvertUTF8toUTF32(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

			String32 result((const char32_t*)resultBuffer);

			delete[] resultBuffer;

			return result;
		}
		nstring StringUtils::UTF32toUTF8(const String32& utf32)
		{
			size_t utf8MaxLength = 4 * utf32.size()+1;
			UTF8* byteBuffer = new UTF8[utf8MaxLength];
			memset(byteBuffer, 0, sizeof(UTF8) * utf8MaxLength);

			const UTF32* sourcestart = reinterpret_cast<const UTF32*>(utf32.c_str());
			const UTF32* sourceend = sourcestart + utf32.size();
			UTF8* targetstart = byteBuffer;
			UTF8* targetend = targetstart + utf8MaxLength;

			ConvertUTF32toUTF8(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);

			nstring result((const char*)byteBuffer);

			delete[] byteBuffer;

			return result;
		}

		String32 StringUtils::UTF16toUTF32(const String& utf16)
		{
			size_t utf16MaxLength = utf16.length()+1;
			UTF32* resultBuffer = new UTF32[utf16MaxLength];
			memset(resultBuffer, 0, sizeof(UTF32) * utf16MaxLength);

			const UTF16* sourceStart = (const UTF16*)utf16.c_str();
			const UTF16* sourceEnd = sourceStart + utf16.length();
			UTF32* targetStart = resultBuffer;
			UTF32* targetEnd = targetStart + utf16MaxLength;

			ConvertUTF16toUTF32(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

			String32 result((const char32_t*)resultBuffer);

			delete[] resultBuffer;

			return result;
		}
		String StringUtils::UTF32toUTF16(const String32& utf32)
		{
			size_t utf16MaxLength = 2 * utf32.size()+1;
			UTF16* byteBuffer = new UTF16[utf16MaxLength];
			memset(byteBuffer, 0, sizeof(UTF16) * utf16MaxLength);

			const UTF32* sourcestart = reinterpret_cast<const UTF32*>(utf32.c_str());
			const UTF32* sourceend = sourcestart + utf32.size();
			UTF16* targetstart = byteBuffer;
			UTF16* targetend = targetstart + utf16MaxLength;

			ConvertUTF32toUTF16(&sourcestart, sourceend, &targetstart, targetend, lenientConversion);

			String result((const wchar_t*)byteBuffer);

			delete[] byteBuffer;

			return result;
		}

		//////////////////////////////////////////////////////////////////////////

		inline byte GetFP(uint64 flags)
		{
			return (byte)(0xff & ((flags & StringUtils::SF_FPCustomPrecisionMask) >> 12));
		}

		inline uint16 GetWidth(uint64 flags)
		{
			return (uint16)(0xfff & ((flags & StringUtils::SF_WidthMask) >> 20));
		}

		inline char32_t GetFill(uint64 flags)
		{
			if (flags & StringUtils::SF_CustomFillChar)
				return (char32_t)(0xffffff & ((flags & StringUtils::SF_FillCharMask) >> 32));
			return ' ';
		}
		//////////////////////////////////////////////////////////////////////////

		namespace Impl
		{
			const char digitCharTableU[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
			const char digitCharTableL[] = "0123456789abcdefghijklmnopqrstuvwxyz";

			const double digitBase10Mult[618] =
			{
				1e-308, 1e-307, 1e-306, 1e-305, 1e-304, 1e-303, 1e-302, 1e-301, 1e-300,
				1e-299, 1e-298, 1e-297, 1e-296, 1e-295, 1e-294, 1e-293, 1e-292, 1e-291, 1e-290, 1e-289, 1e-288, 1e-287, 1e-286, 1e-285, 1e-284, 1e-283, 1e-282, 1e-281, 1e-280,
				1e-279, 1e-278, 1e-277, 1e-276, 1e-275, 1e-274, 1e-273, 1e-272, 1e-271, 1e-270, 1e-269, 1e-268, 1e-267, 1e-266, 1e-265, 1e-264, 1e-263, 1e-262, 1e-261, 1e-260,
				1e-259, 1e-258, 1e-257, 1e-256, 1e-255, 1e-254, 1e-253, 1e-252, 1e-251, 1e-250, 1e-249, 1e-248, 1e-247, 1e-246, 1e-245, 1e-244, 1e-243, 1e-242, 1e-241, 1e-240,
				1e-239, 1e-238, 1e-237, 1e-236, 1e-235, 1e-234, 1e-233, 1e-232, 1e-231, 1e-230, 1e-229, 1e-228, 1e-227, 1e-226, 1e-225, 1e-224, 1e-223, 1e-222, 1e-221, 1e-220,
				1e-219, 1e-218, 1e-217, 1e-216, 1e-215, 1e-214, 1e-213, 1e-212, 1e-211, 1e-210, 1e-209, 1e-208, 1e-207, 1e-206, 1e-205, 1e-204, 1e-203, 1e-202, 1e-201, 1e-200,
				1e-199, 1e-198, 1e-197, 1e-196, 1e-195, 1e-194, 1e-193, 1e-192, 1e-191, 1e-190, 1e-189, 1e-188, 1e-187, 1e-186, 1e-185, 1e-184, 1e-183, 1e-182, 1e-181, 1e-180,
				1e-179, 1e-178, 1e-177, 1e-176, 1e-175, 1e-174, 1e-173, 1e-172, 1e-171, 1e-170, 1e-169, 1e-168, 1e-167, 1e-166, 1e-165, 1e-164, 1e-163, 1e-162, 1e-161, 1e-160,
				1e-159, 1e-158, 1e-157, 1e-156, 1e-155, 1e-154, 1e-153, 1e-152, 1e-151, 1e-150, 1e-149, 1e-148, 1e-147, 1e-146, 1e-145, 1e-144, 1e-143, 1e-142, 1e-141, 1e-140,
				1e-139, 1e-138, 1e-137, 1e-136, 1e-135, 1e-134, 1e-133, 1e-132, 1e-131, 1e-130, 1e-129, 1e-128, 1e-127, 1e-126, 1e-125, 1e-124, 1e-123, 1e-122, 1e-121, 1e-120,
				1e-119, 1e-118, 1e-117, 1e-116, 1e-115, 1e-114, 1e-113, 1e-112, 1e-111, 1e-110, 1e-109, 1e-108, 1e-107, 1e-106, 1e-105, 1e-104, 1e-103, 1e-102, 1e-101, 1e-100,
				1e-99, 1e-98, 1e-97, 1e-96, 1e-95, 1e-94, 1e-93, 1e-92, 1e-91, 1e-90, 1e-89, 1e-88, 1e-87, 1e-86, 1e-85, 1e-84, 1e-83, 1e-82, 1e-81, 1e-80,
				1e-79, 1e-78, 1e-77, 1e-76, 1e-75, 1e-74, 1e-73, 1e-72, 1e-71, 1e-70, 1e-69, 1e-68, 1e-67, 1e-66, 1e-65, 1e-64, 1e-63, 1e-62, 1e-61, 1e-60,
				1e-59, 1e-58, 1e-57, 1e-56, 1e-55, 1e-54, 1e-53, 1e-52, 1e-51, 1e-50, 1e-49, 1e-48, 1e-47, 1e-46, 1e-45, 1e-44, 1e-43, 1e-42, 1e-41, 1e-40,
				1e-39, 1e-38, 1e-37, 1e-36, 1e-35, 1e-34, 1e-33, 1e-32, 1e-31, 1e-30, 1e-29, 1e-28, 1e-27, 1e-26, 1e-25, 1e-24, 1e-23, 1e-22, 1e-21, 1e-20,
				1e-19, 1e-18, 1e-17, 1e-16, 1e-15, 1e-14, 1e-13, 1e-12, 1e-11, 1e-10, 1e-9, 1e-8, 1e-7, 1e-6, 1e-5, 1e-4, 1e-3, 1e-2, 1e-1, 1e+0,
				1e+1, 1e+2, 1e+3, 1e+4, 1e+5, 1e+6, 1e+7, 1e+8, 1e+9, 1e+10, 1e+11, 1e+12, 1e+13, 1e+14, 1e+15, 1e+16, 1e+17, 1e+18, 1e+19, 1e+20,
				1e+21, 1e+22, 1e+23, 1e+24, 1e+25, 1e+26, 1e+27, 1e+28, 1e+29, 1e+30, 1e+31, 1e+32, 1e+33, 1e+34, 1e+35, 1e+36, 1e+37, 1e+38, 1e+39, 1e+40,
				1e+41, 1e+42, 1e+43, 1e+44, 1e+45, 1e+46, 1e+47, 1e+48, 1e+49, 1e+50, 1e+51, 1e+52, 1e+53, 1e+54, 1e+55, 1e+56, 1e+57, 1e+58, 1e+59, 1e+60,
				1e+61, 1e+62, 1e+63, 1e+64, 1e+65, 1e+66, 1e+67, 1e+68, 1e+69, 1e+70, 1e+71, 1e+72, 1e+73, 1e+74, 1e+75, 1e+76, 1e+77, 1e+78, 1e+79, 1e+80,
				1e+81, 1e+82, 1e+83, 1e+84, 1e+85, 1e+86, 1e+87, 1e+88, 1e+89, 1e+90, 1e+91, 1e+92, 1e+93, 1e+94, 1e+95, 1e+96, 1e+97, 1e+98, 1e+99, 1e+100,
				1e+101, 1e+102, 1e+103, 1e+104, 1e+105, 1e+106, 1e+107, 1e+108, 1e+109, 1e+110, 1e+111, 1e+112, 1e+113, 1e+114, 1e+115, 1e+116, 1e+117, 1e+118, 1e+119, 1e+120,
				1e+121, 1e+122, 1e+123, 1e+124, 1e+125, 1e+126, 1e+127, 1e+128, 1e+129, 1e+130, 1e+131, 1e+132, 1e+133, 1e+134, 1e+135, 1e+136, 1e+137, 1e+138, 1e+139, 1e+140,
				1e+141, 1e+142, 1e+143, 1e+144, 1e+145, 1e+146, 1e+147, 1e+148, 1e+149, 1e+150, 1e+151, 1e+152, 1e+153, 1e+154, 1e+155, 1e+156, 1e+157, 1e+158, 1e+159, 1e+160,
				1e+161, 1e+162, 1e+163, 1e+164, 1e+165, 1e+166, 1e+167, 1e+168, 1e+169, 1e+170, 1e+171, 1e+172, 1e+173, 1e+174, 1e+175, 1e+176, 1e+177, 1e+178, 1e+179, 1e+180,
				1e+181, 1e+182, 1e+183, 1e+184, 1e+185, 1e+186, 1e+187, 1e+188, 1e+189, 1e+190, 1e+191, 1e+192, 1e+193, 1e+194, 1e+195, 1e+196, 1e+197, 1e+198, 1e+199, 1e+200,
				1e+201, 1e+202, 1e+203, 1e+204, 1e+205, 1e+206, 1e+207, 1e+208, 1e+209, 1e+210, 1e+211, 1e+212, 1e+213, 1e+214, 1e+215, 1e+216, 1e+217, 1e+218, 1e+219, 1e+220,
				1e+221, 1e+222, 1e+223, 1e+224, 1e+225, 1e+226, 1e+227, 1e+228, 1e+229, 1e+230, 1e+231, 1e+232, 1e+233, 1e+234, 1e+235, 1e+236, 1e+237, 1e+238, 1e+239, 1e+240,
				1e+241, 1e+242, 1e+243, 1e+244, 1e+245, 1e+246, 1e+247, 1e+248, 1e+249, 1e+250, 1e+251, 1e+252, 1e+253, 1e+254, 1e+255, 1e+256, 1e+257, 1e+258, 1e+259, 1e+260,
				1e+261, 1e+262, 1e+263, 1e+264, 1e+265, 1e+266, 1e+267, 1e+268, 1e+269, 1e+270, 1e+271, 1e+272, 1e+273, 1e+274, 1e+275, 1e+276, 1e+277, 1e+278, 1e+279, 1e+280,
				1e+281, 1e+282, 1e+283, 1e+284, 1e+285, 1e+286, 1e+287, 1e+288, 1e+289, 1e+290, 1e+291, 1e+292, 1e+293, 1e+294, 1e+295, 1e+296, 1e+297, 1e+298, 1e+299, 1e+300,
				1e+301, 1e+302, 1e+303, 1e+304, 1e+305, 1e+306, 1e+307, 1e+308
			};

			FORCE_INLINE double GetDigitBase10Mult(int digits) 
			{
				assert(digits <= 308);
				return digits < -308 ? 0.0 : digitBase10Mult[digits + 308]; 
			}

			FORCE_INLINE int32 GetDigitCount(double& n)
			{
				int32 exp = (int32)log10(n);

				if (exp < DBL_MIN_10_EXP)
				{
					// too small, the number is treated as zero
					exp = 0;
					n = 0;
				}

				if (exp < 0)
					exp--;

				return exp;
			}
			

			FORCE_INLINE bool isBlank(char ch) { return ch == ' ' || ch == '\t'; }
			FORCE_INLINE bool isBlank(wchar_t ch) { return ch == ' ' || ch == '\t'; }

			FORCE_INLINE bool isDigit(int ch) { return ch >= '0' && ch <= '9'; }

			template <typename CharType>
			bool cstricmp(const CharType* a, const char* b);

			uint32 flagCut(uint64 v) { return static_cast<uint32>(v & 0xffffffffU); }

			template <typename CharType, typename IntType> FORCE_INLINE
			void ParseDecimalIntegerDigits(const CharType* str, IntType& num, bool* isInvalid)
			{
				while (*str)
				{
					CharType ch = *str++;

					if (isBlank(ch))
						break;

					if (isInvalid)
					{
						*isInvalid |= !isDigit(ch); // invalid char

						IntType prevVal = num;
						num *= 10;
						num += ch - '0';

						if (num < prevVal)
							*isInvalid = false; // overflow
					}
					else
					{
						num *= 10;
						num += ch - '0';
					}
				}
			}

			template <typename CharType, typename IntType> FORCE_INLINE
			void ParseDecimalIntegerDigitsUntilNonDigit(const CharType*& str, IntType& num, int* digitCount, bool* isInvalid)
			{
				while (*str)
				{
					CharType ch = *str;

					if (isBlank(ch) || !isDigit(ch))
						break;

					str++;

					if (isInvalid)
					{
						IntType prevVal = num;
						num *= 10;
						num += ch - '0';

						if (num < prevVal)
							*isInvalid = true; // overflow
					}
					else
					{
						num *= 10;
						num += ch - '0';
					}

					if (digitCount)
						(*digitCount)++;
				}
			}

			template <bool MustExist, typename CharType> FORCE_INLINE
			int ParseDecimalSign(const CharType*& str)
			{
				CharType ch = *str;
				if (ch == '-')
				{
					str++;
					return -1;
				}
				else if (ch == '+')
				{
					str++;
					return 1;
				}
				else if (MustExist)
				{
					return 0;
				}
				return 1;
			}


			template <typename CharType, typename IntType>
			IntType ParseSignedInteger(const CharType* str, bool* isInvalid)
			{
				static_assert(std::is_signed<IntType>::value, "");

				assert(str);
				if (isInvalid) *isInvalid = false;

				while (isBlank(*str))
					str++;

				int sign = ParseDecimalSign<false>(str);

				IntType num = 0;
				ParseDecimalIntegerDigits<CharType, IntType>(str, num, isInvalid);

				return sign * num;
			}

			template <typename CharType, typename UIntType>
			UIntType ParseUnsignedInteger(const CharType* str, bool* isInvalid)
			{
				static_assert(std::is_unsigned<UIntType>::value, "");

				assert(str);
				if (isInvalid) *isInvalid = false;

				while (isBlank(*str))
					str++;

				// just in case a sign in present, skip it and mark invalid
				int sign = ParseDecimalSign<false>(str);
				if (sign < 0 && isInvalid)
					*isInvalid = true;

				UIntType num = 0;
				ParseDecimalIntegerDigits<CharType, UIntType>(str, num, isInvalid);
				
				return num;
			}

			template <typename CharType, typename UIntType>
			UIntType ParseUnsignedIntegerBin(const CharType* str, bool* isInvalid)
			{
				static_assert(std::is_unsigned<UIntType>::value, "");

				assert(str);
				if (isInvalid) *isInvalid = false;

				UIntType result = 0;

				// allow blanks in binary number text
				while (*str)
				{
					CharType ch = *str++;
					if (isBlank(ch))
						continue;

					if (isInvalid)
					{
						*isInvalid |= ch != '0' && ch != '1';

						UIntType prevVal = result;
						result <<= 1;
						result |= (ch == '1') ? 1 : 0;

						if (result < prevVal)
							*isInvalid = true; // overflow
					}
					else
					{
						result <<= 1;
						result |= (ch == '1') ? 1 : 0;
					}
				}

				return result;
			}

			template <typename CharType, typename UIntType>
			UIntType ParseUnsignedIntegerHex(const CharType* str, bool* isInvalid)
			{
				static_assert(std::is_unsigned<UIntType>::value, "");
				
				assert(str);
				if (isInvalid) *isInvalid = false;

				while (isBlank(*str))
					str++;

				// allow prefix "0x"
				if (*str && *str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X'))
					str += 2;

				UIntType result = 0;

				// digits
				while (*str)
				{
					CharType ch = *str++;

					if (isBlank(ch))
						break;

					if (isInvalid)
						*isInvalid &= (!isDigit(ch) && ToUpperCase(ch) > 'F');

					byte digit = ch > '9' ? (ToUpperCase(ch) - 'A' + 10) : (ch - '0');

					if (isInvalid)
					{
						UIntType prevVal = result;
						result <<= 4;
						result |= digit;

						if (result < prevVal)
							*isInvalid = true; // overflow
					}
					else
					{
						result <<= 4;
						result |= digit;
					}
				}

				return result;
			}

			template <typename CharType>
			double ParseFloatingPoint(const CharType* str, bool* isInvalid)
			{
				assert(str);
				if (isInvalid) *isInvalid = false;

				while (isBlank(*str))
					str++;

				uint64 digits = 0;
				uint64 frac = 0;
				int exp = 0;
				int fracExp = 0;

				int sign = ParseDecimalSign<false>(str);
				
				if (cstricmp(str, "inf"))
					return sign * (double)INFINITY;

				if (cstricmp(str, "nan"))
					return sign * NAN;

				ParseDecimalIntegerDigitsUntilNonDigit(str, digits, nullptr, isInvalid);

				if (*str && *str == '.')
				{
					str++;
					ParseDecimalIntegerDigitsUntilNonDigit(str, frac, &fracExp, isInvalid);
					fracExp = -fracExp;
				}

				if (*str && *str == 'E' || *str == 'e')
				{
					str++;

					int expSign = ParseDecimalSign<true>(str);

					if (expSign == 0)
					{
						if (isInvalid)
							*isInvalid = true;
					}

					ParseDecimalIntegerDigitsUntilNonDigit<CharType>(str, exp, nullptr, isInvalid);

					exp *= expSign;
				}

				//double r = digits * GetDigitBase10Mult(exp) + frac * GetDigitBase10Mult(exp + fracExp);
				double r = ((double)digits + frac * GetDigitBase10Mult(fracExp)) * GetDigitBase10Mult(exp);

				if (sign == -1)
					r = -r;

				return r;
			}

			template <typename CharType, typename NumType, NumType (*Parser)(const CharType*, bool*)> FORCE_INLINE
			NumType ParseChecked(const CharType* str)
			{
#if _DEBUG
				bool hasError;
				NumType r = Parser(str, &hasError);
				assert(!hasError);
				return r;
#else
				return Parser(str, nullptr);
#endif
			}

			template <typename CharType, typename NumType, NumType(*Parser)(const CharType*, bool*)> FORCE_INLINE
			bool Parse(const CharType* str, NumType& result)
			{
				bool hasError = false;
				result = Parser(str, &hasError);
				return !hasError;
			}

			template <typename StrType, typename CharType, int32 N1>
			StrType NumberBufferToString(FixedList<CharType, N1>& digits, uint16 width, CharType fill, uint32 flags)
			{
				int32 leftPadding = 0;
				int32 rightPadding = 0;

				int32 paddingWidth = digits.getCount() >= width ? 0 : (width - digits.getCount());

				if (flags & StringUtils::SF_Left)
				{
					rightPadding = paddingWidth;
				}
				else
				{
					leftPadding = paddingWidth;
				}

				int32 totalLength = paddingWidth + digits.getCount();

				StrType str;
				str.reserve(totalLength);

				int32 digitStart = 0;

				if (leftPadding != 0)
				{
					if (fill == '0')
					{
						for (int i = 0; i < digits.getCount(); i++)
						{
							CharType ch = digits[i];

							if (i < digits.getCount() - 1)
							{
								// check the next char in case of 0x
								CharType chn = digits[i + 1];
								
								bool isHexPrefixEnd = chn == 'x' || chn == 'X';

								if (ch == '0' && isHexPrefixEnd)
								{
									digitStart = i + 2;
									break;
								}
							}
							
							if (ch != '+' && ch != '-')
							{
								digitStart = i;
								break;
							}
						}

						for (int i = 0; i < digitStart; i++)
							str.append(1, digits[i]);
					}

					for (int32 i = 0; i < leftPadding; i++)
						str.append(1, fill);
				}
				
				for (int i = digitStart; i < digits.getCount(); i++)
					str.append(1, digits[i]);
				
				for (int32 i = 0; i < rightPadding; i++)
					str.append(1, fill);

				return str;
			}
			
			template <typename CharType, uint32 Base>
			using IntegerDigitBuffer = FixedList < CharType, 4 + (Base == 16 ? 16 : (Base == 10 ? 20 : (Base == 8 ? 22 : 64))) > ;

			// integer base to string: convert the digit to string
			template <typename StrType, typename CharType, uint32 Base, typename UIntType>
			void IntegerBaseToString(UIntType val, IntegerDigitBuffer<CharType, Base>& digits, bool useUpperCase)
			{
				static_assert(std::is_unsigned<UIntType>::value, "");
				static_assert(Base >= 2, "");

				const char* digitChars = useUpperCase ? digitCharTableU : digitCharTableL;

				while (val >= Base)
				{
					digits.Add(digitChars[val % Base]);

					val /= Base;
				}
				digits.Add(digitChars[val]);
			}

			
			struct SignedBaseToStringHelper
			{
				template <typename StrType, typename CharType, uint32 Base, typename IntType>
				static void Invoke(IntType val, uint16 width, CharType fill, uint32 flags, 
					IntegerDigitBuffer<CharType, Base>& digits, char& sign)
				{
					static_assert(std::is_signed<IntType>::value, "");

					std::make_unsigned<IntType>::type digitParts;

					if (val > 0)
					{
						digitParts = val;
						sign = 1;
					}
					else if (val == 0)
					{
						digitParts = val;
						sign = 0;
					}
					else if (val < 0)
					{
						digitParts = -val;
						sign = -1;
					}

					IntegerBaseToString<StrType, CharType, Base, std::make_unsigned<IntType>::type>(digitParts,
						digits, (flags & StringUtils::SF_UpperCase) != 0);
				}
			};

			struct UnsignedBaseToStringHelper
			{
				template <typename StrType, typename CharType, uint32 Base, typename IntType>
				static void Invoke(IntType val, uint16 width, CharType fill, uint32 flags, 
					IntegerDigitBuffer<CharType, Base>& digits, char& sign)
				{
					static_assert(std::is_unsigned<IntType>::value, "");

					IntegerBaseToString<StrType, CharType, Base, IntType>(val, digits, (flags & StringUtils::SF_UpperCase) != 0);
					sign = val > 0 ? 1 : 0;
				}
			};



			template <typename StrType, typename CharType, uint32 Base, typename IntType>
			StrType IntegerToString(IntType val, uint16 width, CharType fill, uint32 flags)
			{
				IntegerDigitBuffer<CharType, Base> digits;
				char sign = 0;

				std::conditional<std::is_signed<IntType>::value, SignedBaseToStringHelper, UnsignedBaseToStringHelper>::
					type::Invoke<StrType, CharType, Base, IntType>(val, width, fill, flags, digits, sign);


				if (sign > 0 && (flags & StringUtils::SF_ShowPositiveSign))
					digits.Add('+');
				else if (sign < 0)
					digits.Add('-');

				if (Base == 16 && (flags & StringUtils::SF_ShowHexBase))
				{
					digits.Add('x');
					digits.Add('0');
				}

				digits.Reverse();

				return NumberBufferToString<StrType, CharType>(digits, width, fill, flags);
			}

			template <uint32 Base, typename IntType> FORCE_INLINE
			String IntegerToString16(IntType val, uint64 flags) 
			{
				return IntegerToString<String, char16_t, Base, IntType>(val, GetWidth(flags), (char16_t)GetFill(flags), flagCut(flags));
			}

			template <uint32 Base, typename IntType> FORCE_INLINE
			nstring IntegerToString8(IntType val, uint64 flags) 
			{
				return IntegerToString<nstring, char, Base, IntType>(val, GetWidth(flags), (char)GetFill(flags), flagCut(flags));
			}


			template <typename CharType>
			using FloatDigitBuffer = FixedList < CharType, 310 > ;

			template <typename StrType, typename CharType>
			StrType DoubleToString(double val, byte meaningfulDigitCount, uint16 width, CharType fill, uint64 flags)
			{
				FloatDigitBuffer<CharType> digits;
				
				int sign = std::signbit(val) ? -1 : 1;

				if (sign > 0 && (flags & StringUtils::SF_ShowPositiveSign))
					digits.Add('+');
				else if (sign < 0)
					digits.Add('-');

				const bool useUpperCase = (flags & StringUtils::SF_UpperCase) != 0;
				const char* digitChars = useUpperCase ? digitCharTableU : digitCharTableL;

				if (std::isnan(val))
				{
					digits.Clear();

					const char* nanTxt = useUpperCase ? "NAN" : "nan";
					while (*nanTxt)
						digits.Add(*nanTxt++);
				}
				else if (std::isinf(val))
				{
					const char* infTxt = useUpperCase ? "INF" : "inf";
					while (*infTxt)
						digits.Add(*infTxt++);
				}
				else
				{
					bool useExp = false;
					bool hasCustomPrecision = (flags & StringUtils::SF_FPCustomPrecision) != 0;

					int16 fracPrecision = GetFP(flags);
					
					double n = abs(val);

					int32 exp = GetDigitCount(n);

					double threshold = GetDigitBase10Mult(exp - meaningfulDigitCount);
					{
						double rounding;

						if (hasCustomPrecision)
						{
							rounding = GetDigitBase10Mult(Math::Min(0, exp) - fracPrecision) * 0.5;
						}
						else
						{
							rounding = threshold * 0.5;
						}
						
						n += rounding;
					}

					exp = GetDigitCount(n);


					if (flags & StringUtils::SF_FPScientific)
						useExp = true;
					else if ((flags & StringUtils::SF_FPDecimal) == 0)		// if not using decimal notion, then auto determine
						useExp = exp >= 14 || (sign < 0 && exp >= 9) || exp <= -10;


					int32 dig = 0;  // digit to start go down. 0 based: 0 is the first digit on the left side of decimal
					if (useExp)
					{
						// normalize to scientific form
						double m = GetDigitBase10Mult(exp);

						n /= m;
						threshold /= m;
					}
					else
					{
						dig = exp;
						if (dig < 0)
							dig = 0;
					}

					uint16 digitsProcessed = 0;

					while ((dig >= 0 || n > threshold) &&
						(!hasCustomPrecision || fracPrecision >= 0))
					{
						if (dig == -1)
							digits.Add('.');

						double weight = GetDigitBase10Mult(dig);

						if (weight > 0)
						{
							double d = floor(n / weight);
							n -= d*weight;

							int32 di = (int32)d;
							if (di < 0) di = 0;

							if (digitsProcessed < meaningfulDigitCount)
								digits.Add(digitChars[di]);
							else
								digits.Add(digitChars[0]);	// out of precision digits, use 0 in digit place
						}
						else
						{
							n = 0;
							digits.Add(digitChars[0]);
						}

						digitsProcessed++;
						dig--;
						if (dig < 0)
							fracPrecision--;
					}

					if (useExp)
					{
						digits.Add(useUpperCase ? 'E' : 'e');
						digits.Add(exp < 0 ? '-' : '+');

						FixedList<CharType, 6> expDigits;

						exp = abs(exp);

						while (exp >= 10)
						{
							expDigits.Add(digitChars[exp % 10]);
							exp /= 10;
						}
						expDigits.Add(digitChars[exp]);
						expDigits.Reverse();

						digits.AddList(expDigits);
					}
				}

				return NumberBufferToString<StrType, CharType>(digits, width, fill, flagCut(flags));
			}





			template <typename CharType>
			CharType ToLowerCase(CharType ch) { return (ch >= 'A' && ch <= 'Z') ? (ch + 'a' - 'A') : ch; }

			template <typename CharType>
			CharType ToUpperCase(CharType ch) { return (ch >= 'a' && ch <= 'z') ? (ch + 'A' - 'a') : ch; }



			template <bool caseInsensitive, typename CharType> 
			bool CompareString(const CharType* a, const CharType* b, size_t len)
			{
				while (len > 0)
				{
					if (caseInsensitive)
					{
						if (ToLowerCase(*a) != ToLowerCase(*b))
							return false;
					}
					else
					{
						if (*a != *b)
							return false;
					}

					a++; b++;
					len--;
				}

				return true;
			}

			template <typename CharType>
			bool CompareString(const CharType* a, const CharType* b, size_t len, bool caseInsensitive)
			{
				return caseInsensitive ? CompareString<true>(a, b, len) : CompareString<false>(a, b, len);
			}


			template <bool caseInsensitive, typename StrType>
			bool StartsWith(const StrType& str, const StrType& pattern)
			{
				size_t len = str.length();
				size_t patternLen = pattern.length();
				if (len < patternLen || patternLen == 0)
					return false;

				return Impl::CompareString<caseInsensitive>(str.c_str(), pattern.c_str(), patternLen);
			}

			template <bool caseInsensitive, typename StrType, typename CharType, int32 N>
			bool StartsWith(const StrType& str, const CharType(&pattern)[N])
			{
				size_t len = str.length();
				if (len < N - 1 || N <= 1)
					return false;

				return Impl::CompareString<caseInsensitive>(str.c_str(), pattern, N - 1);
			}

			template <bool caseInsensitive, typename StrType>
			bool EndsWith(const StrType& str, const StrType& pattern)
			{
				size_t thisLen = str.length();
				size_t patternLen = pattern.length();
				if (thisLen < patternLen || patternLen == 0)
					return false;

				size_t startOffset = thisLen - patternLen;

				return Impl::CompareString<caseInsensitive>(str.c_str() + startOffset, pattern.c_str(), patternLen);
			}


			template <typename StrType>
			bool StartsWith(const StrType& str, const StrType& pattern, bool caseInsensitive)
			{
				return caseInsensitive ? StartsWith<true>(str, pattern) : StartsWith<false>(str, pattern);
			}

			template <typename StrType>
			bool EndsWith(const StrType& str, const StrType& pattern, bool caseInsensitive)
			{
				return caseInsensitive ? EndsWith<true>(str, pattern) : EndsWith<false>(str, pattern);
			}


			template <typename StrType>
			bool EqualsNoCase(const StrType& a, const StrType& b)
			{
				if (a.size() != b.size())
					return false;

				for (size_t i = 0; i < a.size(); i++)
				{
					if (ToLowerCase(a[i]) != ToLowerCase(b[i]))
						return false;
				}
				return true;
			}



			template <typename StrType, typename DelimType = StrType>
			void Split(const StrType& str, List<StrType>& result, const DelimType& delims)
			{
				assert(result.getCount() == 0);

				// Use STL methods 
				size_t start, pos;
				start = 0;
				do
				{
					pos = str.find_first_of(delims, start);
					if (pos == start)
					{
						// Do nothing
						start = pos + 1;
					}
					else if (pos == StrType::npos)
					{
						// Copy the rest of the nstring
						if (start < str.size())
							result.Add(str.substr(start));

						break;
					}
					else
					{
						// Copy up to delimiter
						result.Add(str.substr(start, pos - start));
						start = pos + 1;
					}
					// parse up to next real data
					start = str.find_first_not_of(delims, start);

				} while (pos != StrType::npos);
			}

			template <typename StrType, typename DelimType = StrType>
			List<StrType> Split(const StrType& str, const DelimType& delims)
			{
				List<StrType> result;
				Split<StrType, DelimType>(str, result, delims);
				return result;
			}

			template <typename CharType>
			bool Match(const CharType* nstring, const CharType* wild)
			{
				const CharType* cp = nullptr;
				const CharType* mp = nullptr;

				while ((*nstring) && (*wild != '*'))
				{
					if ((*wild != *nstring) && (*wild != '?'))
						return false;

					wild++;
					nstring++;
				}

				while (*nstring)
				{
					wchar_t wch = *wild;

					if (wch == '*')
					{
						if (!*++wild)
							return true;

						mp = wild;
						cp = nstring + 1;
					}
					else if ((wch == *nstring) || (wch == '?'))
					{
						wild++;
						nstring++;
					}
					else
					{
						wild = mp;
						nstring = cp++;
					}
				}

				while (*wild == '*')
					wild++;
				return !*wild;
			}


			template <typename StrType>
			void Trim(StrType& str, const StrType& delims)
			{
				str.erase(str.find_last_not_of(delims) + 1);
				str.erase(0, str.find_first_not_of(delims));
			}

			template <typename StrType>
			void TrimLeft(StrType& str, const StrType& delims)  { str.erase(0, str.find_first_not_of(delims)); }

			template <typename StrType>
			void TrimRight(StrType& str, const StrType& delims) { str.erase(str.find_last_not_of(delims) + 1); }


			template <typename CharType>
			bool cstricmp(const CharType* a, const char* b)
			{
				for (; ToLowerCase(*a) == ToLowerCase(*b); a++, b++)
					if (*a == 0)
						return true;
				return false;
			}
		}
		

		//////////////////////////////////////////////////////////////////////////

		bool StringUtils::ParseBool(const String& v) { return Impl::StartsWith<true>(v, L"true") || Impl::StartsWith<true>(v, L"yes") || Impl::StartsWith<true>(v, L"1"); }

		uint16 StringUtils::ParseUInt16(const String& val) { return Impl::ParseChecked<wchar_t, uint16, Impl::ParseUnsignedInteger>(val.c_str()); }
		uint32 StringUtils::ParseUInt32(const String& val) { return Impl::ParseChecked<wchar_t, uint32, Impl::ParseUnsignedInteger>(val.c_str()); }
		uint64 StringUtils::ParseUInt64(const String& val) { return Impl::ParseChecked<wchar_t, uint64, Impl::ParseUnsignedInteger>(val.c_str()); }

		uint32 StringUtils::ParseUInt32Hex(const String& val) { return Impl::ParseChecked<wchar_t, uint32, Impl::ParseUnsignedIntegerHex>(val.c_str()); }
		uint64 StringUtils::ParseUInt64Hex(const String& val) { return Impl::ParseChecked<wchar_t, uint64, Impl::ParseUnsignedIntegerHex>(val.c_str()); }

		uint32 StringUtils::ParseUInt32Bin(const String& val) { return Impl::ParseChecked<wchar_t, uint32, Impl::ParseUnsignedIntegerBin>(val.c_str()); }
		uint64 StringUtils::ParseUInt64Bin(const String& val) { return Impl::ParseChecked<wchar_t, uint64, Impl::ParseUnsignedIntegerBin>(val.c_str()); }

		int16 StringUtils::ParseInt16(const String& val) { return Impl::ParseChecked<wchar_t, int16, Impl::ParseSignedInteger>(val.c_str()); }
		int32 StringUtils::ParseInt32(const String& val) { return Impl::ParseChecked<wchar_t, int32, Impl::ParseSignedInteger>(val.c_str()); }
		int64 StringUtils::ParseInt64(const String& val) { return Impl::ParseChecked<wchar_t, int64, Impl::ParseSignedInteger>(val.c_str()); }

		float StringUtils::ParseSingle(const String& val) { return (float)Impl::ParseChecked<wchar_t, double, Impl::ParseFloatingPoint>(val.c_str()); }
		double StringUtils::ParseDouble(const String& val) { return Impl::ParseChecked<wchar_t, double, Impl::ParseFloatingPoint>(val.c_str()); }

		bool StringUtils::ParseUInt16(const String& val, uint16& result) { return Impl::Parse<wchar_t, uint16, Impl::ParseUnsignedInteger>(val.c_str(), result); }
		bool StringUtils::ParseUInt32(const String& val, uint32& result) { return Impl::Parse<wchar_t, uint32, Impl::ParseUnsignedInteger>(val.c_str(), result); }
		bool StringUtils::ParseUInt64(const String& val, uint64& result) { return Impl::Parse<wchar_t, uint64, Impl::ParseUnsignedInteger>(val.c_str(), result); }

		bool StringUtils::ParseInt16(const String& val, int16& result) { return Impl::Parse<wchar_t, int16, Impl::ParseSignedInteger>(val.c_str(), result); }
		bool StringUtils::ParseInt32(const String& val, int32& result) { return Impl::Parse<wchar_t, int32, Impl::ParseSignedInteger>(val.c_str(), result); }
		bool StringUtils::ParseInt64(const String& val, int64& result) { return Impl::Parse<wchar_t, int64, Impl::ParseSignedInteger>(val.c_str(), result); }

		bool StringUtils::ParseSingle(const String& val, float& result) { double r; return Impl::Parse<wchar_t, double, Impl::ParseFloatingPoint>(val.c_str(), r); result = (float)r; }
		bool StringUtils::ParseDouble(const String& val, double& result) { return Impl::Parse<wchar_t, double, Impl::ParseFloatingPoint>(val.c_str(), result); }

		//////////////////////////////////////////////////////////////////////////

		bool StringUtils::ParseBool(const nstring& v) { return Impl::StartsWith<true>(v, "true") || Impl::StartsWith<true>(v, "yes") || Impl::StartsWith<true>(v, "1"); }

		uint16 StringUtils::ParseUInt16(const nstring& val) { return Impl::ParseChecked<char, uint16, Impl::ParseUnsignedInteger>(val.c_str()); }
		uint32 StringUtils::ParseUInt32(const nstring& val) { return Impl::ParseChecked<char, uint32, Impl::ParseUnsignedInteger>(val.c_str()); }
		uint64 StringUtils::ParseUInt64(const nstring& val) { return Impl::ParseChecked<char, uint64, Impl::ParseUnsignedInteger>(val.c_str()); }

		uint32 StringUtils::ParseUInt32Hex(const nstring& val) { return Impl::ParseChecked<char, uint32, Impl::ParseUnsignedIntegerHex>(val.c_str()); }
		uint64 StringUtils::ParseUInt64Hex(const nstring& val) { return Impl::ParseChecked<char, uint64, Impl::ParseUnsignedIntegerHex>(val.c_str()); }

		uint32 StringUtils::ParseUInt32Bin(const nstring& val) { return Impl::ParseChecked<char, uint32, Impl::ParseUnsignedIntegerBin>(val.c_str()); }
		uint64 StringUtils::ParseUInt64Bin(const nstring& val) { return Impl::ParseChecked<char, uint64, Impl::ParseUnsignedIntegerBin>(val.c_str()); }


		int16 StringUtils::ParseInt16(const nstring& val) { return Impl::ParseChecked<char, int16, Impl::ParseSignedInteger>(val.c_str()); }
		int32 StringUtils::ParseInt32(const nstring& val) { return Impl::ParseChecked<char, int32, Impl::ParseSignedInteger>(val.c_str()); }
		int64 StringUtils::ParseInt64(const nstring& val) { return Impl::ParseChecked<char, int64, Impl::ParseSignedInteger>(val.c_str()); }

		float StringUtils::ParseSingle(const nstring& val) { return (float)Impl::ParseChecked<char, double, Impl::ParseFloatingPoint>(val.c_str()); }
		double StringUtils::ParseDouble(const nstring& val) { return Impl::ParseChecked<char, double, Impl::ParseFloatingPoint>(val.c_str()); }


		bool StringUtils::ParseUInt16(const std::string& val, uint16& result) { return Impl::Parse<char, uint16, Impl::ParseUnsignedInteger>(val.c_str(), result); }
		bool StringUtils::ParseUInt32(const std::string& val, uint32& result) { return Impl::Parse<char, uint32, Impl::ParseUnsignedInteger>(val.c_str(), result); }
		bool StringUtils::ParseUInt64(const std::string& val, uint64& result) { return Impl::Parse<char, uint64, Impl::ParseUnsignedInteger>(val.c_str(), result); }
		
		bool StringUtils::ParseInt16(const std::string& val, int16& result) { return Impl::Parse<char, int16, Impl::ParseSignedInteger>(val.c_str(), result); }
		bool StringUtils::ParseInt32(const std::string& val, int32& result) { return Impl::Parse<char, int32, Impl::ParseSignedInteger>(val.c_str(), result); }
		bool StringUtils::ParseInt64(const std::string& val, int64& result) { return Impl::Parse<char, int64, Impl::ParseSignedInteger>(val.c_str(), result); }

		bool StringUtils::ParseSingle(const std::string& val, float& result) { double r; return Impl::Parse<char, double, Impl::ParseFloatingPoint>(val.c_str(), r); result = (float)r; }
		bool StringUtils::ParseDouble(const std::string& val, double& result) { return Impl::Parse<char, double, Impl::ParseFloatingPoint>(val.c_str(), result); }

		//////////////////////////////////////////////////////////////////////////
		
		String StringUtils::BoolToString(bool val) { return val ? L"true" : L"false"; }

		String StringUtils::IntToString(int64 val, uint64 flags) { return Impl::IntegerToString16<10>(val, flags); } 
		String StringUtils::UIntToString(uint64 val, uint64 flags) { return Impl::IntegerToString16<10>(val, flags); } 

		String StringUtils::SingleToString(float val, uint64 flags) 
		{
			return Impl::DoubleToString<String, char16_t>(val, FLT_DIG, GetWidth(flags), (char16_t)GetFill(flags), static_cast<uint32>(flags & 0xfffff));
		}
		String StringUtils::DoubleToString(double val, uint64 flags)
		{
			return Impl::DoubleToString<String, char16_t>(val, DBL_DIG, GetWidth(flags), (char16_t)GetFill(flags), static_cast<uint32>(flags & 0xfffff));
		}

		String StringUtils::IntToString(int16 val, uint64 flags) { return Impl::IntegerToString16<10>(val, flags); } 
		String StringUtils::IntToString(int32 val, uint64 flags) { return Impl::IntegerToString16<10>(val, flags); } 
		String StringUtils::UIntToString(uint16 val, uint64 flags) { return Impl::IntegerToString16<10>(val, flags); } 
		String StringUtils::UIntToString(uint32 val, uint64 flags) { return Impl::IntegerToString16<10>(val, flags); } 

		String StringUtils::UIntToStringHex(uint32 val, uint64 flags) { return Impl::IntegerToString16<16>(val, flags); }
		String StringUtils::UIntToStringHex(uint64 val, uint64 flags) { return Impl::IntegerToString16<16>(val, flags); }
		
		String StringUtils::UIntToStringBin(uint32 val, uint64 flags) { return Impl::IntegerToString16<2>(val, flags); }
		String StringUtils::UIntToStringBin(uint64 val, uint64 flags) { return Impl::IntegerToString16<2>(val, flags); }

		//////////////////////////////////////////////////////////////////////////

		nstring StringUtils::BoolToNarrowString(bool val) { return val ? "true" : "false"; }

		nstring StringUtils::IntToNarrowString(int64 val, uint64 flags) { return Impl::IntegerToString8<10>(val, flags); }
		nstring StringUtils::UIntToNarrowString(uint64 val, uint64 flags) { return Impl::IntegerToString8<10>(val, flags); }
		nstring StringUtils::SingleToNarrowString(float val, uint64 flags)
		{
			return Impl::DoubleToString<nstring, char>(val, FLT_DIG, GetWidth(flags), (char)GetFill(flags), static_cast<uint32>(flags & 0xfff)); 
		}
		nstring StringUtils::DoubleToNarrowString(double val, uint64 flags)
		{
			return Impl::DoubleToString<nstring, char>(val, DBL_DIG, GetWidth(flags), (char)GetFill(flags), static_cast<uint32>(flags & 0xfff)); 
		}

		nstring StringUtils::IntToNarrowString(int16 val, uint64 flags) { return Impl::IntegerToString8<10>(val, flags);  }
		nstring StringUtils::IntToNarrowString(int32 val, uint64 flags) { return Impl::IntegerToString8<10>(val, flags); }
		nstring StringUtils::UIntToNarrowString(uint16 val, uint64 flags) { return Impl::IntegerToString8<10>(val, flags); }
		nstring StringUtils::UIntToNarrowString(uint32 val, uint64 flags) { return Impl::IntegerToString8<10>(val, flags); }

		nstring StringUtils::UIntToNarrowStringHex(uint32 val, uint64 flags) { return Impl::IntegerToString8<16>(val, flags); }
		nstring StringUtils::UIntToNarrowStringHex(uint64 val, uint64 flags) { return Impl::IntegerToString8<16>(val, flags); }

		nstring StringUtils::UIntToNarrowStringBin(uint32 val, uint64 flags) { return Impl::IntegerToString8<2>(val, flags); }
		nstring StringUtils::UIntToNarrowStringBin(uint64 val, uint64 flags) { return Impl::IntegerToString8<2>(val, flags); }

		//////////////////////////////////////////////////////////////////////////


		bool StringUtils::EqualsNoCase(const String& a, const String& b) { return Impl::EqualsNoCase(a,b); }
		bool StringUtils::EqualsNoCase(const nstring& a, const nstring& b) { return Impl::EqualsNoCase(a,b); }

		
		bool StringUtils::Match(const String& str, const String& pattern) { return Impl::Match(str.c_str(), pattern.c_str()); }
		bool StringUtils::Match(const nstring& str, const nstring& pattern) { return Impl::Match(str.c_str(), pattern.c_str()); }

		//////////////////////////////////////////////////////////////////////////

		void StringUtils::Trim(String& str, const String& delims) { Impl::Trim(str, delims); }
		void StringUtils::TrimLeft(String& str, const String& delims) { Impl::TrimLeft(str, delims); }
		void StringUtils::TrimRight(String& str, const String& delims) { Impl::TrimRight(str, delims); }

		void StringUtils::Trim(std::string& str, const std::string& delims) { Impl::Trim(str, delims); }
		void StringUtils::TrimLeft(std::string& str, const std::string& delims) { Impl::TrimLeft(str, delims); }
		void StringUtils::TrimRight(std::string& str, const std::string& delims) { Impl::TrimRight(str, delims); }


		void StringUtils::Split(const String& str, List<String>& result, const String& delims) { Impl::Split(str, result, delims); }
		void StringUtils::Split(const nstring& str, List<nstring>& result, const nstring& delims) { Impl::Split(str, result, delims); }
		
		void StringUtils::Split(const String& str, List<String>& result, char16_t delims) { Impl::Split(str, result, delims); }
		void StringUtils::Split(const nstring& str, List<nstring>& result, char delims) { Impl::Split(str, result, delims); }

		List<String> StringUtils::Split(const String& str, const String& delims) { return Impl::Split(str, delims); }
		List<nstring> StringUtils::Split(const nstring& str, const nstring& delims) { return Impl::Split(str, delims); }

		List<String> StringUtils::Split(const String& str, char16_t delims) { return Impl::Split(str, delims); }
		List<nstring> StringUtils::Split(const nstring& str, char delims) { return Impl::Split(str, delims); }


		//////////////////////////////////////////////////////////////////////////

		int32 StringUtils::SplitParseSingles(const String& str, float* flts, int32 maxCount, const String& delims)
		{
			WrappedList<float> lst(flts, maxCount);
			SplitParse<String, WrappedList<float>, float, StringUtils::ParseSingle>(str, lst, delims);
			return lst.getCount();
		}
		List<float> StringUtils::SplitParseSingles(const String& str, const String& delims)
		{
			List<float> lst;
			SplitParse<String, List<float>, float, StringUtils::ParseSingle>(str, lst, delims);
			return lst;
		}
		void StringUtils::SplitParseSingles(const String& str, Apoc3D::Collections::List<float>& results, const String& delims) 
		{
			SplitParse<String, List<float>, float, StringUtils::ParseSingle>(str, results, delims); 
		}

		int32 StringUtils::SplitParseSingles(const nstring& str, float* flts, int32 maxCount, const nstring& delims)
		{
			WrappedList<float> lst(flts, maxCount);
			SplitParse<nstring, WrappedList<float>, float, StringUtils::ParseSingle>(str, lst, delims);
			return lst.getCount();
		}
		List<float> StringUtils::SplitParseSingles(const nstring& str, const nstring& delims)
		{
			List<float> lst;
			SplitParse<nstring, List<float>, float, StringUtils::ParseSingle>(str, lst, delims); 
			return lst;
		}
		void StringUtils::SplitParseSingles(const nstring& str, Apoc3D::Collections::List<float>& results, const nstring& delims)
		{
			SplitParse<nstring, List<float>, float, StringUtils::ParseSingle>(str, results, delims); 
		}

		//////////////////////////////////////////////////////////////////////////

		int32 StringUtils::SplitParseInts(const String& str, int32* ints, int32 maxCount, const String& delims)
		{
			WrappedList<int32> lst(ints, maxCount);
			SplitParse<String, WrappedList<int32>, int32, StringUtils::ParseInt32>(str, lst, delims);
			return lst.getCount();
		}
		List<int32> StringUtils::SplitParseInts(const String& str, const String& delims)
		{
			List<int32> lst;
			SplitParse<String, List<int32>, int32, StringUtils::ParseInt32>(str, lst, delims); 
			return lst;
		}
		void StringUtils::SplitParseInts(const String& str, Apoc3D::Collections::List<int32>& results, const String& delims) 
		{
			SplitParse<String, List<int32>, int32, StringUtils::ParseInt32>(str, results, delims); 
		}
		

		int32 StringUtils::SplitParseInts(const nstring& str, int32* ints, int32 maxCount, const nstring& delims)
		{
			WrappedList<int32> lst(ints, maxCount);
			SplitParse<nstring, WrappedList<int32>, int32, StringUtils::ParseInt32>(str, lst, delims);
			return lst.getCount();
		}
		List<int32> StringUtils::SplitParseInts(const nstring& str, const nstring& delims)
		{
			List<int32> lst;
			SplitParse<nstring, List<int32>, int32, StringUtils::ParseInt32>(str, lst, delims); 
			return lst;
		}
		void StringUtils::SplitParseInts(const nstring& str, Apoc3D::Collections::List<int32>& results, const nstring& delims) 
		{
			SplitParse<nstring, List<int32>, int32, StringUtils::ParseInt32>(str, results, delims); 
		}
		
		//////////////////////////////////////////////////////////////////////////
		String DummyRet(const String& r) { return r; }

		String StringUtils::PackStrings(const Apoc3D::Collections::List<String>& v, bool useQuotes, wchar_t sep)
		{
			String r;
			if (useQuotes)
				Pack<String, DummyRet, true>(v, r, sep);
			else
				Pack<String, DummyRet, false>(v, r, sep);
			return r;
		}

		//////////////////////////////////////////////////////////////////////////

		bool StringUtils::Replace(String& str, const String& from, const String& to)
		{
			size_t start_pos = str.find(from);
			if (start_pos == String::npos)
				return false;
			str.replace(start_pos, from.length(), to);
			return true;
		}
		void StringUtils::ReplaceAll(String& str, const String& from, const String& to)
		{
			if (from.empty())
				return;
			size_t start_pos = 0;
			while ((start_pos = str.find(from, start_pos)) != String::npos)
			{
				str.replace(start_pos, from.length(), to);
				start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
			}
		}

		//////////////////////////////////////////////////////////////////////////

		bool StringUtils::StartsWith(const String& str, const String& v, bool caseInsensitive) { return Impl::StartsWith(str, v, caseInsensitive); }
		bool StringUtils::EndsWith(const String& str, const String& v, bool caseInsensitive) { return Impl::EndsWith(str, v, caseInsensitive); }
		bool StringUtils::StartsWith(const nstring& str, const nstring& v, bool caseInsensitive) { return Impl::StartsWith(str, v, caseInsensitive); }
		bool StringUtils::EndsWith(const nstring& str, const nstring& v, bool caseInsensitive) { return Impl::EndsWith(str, v, caseInsensitive); }

		char StringUtils::ToLowerCase(char ch) { return Impl::ToLowerCase(ch); }
		char StringUtils::ToUpperCase(char ch) { return Impl::ToUpperCase(ch); }

		wchar_t StringUtils::ToUpperCase(wchar_t ch) { return Impl::ToUpperCase(ch); }
		wchar_t StringUtils::ToLowerCase(wchar_t ch) { return Impl::ToLowerCase(ch); }

		void StringUtils::ToLowerCase(String& str)
		{
			for (size_t i = 0; i < str.size(); i++)
				str[i] = ToLowerCase(str[i]);
		}
		void StringUtils::ToUpperCase(String& str)
		{
			for (size_t i = 0; i < str.size(); i++)
				str[i] = ToUpperCase(str[i]);
		}
		void StringUtils::ToLowerCase(std::string& str)
		{
			for (size_t i = 0; i < str.size(); i++)
				str[i] = ToLowerCase(str[i]);
		}
		void StringUtils::ToUpperCase(std::string& str)
		{
			for (size_t i = 0; i < str.size(); i++)
				str[i] = ToUpperCase(str[i]);
		}

		/************************************************************************/
		/*  StringUtilsLocalized                                                */
		/************************************************************************/

		void StringUtilsLocalized::ToLowerCase(String& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), tolower);
		}
		void StringUtilsLocalized::ToUpperCase(String& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), toupper);
		}

		using std::wistringstream;
		using std::wostringstream;
		using std::istringstream;
		using std::ostringstream;

		//////////////////////////////////////////////////////////////////////////

		uint16 StringUtilsLocalized::ParseUInt16(const String& val)
		{
			wistringstream str(val);
			uint16 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint32 StringUtilsLocalized::ParseUInt32(const String& val)
		{
			wistringstream str(val);
			uint32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint64 StringUtilsLocalized::ParseUInt64(const String& val)
		{
			wistringstream str(val);
			uint64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}

		int16 StringUtilsLocalized::ParseInt16(const String& val)
		{
			wistringstream str(val);
			int16 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		int32 StringUtilsLocalized::ParseInt32(const String& val)
		{
			wistringstream str(val);
			int32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		int64 StringUtilsLocalized::ParseInt64(const String& val)
		{
			wistringstream str(val);
			int64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		float StringUtilsLocalized::ParseSingle(const String& val)
		{
			wistringstream str(val);

			float ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		double StringUtilsLocalized::ParseDouble(const String& val)
		{
			wistringstream str(val);
			double ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}

		String StringUtilsLocalized::IntToString(int64 val, uint16 width, char16_t fill, std::ios::fmtflags flags)
		{
			wostringstream stream;
			stream.width(width);
			stream.fill(fill);
			if (flags)
				stream.setf(flags);
			stream << val;
			return stream.str();
		}
		String StringUtilsLocalized::UIntToString(uint64 val, uint16 width, char16_t fill, std::ios::fmtflags flags)
		{
			wostringstream stream;
			stream.width(width);
			stream.fill(fill);
			if (flags)
				stream.setf(flags);
			stream << val;
			return stream.str();
		}
		String StringUtilsLocalized::SingleToString(float val, uint16 precision, uint16 width, char16_t fill, std::ios::fmtflags flags)
		{
			wostringstream stream;
			stream.precision(precision);
			stream.width(width);
			stream.fill(fill);

			stream.setf(flags, std::ios::floatfield);
			stream << val;
			return stream.str();
		}

		String StringUtilsLocalized::IntToString(int16 val, uint16 width, char16_t fill, std::ios::fmtflags flags)
		{
			return IntToString((int64)val, width, fill, flags);
		}
		String StringUtilsLocalized::IntToString(int32 val, uint16 width, char16_t fill, std::ios::fmtflags flags)
		{
			return IntToString((int64)val, width, fill, flags);
		}
		String StringUtilsLocalized::UIntToString(uint16 val, uint16 width, char16_t fill, std::ios::fmtflags flags)
		{
			return UIntToString((uint64)val, width, fill, flags);
		}
		String StringUtilsLocalized::UIntToString(uint32 val, uint16 width, char16_t fill, std::ios::fmtflags flags)
		{
			return UIntToString((uint64)val, width, fill, flags);
		}

		//////////////////////////////////////////////////////////////////////////

		uint16 StringUtilsLocalized::ParseUInt16(const nstring& val)
		{
			istringstream str(val);
			uint16 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint32 StringUtilsLocalized::ParseUInt32(const nstring& val)
		{
			istringstream str(val);
			uint32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		uint64 StringUtilsLocalized::ParseUInt64(const nstring& val)
		{
			istringstream str(val);
			uint64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}

		int16 StringUtilsLocalized::ParseInt16(const nstring& val)
		{
			istringstream str(val);
			int16 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		int32 StringUtilsLocalized::ParseInt32(const nstring& val)
		{
			istringstream str(val);
			int32 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		int64 StringUtilsLocalized::ParseInt64(const nstring& val)
		{
			istringstream str(val);
			int64 ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		float StringUtilsLocalized::ParseSingle(const nstring& val)
		{
			istringstream str(val);

			float ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}
		double StringUtilsLocalized::ParseDouble(const nstring& val)
		{
			istringstream str(val);

			double ret = 0;
			str >> ret;
			assert(!str.fail());
			return ret;
		}


		nstring StringUtilsLocalized::IntToNarrowString(int64 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			ostringstream stream;
			stream.width(width);
			stream.fill(fill);
			if (flags)
				stream.setf(flags);
			stream << val;
			return stream.str();
		}
		nstring StringUtilsLocalized::UIntToNarrowString(uint64 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			ostringstream stream;
			stream.width(width);
			stream.fill(fill);
			if (flags)
				stream.setf(flags);
			stream << val;
			return stream.str();
		}
		nstring StringUtilsLocalized::SingleToNarrowString(float val, uint16 precision, uint16 width, char fill, std::ios::fmtflags flags)
		{
			ostringstream stream;
			stream.precision(precision);
			stream.width(width);
			stream.fill(fill);

			stream.setf(flags, std::ios::floatfield);
			stream << val;
			return stream.str();
		}

		nstring StringUtilsLocalized::IntToNarrowString(int16 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			return IntToNarrowString((int64)val, width, fill, flags);
		}
		nstring StringUtilsLocalized::IntToNarrowString(int32 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			return IntToNarrowString((int64)val, width, fill, flags);
		}
		nstring StringUtilsLocalized::UIntToNarrowString(uint16 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			return UIntToNarrowString((uint64)val, width, fill, flags);
		}
		nstring StringUtilsLocalized::UIntToNarrowString(uint32 val, uint16 width, char fill, std::ios::fmtflags flags)
		{
			return UIntToNarrowString((uint64)val, width, fill, flags);
		}


	}

}