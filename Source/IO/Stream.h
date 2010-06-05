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

#ifndef STREAM_H
#define STREAM_H

#pragma once

#include "..\Common.h"

namespace Apoc3D
{
	namespace IO
	{
		enum _Export SeekMode
		{
			SM_Begin,
			SM_Current,
			SM_End
		};
		/* 
		*/
		class _Export Stream
		{
		protected:
			Stream() {}
			~Stream() {}

		public:

			virtual int Read(const byte* dest, int count) = 0;
			virtual int ReadByte()
			{
				byte buffer;
				if (Read(&buffer, 1) == 0)
				{
					return -1;
				}
				return buffer;
			}

			
			virtual void Write(const byte* src, int count) = 0;
			virtual void WriteByte(byte value)
			{
				Write(&value, 1);
			}

			virtual void Seek(int offset, SeekMode mode) = 0;
		};
	};
}

#endif