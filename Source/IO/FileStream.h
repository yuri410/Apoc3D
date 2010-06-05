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
#ifndef FILESTREAM_H
#define FILESTREAM_H

#pragma once

#include "..\Common.h"
#include "Stream.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace Apoc3D
{
	namespace IO
	{

		enum _Export FileMode
		{
			FM_Open,
			FM_OpenCreate,
			FM_Append
		};
		enum _Export FileAccess
		{
			FA_Read,
			FA_Write,
			FA_ReadWrite
		};

		class _Export FileStream : public Stream
		{
		private:
			ofstream* m_out;
			ifstream* m_in;
			fstream* m_io;

		public:
			FileStream(const String& filename, FileMode mode, FileAccess access);
			~FileStream();

			int Read(const byte* dest, int count);
			void Write(const byte* src, int count);

			void Seek(int offset, SeekMode mode);
		};
	};
};

#endif