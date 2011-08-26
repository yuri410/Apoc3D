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
#include "CommonDialog/ChooseColor.h"

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
#include "UILib/CheckBox.h"
#include "Utility/StringUtils.h"
#include "Vfs/ResourceLocation.h"
#include "Vfs/FileSystem.h"
#include "Vfs/FileLocateRule.h"

//#include <d3dx9.h>

using namespace Apoc3D::Input;
using namespace Apoc3D::Utility;
using namespace Apoc3D::IO;
using namespace APDesigner::CommonDialog;

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

		m_modelViewer = new PictureBox(Point(10,10 + 17+90), 1);
		m_modelViewer->Size = Point(512,512);
		m_modelViewer->SetSkin(window->getUISkin());
		m_modelViewer->eventPictureDraw().bind(this, &ModelDocument::ModelView_Draw);

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

			lbl = new Label(Point(21 + 522, 133), L"Material", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			List<String> items;
			items.Add(L"0001");items.Add(L"0002");items.Add(L"0003");items.Add(L"0004");
			items.Add(L"0005");items.Add(L"0006");items.Add(L"0007");items.Add(L"0008");
			items.Add(L"0009");items.Add(L"0010");items.Add(L"0011");items.Add(L"0012");
			m_cbMesh = new ComboBox(Point(21+522+100, 107), 200, items);
			m_cbMesh->SetSkin(window->getUISkin());
			m_cbMtrlPart = new ComboBox(Point(21+522+100, 133), 200, items);
			m_cbMtrlPart->SetSkin(window->getUISkin());


			m_applyMtrl = new Button(Point(21 + 522+100+220, 159),150, L"Apply Changes");
			m_applyMtrl->SetSkin(window->getUISkin());


			lbl = new Label(Point(21 + 522, 159), L"SubMaterial", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			m_cbSubMtrl = new ComboBox(Point(21+522+100, 159), 200, items);
			m_cbSubMtrl->SetSkin(window->getUISkin());


			m_addMtrlFrame = new Button(Point(21 + 522+100+220, 107),150, L"Add Sub Material");
			m_addMtrlFrame->SetSkin(window->getUISkin());

			m_removeMtrlFrame = new Button(Point(21 + 522+100+220, 133),150, L"Remove Sub Material");
			m_removeMtrlFrame->SetSkin(window->getUISkin());
		}
		{
			int sx = 21 + 522;
			int sy = 210;
			m_cfAmbient = new ColorField(Point(sx, sy), CV_Red);
			m_cfAmbient->Text = L"Ambient";
			m_cfAmbient->SetSkin(window->getUISkin());

			m_cfDiffuse = new ColorField(Point(sx + 250, sy), CV_Red);
			m_cfDiffuse->Text = L"Diffuse";
			m_cfDiffuse->SetSkin(window->getUISkin());

			sy += 30;

			m_cfSpecular = new ColorField(Point(sx, sy), CV_Red);
			m_cfSpecular->Text = L"Specular";
			m_cfSpecular->SetSkin(window->getUISkin());

			m_cfEmissive = new ColorField(Point(sx + 250, sy), CV_Red);
			m_cfEmissive->Text = L"Emissive";
			m_cfEmissive->SetSkin(window->getUISkin());



			sy += 30;
			Label* lbl = new Label(Point(sx, sy), L"Shininess", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_tbShinness = new TextBox(Point(sx + 100, sy), 200, L"");
			m_tbShinness->SetSkin(window->getUISkin());

			sy += 40;
			lbl = new Label(Point(sx, sy), L"Texture1", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex1 = new TextBox(Point(sx + 100, sy), 200, L"");
			m_tbTex1->SetSkin(window->getUISkin());


			sy += 25;
			lbl = new Label(Point(sx, sy), L"Texture2", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex2 = new TextBox(Point(sx + 100, sy), 200, L"");
			m_tbTex2->SetSkin(window->getUISkin());


			sy += 25;
			lbl = new Label(Point(sx, sy), L"Texture3", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex3 = new TextBox(Point(sx + 100, sy), 200, L"");
			m_tbTex3->SetSkin(window->getUISkin());


			sy += 25;
			lbl = new Label(Point(sx, sy), L"Texture4", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex4 = new TextBox(Point(sx + 100, sy), 200, L"");
			m_tbTex4->SetSkin(window->getUISkin());


			sy += 25;
			lbl = new Label(Point(sx, sy), L"Texture5", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex5 = new TextBox(Point(sx + 100, sy), 200, L"");
			m_tbTex5->SetSkin(window->getUISkin());


			//sy += 25;
			//lbl = new Label(Point(sx, sy), L"Texture6", 100);
			//lbl->SetSkin(window->getUISkin());
			//m_mtrlPanelLabels.Add(lbl);
			//m_tbTex6 = new TextBox(Point(sx + 100, sy), 200, L"");
			//m_tbTex6->SetSkin(window->getUISkin());


			sy += 35;
			lbl = new Label(Point(sx, sy), L"Priority[0,127]", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_tbPriority = new TextBox(Point(sx + 100, sy), 200, L"");
			m_tbPriority->SetSkin(window->getUISkin());

			m_cbDepthTest = new CheckBox(Point(sx+ 250 + 100, sy), L"DepthTest", false);
			m_cbDepthTest->SetSkin(window->getUISkin());


			sy += 25;
			List<String> items;
			items.Add(GraphicsCommonUtils::ToString(CULL_None));
			items.Add(GraphicsCommonUtils::ToString(CULL_Clockwise));
			items.Add(GraphicsCommonUtils::ToString(CULL_CounterClockwise));
			lbl = new Label(Point(sx, sy), L"Cull Mode", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_cbCull = new ComboBox(Point(sx + 100, sy), 200, items);
			m_cbCull->SetSkin(window->getUISkin());

			m_cbDepthWrite = new CheckBox(Point(sx+ 250 + 100, sy), L"DepthWrite", false);
			m_cbDepthWrite->SetSkin(window->getUISkin());

			sy += 25;
			lbl = new Label(Point(sx, sy), L"AlphaTest[0,1]", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_tbAlphaTest = new TextBox(Point(sx + 100, sy), 200, L"");
			m_tbAlphaTest->SetSkin(window->getUISkin());
			

			sy += 25;

			m_cbTransparent = new CheckBox(Point(sx + 250 + 100, sy), L"Transparent", false);
			m_cbTransparent->SetSkin(window->getUISkin());

			items.Clear();
			items.Add(GraphicsCommonUtils::ToString(BLFUN_Add));
			items.Add(GraphicsCommonUtils::ToString(BLFUN_Subtract));
			items.Add(GraphicsCommonUtils::ToString(BLFUN_ReverseSubtract));
			items.Add(GraphicsCommonUtils::ToString(BLFUN_Max));
			items.Add(GraphicsCommonUtils::ToString(BLFUN_Min));
			lbl = new Label(Point(sx, sy), L"Blend Func", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_cbBlendFunction = new ComboBox(Point(sx + 100, sy), 200, items);
			m_cbBlendFunction->SetSkin(window->getUISkin());

			items.Clear();
			items.Add(GraphicsCommonUtils::ToString(BLEND_Zero));
			items.Add(GraphicsCommonUtils::ToString(BLEND_One));
			items.Add(GraphicsCommonUtils::ToString(BLEND_SourceColor));
			items.Add(GraphicsCommonUtils::ToString(BLEND_InverseSourceColor));
			items.Add(GraphicsCommonUtils::ToString(BLEND_SourceAlpha));
			items.Add(GraphicsCommonUtils::ToString(BLEND_InverseSourceAlpha));
			items.Add(GraphicsCommonUtils::ToString(BLEND_DestinationAlpha));
			items.Add(GraphicsCommonUtils::ToString(BLEND_InverseDestinationAlpha));
			items.Add(GraphicsCommonUtils::ToString(BLEND_DestinationColor));
			items.Add(GraphicsCommonUtils::ToString(BLEND_InverseDestinationColor));

			items.Add(GraphicsCommonUtils::ToString(BLEND_SourceAlphaSaturation));
			items.Add(GraphicsCommonUtils::ToString(BLEND_BothSourceAlpha));
			items.Add(GraphicsCommonUtils::ToString(BLEND_BlendFactor));
			
			sy += 25;
			lbl = new Label(Point(sx, sy), L"Src Blend", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_cbSrcBlend = new ComboBox(Point(sx + 100, sy), 200, items);
			m_cbSrcBlend->SetSkin(window->getUISkin());

			sy += 25;
			lbl = new Label(Point(sx, sy), L"Dest Blend", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_cbDstBlend = new ComboBox(Point(sx + 100, sy), 200, items);
			m_cbDstBlend->SetSkin(window->getUISkin());

			sy += 35;
			lbl = new Label(Point(sx, sy), L"Pass Flags", 100);
			lbl->SetSkin(window->getUISkin());
			m_mtrlPanelLabels.Add(lbl);
			m_pbPassFlag = new PictureBox(Point(sx+100, sy),1);
			m_pbPassFlag->SetSkin(window->getUISkin());
			//m_pbPassFlag->Size.Y = getDocumentForm()->getFontRef()->getLineHeight();
			m_pbPassFlag->Size.X = 256;

			m_btnPassFlag = new Button(Point(sx+100+300, sy), L"Edit");
			m_btnPassFlag->SetSkin(window->getUISkin());
			m_btnPassFlag->eventPress().bind(this, &ModelDocument::PassButton_Pressed);
		}

		getDocumentForm()->setMinimumSize(Point(1070,512+137));
		//getDocumentForm()->setMaximumSize(Point(1071,512+138));
		getDocumentForm()->setTitle(file);

		m_passEditor = new PassFlagDialog(window, window->getDevice());
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
		delete m_cbSubMtrl;

		delete m_applyMtrl;
		delete m_addMtrlFrame;
		delete m_removeMtrlFrame;

		delete m_cfAmbient;
		delete m_cfDiffuse;
		delete m_cfEmissive;
		delete m_cfSpecular;
		delete m_tbShinness;

		delete m_tbTex1;
		delete m_tbTex2;
		delete m_tbTex3;
		delete m_tbTex4;
		delete m_tbTex5;
		//delete m_tbTex6;

		delete m_tbPriority;
		delete m_tbAlphaTest;
		delete m_cbTransparent;
		delete m_cbSrcBlend;
		delete m_cbDstBlend;
		delete m_cbBlendFunction;

		delete m_cbDepthTest;
		delete m_cbDepthWrite;

		delete m_pbPassFlag;
		delete m_btnPassFlag;

		delete m_cbCull;
		delete m_passEditor;
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
			getDocumentForm()->getControls().Add(m_cbSubMtrl);

			getDocumentForm()->getControls().Add(m_applyMtrl);
			getDocumentForm()->getControls().Add(m_addMtrlFrame);
			getDocumentForm()->getControls().Add(m_removeMtrlFrame);
			
		}
		{
			getDocumentForm()->getControls().Add(m_cfAmbient);
			getDocumentForm()->getControls().Add(m_cfDiffuse);
			getDocumentForm()->getControls().Add(m_cfEmissive);
			getDocumentForm()->getControls().Add(m_cfSpecular);
			getDocumentForm()->getControls().Add(m_tbShinness);

			getDocumentForm()->getControls().Add(m_tbTex1);
			getDocumentForm()->getControls().Add(m_tbTex2);
			getDocumentForm()->getControls().Add(m_tbTex3);
			getDocumentForm()->getControls().Add(m_tbTex4);
			getDocumentForm()->getControls().Add(m_tbTex5);
			//getDocumentForm()->getControls().Add(m_tbTex6);

			getDocumentForm()->getControls().Add(m_tbPriority);
			getDocumentForm()->getControls().Add(m_tbAlphaTest);

			getDocumentForm()->getControls().Add(m_cbTransparent);
			getDocumentForm()->getControls().Add(m_cbBlendFunction);
			getDocumentForm()->getControls().Add(m_cbSrcBlend);
			getDocumentForm()->getControls().Add(m_cbDstBlend);

			getDocumentForm()->getControls().Add(m_cbDepthTest);
			getDocumentForm()->getControls().Add(m_cbDepthWrite);


			getDocumentForm()->getControls().Add(m_pbPassFlag);
			getDocumentForm()->getControls().Add(m_btnPassFlag);
			getDocumentForm()->getControls().Add(m_cbCull);
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

				Matrix temp2;
				Matrix::Multiply(temp2, ori, temp);

				m_object.setOrientation(temp2);
			}

			m_distance -= 0.5f * mouse->getDZ();
		}
		m_camera->setChaseDirection(Vector3Utils::LDVector(cosf(m_xang), -sinf(m_yang), sinf(m_xang)));
		m_camera->setDesiredOffset(Vector3Utils::LDVector(0,0,m_distance));

		if (getDocumentForm()->getFontRef())
		{
			m_pbPassFlag->Size.Y = getDocumentForm()->getFontRef()->getLineHeight();
		}
	}
	void ModelDocument::Render()
	{
		m_sceneRenderer->RenderScene(&m_scene);
	}

	void ModelDocument::PassButton_Pressed(Control* ctrl)
	{
		m_passEditor->ShowModal();
	}
	void ModelDocument::ModelView_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
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
	void ModelDocument::PassFlags_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		for (int i=0;i<MaxScenePass;i++)
		{

		}
	}
	void ModelDocument::Timeline_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{

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
		m_lblAmbient->setOwner(getOwner());
		m_lblAmbient->Initialize(device);
		
		m_pbAmbient = new PictureBox(Position + Point(100, 0), 1);
		m_pbAmbient->SetSkin(m_skin);
		m_pbAmbient->Size = Point(50, m_lblAmbient->Size.Y);
		m_pbAmbient->eventPictureDraw().bind(this, &ModelDocument::ColorField::PictureBox_Draw);
		m_pbAmbient->setOwner(getOwner());
		m_pbAmbient->Initialize(device);

		m_btnAmbient = new Button(Position + Point(160, 0), L"...");
		
		Size.Y = m_lblAmbient->Size.Y;
		Size.X = m_btnAmbient->Position.X + m_btnAmbient->Size.X;
		
		m_btnAmbient->SetSkin(m_skin);
		m_btnAmbient->setOwner(getOwner());
		m_btnAmbient->Initialize(device);
		m_btnAmbient->Position.Y += (m_lblAmbient->Size.Y - m_btnAmbient->Size.Y)/2;
		m_btnAmbient->eventPress().bind(this, &ModelDocument::ColorField::Button_Press);
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
		m_pbAmbient->Position = Position + Point(100, 0);
		m_btnAmbient->Position = Position + Point(160, 0);
		m_btnAmbient->Position.Y += (m_lblAmbient->Size.Y - m_btnAmbient->Size.Y)/2;

		m_lblAmbient->Update(time);
		m_pbAmbient->Update(time);
		m_btnAmbient->Update(time);
	}

	void ModelDocument::ColorField::PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		sprite->Draw(m_skin->WhitePixelTexture, *dstRect, 0, m_color);
	}
	void ModelDocument::ColorField::Button_Press(Control* ctrl)
	{
		ChooseColorDialog dlg;
		if (dlg.ShowDialog() == DLGRES_OK)
		{
			m_color = dlg.getSelectedColor();
		}
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	ModelDocument::PassFlagDialog::PassFlagDialog(MainWindow* window,RenderDevice* device)
	{
		m_form = new Form(FBS_Fixed, L"Pass Editor");

		Apoc3D::Math::Rectangle rect = UIRoot::GetUIArea(device);
		m_form->Size = Point(980, 450);
		m_form->Position = Point(rect.Width, rect.Height) - m_form->Size;
		m_form->Position.X /= 2;
		m_form->Position.Y /= 2;
		m_form->SetSkin(window->getUISkin());

		int ofsX = 10;
		int ofsY = 27;
		int counter = 0;
		for (int i=0;i<4;i++)
		{
			for (int j=0;j<16;j++)
			{
				Label* lbl = new Label(Point(i * 250+ofsX,j * 25+ofsY), L"Pass " + StringUtils::ToString(counter++), 50);
				lbl->SetSkin(window->getUISkin());
				m_lblTable.Add(lbl);

				TextBox* text = new TextBox(Point(i * 250+ofsX+lbl->Size.X,j * 25+ofsY), 150, L"");
				text->SetSkin(window->getUISkin());
				m_tbTable.Add(text);
			}
		}

		for (int i=0;i<m_lblTable.getCount();i++)
		{
			m_form->getControls().Add(m_lblTable[i]);
			m_form->getControls().Add(m_tbTable[i]);
		}
		m_form->Initialize(device);
		UIRoot::Add(m_form);
	}

	ModelDocument::PassFlagDialog::~PassFlagDialog()
	{
		UIRoot::Remove(m_form);
		delete m_form;
		for (int i=0;i<m_lblTable.getCount();i++)
		{
			delete m_lblTable[i];
			delete m_tbTable[i];
		}
	}

	void ModelDocument::PassFlagDialog::ShowModal()
	{
		m_form->ShowModal();
	}
}