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

#include "Input/InputAPI.h"
#include "Input/Mouse.h"

#include "Graphics/Animation/AnimationData.h"
#include "Graphics/Animation/AnimationManager.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/ObjectFactory.h"
#include "Graphics/RenderSystem/RenderStateManager.h"
#include "Graphics/RenderSystem/RenderTarget.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/ModelManager.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Graphics/Camera.h"

#include "Scene/SceneRenderer.h"
#include "Scene/SceneProcedure.h"
#include "Scene/SceneObject.h"

#include "UILib/Form.h"
#include "UILib/Button.h"
#include "UILib/PictureBox.h"
#include "UILib/FontManager.h"
#include "Utility/StringUtils.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"

//#include <d3dx9.h>

using namespace Apoc3D::Input;
using namespace Apoc3D::Utility;

namespace APDesigner
{
	ModelDocument::ModelDocument(MainWindow* window, const String& name, const String& file, const String& animationFile)
		: Document(window), m_filePath(file), m_animPath(animationFile), 
		m_name(name), m_model(0), m_modelSData(0), m_animData(0),
		m_distance(7),m_xang(ToRadian(45)),m_yang(ToRadian(45))
	{
		
		m_sceneRenderer = new SceneRenderer(window->getDevice());

		FileLocation* fl = FileSystem::getSingleton().Locate(L"ModelViewSceneRenderer.xml", FileLocateRule::Default);

		XMLConfiguration* config = new XMLConfiguration(fl);
		m_sceneRenderer->Load(config);
		delete config;
		delete fl;
		m_camera = new ChaseCamera();
		m_camera->setChaseDirection(Vector3Utils::LDVector(0, 0, 1));
		m_camera->setChasePosition(Vector3Utils::LDVector(0, 0, 0));
		m_camera->setDesiredOffset(Vector3Utils::LDVector(0, 0, 40));
		m_camera->setLookAtOffset(Vector3Utils::Zero);
		m_camera->setFar(10000);
		m_camera->setNear(5);
		m_sceneRenderer->RegisterCamera(m_camera);

		m_scene.AddObject(&m_object);

		m_pictureBox = new PictureBox(Point(5,5 + 17), 1);
		m_pictureBox->Size = Point(512,512);
		m_pictureBox->SetSkin(window->getUISkin());
		m_pictureBox->eventPictureDraw().bind(this, &ModelDocument::PixtureBox_Draw);

		getDocumentForm()->setMinimumSize(Point(512+200+10,512+45));
		getDocumentForm()->setTitle(file);

	}

