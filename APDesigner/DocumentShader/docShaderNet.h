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

#ifndef AUTOEFFECTDOCUMENT_H
#define AUTOEFFECTDOCUMENT_H

#include "APDesigner/Document.h"

namespace APDesigner
{
	/** The shader net doc provides access to the shader network.
	 *  This document contains a VS and a PS graph(for now). And it can be
	 *  build to an AFX effect by APBuild.
	 */
	class ShaderNetDocument : public Document
	{
	public:
		ShaderNetDocument(MainWindow* window, EditorExtension* ext, const String& file);
		~ShaderNetDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; }

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const AppTime* time);
		virtual void Render();
	private:
		void Form_Resized(Control* ctrl);

		//ButtonRow* m_shaderSwitch;
		
		ShaderGraph* m_stateGraph;
		//ShaderGraph* m_psGraph;
		String m_filePath;

		RenderTarget* m_renderTarget;
		DepthStencilBuffer* m_depthStencil;

		Texture* m_graphRender;
	};
}

#endif