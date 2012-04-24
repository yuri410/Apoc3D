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
#include "SNetCommonTypes.h"

#include "Core/Singleton.h"
#include "Collections/FastMap.h"
#include "Graphics/EffectSystem/EffectParameter.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::VFS;

namespace APDesigner
{
	/** A part of one ShaderAtomType, defines the input and outputs of a
	 *  ShaderAtomType.
	 */
	struct ShaderAtomPort
	{
		bool IsInputOrOutput;
		String Name;
		ShaderAtomDataExchangeType DataType;

		/** used for auto binding a varying input node
		*/
		String VaringTypeName;
		/** used for auto binding a constant input node.
		 *  custom constant is not the case here.
		 */
		EffectParamUsage Usage;

		bool IsTypeCompatible(ShaderAtomDataExchangeType other);

		void Parse(ConfigurationSection* sect);
		ConfigurationSection* Save();
	};

	/** Represents a shader atom type
	*/
	class ShaderAtomType
	{
	public:
		/** Tells if the atom type's port setting can automatically match the other one's
		 *  ports based on the usage information.
		 */
		bool MatchPorts(const ShaderAtomType* other);

		void CopyFrom(const ShaderAtomType* newOne);


		const String& getName() const { return m_name; }
		const String& getCodeBody() const { return m_codeBody; }

		ShaderType getShaderType() const { return m_type; }

		/** minimum SM version required to use this atom type
		*/
		int getMajorSMVersion() const { return m_majorSMVersion; }
		int getMinorSMVersion() const { return m_minorSMVersion; }


		void Load(const String& filePath);
		void Save(const String& filePath);
	private:
		List<ShaderAtomPort> m_ports;

		String m_codeBody;
		String m_name;
		
		ShaderType m_type;
		int m_majorSMVersion;
		int m_minorSMVersion;
	};

	/** A singleton to keep track of all available shader atom types,
	 *  providing load, add/remove and find function.
	 *
	 *  This also provides all available shader output nodes.
	 */
	class ShaderAtomLibraryManager : public Singleton<ShaderAtomLibraryManager>
	{
	public:
		SINGLETON_DECL_HEARDER(APDesigner::ShaderAtomLibraryManager);

		typedef FastMap<String, ShaderAtomType*>::Enumerator LibraryEnumerator;

		/** Load all atom types from give file.
		 *  Before that all previous loaded atom types will be unloaded,
		 *  if any.
		 */
		void Load(const FileLocation* fl);
		void AddAtomType(ShaderAtomType* type);
		void RemoveAtomType(ShaderAtomType* type);

		ShaderAtomType* FindAtomType(const String& name);

		LibraryEnumerator GetEnumerator() { return m_table.GetEnumerator(); }
	private:
		FastMap<String, ShaderAtomType*> m_table;
	};

}

#endif