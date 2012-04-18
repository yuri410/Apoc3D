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

#ifndef SHADERATOMTYPE_H
#define SHADERATOMTYPE_H

#include "APDCommon.h"
#include "ShaderNetworkTypes.h"

#include "Core/Singleton.h"
#include "Collections/FastMap.h"
#include "Graphics/EffectSystem/EffectParameter.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics::EffectSystem;

namespace APDesigner
{
	struct ShaderAtomPort
	{
		bool IsInputOrOutput;
		String Name;
		ShaderAtomDataExchangeType DataType;
		EffectParamUsage Usage;

		bool IsTypeCompatible(ShaderAtomDataExchangeType other);
	};

	class ShaderAtomType
	{
	public:

		bool MatchPorts(const ShaderAtomType* other);

		void UpdateTo(const ShaderAtomType* newOne);
	private:
		String m_codeBody;
		String m_name;
		
		
	};

	/** A singleton to keep track of all available shader atom types,
	 *  providing load, add/remove and find function.
	 */
	class ShaderAtomManager : public Singleton<ShaderAtomManager>
	{
	public:
		SINGLETON_DECL_HEARDER(APDesigner::ShaderAtomManager);

		/** Load all atom types from give file.
		 *  Before that all previous loaded atom types will be unloaded,
		 *  if any.
		 */
		void Load(const String& atomLib);
		void AddAtomType(ShaderAtomType* type);
		void RemoveAtomType(ShaderAtomType* type);

		ShaderAtomType* FindAtomType(const String& name);

	private:
		FastMap<String, ShaderAtomType*> m_table;
	};
}

#endif