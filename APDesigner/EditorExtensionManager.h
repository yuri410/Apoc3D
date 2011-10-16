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

#ifndef EDITOREXTENSIONMANAGER
#define EDITOREXTENSIONMANAGER

#include "APDCommon.h"
#include "Core/Singleton.h"
#include "Collections/FastMap.h"

#if APOC3DES_DLLEX
#define APDAPI __declspec( dllexport )
#else
#define APDAPI __declspec( dllimport )
#endif

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;

namespace APDesigner
{
	class EditorExtension
	{
	public:
		virtual String GetName() = 0;
		virtual Document* OpenItem(const ProjectCustomItem* item);
	};

	class APDAPI EditorExtensionManager : public Singleton<EditorExtensionManager>
	{
	public:
		SINGLETON_DECL_HEARDER(APDesigner::EditorExtensionManager);

		EditorExtension* FindExtension(const String& id)
		{
			EditorExtension* ext;
			if (m_extensions.TryGetValue(id, ext))
			{
				return ext;
			}
			return 0;
		}
		void RegisterExtension(EditorExtension* ext);
		void UnregisterExtension(EditorExtension* ext);
	private:
		FastMap<String, EditorExtension*> m_extensions;
	};
}

#endif