	ModelDocument::~ModelDocument()
	{
		delete m_pictureBox;
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
			m_object.setmdl(0);
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
		m_object.setmdl(m_model);
		
		//m_texture = TextureManager::getSingleton().CreateUnmanagedInstance(getMainWindow()->getDevice(), fl, false);
	}
	void ModelDocument::SaveRes()
	{

	}
	void ModelDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_pictureBox);
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
		m_scene.Update(time);
		m_camera->Update(time);

		Apoc3D::Math::Rectangle rect = m_pictureBox->getAbsoluteArea();

		Mouse* mouse = InputAPIManager::getSingleton().getMouse();
		
		if (rect.Contains(mouse->GetCurrentPosition()))
		{
			if (mouse->IsRightPressedState())
			{
				m_xang += ToRadian(mouse->getDX()) * 0.5f;
				m_yang += ToRadian(mouse->getDY()) * 0.5f;
			}
			else if (mouse->IsLeftPressedState())
			{
				const Matrix& ori = m_object.getOrientation();
				Matrix temp;
				Matrix::CreateRotationY(temp, ToRadian(mouse->getDX()) * 0.5f);
				//Matrix::CreateTranslation(temp, mouse->getDX(), 0,0);
				Matrix temp2;
				Matrix::Multiply(temp2, ori, temp);
				//D3DXMatrixMultiply(reinterpret_cast<D3DXMATRIX*>(&temp), reinterpret_cast<const D3DXMATRIX*>(&ori), reinterpret_cast<D3DXMATRIX*>(&temp));
				//assert(temp == temp2);
				m_object.setOrientation(temp2);
			}

			m_distance -= 0.5f * mouse->getDZ();
		}
		m_camera->setChaseDirection(Vector3Utils::LDVector(cosf(m_xang), -sinf(m_yang), sinf(m_xang)));
		m_camera->setDesiredOffset(Vector3Utils::LDVector(0,0,m_distance));
	}
	void ModelDocument::Render()
	{
		m_sceneRenderer->RenderScene(&m_scene);
	}

	void ModelDocument::PixtureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		SceneProcedure* proc = m_sceneRenderer->getSelectedProc();

		if (proc)
		{
			RenderTarget* rt = proc->FindRenderTargetVar(L"View");

			Texture* texture = rt->GetColorTexture();
			sprite->Draw(texture, *dstRect, 0, CV_White);
		}
		//{
		//	Texture* alphaGrid = UIResources::GetTexture(L"alphagrid");
		//	int x ,y;
		//	for (x =0;x<dstRect->Width;x+=alphaGrid->getWidth())
		//	{
		//		for (y =0;y<dstRect->Height;y+=alphaGrid->getHeight())
		//		{
		//			Apoc3D::Math::Rectangle destRect(dstRect->X+x,dstRect->Y+y,dstRect->Width-x, dstRect->Height-y);
		//			if (destRect.Width>alphaGrid->getWidth())
		//				destRect.Width = alphaGrid->getWidth();
		//			if (destRect.Height>alphaGrid->getHeight())
		//				destRect.Height = alphaGrid->getHeight();

		//			Apoc3D::Math::Rectangle srcRect(0,0,destRect.Width, destRect.Height);
		//			sprite->Draw(alphaGrid, destRect,&srcRect, CV_White);
		//		}
		//	}


		//}

		//if (m_texture)
		//{

		//	float scale = powf(2, (float)m_scale);


		//	//Point newSize = m_pictureBox->Size;
		//	Apoc3D::Math::Rectangle dr(*dstRect);
		//	if (dr.Width > (int)(m_texture->getWidth()*scale))
		//	{
		//		dr.Width = (int)(m_texture->getWidth()*scale);
		//	}
		//	if (dr.Height > (int)(m_texture->getHeight()*scale))
		//	{
		//		dr.Height = (int)(m_texture->getHeight()*scale);
		//	}

		//	Apoc3D::Math::Rectangle srcRect(0,0,(int)(dr.Width/scale),(int)(dr.Height/scale));
		//	if (srcRect.Width > m_texture->getWidth())
		//		srcRect.Width = m_texture->getWidth();
		//	if (srcRect.Height > m_texture->getHeight())
		//		srcRect.Height = m_texture->getHeight();

		//	sprite->Draw(m_texture,dr,&srcRect,CV_White);
		//	switch(m_texture->getType())
		//	{
		//	case TT_Texture1D:
		//	case TT_Texture2D:
		//		{

		//			String msg = L"Type: 2D.\nFormat: ";
		//			msg.append(PixelFormatUtils::ToString(m_texture->getFormat()));
		//			msg.append(L"\nDimension:");
		//			msg.append(StringUtils::ToString(m_texture->getWidth()));
		//			msg.append(1,'x');
		//			msg.append(StringUtils::ToString(m_texture->getHeight()));
		//			msg.append(L"\nMip Levels:");
		//			msg.append(StringUtils::ToString(m_texture->getLevelCount()));	

		//			m_pictureBox->getFontRef()->DrawString(sprite, msg, Point(5+dr.X, 6+dr.Y), CV_Black);
		//			m_pictureBox->getFontRef()->DrawString(sprite, msg, Point(5+dr.X, 5+dr.Y), CV_White);
		//		}
		//		break;
		//	}

		//	//sprite->Flush();
		//	//if (restoreScissor)
		//	//{
		//	//	manager->setScissorTest(true, &oldScissorRect);
		//	//}
		//	//else
		//	//{
		//	//	manager->setScissorTest(false,0);
		//	//}
		//	//switch(m_texture->getType())
		//	//{
		//	//case TT_Texture2D:
		//	//	if (m_texture->getWidth() < MaxSize.X && m_texture->getHeight() < MaxSize.Y)
		//	//	{
		//	//		newSize.X = m_texture->getWidth();
		//	//		newSize.Y = m_texture->getHeight();		

		//	//	}
		//	//	else
		//	//	{
		//	//		if (m_texture->getWidth()>m_texture->getHeight())
		//	//		{
		//	//			newSize.X = MaxSize.X;
		//	//			newSize.Y = static_cast<int>(m_texture->getHeight() * MaxSize.X / (float)m_texture->getWidth());
		//	//		}
		//	//		else
		//	//		{
		//	//			newSize.X = static_cast<int>(m_texture->getWidth() * MaxSize.Y / (float)m_texture->getHeight());
		//	//			newSize.Y = MaxSize.Y;
		//	//		}
		//	//	}

		//	//	break;
		//	//case TT_Texture1D:
		//	//	if (m_texture->getWidth() >1)
		//	//	{
		//	//		newSize.X = MaxSize.X;
		//	//		newSize.Y = MinSize.Y;
		//	//	}
		//	//	else
		//	//	{
		//	//		newSize.X = MinSize.X;
		//	//		newSize.Y = MaxSize.Y;
		//	//	}
		//	//	break;
		//	//}

		//}
	}
}