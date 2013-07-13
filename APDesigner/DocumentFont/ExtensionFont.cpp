#include "ExtensionFont.h"
#include "FontDocument.h"
#include "apoc3d/Project/Project.h"
#include "apoc3d/Vfs/PathUtils.h"
#include "apoc3d/Vfs/File.h"

using namespace Apoc3D::VFS;

namespace APDesigner
{
	Document* ExtensionFont::OpenItem(const ProjectItem* item)
	{
		if (item->getType() == PRJITEM_FontGlyphDist)
		{
			const Project* prj = item->getProject();
			ProjectResFontGlyphDist* mdl = static_cast<ProjectResFontGlyphDist*>(item->getData());
			String path = PathUtils::Combine(prj->getOutputPath(), mdl->DestFile);
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
		if (item->getType() == PRJITEM_FontGlyphDist)
			return true;
		return false;
	}

}