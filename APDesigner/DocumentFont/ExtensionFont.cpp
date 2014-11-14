#include "ExtensionFont.h"
#include "FontDocument.h"

namespace APDesigner
{
	Document* ExtensionFont::OpenItem(const ProjectItem* item)
	{
		if (item->getType() == ProjectItemType::FontGlyphDist)
		{
			const Project* prj = item->getProject();
			ProjectResFontGlyphDist* mdl = static_cast<ProjectResFontGlyphDist*>(item->getData());
			String path = mdl->GetAbsoluteDestinationPath(mdl->DestFile);
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
		if (item->getType() == ProjectItemType::FontGlyphDist)
			return true;
		return false;
	}

}