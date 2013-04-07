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

#ifndef EDITOREXTENSIONMANAGER
#define EDITOREXTENSIONMANAGER

#include "APDCommon.h"

#include "apoc3d/Core/Singleton.h"
#include "apoc3d/Collections/HashMap.h"
#include "apoc3d/Collections/List.h"

using namespace Apoc3D;
using namespace Apoc3D::Core;
using namespace Apoc3D::Collections;

namespace APDesigner
{
	class APDAPI EditorExtension
	{
	public:
		virtual String GetName() = 0;
		virtual Document* OpenItem(const ProjectItem* item) = 0;

		virtual bool SupportsItem(const ProjectItem* item) = 0;

		/** Should this tool appears in the tools menu?
		*/
		virtual bool SupportsIndependentEditing() { return false; };

		virtual Document* DirectOpen(const String& filePath) { return nullptr; }

		/** Extension to use when opening the file dialog
		*/
		virtual List<String> GetFileExtensions() = 0;
	};

	class APDAPI IndenpendentEditorExtension : public EditorExtension
	{
	public:
		virtual bool SupportsIndependentEditing() { return true; };

		virtual Document* NewDocument() = 0;
	};

	class APDAPI IndenpendentEditor
	{
	public:
		virtual bool NeedsSaveAs() { return false; }
		virtual void SaveAs(const String& filePath) = 0;

		virtual void LoadNewDocument() = 0;

	};

	class EditorExtensionManager : public Apoc3D::Core::Singleton<EditorExtensionManager>
	{
	public:
		typedef HashMap<String, EditorExtension*>::Enumerator ExtensionEnumerator;

		SINGLETON_DECL_HEARDER(EditorExtensionManager);

		EditorExtension* FindSuitableExtension(const ProjectItem* item);
		
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

		ExtensionEnumerator GetEnumerator() const { return m_extensions.GetEnumerator(); }

		EditorExtensionManager() { }
	private:
		HashMap<String, EditorExtension*> m_extensions;
	};
}
extern "C"
{
	void RegisterExtension(APDesigner::EditorExtension* ext);
	void UnregisterExtension(APDesigner::EditorExtension* ext);
};

#endif