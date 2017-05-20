#include "ExtensionFont.h"
#include "FontDocument.h"

namespace APDesigner
{
	Document* ExtensionFont::OpenItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::Font)
		{
			const Project* prj = item->getProject();
			ProjectResFont* font = static_cast<ProjectResFont*>(item->getData());
			String path = font->GetAbsoluteDestinationPath(font->DestFile);
			if (File::FileExists(path))
			{
				FontDocument* md = new FontDocument(m_mainWindow, this, path);
				return md;
			}
		}
		return nullptr;
	}
	Document* ExtensionFont::DirectOpen(const String& filePath)
	{
		return new FontDocument(m_mainWindow, this, filePath);
	}

	bool ExtensionFont::SupportsItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::Font)
			return true;
		return false;
	}

}