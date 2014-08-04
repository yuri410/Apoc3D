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
#include "ConstantTable.h"

#include "apoc3d/ApocException.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Utility/StringUtils.h"

using namespace Apoc3D::Core;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace D3D9RenderSystem
		{
			void ConstantTable::ThrowKeyNotFoundEx(const String& name)
			{
				throw AP_EXCEPTION(ApocExceptionType::KeyNotFound, name);
			}
			

			ConstantTable::ConstantTable(const DWORD* bytes)
			{
				// http://msdn.microsoft.com/en-us/library/ff552891.aspx
				const DWORD* ptr = bytes;

				while (*ptr != D3DSIO_END)
				{
					DWORD cur = *ptr;

					if ((cur & D3DSI_OPCODE_MASK) == D3DSIO_COMMENT)
					{
						DWORD commentBlobSize = (cur & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;
						DWORD commentBlobName = *(ptr+1);

						// this is a section added by APBuild
						// constant information can be retrieved from this
						if (commentBlobName == 'APBM' && commentBlobSize>2)
						{
							DWORD crc = *(ptr+2);

							char* payload = (char*)(ptr+3);
							int32 payloadSize = (commentBlobSize-2) * sizeof(DWORD); // 2 = 1x blob name + 1x crc

							uint32 realCrc = CalculateCRC32(payload, payloadSize);

							if (crc == realCrc)
							{
								ReadShaderComment((char*)(ptr+3), payloadSize);
							}
							else
							{
								ApocLog(LOG_Graphics, L"Invalid APBM shader constant data.");
							}

							break;
						}
					}
					ptr++;
				}
			}


			void ConstantTable::ReadShaderComment(char* data, int32 size)
			{
				MemoryStream ms(data, size);
				BinaryReader br(&ms);
				br.SuspendStreamRelease();

				Read(&br);

				br.Close();
			}

			ConstantTable::~ConstantTable()
			{

			}



			void ConstantTable::Read(BinaryReader* br)
			{
				int32 constantCount = br->ReadInt32();

				for (int32 i=0;i<constantCount;i++)
				{
					ShaderConstant sc;

					char ch = br->ReadByte();
					while (ch)
					{
						sc.Name.append(1, ch);
						ch = br->ReadByte();
					}

					sc.RegisterIndex = br->ReadUInt16();
					sc.RegisterCount = br->ReadUInt16();
					sc.SamplerIndex = br->ReadByte();
					sc.IsSampler = br->ReadBoolean();


					sc.RegisterSet = (ShaderRegisterSetType)br->ReadByte();
					sc.Class = (ShaderConstantClass)br->ReadByte();
					sc.Type = (ShaderConstantType)br->ReadByte();
					sc.Rows = br->ReadByte();
					sc.Columns = br->ReadByte();
					sc.Elements = br->ReadUInt16();
					sc.StructMembers = br->ReadUInt16();
					sc.SizeInBytes = br->ReadUInt32();


					m_table.Add(sc.Name, sc);
				}
			}
			void ConstantTable::Write(BinaryWriter* bw)
			{
				bw->WriteInt32(m_table.getCount());

				for (HashMap<String, ShaderConstant>::Enumerator e = m_table.GetEnumerator();e.MoveNext();)
				{
					const ShaderConstant& sc = e.getCurrentValue();

					std::string name = StringUtils::toASCIINarrowString(sc.Name);
					bw->Write(name.c_str(), name.size()+1);

					bw->WriteUInt16(sc.RegisterIndex);
					bw->WriteUInt16(sc.RegisterCount);
					bw->WriteByte(sc.SamplerIndex);
					bw->WriteBoolean(sc.IsSampler);

					bw->WriteByte(sc.RegisterSet);
					bw->WriteByte(sc.Class);
					bw->WriteByte(sc.Type);
					bw->WriteByte(sc.Rows);
					bw->WriteByte(sc.Columns);
					bw->WriteUInt16(sc.Elements);
					bw->WriteUInt16(sc.StructMembers);
					bw->WriteUInt32(sc.SizeInBytes);
				}
			}
		}
	}
}