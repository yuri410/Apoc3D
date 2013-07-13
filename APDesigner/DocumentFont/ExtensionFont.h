#pragma once

#ifndef APOC3D_EXTENSIONFONT_H
#define APOC3D_EXTENSIONFONT_H

#include "APDesigner/EditorExtensionManager.h"

namespace APDesigner
{
	/** Model editor extension
	*/
	class ExtensionFont : public EditorExtension
	{
	public:
		virtual String GetName() { return L"Font Checker"; }
		virtual Document* OpenItem(const ProjectItem* item);
		virtual Document* DirectOpen(const String& filePath);

		virtual bool SupportsItem(const ProjectItem* item);

		virtual bool SupportsIndependentEditing() { return true; };

		virtual List<String> GetFileExtensions() 
		{
			List<String> r;
			r.Add(L".ttf");
			r.Add(L".ttc");
			r.Add(L".otf");
			return r;
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