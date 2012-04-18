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
#include "AutoEffectDocument.h"

#include "Utility/StringUtils.h"
#include "UILib/Form.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/RenderTarget.h"

#include "MainWindow.h"
#include "ShaderGraph.h"

using namespace Apoc3D::Input;
using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;
using namespace Apoc3D::Graphics::RenderSystem;
//using namespace APDesigner::CommonDialog;

namespace APDesigner
{
	ShaderNetDocument::ShaderNetDocument(MainWindow* window, const String& file)
		: Document(window), m_filePath(file), m_graph(0)
	{
		getDocumentForm()->setTitle(file);
		getDocumentForm()->eventResized().bind(this, &ShaderNetDocument::Form_Resized);

		ObjectFactory* fac = window->getDevice()->getObjectFactory();
		m_renderTarget = fac->CreateRenderTarget(getDocumentForm()->Size.X,getDocumentForm()->Size.Y, FMT_X8R8G8B8, DEPFMT_Depth24X8);
	}

	ShaderNetDocument::~ShaderNetDocument()
	{
		if (m_graph)
			delete m_graph;
	}

	void ShaderNetDocument::LoadRes()
	{
		if (m_graph)
		{
			delete m_graph;
		}


	}

	/** Similar to model document, the content: graph is rendered to a separate render target
	  * which is latter presented on GUI.
	  * But here we do it manually.
	  */
	void ShaderNetDocument::Render()
	{
		RenderDevice* device = getMainWindow()->getDevice();

		device->SetRenderTarget(0, m_renderTarget);
		if (m_graph)
		{
			m_graph->Draw();
		}
		device->SetRenderTarget(0,0);

		m_graphRender = m_renderTarget->GetColorTexture();
	}

	void ShaderNetDocument::Form_Resized(Control* ctrl)
	{
		delete m_renderTarget;

		ObjectFactory* fac = getMainWindow()->getDevice()->getObjectFactory();
		m_renderTarget = fac->CreateRenderTarget(getDocumentForm()->Size.X,getDocumentForm()->Size.Y, FMT_X8R8G8B8, DEPFMT_Depth24X8);
	}
}