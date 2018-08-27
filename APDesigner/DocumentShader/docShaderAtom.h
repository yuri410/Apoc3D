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

#ifndef SHADERATOMDOCUMENT_H
#define SHADERATOMDOCUMENT_H

#include "APDesigner/Document.h"

namespace APDesigner
{
	class ShaderAtomDocument : public Document
	{
	public:
		ShaderAtomDocument(MainWindow* window, EditorExtension* ext, const String& name);
		~ShaderAtomDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const GameTime* time);
		virtual void Render();
	private:
		void Form_Resized(Control* ctrl);

		String m_atomName;

		ShaderAtomType* m_currentWorkingCopy;

		TextBox* m_tbName;
		TextBox* m_tbCode;
		ComboBox* m_cbProfile;
		ComboBox* m_cbShaderType;

		ListView* m_lbPorts;

		List<Label*> m_labels;

	};
}

#endif