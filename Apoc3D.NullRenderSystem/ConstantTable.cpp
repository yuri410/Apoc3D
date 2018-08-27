
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "ConstantTable.h"

#include "apoc3d/Exception.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Utility/StringUtils.h"

enum D3DSHADER_INSTRUCTION_OPCODE_TYPE
{
	D3DSIO_NOP = 0,
	D3DSIO_MOV,
	D3DSIO_ADD,
	D3DSIO_SUB,
	D3DSIO_MAD,
	D3DSIO_MUL,
	D3DSIO_RCP,
	D3DSIO_RSQ,
	D3DSIO_DP3,
	D3DSIO_DP4,
	D3DSIO_MIN,
	D3DSIO_MAX,
	D3DSIO_SLT,
	D3DSIO_SGE,
	D3DSIO_EXP,
	D3DSIO_LOG,
	D3DSIO_LIT,
	D3DSIO_DST,
	D3DSIO_LRP,
	D3DSIO_FRC,
	D3DSIO_M4x4,
	D3DSIO_M4x3,
	D3DSIO_M3x4,
	D3DSIO_M3x3,
	D3DSIO_M3x2,
	D3DSIO_CALL,
	D3DSIO_CALLNZ,
	D3DSIO_LOOP,
	D3DSIO_RET,
	D3DSIO_ENDLOOP,
	D3DSIO_LABEL,
	D3DSIO_DCL,
	D3DSIO_POW,
	D3DSIO_CRS,
	D3DSIO_SGN,
	D3DSIO_ABS,
	D3DSIO_NRM,
	D3DSIO_SINCOS,
	D3DSIO_REP,
	D3DSIO_ENDREP,
	D3DSIO_IF,
	D3DSIO_IFC,
	D3DSIO_ELSE,
	D3DSIO_ENDIF,
	D3DSIO_BREAK,
	D3DSIO_BREAKC,
	D3DSIO_MOVA,
	D3DSIO_DEFB,
	D3DSIO_DEFI,

	D3DSIO_TEXCOORD = 64,
	D3DSIO_TEXKILL,
	D3DSIO_TEX,
	D3DSIO_TEXBEM,
	D3DSIO_TEXBEML,
	D3DSIO_TEXREG2AR,
	D3DSIO_TEXREG2GB,
	D3DSIO_TEXM3x2PAD,
	D3DSIO_TEXM3x2TEX,
	D3DSIO_TEXM3x3PAD,
	D3DSIO_TEXM3x3TEX,
	D3DSIO_RESERVED0,
	D3DSIO_TEXM3x3SPEC,
	D3DSIO_TEXM3x3VSPEC,
	D3DSIO_EXPP,
	D3DSIO_LOGP,
	D3DSIO_CND,
	D3DSIO_DEF,
	D3DSIO_TEXREG2RGB,
	D3DSIO_TEXDP3TEX,
	D3DSIO_TEXM3x2DEPTH,
	D3DSIO_TEXDP3,
	D3DSIO_TEXM3x3,
	D3DSIO_TEXDEPTH,
	D3DSIO_CMP,
	D3DSIO_BEM,
	D3DSIO_DP2ADD,
	D3DSIO_DSX,
	D3DSIO_DSY,
	D3DSIO_TEXLDD,
	D3DSIO_SETP,
	D3DSIO_TEXLDL,
	D3DSIO_BREAKP,

	D3DSIO_PHASE = 0xFFFD,
	D3DSIO_COMMENT = 0xFFFE,
	D3DSIO_END = 0xFFFF,

	D3DSIO_FORCE_DWORD = 0x7fffffff,   // force 32-bit size enum
};

#define D3DSI_OPCODE_MASK		    0x0000FFFF
#define D3DSI_COMMENTSIZE_SHIFT     16
#define D3DSI_COMMENTSIZE_MASK      0x7FFF0000

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Graphics
	{
		namespace NullRenderSystem
		{
			void ConstantTable::ThrowKeyNotFoundEx(const String& name)
			{
				throw AP_EXCEPTION(ExceptID::KeyNotFound, name);
			}
			

			ConstantTable::ConstantTable(const byte* bytes)
			{
				// http://msdn.microsoft.com/en-us/library/ff552891.aspx
				const uint32* ptr = (const uint32*)bytes;

				while (*ptr != D3DSIO_END)
				{
					uint32 cur = *ptr;

					if ((cur & D3DSI_OPCODE_MASK) == D3DSIO_COMMENT)
					{
						uint32 commentBlobSize = (cur & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;
						uint32 commentBlobName = *(ptr+1);

						// this is a section added by APBuild
						// constant information can be retrieved from this
						if (commentBlobName == 'APBM' && commentBlobSize>2)
						{
							uint32 crc = *(ptr+2);

							char* payload = (char*)(ptr+3);
							int32 payloadSize = (commentBlobSize-2) * sizeof(uint32); // 2 = 1x blob name + 1x crc

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
				BinaryReader br(&ms, false);

				Read(&br);
			}

			ConstantTable::~ConstantTable()
			{

			}


			void ConstantTable::Read(BinaryReader* br)
			{
				int32 constantCount = br->ReadInt32();

				for (int32 i = 0; i < constantCount; i++)
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

				for (const ShaderConstant& sc : m_table.getValueAccessor())
				{
					std::string name = StringUtils::toASCIINarrowString(sc.Name);
					bw->WriteBytes(name.c_str(), name.size()+1);

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