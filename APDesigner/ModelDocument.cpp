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

#include "IOLib/ModelData.h"
#include "IOLib/Streams.h"

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
#include "UILib/Label.h"
#include "UILib/FontManager.h"
#include "UILib/ComboBox.h"
#include "UILib/StyleSkin.h"
#include "Utility/StringUtils.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"

//#include <d3dx9.h>

using namespace Apoc3D::Input;
using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;
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

		m_modelViewer = new PictureBox(Point(10,10 + 17+80), 1);
		m_modelViewer->Size = Point(512,512);
		m_modelViewer->SetSkin(window->getUISkin());
		m_modelViewer->eventPictureDraw().bind(this, &ModelDocument::PixtureBox_Draw);

		{
			Label* lbl = new Label(Point(23, 33), L"Time", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			m_pbTime = new PictureBox(Point(94, 27), 1);
			m_pbTime->Size = Point(957, 30);
			m_pbTime->SetSkin(window->getUISkin());
		}
		{
			Label* lbl = new Label(Point(21, 68), L"Material\nKeyframe Props", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			lbl = new Label(Point(141, 73), L"Time", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			lbl = new Label(Point(331, 73), L"Material Sub-Index", 150);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			m_tbMKeyTime = new TextBox(Point(187, 73), 100);
			m_tbMKeyTime->SetSkin(window->getUISkin());


			m_tbMKeyIndex = new TextBox(Point(476, 73), 100);
			m_tbMKeyIndex->SetSkin(window->getUISkin());

			m_btnRefreshTimeLine = new Button(Point(693, 69), L"Refresh");
			m_btnRefreshTimeLine->SetSkin(window->getUISkin());

			m_btnModify = new Button(Point(788, 69), L"Modify");
			m_btnModify->SetSkin(window->getUISkin());

			m_btnAddMkey = new Button(Point(881, 69), L"Add");
			m_btnAddMkey->SetSkin(window->getUISkin());
			m_btnRemoveMKey = new Button(Point(950, 69), L"Remove");
			m_btnRemoveMKey->SetSkin(window->getUISkin());

		}
		{
			Label* lbl = new Label(Point(21 + 522, 107), L"Mesh", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			lbl = new Label(Point(21 + 522, 133), L"Mesh Part", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			List<String> items;
			m_cbMesh = new ComboBox(Point(21+522+100, 107), 200, items);
			m_cbMesh->SetSkin(window->getUISkin());
			m_cbMtrlPart = new ComboBox(Point(21+522+100, 133), 200, items);
			m_cbMtrlPart->SetSkin(window->getUISkin());


			m_applyMtrl = new Button(Point(21 + 522+100+220, 107), L"Apply Changes");
			m_applyMtrl->SetSkin(window->getUISkin());

			m_addMtrlFrame = new Button(Point(21 + 522+100+220, 133), L"Add Sub Material");
			m_addMtrlFrame->SetSkin(window->getUISkin());

			m_removeMtrlFrame = new Button(Point(21 + 522+100+220, 159), L"Remove Sub Material");
			m_removeMtrlFrame->SetSkin(window->getUISkin());
		}
		{
			Label* lbl = new Label(Point(21 + 522, 133+25), L"Ambient", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_cfAmbient = new ColorField(lbl->Position + Point(100, 0), CV_Red);
			m_cfAmbient->Text = L"Ambient";
			m_cfAmbient->SetSkin(window->getUISkin());

			lbl = new Label(Point(21 + 522, 133+25*2), L"Diffuse", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);

			lbl = new Label(Point(21 + 522, 133+25*3), L"Specular", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);

			lbl = new Label(Point(21 + 522, 133+25*4), L"Emissive", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);

			lbl = new Label(Point(21 + 522, 133+25*5), L"Shininess", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);



		}

		getDocumentForm()->setMinimumSize(Point(1070,512+200));
		getDocumentForm()->setTitle(file);

	}

	ModelDocument::~ModelDocument()
	{
		delete m_modelViewer;
		if (m_model)
		{
			delete m_model;
		}
		delete m_sceneRenderer;
		delete m_camera;
		for (int i=0;i<m_labels.getCount();i++)
		{
			delete m_labels[i];
		}
		for (int i=0;i<m_mtrlPanelLabels.getCount();i++)
		{
			delete m_mtrlPanelLabels[i];
		}
		

		delete m_pbTime;
		delete m_tbMKeyTime;
		delete m_tbMKeyIndex;
		delete m_btnRefreshTimeLine;
		delete m_btnModify;
		delete m_btnAddMkey;
		delete m_btnRemoveMKey;

		delete m_cbMesh;
		delete m_cbMtrlPart;

		delete m_applyMtrl;
		delete m_addMtrlFrame;
		delete m_removeMtrlFrame;

		delete m_cfAmbient;

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
		ModelData data;
		m_modelSData->Save(&data);
		data.Save(new FileOutStream(m_filePath));
		if (m_animPath.size())
		{
			m_animData->Save(new FileOutStream(m_animPath));
		}
	}
	void ModelDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_modelViewer);
		//getDocumentForm()->getControls().Add(m_btnZoomIn);
		//getDocumentForm()->getControls().Add(m_btnZoomOut);
		{
			getDocumentForm()->getControls().Add(m_pbTime);
			getDocumentForm()->getControls().Add(m_tbMKeyTime);
			getDocumentForm()->getControls().Add(m_tbMKeyIndex);

			getDocumentForm()->getControls().Add(m_btnRefreshTimeLine);
			getDocumentForm()->getControls().Add(m_btnModify);
			getDocumentForm()->getControls().Add(m_btnAddMkey);
			getDocumentForm()->getControls().Add(m_btnRemoveMKey);

		}
		{
			getDocumentForm()->getControls().Add(m_cbMtrlPart);
			getDocumentForm()->getControls().Add(m_cbMesh);

			getDocumentForm()->getControls().Add(m_applyMtrl);
			getDocumentForm()->getControls().Add(m_addMtrlFrame);
			getDocumentForm()->getControls().Add(m_removeMtrlFrame);
			
		}
		{
			getDocumentForm()->getControls().Add(m_cfAmbient);

		}
		for (int i=0;i<m_mtrlPanelLabels.getCount();i++)
		{
			getDocumentForm()->getControls().Add(m_mtrlPanelLabels[i]);
		}
		for (int i=0;i<m_labels.getCount();i++)
		{
			getDocumentForm()->getControls().Add(m_labels[i]);
		}

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

		Apoc3D::Math::Rectangle rect = m_modelViewer->getAbsoluteArea();

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
	
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	ModelDocument::ColorField::ColorField(const Point& position, ColorValue defaultColor)
		: Control(position), m_lblAmbient(0), m_pbAmbient(0), m_btnAmbient(0), m_color(defaultColor)
	{

	}
	ModelDocument::ColorField::~ColorField()
	{
		if (m_lblAmbient)
			delete m_lblAmbient;
		if (m_pbAmbient)
			delete m_pbAmbient;

		if (m_btnAmbient)
			delete m_btnAmbient;
	}

	void ModelDocument::ColorField::Initialize(RenderDevice* device)
	{
		m_lblAmbient = new Label(Position, Text, 80);
		m_lblAmbient->SetSkin(m_skin);
		m_lblAmbient->Initialize(device);

		m_pbAmbient = new PictureBox(Position + Point(80, 0), 1);
		m_pbAmbient->SetSkin(m_skin);
		m_pbAmbient->Size = Point(50, m_lblAmbient->Size.Y);
		m_pbAmbient->eventPictureDraw().bind(this, &ModelDocument::ColorField::PictureBox_Draw);
		m_pbAmbient->Initialize(device);

		m_btnAmbient = new Button(Position + Point(140, 0), L"...");
		
		Size.Y = m_lblAmbient->Size.Y;
		Size.X = m_btnAmbient->Position.X + m_btnAmbient->Size.X;
		
		m_btnAmbient->SetSkin(m_skin);
		m_btnAmbient->Initialize(device);
		m_btnAmbient->Position.Y += (m_lblAmbient->Size.Y - m_btnAmbient->Size.Y)/2;
	}
	void ModelDocument::ColorField::Draw(Sprite* sprite)
	{
		m_lblAmbient->Draw(sprite);
		m_pbAmbient->Draw(sprite);
		m_btnAmbient->Draw(sprite);
	}
	void ModelDocument::ColorField::Update(const GameTime* const time)
	{
		m_lblAmbient->Position = Position;
		m_pbAmbient->Position = Position + Point(80, 0);
		m_btnAmbient->Position = Position + Point(140, 0);
	}

	void ModelDocument::ColorField::PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		sprite->Draw(m_skin->WhitePixelTexture, *dstRect, 0, m_color);
	}
}