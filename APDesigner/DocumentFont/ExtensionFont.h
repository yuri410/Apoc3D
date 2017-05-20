#pragma once

#ifndef APOC3D_EXTENSIONFONT_H
#define APOC3D_EXTENSIONFONT_H

#include "APDesigner/EditorExtensionManager.h"

namespace APDesigner
{
	/** Font viewer extension */
	class ExtensionFont : public EditorExtension
	{
	public:
		virtual String GetName() { return L"Font Viewer"; }
		virtual Document* OpenItem(const ProjectItem* item);
		virtual Document* DirectOpen(const String& filePath);

		virtual bool SupportsItem(const ProjectItem* item);

		virtual bool SupportsIndependentEditing() { return true; };

		virtual List<String> GetFileExtensions() 
		{
			return{ L".fnt" };
		}

		ExtensionFont(MainWindow* wnd)
			: m_mainWindow(wnd)
		{

		}
	private:
		MainWindow* m_mainWindow;
	};
}

#endif