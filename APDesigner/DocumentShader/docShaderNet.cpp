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
	void ShaderNetDocument::Update(const AppTime* time)
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