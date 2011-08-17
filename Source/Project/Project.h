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
#include "Common.h"

#include "Graphics/PixelFormat.h"
#include "Collections/FastList.h"
#include "Collections/FastMap.h"

using namespace Apoc3D;
using namespace Apoc3D::Collections;
using namespace Apoc3D::Config;
using namespace Apoc3D::Graphics;

namespace Apoc3D
{
	/** Holds, load and save project configuration file.
		Include project directory structure, and all parameters used to compile assets to
		final formats.
	*/


	class ProjectItem;

	enum ProjectItemType
	{
		PRJITEM_Custom,
		PRJITEM_Folder,
		PRJITEM_Texture,
		PRJITEM_Model,
		PRJITEM_Animation,
		PRJITEM_Effect,
		PRJITEM_Font
	};
	
	/** Represent the build configuration for a specific type of assert in the project.
	*/
	class ProjectItemData
	{
	private:
		ProjectItemType m_type;

	public:
		virtual ProjectItemType getType() const = 0;
		virtual void Parse(const ConfigurationSection* sect) = 0;

		virtual ~ProjectItemData() { }
	};
	class ProjectCustomItem : public ProjectItemData
	{
	public:
		String DestFile;

		virtual ProjectItemType getType() const { return PRJITEM_Custom; }
		virtual void Parse(const ConfigurationSection* sect);
	};
	class ProjectResource : public ProjectItemData
	{

	};
	class ProjectFolder : public ProjectItemData
	{
	public:
		FastList<ProjectItem*> SubItems;

		virtual ProjectItemType getType() const { return PRJITEM_Folder; }
		virtual void Parse(const ConfigurationSection* sect);
	};

	class ProjectResTexture : public ProjectResource
	{
	public:

		enum TextureFilterType
		{
			TFLT_Nearest,
			TFLT_Box,
			TFLT_BSpline
		};
		enum TextureBuildMethod
		{
			TEXBUILD_D3D,
			TEXBUILD_Devil,
			TEXBUILD_BuiltIn
		};

		String SourceFile;
		String DestinationFile;
		bool GenerateMipmaps;
		bool Resize;
		int NewWidth;
		int NewHeight;
		int NewDepth;
		TextureFilterType ResizeFilterType;
		Apoc3D::Graphics::PixelFormat NewFormat;

		bool AssembleCubemap;
		bool AssembleVolumeMap;

		FastMap<uint, String> SubMapTable;
		FastMap<uint, String> SubAlphaMapTable;

		TextureBuildMethod Method;

		virtual ProjectItemType getType() const { return PRJITEM_Texture; }
		virtual void Parse(const ConfigurationSection* sect);

	};

	class ProjectResModel : public ProjectResource
	{
	public:
		enum MeshBuildMethod
		{
			MESHBUILD_ASS,
			MESHBUILD_FBX
		};


		String SrcFile;
		String DstFile;
		String DstAnimationFile;

		MeshBuildMethod Method;

		virtual ProjectItemType getType() const { return PRJITEM_Model; }
		virtual void Parse(const ConfigurationSection* sect);
	};
	

	class ProjectResEffect : public ProjectResource
	{
	public:
		String SrcFile;
		String PListFile;
		String DestFile;
		String EntryPoint;
		String Profile;

		virtual ProjectItemType getType() const { return PRJITEM_Effect; }
		virtual void Parse(const ConfigurationSection* sect);
	};
	class ProjectResFont : public ProjectResource
	{
	public:
		struct CharRange
		{
			int MinChar;
			int MaxChar;
		};
		enum FontStyle
		{
			FONTSTYLE_Regular,
			FONTSTYLE_Bold,
			FONTSTYLE_Italic,
			FONTSTYLE_BoldItalic,
			FONTSTYLE_Strikeout

		};

		FastList<CharRange> Ranges;
		FontStyle Style;
		String Name;
		float Size;

		String DestFile;

		virtual ProjectItemType getType() const { return PRJITEM_Font; }
		virtual void Parse(const ConfigurationSection* sect);
	};

	/** Represents one asset in the project.
	*/
	class ProjectItem
	{
	private:
		ProjectFolder* m_parent;

		ProjectItemData* m_typeData;

		String m_name;

	public:
		ProjectItem()
			: m_parent(0), m_typeData(0)
		{

		}

		const String& getName() const { return m_name; }
		ProjectItemType getType() const { return m_typeData->getType(); }
		ProjectItemData* getData() const { return m_typeData; }
		virtual void Rename(const String& newName)
		{
			m_name = newName;
		}

		void Parse(const ConfigurationSection* sect);

	};
	class Project
	{
	private:
		FastList<ProjectItem*> m_items;
		String m_name;

		String m_basePath;
		String m_outputPath;
	public:
		const String& getName() const { return m_name; }
		const String& getBasePath() const { return m_basePath; }
		void setBasePath(const String& path);

		const FastList<ProjectItem*>& getItems() const { return m_items; }

		void Parse(const ConfigurationSection* sect);
		void Save(const String& file);
	};


}