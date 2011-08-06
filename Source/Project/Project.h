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

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	enum ProjectItemType
	{

	};
	
	class ProjectItem
	{
	private:
		ProjectFolder* m_parent;
		ProjectItemType* m_type;
		ProjectItemTypeData* m_typeData;

		String m_name;
		
	public:
		const String& getName() const { return m_name; }
		ProjectItemType getType() const { return m_type; }

		virtual void Rename(const String& newName)
		{
			m_name = newName;
		}

	};
	class ProjectItemTypeData
	{

	};
	class ProjectCustomItem : public ProjectItemTypeData
	{
	private:

	};
	class ProjectResource : public ProjectItemTypeData
	{

	};
	class ProjectFolder : public ProjectItemTypeData
	{
	private:

	};
	class ProjectResTexture : public ProjectResource
	{
	private:

	};
	class ProjectResModel : public ProjectResource
	{
	private:

	};
	class ProjectResAnimation : public ProjectResource
	{

	};
	class ProjectResEffect : public ProjectResource
	{

	};
	class ProjectResFont : public ProjectResource
	{

	};

	class Project
	{
	private:

	};


}