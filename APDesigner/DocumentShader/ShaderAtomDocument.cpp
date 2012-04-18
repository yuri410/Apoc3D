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

#include "ShaderAtomDocument.h"

#include "ShaderAtomType.h"

#include "UILib/Form.h"

namespace APDesigner
{
	ShaderAtomDocument::ShaderAtomDocument(MainWindow* window, const String& name)
		: Document(window), m_atomName(name), m_currentWorkingCopy(0)
	{
		getDocumentForm()->setTitle(L"Shader Atom: " + name);
	}

	ShaderAtomDocument::~ShaderAtomDocument()
	{

	}

	void ShaderAtomDocument::LoadRes()
	{
		ShaderAtomType* requested = ShaderAtomLibraryManager::getSingleton().FindAtomType(m_atomName);
		if (requested)
		{
			if (m_currentWorkingCopy)
			{
				delete m_currentWorkingCopy;
			}

			m_currentWorkingCopy = new ShaderAtomType(*requested);
		}
	}
	void ShaderAtomDocument::SaveRes()
	{
		if (m_currentWorkingCopy)
		{
			ShaderAtomType* requested = ShaderAtomLibraryManager::getSingleton().FindAtomType(m_atomName);
			if (requested)
			{
				requested->UpdateTo(m_currentWorkingCopy);
			}
		}
	}
}