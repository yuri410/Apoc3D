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
#include "ModelDocument.h"
#include "MainWindow.h"
#include "UIResources.h"

#include "Core/ResourceHandle.h"

#include "Config/XmlConfiguration.h"

#include "Graphics/Animation/AnimationData.h"
#include "Graphics/Animation/AnimationManager.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/RenderStateManager.h"
#include "Graphics/ModelManager.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Graphics/Camera.h"

#include "Scene/SceneRenderer.h"

#include "UILib/Form.h"
#include "UILib/Button.h"
#include "UILib/FontManager.h"
#include "Utility/StringUtils.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"

using namespace Apoc3D::Utility;

namespace APDesigner
{
	ModelDocument::ModelDocument(MainWindow* window, const String& name, const String& file, const String& animationFile)
		: Document(window), m_filePath(file), m_animPath(animationFile), 
		m_name(name), m_model(0), m_modelSData(0), m_animData(0)
	{
		getDocumentForm()->setTitle(file);

		m_sceneRenderer = new SceneRenderer(window->getDevice());

		FileLocation* fl = FileSystem::getSingleton().Locate(L"ModelViewSceneRenderer.xml", FileLocateRule::Default);

		XMLConfiguration* config = new XMLConfiguration(fl);
		m_sceneRenderer->Load(config);
		delete config;
		delete fl;
		m_camera = new ChaseCamera();
		m_sceneRenderer->RegisterCamera(m_camera);
	}

	ModelDocument::~ModelDocument()
	{
		if (m_model)
		{
			delete m_model;


		}
		delete m_sceneRenderer;
		delete m_camera;
		
		//delete m_modelSDat/*a*/;;
	}
	

	void ModelDocument::LoadRes()
	{
		if (m_model)
		{
			delete m_model;
			//delete m_modelSData;
		}
		FileLocation* fl = new FileLocation(m_filePath);
		
		m_modelSData = ModelManager::getSingleton().CreateInstanceUnmanaged(getMainWindow()->getDevice(), fl);
		if (m_animPath.size())
		{
			FileLocation* afl = new FileLocation(m_animPath);
			m_animData = AnimationManager::getSingleton().CreateInstance(afl);
			delete afl;
		}
		
		m_model = new Model(new ResourceHandle<ModelSharedData>(m_modelSData,true), m_animData);

		//m_texture = TextureManager::getSingleton().CreateUnmanagedInstance(getMainWindow()->getDevice(), fl, false);
	}
	void ModelDocument::SaveRes()
	{

	}
	void ModelDocument::Initialize(RenderDevice* device)
	{
		//getDocumentForm()->getControls().Add(m_pictureBox);
		//getDocumentForm()->getControls().Add(m_btnZoomIn);
		//getDocumentForm()->getControls().Add(m_btnZoomOut);

		Document::Initialize(device);

	}
	void ModelDocument::Update(const GameTime* const time)
	{
		//m_pictureBox->Size = getDocumentForm()->Size;
		//m_pictureBox->Size.X -= m_pictureBox->Position.X*2;
		//m_pictureBox->Size.Y -= m_pictureBox->Position.Y*2;

		//m_btnZoomIn->Position.X = m_pictureBox->Size.X-65;
		//m_btnZoomOut->Position.X = m_pictureBox->Size.X-30;
		
	}
	void ModelDocument::Render()
	{

	}

}