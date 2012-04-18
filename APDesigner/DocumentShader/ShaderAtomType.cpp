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

SINGLETON_DECL(APDesigner::ShaderAtomManager);

namespace APDesigner
{
	/************************************************************************/
	/*  ShaderAtomPort                                                      */
	/************************************************************************/
	bool ShaderAtomPort::IsTypeCompatible(ShaderAtomDataExchangeType other)
	{
		return false;
	}

	/************************************************************************/
	/*  ShaderAtomType                                                      */
	/************************************************************************/
	bool ShaderAtomType::MatchPorts(const ShaderAtomType* other)
	{
		return false;
	}

	void ShaderAtomType::UpdateTo(const ShaderAtomType* newOne)
	{
		m_codeBody = newOne->m_codeBody;
		m_name = newOne->m_name;
		m_minorSMVersion = newOne->m_minorSMVersion;
		m_majorSMVersion = newOne->m_majorSMVersion;
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void ShaderAtomManager::Load(const String& atomLib)
	{

	}

	void ShaderAtomManager::AddAtomType(ShaderAtomType* type)
	{
		m_table.Add(type->getName(), type);
	}
	void ShaderAtomManager::RemoveAtomType(ShaderAtomType* type)
	{
		m_table.Remove(type->getName());
	}

	ShaderAtomType* ShaderAtomManager::FindAtomType(const String& name)
	{
		ShaderAtomType* result = 0;
		m_table.TryGetValue(name, result);
		return result;
	}
}