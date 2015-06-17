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
#include "docShaderNet.h"

#include "APDesigner/MainWindow.h"
#include "ShaderGraph.h"
#include "ShaderDataIO.h"

namespace APDesigner
{
	ShaderNetDocument::ShaderNetDocument(MainWindow* window, EditorExtension* ext, const String& file)
		: Document(window, ext), m_filePath(file), m_stateGraph(0)
	{
		getDocumentForm()->setTitle(file);
		getDocumentForm()->setMinimumSize(Point(970,635));

		getDocumentForm()->eventResized.Bind(this, &ShaderNetDocument::Form_Resized);

		Form_Resized(nullptr);
	}

	ShaderNetDocument::~ShaderNetDocument()
	{
		getDocumentForm()->eventResized.Unbind(this, &ShaderNetDocument::Form_Resized);

		if (m_stateGraph)
			delete m_stateGraph;
	}
	void ShaderNetDocument::Initialize(RenderDevice* device)
	{


		Document::Initialize(device);

	}
	void ShaderNetDocument::LoadRes()
	{
		if (m_stateGraph)
		{
			delete m_stateGraph;
		}


		ShaderDocumentData data;
		data.Load(m_filePath);


	}
	void ShaderNetDocument::SaveRes()
	{

	}

	/** Similar to model document, the content: graph is rendered to a separate render target
	  * which is latter presented on GUI.
	  * But here we do it manually.
	  */
	void ShaderNetDocument::Render()
	{
		RenderDevice* device = getMainWindow()->getDevice();

		device->SetRenderTarget(0, m_renderTarget);
		
		if (m_stateGraph)
		{
			m_stateGraph->Draw();
		}
		
		device->SetRenderTarget(0,0);

		m_graphRender = m_renderTarget->GetColorTexture();
	}
	void ShaderNetDocument::Update(const GameTime* time)
	{
		if (m_stateGraph)
		{
			m_stateGraph->Update(time);
		}

		Document::Update(time);
	}
	void ShaderNetDocument::Form_Resized(Control* ctrl)
	{
		DELETE_AND_NULL(m_renderTarget);
		DELETE_AND_NULL(m_depthStencil);

		ObjectFactory* fac = getMainWindow()->getDevice()->getObjectFactory();
		m_renderTarget = fac->CreateRenderTarget(getDocumentForm()->getSize(), FMT_X8R8G8B8);
		m_depthStencil = fac->CreateDepthStencilBuffer(getDocumentForm()->getSize(), DEPFMT_Depth24X8);
	}
}