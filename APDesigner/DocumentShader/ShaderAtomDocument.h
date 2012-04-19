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
#ifndef SHADERATOMDOCUMENT_H
#define SHADERATOMDOCUMENT_H

#include "Document.h"

using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Scene;

namespace APDesigner
{
	class ShaderAtomDocument : public Document
	{
	public:
		ShaderAtomDocument(MainWindow* window, const String& name);
		~ShaderAtomDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const GameTime* const time);
		virtual void Render();
	private:
		void Form_Resized(Control* ctrl);

		String m_atomName;

		ShaderAtomType* m_currentWorkingCopy;

		TextBox* m_tbName;
		TextBox* m_tbCode;
		ComboBox* m_cbProfile;
		ComboBox* m_cbShaderType;

		FastList<Label*> m_labels;

	};
}

#endif