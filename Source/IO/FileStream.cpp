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

#include "FileStream.h"

namespace Apoc3D
{
	namespace IO
	{
		FileStream::FileStream(const String& filename, FileMode mode, FileAccess access)
		{
			m_io = 0; 
			m_in = 0;
			m_out = 0;

			ios::openmode omode;


			switch (mode)
			{
			case FM_Open:

				break;
			case FM_Append:

				break;
			}

			switch (access)
			{
			case FA_Read:
				m_in = new std::ifstream();
				break;
			case FA_Write:
				m_out = new std::ofstream(filename.c_str());
				break;
			case FA_ReadWrite:
				m_io = new std::fstream();
				break;
			}
		}
		FileStream::~FileStream()
		{
			if (m_out)
				delete m_out;
			if (m_in)
				delete m_in;
			if (m_io)
				delete m_io;
		}

		int FileStream::Read(const byte* dest, int count)
		{

		}

		void FileStream::Write(const byte* src, int count)
		{

		}

		void FileStream::Seek(int offset, SeekMode mode)
		{
			ios::seekdir dir;
			switch (mode)
			{
			case SM_Begin:
				dir = ios::beg;
				break;
			case SM_End:
				dir = ios::end;
				break;
			default:
				dir = ios::cur;
				break;
			}

			if (m_out)
				m_out->seekp(offset, dir);
			if (m_in)
				m_in->seekg(offset, dir);
			if (m_io)
			{
				m_io->seekg(offset, dir);
				m_io->seekp(offset, dir);
			}
		}
	};
};