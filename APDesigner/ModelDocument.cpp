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
#include "Graphics/RenderSystem/VertexElement.h"
#include "Graphics/RenderSystem/Buffer/HardwareBuffer.h"
#include "Graphics/EffectSystem/EffectManager.h"
#include "Graphics/ModelManager.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Graphics/Mesh.h"
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

			m_pbTime->eventPress().bind(this, &ModelDocument::PBTime_Pressed);
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
			m_cbMesh = new ComboBox(Point(21+522+100, 107), 200, items);
			m_cbMesh->SetSkin(window->getUISkin());
			m_cbMesh->eventSelectionChanged().bind(this, &ModelDocument::CBMesh_SelectionChanged);

			m_cbMeshPart = new ComboBox(Point(21+522+100, 133), 200, items);
			m_cbMeshPart->SetSkin(window->getUISkin());
			m_cbMeshPart->eventSelectionChanged().bind(this, &ModelDocument::CBMeshPart_SelectionChanged);

			lbl = new Label(Point(21 + 522, 183), L"Mesh Name", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);

			m_tbMeshName = new TextBox(Point(21+522+100, 183), 200);
			m_tbMeshName->SetSkin(window->getUISkin());



			m_applyMtrl = new Button(Point(21 + 522+100+220, 159),150, L"Apply Changes");
			m_applyMtrl->SetSkin(window->getUISkin());
			m_applyMtrl->eventRelease().bind(this, &ModelDocument::BtnApplyMtrl_Pressed);

			lbl = new Label(Point(21 + 522, 159), L"SubMaterial", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			m_cbSubMtrl = new ComboBox(Point(21+522+100, 159), 200, items);
			m_cbSubMtrl->SetSkin(window->getUISkin());
			m_cbSubMtrl->eventSelectionChanged().bind(this, &ModelDocument::CBSubMtrl_SelectionChanged);


			m_addMtrlFrame = new Button(Point(21 + 522+100+220, 107),150, L"Add Sub Material");
			m_addMtrlFrame->SetSkin(window->getUISkin());
			m_addMtrlFrame->eventRelease().bind(this, &ModelDocument::BtnAddMtrl_Pressed);

			m_removeMtrlFrame = new Button(Point(21 + 522+100+220, 133),150, L"Remove Sub Material");
			m_removeMtrlFrame->SetSkin(window->getUISkin());
			m_removeMtrlFrame->eventRelease().bind(this, &ModelDocument::BtnRemoveMtrl_Pressed);


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
			m_pbPassFlag->eventPictureDraw().bind(this, &ModelDocument::PassFlags_Draw);

			m_btnPassFlag = new Button(Point(sx+100+300, sy), L"Edit");
			m_btnPassFlag->SetSkin(window->getUISkin());
			m_btnPassFlag->eventPress().bind(this, &ModelDocument::PassButton_Pressed);
		}
		{
			int sx = 21;
			int sy = 522 + 115;


			Label* lbl = new Label(Point(sx, sy), L"Utilities: ", 100);
			lbl->SetSkin(window->getUISkin());
			m_labels.Add(lbl);
			sy+=20;

			m_revertZ = new Button(Point(sx , sy),100, L"Revert Z");
			m_revertZ->SetSkin(window->getUISkin());
			m_revertZ->eventPress().bind(this, &ModelDocument::RevertZ_Pressed);

			sx += 110;
			m_recenterModel = new Button(Point(sx, sy),100, L"Center model");
			m_recenterModel->SetSkin(window->getUISkin());
			m_recenterModel->eventPress().bind(this, &ModelDocument::RecenterModel_Pressed);

		}
		getDocumentForm()->setMinimumSize(Point(1070,512+137+50));
		
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
		delete m_cbMeshPart;
		delete m_cbSubMtrl;
		
		delete m_applyMtrl;
		delete m_addMtrlFrame;
		delete m_removeMtrlFrame;
		delete m_tbMeshName;

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

		delete m_recenterModel;
		delete m_revertZ;
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
		m_model->PlayAnimation();
		
		//m_selectedMeshIndex = m_modelSData->getEntities().getCount() > 0 ? 0 : -1;
		//if (m_selectedMeshIndex!=-1)
		//{
		//	m_selectedMeshPartIndex = m_modelSData->getEntities()[m_selectedMeshIndex]->getMaterials()->getMaterialCount() > 0 ? 0 : -1;
		//}
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
			getDocumentForm()->getControls().Add(m_cbMeshPart);
			getDocumentForm()->getControls().Add(m_cbMesh);
			getDocumentForm()->getControls().Add(m_cbSubMtrl);
			getDocumentForm()->getControls().Add(m_tbMeshName);

			getDocumentForm()->getControls().Add(m_applyMtrl);
			getDocumentForm()->getControls().Add(m_addMtrlFrame);
			getDocumentForm()->getControls().Add(m_removeMtrlFrame);
			
		}
		getDocumentForm()->getControls().Add(m_recenterModel);
		getDocumentForm()->getControls().Add(m_revertZ);
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


		m_cbMesh->getItems().Clear();
		m_cbMeshPart->getItems().Clear();
		m_cbSubMtrl->getItems().Clear();

		{
			const FastList<Mesh*> ents = m_modelSData->getEntities();
			for (int i=0;i<ents.getCount();i++)
			{
				m_cbMesh->getItems().Add(L"[Mesh]" + ents[i]->getName());
			}
		}
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
		Document::Update(time);
	}
	void ModelDocument::Render()
	{
		m_sceneRenderer->RenderScene(&m_scene);
	}

	void ModelDocument::PassButton_Pressed(Control* ctrl)
	{
		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx !=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				m_passEditor->ShowModal(mtrls->getMaterial(partIdx, frameIndex));
			}
		}
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

		Font* font = getDocumentForm()->getFontRef();
		int totalVertexCount = 0;
		int totalFaceCount = 0;
		bool hasSkin = false;
		for (int i=0;i<m_modelSData->getEntities().getCount();i++)
		{
			totalFaceCount+=m_modelSData->getEntities()[i]->getPrimitiveConut();
			totalVertexCount+=m_modelSData->getEntities()[i]->getVertexCount();

			if (!hasSkin)
			{
				hasSkin |= !!VertexElement::FindElementBySemantic(m_modelSData->getEntities()[i]->getVertexElement(), VEU_BlendWeight);
			}
		}
		String message = L"Vertex Count: " + StringUtils::ToString(totalVertexCount);
		message.append(L"\nFace Count:");
		message.append(StringUtils::ToString(totalFaceCount));
		if (hasSkin)
		{
			message.append(L"\n");
			message.append(L"Skinned");
		}

		font->DrawString(sprite, message, Point(dstRect->X+11, dstRect->Y+11), CV_Black);
		font->DrawString(sprite, message, Point(dstRect->X+10, dstRect->Y+10), CV_White);
		
	}
	void ModelDocument::PassFlags_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				Material* mtrl = mtrls->getMaterial(partIdx, frameIndex);

				uint64 flag = mtrl->getPassFlags();

				const int CX = 4;
				for (int i=0;i<64;i++)
				{
					Apoc3D::Math::Rectangle dr(*dstRect);
					dr.X += i * CX;
					dr.Width = CX;
					sprite->Draw(getDocumentForm()->getSkin()->WhitePixelTexture, dr, 0, (flag & 1) ? CV_Red : CV_White);
					flag = flag>>1;
				}
			}
		}
	}
	void ModelDocument::Timeline_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx!=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			if (m_cbMeshPart->getSelectedIndex() !=-1)
			{

			}
		}
	}
	void ModelDocument::CBMesh_SelectionChanged(Control* ctrl)
	{
		m_cbMeshPart->getItems().Clear();
		m_cbMeshPart->setSelectedIndex(-1);
		m_cbSubMtrl->getItems().Clear();
		m_cbSubMtrl->setSelectedIndex(-1);


		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx!=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			
			m_tbMeshName->setText(ents[selMeshIdx]->getName());

			for (uint i=0;i<mtrls->getMaterialCount();i++)
			{
				m_cbMeshPart->getItems().Add(L"Part(Material Set)" + StringUtils::ToString(i, 4, '0'));
			}
		}
	}
	void ModelDocument::CBMeshPart_SelectionChanged(Control* ctrl)
	{
		m_cbSubMtrl->getItems().Clear();
		m_cbSubMtrl->setSelectedIndex(-1);

		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx!=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			if (partIdx != -1)
			{
				for (uint i=0;i<mtrls->getFrameCount(partIdx);i++)
				{
					m_cbSubMtrl->getItems().Add(L"Frame(Material)" + StringUtils::ToString(i, 4, '0'));
				}
			}
		}
	}
	void ModelDocument::CBSubMtrl_SelectionChanged(Control* ctrl)
	{
		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx!=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				DisplayMaterialEditor(mtrls->getMaterial(partIdx, frameIndex));
			}
		}
	}
	void ModelDocument::PBTime_Pressed(Control* ctrl)
	{
		if (m_animData)
		{
			//const MaterialAnimationClip* clip = m_animData->getMaterialAnimationClips();
		}
	}
	void ModelDocument::DisplayMaterialEditor(Material* mtrl)
	{
		if (mtrl)
		{
			for (int i=0;i<m_mtrlPanelLabels.getCount();i++)
			{
				m_mtrlPanelLabels[i]->Visible = true;
			}
			m_cfAmbient->Visible = true;
			m_cfDiffuse->Visible = true;
			m_cfSpecular->Visible = true;
			m_cfEmissive->Visible = true;

			m_tbShinness->Visible = true;

			m_tbTex1->Visible = true;
			m_tbTex2->Visible = true;
			m_tbTex3->Visible = true;
			m_tbTex4->Visible = true;
			m_tbTex5->Visible = true;

			m_tbPriority->Visible = true;
			m_tbAlphaTest->Visible = true;

			m_cbDepthTest->Visible = true;
			m_cbDepthWrite->Visible = true;

			m_cbTransparent->Visible = true;

			m_cbSrcBlend->Visible = true;
			m_cbDstBlend->Visible = true;
			m_cbBlendFunction->Visible = true;
			m_cbCull->Visible = true;


			m_cfAmbient->SetValue(mtrl->Ambient);
			m_cfDiffuse->SetValue(mtrl->Diffuse);
			m_cfSpecular->SetValue(mtrl->Specular);
			m_cfEmissive->SetValue(mtrl->Emissive);

			m_tbShinness->Text = StringUtils::ToString(mtrl->Power);

			m_tbTex1->setText(mtrl->getTextureName(0));
			m_tbTex2->setText(mtrl->getTextureName(1));
			m_tbTex3->setText(mtrl->getTextureName(2));
			m_tbTex4->setText(mtrl->getTextureName(3));
			m_tbTex5->setText(mtrl->getTextureName(4));

			m_tbPriority->setText(StringUtils::ToString(mtrl->getPriority()));
			m_tbAlphaTest->setText(StringUtils::ToString(mtrl->AlphaReference));

			m_cbDepthTest->setValue(mtrl->DepthTestEnabled);
			m_cbDepthWrite->setValue(mtrl->DepthWriteEnabled);

			m_cbTransparent->setValue(mtrl->IsBlendTransparent);

			m_cbSrcBlend->SetSelectedByName(GraphicsCommonUtils::ToString(mtrl->SourceBlend));
			m_cbDstBlend->SetSelectedByName(GraphicsCommonUtils::ToString(mtrl->DestinationBlend));
			m_cbBlendFunction->SetSelectedByName(GraphicsCommonUtils::ToString(mtrl->BlendFunction));
			m_cbCull->SetSelectedByName(GraphicsCommonUtils::ToString(mtrl->Cull));
		}
		else
		{
			for (int i=0;i<m_mtrlPanelLabels.getCount();i++)
			{
				m_mtrlPanelLabels[i]->Visible = false;
			}
			m_cfAmbient->Visible = false;
			m_cfDiffuse->Visible = false;
			m_cfSpecular->Visible = false;
			m_cfEmissive->Visible = false;

			m_tbShinness->Visible = false;

			m_tbTex1->Visible = false;
			m_tbTex2->Visible = false;
			m_tbTex3->Visible = false;
			m_tbTex4->Visible = false;
			m_tbTex5->Visible = false;

			m_tbPriority->Visible = false;
			m_tbAlphaTest->Visible = false;

			m_cbDepthTest->Visible = false;
			m_cbDepthWrite->Visible = false;

			m_cbTransparent->Visible = false;

			m_cbSrcBlend->Visible = false;
			m_cbDstBlend->Visible = false;
			m_cbBlendFunction->Visible = false;
			m_cbCull->Visible = false;


		}
	}

	void ModelDocument::BtnAddMtrl_Pressed(Control* ctrl)
	{
		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();

			Material* newMtrl = new Material(*mtrls->getMaterial(partIdx, mtrls->getFrameCount(partIdx)-1));
			mtrls->AddFrame(newMtrl, partIdx);

			CBMeshPart_SelectionChanged(ctrl);
		}
	}
	void ModelDocument::BtnApplyMtrl_Pressed(Control* ctrl)
	{
		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			ents[selMeshIdx]->setName(m_tbMeshName->Text);

			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				Material* mtrl = mtrls->getMaterial(partIdx, frameIndex);

				mtrl->Ambient = Color4(m_cfAmbient->GetValue());
				mtrl->Diffuse = Color4(m_cfDiffuse->GetValue());
				mtrl->Specular = Color4(m_cfSpecular->GetValue());
				mtrl->Emissive = Color4(m_cfEmissive->GetValue());
				mtrl->Power = StringUtils::ParseSingle(m_tbShinness->Text);

				mtrl->setTextureName(0, m_tbTex1->Text);
				mtrl->setTextureName(1, m_tbTex2->Text);
				mtrl->setTextureName(2, m_tbTex3->Text);
				mtrl->setTextureName(3, m_tbTex4->Text);
				mtrl->setTextureName(4, m_tbTex5->Text);

				mtrl->setPriority(StringUtils::ParseUInt32(m_tbPriority->Text));
				mtrl->AlphaReference = StringUtils::ParseUInt32(m_tbAlphaTest->Text);
				
				mtrl->DepthTestEnabled = m_cbDepthTest->getValue();
				mtrl->DepthWriteEnabled = m_cbDepthWrite->getValue();
				
				mtrl->IsBlendTransparent = m_cbTransparent->getValue();
				
				if (m_cbSrcBlend->getSelectedIndex()!=-1)
				{
					mtrl->SourceBlend = GraphicsCommonUtils::ParseBlend(
						m_cbSrcBlend->getItems()[m_cbSrcBlend->getSelectedIndex()]);
				}
				if (m_cbDstBlend->getSelectedIndex()!=-1)
				{
					mtrl->DestinationBlend = GraphicsCommonUtils::ParseBlend(
						m_cbDstBlend->getItems()[m_cbDstBlend->getSelectedIndex()]);
				}
				if (m_cbBlendFunction->getSelectedIndex() !=-1)
				{
					mtrl->BlendFunction = GraphicsCommonUtils::ParseBlendFunction(
						m_cbBlendFunction->getItems()[m_cbBlendFunction->getSelectedIndex()]);
				}

				if (m_cbCull->getSelectedIndex() !=-1)
				{
					mtrl->Cull = GraphicsCommonUtils::ParseCullMode(
						m_cbCull->getItems()[m_cbCull->getSelectedIndex()]);
				}
				
				mtrl->Reload();
			}
		}
	}
	void ModelDocument::BtnRemoveMtrl_Pressed(Control* ctrl)
	{
		const FastList<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();

			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				Material* m = mtrls->getMaterial(partIdx, frameIndex);
				mtrls->RemoveFrame(partIdx, frameIndex);
				delete m;
				CBMeshPart_SelectionChanged(ctrl);
			}
		}
	}

	void ModelDocument::RecenterModel_Pressed(Control* ctrl)
	{
		Vector3 center = Vector3Utils::Zero;
		int totalVertexCount = 0;
		for (int i=0;i<m_modelSData->getEntities().getCount();i++)
		{
			Mesh* mesh = m_modelSData->getEntities()[i];
			center = Vector3Utils::Add(center, Vector3Utils::Multiply(mesh->getBoundingSphere().Center, (float)mesh->getVertexCount()));
			totalVertexCount+=mesh->getVertexCount();
		}

		center = Vector3Utils::Divide(center, (float)totalVertexCount);

		for (int i=0;i<m_modelSData->getEntities().getCount();i++)
		{
			Mesh* mesh = m_modelSData->getEntities()[i];
		
			const VertexElement* posElem = VertexElement::FindElementBySemantic(mesh->getVertexElement(), VEU_Position);
			//const VertexElement* nrmElem = VertexElement::FindElementBySemantic(mesh->getVertexElement(), VEU_Normal);

			if (posElem && posElem->getType() == VEF_Vector3)
			{
				char* vtxData = reinterpret_cast<char*>(mesh->getVertexBuffer()->Lock(LOCK_None));
				for (int j=0;j<mesh->getVertexCount();j++)
				{
					float* vtxPosOfs = reinterpret_cast<float*>(vtxData+posElem->getOffset()+j*mesh->getVertexSize());
					vtxPosOfs[0] -= _V3X(center);
					vtxPosOfs[1] -= _V3Y(center);
					vtxPosOfs[2] -= _V3Z(center);
				}
				mesh->getVertexBuffer()->Unlock();
			}

			BoundingSphere sphere = mesh->getBoundingSphere();
			sphere.Center = Vector3Utils::Subtract(sphere.Center, center);
			mesh->setBoundingSphere(sphere);
		}
	}
	void ModelDocument::RevertZ_Pressed(Control* ctrl)
	{
		for (int i=0;i<m_modelSData->getEntities().getCount();i++)
		{
			Mesh* mesh = m_modelSData->getEntities()[i];

			const VertexElement* posElem = VertexElement::FindElementBySemantic(mesh->getVertexElement(), VEU_Position);
			const VertexElement* nrmElem = VertexElement::FindElementBySemantic(mesh->getVertexElement(), VEU_Normal);

			if (posElem || nrmElem)
			{
				char* vtxData = reinterpret_cast<char*>(mesh->getVertexBuffer()->Lock(LOCK_None));
				for (int j=0;j<mesh->getVertexCount();j++)
				{
					if (posElem &&  posElem->getType() == VEF_Vector3)
					{
						float* vtxPosOfs = reinterpret_cast<float*>(vtxData+posElem->getOffset()+j*mesh->getVertexSize());

						vtxPosOfs[2] = -vtxPosOfs[2];
					}

					if (nrmElem &&  nrmElem->getType() == VEF_Vector3)
					{
						float* vtxPosOfs = reinterpret_cast<float*>(vtxData+nrmElem->getOffset()+j*mesh->getVertexSize());

						//vtxPosOfs[2] = -vtxPosOfs[2];
						vtxPosOfs[1] = -vtxPosOfs[1];
						vtxPosOfs[0] = -vtxPosOfs[0];
					}

				}
				mesh->getVertexBuffer()->Unlock();
			}

			BoundingSphere sphere = mesh->getBoundingSphere();
			_V3Z(sphere.Center) = -_V3Z(sphere.Center);
			mesh->setBoundingSphere(sphere);
		}
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
		ChooseColorDialog dlg(m_color);
		if (dlg.ShowDialog() == DLGRES_OK)
		{
			m_color = dlg.getSelectedColor();

			if (!m_selected.empty())
				m_selected(this);
		}
	}

	void ModelDocument::ColorField::SetValue(const Color4& color)
	{
		m_color = color.ToArgb();
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
		m_form->eventClosed().bind(this, &ModelDocument::PassFlagDialog::Form_Closed);
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

	void ModelDocument::PassFlagDialog::ShowModal(Material* mtrl)
	{
		m_mtrl = mtrl;
		for (int i=0;i<MaxScenePass;i++)
		{
			m_tbTable[i]->setText(mtrl->getPassEffectName(i));
		}
		m_form->ShowModal();
	}

	void ModelDocument::PassFlagDialog::Form_Closed(Control* ctrl)
	{
		uint64 passFlags = 0;
		for (int i=0;i<MaxScenePass;i++)
		{
			if (m_tbTable[i]->Text.size())
			{
				passFlags |= (uint64)1<<i;
			}
			//passFlags = passFlags << (uint)1;
			m_mtrl->getPassEffectName(i) = m_tbTable[i]->Text;
			if (m_tbTable[i]->Text.size())
			{
				m_mtrl->setPassEffect(i, EffectManager::getSingleton().getEffect(m_mtrl->getPassEffectName(i)));
			}
			else
			{
				m_mtrl->setPassEffect(i, 0);
			}
			
		}
		if (!passFlags)
			passFlags = 1;
		m_mtrl->setPassFlags(passFlags);
		m_mtrl = 0;
	}
}