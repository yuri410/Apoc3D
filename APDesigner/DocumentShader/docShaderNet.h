/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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
		virtual void Update(const GameTime* time);
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