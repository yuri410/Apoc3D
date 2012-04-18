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

#include "ShaderAtomType.h"

SINGLETON_DECL(APDesigner::ShaderAtomLibraryManager);

namespace APDesigner
{
	/************************************************************************/
	/*  ShaderAtomPort                                                      */
	/************************************************************************/

	// 0 NA
	// 1 direct copy
	// 2 expand copy
	// 3 narrow copy
	// 4 
	static const byte CompatibleTable[ATOMDATA_Count][ATOMDATA_Count]=
	{
		// Int1 2 3 4  2x1 2x2 2x3 2x4  3x1 3x2 3x3 3x4  4x1 4x2 4x3 4x4    Float1 2 3 4  2x1 2x2 2x3 2x4  3x1 3x2 3x3 3x4  4x1 4x2 4x3 4x4   Other
/*Int1  */{   1,2,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        1,2,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*Int2  */{   3,1,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,1,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*Int3  */{   3,3,1,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,3,1,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*Int4  */{   3,3,3,1,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,3,3,1,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*Int2x1*/{   0,0,0,0,   1,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   1,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int2x2*/{   0,0,0,0,   0,  1,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  1,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int2x3*/{   0,0,0,0,   0,  0,  1,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  1,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int2x4*/{   0,0,0,0,   0,  0,  0,  1,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  1,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int3x1*/{   0,0,0,0,   0,  0,  0,  0,   1,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   1,  0,  0,  0,   0,  0,  0,  0,      0},
/*Int3x2*/{   0,0,0,0,   0,  0,  0,  0,   0,  1,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  1,  0,  0,   0,  0,  0,  0,      0},
/*Int3x3*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  1,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  1,  0,   0,  0,  0,  0,      0},
/*Int3x4*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  1,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  1,   0,  0,  0,  0,      0},
/*Int4x1*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  0,  0,  0,      0},
/*Int4x2*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  1,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  1,  0,  0,      0},
/*Int4x3*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  0,      0},
/*Int4x4*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  1,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  1,      0},
/*FLT1  */{   1,2,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        1,2,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*FLT2  */{   3,1,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,1,2,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*FLT3  */{   3,3,1,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,3,1,2,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*FLT4  */{   3,3,3,1,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,        3,3,3,1,   2,  2,  2,  2,   2,  2,  2,  2,   2,  2,  2,  2,      0},
/*FLT2x1*/{   0,0,0,0,   1,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   1,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT2x2*/{   0,0,0,0,   0,  1,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  1,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT2x3*/{   0,0,0,0,   0,  0,  1,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  1,  0,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT2x4*/{   0,0,0,0,   0,  0,  0,  1,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  1,   0,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT3x1*/{   0,0,0,0,   0,  0,  0,  0,   1,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   1,  0,  0,  0,   0,  0,  0,  0,      0},
/*FLT3x2*/{   0,0,0,0,   0,  0,  0,  0,   0,  1,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  1,  0,  0,   0,  0,  0,  0,      0},
/*FLT3x3*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  1,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  1,  0,   0,  0,  0,  0,      0},
/*FLT3x4*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  1,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  1,   0,  0,  0,  0,      0},
/*FLT4x1*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   1,  0,  0,  0,      0},
/*FLT4x2*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  1,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  1,  0,  0,      0},
/*FLT4x3*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  1,  0,      0},
/*FLT4x4*/{   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  1,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  1,      0},

		  {   0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,        0,0,0,0,   0,  0,  0,  0,   0,  0,  0,  0,   0,  0,  0,  0,      4}

	};

	bool ShaderAtomPort::IsTypeCompatible(ShaderAtomDataExchangeType other)
	{
		byte r = CompatibleTable[DataType][other];
		if (r==4)
		{

		}
		return !!r;
	}

	/************************************************************************/
	/*  ShaderAtomType                                                      */
	/************************************************************************/
	bool ShaderAtomType::MatchPorts(const ShaderAtomType* other)
	{
		return false;
	}

	void ShaderAtomType::CopyFrom(const ShaderAtomType* newOne)
	{
		m_codeBody = newOne->m_codeBody;
		m_name = newOne->m_name;
		m_minorSMVersion = newOne->m_minorSMVersion;
		m_majorSMVersion = newOne->m_majorSMVersion;
	}

	/************************************************************************/
	/*  ShaderAtomLibraryManager                                            */
	/************************************************************************/

	void ShaderAtomLibraryManager::Load(const String& atomLib)
	{

	}

	void ShaderAtomLibraryManager::AddAtomType(ShaderAtomType* type)
	{
		m_table.Add(type->getName(), type);
	}
	void ShaderAtomLibraryManager::RemoveAtomType(ShaderAtomType* type)
	{
		m_table.Remove(type->getName());
	}

	ShaderAtomType* ShaderAtomLibraryManager::FindAtomType(const String& name)
	{
		ShaderAtomType* result = 0;
		m_table.TryGetValue(name, result);
		return result;
	}
}