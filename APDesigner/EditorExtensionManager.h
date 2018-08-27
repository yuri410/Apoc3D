#pragma once
/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
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

#ifndef EDITOREXTENSIONMANAGER
#define EDITOREXTENSIONMANAGER

#include "APDCommon.h"

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

	/*class APDAPI IndenpendentEditor
	{
	public:
		virtual bool NeedsSaveAs() { return false; }
		virtual void SaveAs(const String& filePath) = 0;

		virtual void LoadNewDocument() = 0;
	};*/


	class EditorExtensionManager
	{
		SINGLETON_DECL(EditorExtensionManager);

	public:
		typedef HashMap<String, EditorExtension*>::ValueAccessor ExtensionAccessor;

		EditorExtension* FindSuitableExtension(const ProjectItem* item);
		
		EditorExtension* FindExtension(const String& id)
		{
			EditorExtension* ext;
			if (m_extensions.TryGetValue(id, ext))
			{
				return ext;
			}
			return nullptr;
		}
		void RegisterExtension(EditorExtension* ext);
		void UnregisterExtension(EditorExtension* ext);

		ExtensionAccessor GetExtensions() const { return m_extensions.getValueAccessor(); }

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