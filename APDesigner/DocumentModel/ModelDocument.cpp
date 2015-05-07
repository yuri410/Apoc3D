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
#include "ModelDocument.h"

#include "MaterialDocument.h"
#include "APDesigner/MainWindow.h"
#include "APDesigner/UIResources.h"
#include "APDesigner/CommonDialog/ChooseColor.h"

using namespace APDesigner::CommonDialog;

namespace APDesigner
{
	ModelDocument::ModelDocument(MainWindow* window, EditorExtension* ext, const String& name, const String& file, const String& animationFile)
		: Document(window, ext), m_filePath(file), m_animPath(animationFile), 
		m_name(name), m_model(0), m_modelSData(0), m_animData(0),
		m_distance(7),m_xang(ToRadian(45)),m_yang(ToRadian(45))
	{
		m_sceneRenderer = new SceneRenderer(window->getDevice());

		FileLocation fl = FileSystem::getSingleton().Locate(L"ModelViewSceneRenderer.xml", FileLocateRule::Default);

		Configuration config;
		XMLConfigurationFormat::Instance.Load(fl, &config);
		m_sceneRenderer->Load(&config);
		

		m_camera = new ChaseCamera();
		m_camera->setChaseDirection(Vector3(0, 0, 1));
		m_camera->setChasePosition(Vector3(0, 0, 0));
		m_camera->setDesiredOffset(Vector3(0, 0, 40));
		m_camera->setLookAtOffset(Vector3::Zero);
		m_camera->setFar(10000);
		m_camera->setNear(5);
		m_sceneRenderer->RegisterCamera(m_camera);

		m_scene.AddObject(&m_object);

		int32 lineHeight = window->getUISkin()->ContentTextFont->getLineHeightInt();
		
		const StyleSkin* skin = window->getUISkin();

		{
			m_modelViewer = new PictureBox(skin, Point(15, 10 + 25 + 90), 1);
			m_modelViewer->setSize(512, 512);
			m_modelViewer->eventPictureDraw.Bind(this, &ModelDocument::ModelView_Draw);

			List<String> items(MaxScenePass+1);
			items.Add(L"First Pass");
			for (int32 i=0;i<MaxScenePass;i++)
			{
				items.Add(L"Pass " + StringUtils::IntToString(i));
			}
			m_passViewSelect = new ComboBox(skin, Point(m_modelViewer->Position.X, m_modelViewer->Position.Y - lineHeight), 150, items);
			m_passViewSelect->eventSelectionChanged.Bind(this, &ModelDocument::PassViewSelect_SelectionChanged);
		}

		{
			Label* lbl = new Label(skin, Point(23, 33), L"Time", 100);
			m_labels.Add(lbl);
			m_pbTime = new PictureBox(skin, Point(94, 27), 1);
			m_pbTime->setSize(957, 30);

			m_pbTime->eventPress.Bind(this, &ModelDocument::PBTime_Pressed);
		}
		{
			int32 sy = 68;
			Label* lbl = new Label(skin, Point(21, sy), L"Material Keyframe Props", 180);
			m_labels.Add(lbl);

			lbl = new Label(skin, Point(200, sy), L"Time", 100);
			m_labels.Add(lbl);

			m_tbMKeyTime = new TextBox(skin, Point(241, sy), 100);


			lbl = new Label(skin, Point(391, sy), L"Material Sub-Index", 150);
			m_labels.Add(lbl);
			m_tbMKeyIndex = new TextBox(skin, Point(536, sy), 100);

			m_btnRefreshTimeLine = new Button(skin, Point(693, sy), L"Refresh");
			m_btnRefreshTimeLine->SetSizeY(lineHeight);

			m_btnModify = new Button(skin, Point(788, sy), L"Modify");
			m_btnModify->SetSizeY(lineHeight);

			m_btnAddMkey = new Button(skin, Point(881, sy), L"Add");
			m_btnAddMkey->SetSizeY(lineHeight);

			m_btnRemoveMKey = new Button(skin, Point(950, sy), L"Remove");
			m_btnRemoveMKey->SetSizeY(lineHeight);
		}
		{
			Label* lbl = new Label(skin, Point(21 + 522, 107), L"Mesh", 100);
			m_labels.Add(lbl);

			lbl = new Label(skin, Point(21 + 522, 133), L"Material", 100);
			m_labels.Add(lbl);

			List<String> items;
			m_cbMesh = new ComboBox(skin, Point(21+522+100, 107), 200, items);
			m_cbMesh->eventSelectionChanged.Bind(this, &ModelDocument::CBMesh_SelectionChanged);

			m_cbMeshPart = new ComboBox(skin, Point(21+522+100, 133), 200, items);
			m_cbMeshPart->eventSelectionChanged.Bind(this, &ModelDocument::CBMeshPart_SelectionChanged);

			lbl = new Label(skin, Point(21 + 522, 183), L"Mesh Name", 100);
			m_labels.Add(lbl);

			m_tbMeshName = new TextBox(skin, Point(21+522+100, 183), 200);



			m_applyMtrl = new Button(skin, Point(21 + 522+100+220, 159),175, L"Apply Changes");
			m_applyMtrl->eventRelease.Bind(this, &ModelDocument::BtnApplyMtrl_Pressed);
			m_applyMtrl->SetSizeY(lineHeight);

			m_applyAllMtrl = new Button(skin, Point(21 + 522+100+220, 185),175, L"Apply To All");
			m_applyAllMtrl->eventRelease.Bind(this, &ModelDocument::BtnApplyAllMtrl_Pressed);
			m_applyAllMtrl->SetSizeY(lineHeight);


			lbl = new Label(skin, Point(21 + 522, 159), L"SubMaterial", 100);
			m_labels.Add(lbl);
			m_cbSubMtrl = new ComboBox(skin, Point(21+522+100, 159), 200, items);
			m_cbSubMtrl->eventSelectionChanged.Bind(this, &ModelDocument::CBSubMtrl_SelectionChanged);
			

			m_addMtrlFrame = new Button(skin, Point(21 + 522+100+220, 107),175, L"Add Sub Material");
			m_addMtrlFrame->eventRelease.Bind(this, &ModelDocument::BtnAddMtrl_Pressed);
			m_addMtrlFrame->SetSizeY(lineHeight);

			m_removeMtrlFrame = new Button(skin, Point(21 + 522+100+220, 133),175, L"Remove Sub Material");
			m_removeMtrlFrame->eventRelease.Bind(this, &ModelDocument::BtnRemoveMtrl_Pressed);
			m_removeMtrlFrame->SetSizeY(lineHeight);

		}
		{
			int sx = 21 + 522;
			int sx2 = sx + 100;
			int sy = 210;

			m_cbUseRef = new CheckBox(skin, Point(sx, sy), L"Use External", false);
			m_cbUseRef->eventToggled.Bind(this, &ModelDocument::CBUseRef_Checked);

			m_tbRefMaterialName = new TextBox(skin, Point(sx + 150, sy), 150);

			m_btnRefMaterial = new Button(skin, Point(sx + 310, sy), L"...");
			m_btnRefMaterial->eventPress.Bind(this, &ModelDocument::BtnRefMtrl_Pressed);
			m_btnRefMaterial->SetSizeY(lineHeight);

			sy += 30;
			m_cfAmbient = new ColorField(skin, Point(sx, sy), L"Ambient", CV_Red);
			m_cfDiffuse = new ColorField(skin, Point(sx + 250, sy), L"Diffuse", CV_Red);

			sy += 25;
			m_cfSpecular = new ColorField(skin, Point(sx, sy), L"Specular", CV_Red);
			m_cfEmissive = new ColorField(skin, Point(sx + 250, sy), L"Emissive", CV_Red);

			sy += 25;
			Label* lbl = new Label(skin, Point(sx, sy), L"Shininess", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_tbShinness = new TextBox(skin, Point(sx + 100, sy), 200, L"");

			sy += 30;
			lbl = new Label(skin, Point(sx, sy), L"Texture1", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex1 = new TextBox(skin, Point(sx2, sy), 200, L"");

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Texture2", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex2 = new TextBox(skin, Point(sx2, sy), 200, L"");

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Texture3", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex3 = new TextBox(skin, Point(sx2, sy), 200, L"");

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Texture4", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex4 = new TextBox(skin, Point(sx2, sy), 200, L"");

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Texture5", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex5 = new TextBox(skin, Point(sx2, sy), 200, L"");



			sy += 35;
			lbl = new Label(skin, Point(sx, sy), L"Priority[0, " + StringUtils::UIntToString(BatchData::MaxPriority - 1) + L"]", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_tbPriority = new TextBox(skin, Point(sx2, sy), 200, L"");

			m_cbDepthTest = new CheckBox(skin, Point(sx+ 250 + 100, sy), L"DepthTest", false);


			sy += 25;
			List<String> items;
			items.Add(CullModeConverter.ToString(CULL_None));
			items.Add(CullModeConverter.ToString(CULL_Clockwise));
			items.Add(CullModeConverter.ToString(CULL_CounterClockwise));
			lbl = new Label(skin, Point(sx, sy), L"Cull Mode", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_cbCull = new ComboBox(skin, Point(sx2, sy), 200, items);

			m_cbDepthWrite = new CheckBox(skin, Point(sx+ 250 + 100, sy), L"DepthWrite", false);

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"AlphaTest[0,1]", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_tbAlphaTest = new TextBox(skin, Point(sx2, sy), 200, L"");
			

			sy += 25;

			m_cbTransparent = new CheckBox(skin, Point(sx + 250 + 100, sy), L"Transparent", false);

			items.Clear();
			items.Add(BlendFunctionConverter.ToString(BlendFunction::Add));
			items.Add(BlendFunctionConverter.ToString(BlendFunction::Subtract));
			items.Add(BlendFunctionConverter.ToString(BlendFunction::ReverseSubtract));
			items.Add(BlendFunctionConverter.ToString(BlendFunction::Max));
			items.Add(BlendFunctionConverter.ToString(BlendFunction::Min));
			lbl = new Label(skin, Point(sx, sy), L"Blend Func", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_cbBlendFunction = new ComboBox(skin, Point(sx2, sy), 200, items);

			items.Clear();
			items.Add(BlendConverter.ToString(Blend::Zero));
			items.Add(BlendConverter.ToString(Blend::One));
			items.Add(BlendConverter.ToString(Blend::SourceColor));
			items.Add(BlendConverter.ToString(Blend::InverseSourceColor));
			items.Add(BlendConverter.ToString(Blend::SourceAlpha));
			items.Add(BlendConverter.ToString(Blend::InverseSourceAlpha));
			items.Add(BlendConverter.ToString(Blend::DestinationAlpha));
			items.Add(BlendConverter.ToString(Blend::InverseDestinationAlpha));
			items.Add(BlendConverter.ToString(Blend::DestinationColor));
			items.Add(BlendConverter.ToString(Blend::InverseDestinationColor));

			items.Add(BlendConverter.ToString(Blend::SourceAlphaSaturation));
			items.Add(BlendConverter.ToString(Blend::BlendFactor));
			
			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Src Blend", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_cbSrcBlend = new ComboBox(skin, Point(sx2, sy), 200, items);

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Dest Blend", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_cbDstBlend = new ComboBox(skin, Point(sx2, sy), 200, items);

			sy += 30;
			lbl = new Label(skin, Point(sx, sy), L"Pass Flags", 120);
			m_mtrlPanelLabels.Add(lbl);
			m_pbPassFlag = new PictureBox(skin, Point(sx2, sy), 1);
			//m_pbPassFlag->Size.Y = getDocumentForm()->getFontRef()->getLineHeight();
			m_pbPassFlag->setSize(256, getDocumentForm()->getFont()->getLineHeightInt());
			m_pbPassFlag->eventPictureDraw.Bind(this, &ModelDocument::PassFlags_Draw);

			m_btnPassFlag = new Button(skin, Point(sx+100+300, sy), L"Edit");
			m_btnPassFlag->eventPress.Bind(this, &ModelDocument::PassButton_Pressed);
		}

		// utilities
		{
			int sx = 21;
			int sy = 522 + 115;

			Label* lbl = new Label(skin, Point(sx, sy), L"Utilities: ", 100);
			m_labels.Add(lbl);
			sy+=20;

			int btnWidth = 80;
			m_revertZ = new Button(skin, Point(sx , sy),btnWidth, L"Revert Z");
			m_revertZ->eventPress.Bind(this, &ModelDocument::RevertZ_Pressed);
			sx += btnWidth+10;

			btnWidth = 100;
			m_recenterModel = new Button(skin, Point(sx, sy),btnWidth, L"Center model");
			m_recenterModel->eventPress.Bind(this, &ModelDocument::RecenterModel_Pressed);
			sx += btnWidth+10;

			btnWidth = 80;
			m_swapYZ = new Button(skin, Point(sx, sy), btnWidth, L"Swap YZ");
			m_swapYZ->eventPress.Bind(this, &ModelDocument::RevertYZ_Pressed);
			sx += btnWidth+10;

			btnWidth = 100;
			m_rotateY = new Button(skin, Point(sx, sy),btnWidth, L"Rotation Y 90");
			m_rotateY->eventPress.Bind(this, &ModelDocument::RotY_Pressed);
			sx += btnWidth+10;

			m_rotateZ = new Button(skin, Point(sx, sy),btnWidth, L"Rotation Z 90");
			m_rotateZ->eventPress.Bind(this, &ModelDocument::RotZ_Pressed);
			sx += btnWidth+10;

			btnWidth = 30;
			m_zoomIn = new Button(skin, Point(sx, sy),btnWidth, L"+");
			m_zoomIn->eventPress.Bind(this, &ModelDocument::ZoomIn_Pressed);
			sx += btnWidth+10;

			m_zoomOut = new Button(skin, Point(sx, sy),btnWidth, L"-");
			m_zoomOut->eventPress.Bind(this, &ModelDocument::ZoomOut_Pressed);
			sx += btnWidth+10;

			btnWidth = 170;
			m_setSequenceImages = new Button(skin, Point(sx, sy),btnWidth, L"Set Sequence Material");
			m_setSequenceImages->eventPress.Bind(this, &ModelDocument::SetSequenceImages_Pressed);
			sx += btnWidth+10;

			btnWidth = 80;
			m_applyColorToAll = new Button(skin, Point(sx, sy),btnWidth, L"Color All");
			m_applyColorToAll->eventPress.Bind(this, &ModelDocument::ApplyColorToAll_Pressed);
			sx += btnWidth + 10;

			btnWidth = 50;
			m_applyFXToAll = new Button(skin, Point(sx, sy),btnWidth, L"FX All");
			m_applyFXToAll->eventPress.Bind(this, &ModelDocument::ApplyFXToAll_Pressed);
			sx += btnWidth + 10;

			btnWidth = 80;
			m_autoTex = new Button(skin, Point(sx, sy),btnWidth, L"Auto Tex");
			m_autoTex->eventPress.Bind(this, &ModelDocument::AutoTex_Pressed);
			sx += btnWidth + 10;

		}

		getDocumentForm()->setMinimumSize(Point(1070,512+137+50));
		
		//getDocumentForm()->setMaximumSize(Point(1071,512+138));
		getDocumentForm()->setTitle(file);

		m_passEditor = new PassFlagDialog(window, window->getDevice());
		m_batchCopyMtrl = new CopyMaterialDialog(this, window, window->getDevice());
		m_selectMtrl = new SelectMaterialDialog(this, window, window->getDevice());
	}

	ModelDocument::~ModelDocument()
	{
		delete m_passEditor;
		delete m_batchCopyMtrl;
		delete m_selectMtrl;

		delete m_modelViewer;
		if (m_model)
		{
			delete m_model;
		}
		m_modelSData = nullptr;

		delete m_sceneRenderer;
		delete m_camera;

		m_labels.DeleteAndClear();
		m_mtrlPanelLabels.DeleteAndClear();

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
		
		delete m_applyAllMtrl;
		delete m_applyMtrl;
		delete m_addMtrlFrame;
		delete m_removeMtrlFrame;
		delete m_tbMeshName;

		delete m_cbUseRef;
		delete m_tbRefMaterialName;
		delete m_btnRefMaterial;

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

		delete m_recenterModel;
		delete m_revertZ;
		delete m_swapYZ;
		delete m_rotateY;
		delete m_zoomIn;
		delete m_zoomOut;
		delete m_setSequenceImages;
		delete m_applyColorToAll;
		delete m_applyFXToAll;
		delete m_autoTex;
		delete m_passViewSelect;
	}
	

	void ModelDocument::LoadRes()
	{
		if (m_model)
		{
			m_object.setmdl(0);
			delete m_model;
			//delete m_modelSData;
		}
		FileLocation fl(m_filePath);
		
		m_modelSData = ModelManager::getSingleton().CreateInstanceUnmanaged(getMainWindow()->getDevice(), fl);

		if (m_animPath.empty())
		{
			String fn;
			String ext;
			PathUtils::SplitFileNameExtension(m_filePath, fn, ext);

			fn.append(L".anim");
			if (File::FileExists(fn))
			{
				m_animPath = fn;
			}
		}

		if (m_animPath.size())
		{
			FileLocation afl(m_animPath);
			m_animData = AnimationManager::getSingleton().CreateInstance(afl);
		}

		m_model = new Model(new ResourceHandle<ModelSharedData>(m_modelSData, ResourceHandle<ModelSharedData>::FLGMIX_ContainerHandle), m_animData);
		m_object.setmdl(m_model);
		m_model->PlayAnimation();
		
		float maxR = 7;
		for (int i=0;i<m_modelSData->getEntities().getCount();i++)
		{
			if (m_modelSData->getEntities()[i]->getBoundingSphere().Radius > maxR)
				maxR = m_modelSData->getEntities()[i]->getBoundingSphere().Radius;
		}
		m_distance = maxR * 1.5f;

		//m_selectedMeshIndex = m_modelSData->getEntities().getCount() > 0 ? 0 : -1;
		//if (m_selectedMeshIndex!=-1)
		//{
		//	m_selectedMeshPartIndex = m_modelSData->getEntities()[m_selectedMeshIndex]->getMaterials()->getMaterialCount() > 0 ? 0 : -1;
		//}
		//m_texture = TextureManager::getSingleton().CreateUnmanagedInstance(getMainWindow()->getDevice(), fl, false);
		DisplayMaterialEditor(0,false);
	}
	void ModelDocument::SaveRes()
	{
		ModelData data;
		m_modelSData->Save(&data);
		data.Save(FileOutStream(m_filePath));
		if (m_animPath.size())
		{
			m_animData->Save(FileOutStream(m_animPath));
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
			getDocumentForm()->getControls().Add(m_applyAllMtrl);
			getDocumentForm()->getControls().Add(m_addMtrlFrame);
			getDocumentForm()->getControls().Add(m_removeMtrlFrame);
			
		}
		getDocumentForm()->getControls().Add(m_recenterModel);
		getDocumentForm()->getControls().Add(m_revertZ);
		getDocumentForm()->getControls().Add(m_swapYZ);
		getDocumentForm()->getControls().Add(m_rotateY);
		getDocumentForm()->getControls().Add(m_rotateZ);
		getDocumentForm()->getControls().Add(m_zoomIn);
		getDocumentForm()->getControls().Add(m_zoomOut);
		getDocumentForm()->getControls().Add(m_setSequenceImages);
		getDocumentForm()->getControls().Add(m_applyColorToAll);
		getDocumentForm()->getControls().Add(m_applyFXToAll);
		getDocumentForm()->getControls().Add(m_autoTex);

		getDocumentForm()->getControls().Add(m_passViewSelect);
		{
			getDocumentForm()->getControls().Add(m_cbUseRef);
			getDocumentForm()->getControls().Add(m_tbRefMaterialName);
			getDocumentForm()->getControls().Add(m_btnRefMaterial);
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


		m_cbMesh->getItems().Clear();
		m_cbMeshPart->getItems().Clear();
		m_cbSubMtrl->getItems().Clear();

		{
			const List<Mesh*> ents = m_modelSData->getEntities();
			for (int i=0;i<ents.getCount();i++)
			{
				m_cbMesh->getItems().Add(L"[Mesh]" + ents[i]->getName());
			}
		}
		m_passViewSelect->setSelectedIndex(0);
	}
	void ModelDocument::Update(const GameTime* time)
	{
		m_scene.Update(time);
		m_camera->Update(time);

		Apoc3D::Math::Rectangle rect = m_modelViewer->getAbsoluteArea();

		Mouse* mouse = InputAPIManager::getSingleton().getMouse();
		
		if (rect.Contains(mouse->GetPosition()))
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
		m_camera->setChaseDirection(Vector3(cosf(m_xang), -sinf(m_yang), sinf(m_xang)));
		m_camera->setDesiredOffset(Vector3(0,0,m_distance));

		if (getDocumentForm()->getFont())
		{
			m_pbPassFlag->setHeight(static_cast<int>(getDocumentForm()->getFont()->getLineBackgroundHeight()));
		}
		Document::Update(time);
	}
	void ModelDocument::Render()
	{
		m_sceneRenderer->RenderScene(&m_scene);
	}

	void ModelDocument::UpdateSelectedPart()
	{
		CBMeshPart_SelectionChanged(m_cbMeshPart);
	}
	void ModelDocument::UpdateSelectedMaterial()
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				Material* m = mtrls->getMaterial(partIdx, frameIndex);
				bool isRef = false;
				String refName;
				if (m->ExternalReferenceName.size())
				{
					refName = m->ExternalReferenceName;
					isRef = true;

					m->LoadReferencedMaterial(m->ExternalReferenceName);
				}

				m->ExternalReferenceName = refName;
				DisplayMaterialEditor(m, isRef);
			}
		}
	}

	void ModelDocument::PassButton_Pressed(Button* ctrl)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
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

		Font* font = getDocumentForm()->getFont();
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
		String message = L"Vertex Count: " + StringUtils::IntToString(totalVertexCount);
		message.append(L"\nFace Count:");
		message.append(StringUtils::IntToString(totalFaceCount));
		if (hasSkin)
		{
			message.append(L"\n");
			message.append(L"Skinned");
		}
		message.append(L"\nZ: " + StringUtils::SingleToString(m_distance, StrFmt::fp<1>::val));

		font->DrawString(sprite, message, Point(dstRect->X+11, dstRect->Y+11), CV_Black);
		font->DrawString(sprite, message, Point(dstRect->X+10, dstRect->Y+10), CV_White);
		
	}
	void ModelDocument::PassFlags_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
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
					sprite->Draw(SystemUI::GetWhitePixel(), dr, 0, (flag & 1) ? CV_Red : CV_White);
					flag = flag>>1;
				}
			}
		}
	}
	void ModelDocument::Timeline_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx!=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			if (m_cbMeshPart->getSelectedIndex() !=-1)
			{
				
			}
		}
	}
	void ModelDocument::CBMesh_SelectionChanged(ComboBox* ctrl)
	{
		m_cbMeshPart->getItems().Clear();
		m_cbMeshPart->setSelectedIndex(-1);
		m_cbSubMtrl->getItems().Clear();
		m_cbSubMtrl->setSelectedIndex(-1);


		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();

			m_tbMeshName->SetText(ents[selMeshIdx]->getName());

			for (int32 i = 0; i < mtrls->getMaterialCount(); i++)
			{
				m_cbMeshPart->getItems().Add(L"Part(Material Set)" + StringUtils::IntToString(i, StrFmt::a<4, '0'>::val));
			}

			// select default
			if (m_cbMeshPart->getItems().getCount() > 0)
			{
				m_cbMeshPart->setSelectedIndex(0);
				CBMeshPart_SelectionChanged(m_cbMeshPart);
			}
		}
	}
	void ModelDocument::CBMeshPart_SelectionChanged(ComboBox* ctrl)
	{
		m_cbSubMtrl->getItems().Clear();
		m_cbSubMtrl->setSelectedIndex(-1);

		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			if (partIdx != -1)
			{
				for (int32 i = 0; i < mtrls->getFrameCount(partIdx); i++)
				{
					m_cbSubMtrl->getItems().Add(L"Frame(Material)" + StringUtils::IntToString(i, StrFmt::a<4, '0'>::val));
				}

				if (m_cbSubMtrl->getItems().getCount() > 0)
				{
					m_cbSubMtrl->setSelectedIndex(0);
					CBSubMtrl_SelectionChanged(m_cbSubMtrl);
				}
			}
		}
	}
	void ModelDocument::CBSubMtrl_SelectionChanged(ComboBox* ctrl)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				Material* m = mtrls->getMaterial(partIdx, frameIndex);
				DisplayMaterialEditor(m, !m->ExternalReferenceName.empty());
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

	void ModelDocument::CBUseRef_Checked(CheckBox* ctrl)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx!=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				Material* m = mtrls->getMaterial(partIdx, frameIndex);
				DisplayMaterialEditor(m, m_cbUseRef->Checked);
			}
		}
	}
	void ModelDocument::BtnRefMtrl_Pressed(Button* ctrl)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx!=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				Material* m = mtrls->getMaterial(partIdx, frameIndex);
				m_selectMtrl->ShowModal(m);
			}
		}
	}

	void ModelDocument::DisplayMaterialEditor(Material* mtrl, bool usingRef)
	{
		if (mtrl)
		{
			m_cbUseRef->Visible = true;
			m_cbUseRef->Checked = usingRef;

			m_tbRefMaterialName->Visible = usingRef;
			m_tbRefMaterialName->SetText(mtrl->ExternalReferenceName);
			m_btnRefMaterial->Visible = usingRef;

			bool v = !usingRef;

			for (int i=0;i<m_mtrlPanelLabels.getCount();i++)
			{
				m_mtrlPanelLabels[i]->Visible = v;
			}
			m_cfAmbient->Visible = v;
			m_cfDiffuse->Visible = v;
			m_cfSpecular->Visible = v;
			m_cfEmissive->Visible = v;

			m_tbShinness->Visible = v;

			m_tbTex1->Visible = v;
			m_tbTex2->Visible = v;
			m_tbTex3->Visible = v;
			m_tbTex4->Visible = v;
			m_tbTex5->Visible = v;

			m_tbPriority->Visible = v;
			m_tbAlphaTest->Visible = v;

			m_cbDepthTest->Visible = v;
			m_cbDepthWrite->Visible = v;

			m_cbTransparent->Visible = v;

			m_cbSrcBlend->Visible = v;
			m_cbDstBlend->Visible = v;
			m_cbBlendFunction->Visible = v;
			m_cbCull->Visible = v;

			if (v)
			{
				m_cfAmbient->SetValue(mtrl->Ambient);
				m_cfDiffuse->SetValue(mtrl->Diffuse);
				m_cfSpecular->SetValue(mtrl->Specular);
				m_cfEmissive->SetValue(mtrl->Emissive);

				m_tbShinness->SetText(StringUtils::SingleToString(mtrl->Power));

				m_tbTex1->SetText(mtrl->getTextureName(0));
				m_tbTex2->SetText(mtrl->getTextureName(1));
				m_tbTex3->SetText(mtrl->getTextureName(2));
				m_tbTex4->SetText(mtrl->getTextureName(3));
				m_tbTex5->SetText(mtrl->getTextureName(4));

				m_tbPriority->SetText(StringUtils::UIntToString(mtrl->getPriority()));
				m_tbAlphaTest->SetText(StringUtils::UIntToString(mtrl->AlphaReference));

				m_cbDepthTest->Checked = mtrl->DepthTestEnabled;
				m_cbDepthWrite->Checked = mtrl->DepthWriteEnabled;

				m_cbTransparent->Checked = mtrl->IsBlendTransparent;

				m_cbSrcBlend->SetSelectionByName(BlendConverter.ToString(mtrl->SourceBlend));
				m_cbDstBlend->SetSelectionByName(BlendConverter.ToString(mtrl->DestinationBlend));
				m_cbBlendFunction->SetSelectionByName(BlendFunctionConverter.ToString(mtrl->BlendFunction));
				m_cbCull->SetSelectionByName(CullModeConverter.ToString(mtrl->Cull));

			}
			m_pbPassFlag->Visible = v;
			m_btnPassFlag->Visible = v;
			
			
		}
		else
		{
			for (int i=0;i<m_mtrlPanelLabels.getCount();i++)
			{
				m_mtrlPanelLabels[i]->Visible = false;
			}
			m_cbUseRef->Visible = false;
			m_tbRefMaterialName->Visible = false;
			m_btnRefMaterial->Visible = false;

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

			m_pbPassFlag->Visible = false;
			m_btnPassFlag->Visible = false;
		}
	}


	void ModelDocument::BtnApplyAllMtrl_Pressed(Button* ctrl)
	{
		Material* currentMtrl = 0;
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			ents[selMeshIdx]->setName(m_tbMeshName->getText());

			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				currentMtrl = mtrls->getMaterial(partIdx, frameIndex);
			}
		}


		for (int i=0;i<ents.getCount();i++)
		{
			MeshMaterialSet<Material*>* mtrls = ents[i]->getMaterials();
			
			for (int32 j=0;j<mtrls->getMaterialCount();j++)
			{
				for (int32 k=0;k<mtrls->getFrameCount(j);k++)
				{
					Material* mtrl = mtrls->getMaterial(j,k);

					mtrl->Ambient = Color4(m_cfAmbient->GetValue());
					mtrl->Diffuse = Color4(m_cfDiffuse->GetValue());
					mtrl->Specular = Color4(m_cfSpecular->GetValue());
					mtrl->Emissive = Color4(m_cfEmissive->GetValue());
					mtrl->Power = StringUtils::ParseSingle(m_tbShinness->getText());

					mtrl->setTextureName(0, m_tbTex1->getText());
					mtrl->setTextureName(1, m_tbTex2->getText());
					mtrl->setTextureName(2, m_tbTex3->getText());
					mtrl->setTextureName(3, m_tbTex4->getText());
					mtrl->setTextureName(4, m_tbTex5->getText());

					mtrl->setPriority(StringUtils::ParseUInt32(m_tbPriority->getText()));
					mtrl->AlphaReference = StringUtils::ParseUInt32(m_tbAlphaTest->getText());

					mtrl->DepthTestEnabled = m_cbDepthTest->Checked;
					mtrl->DepthWriteEnabled = m_cbDepthWrite->Checked;

					mtrl->IsBlendTransparent = m_cbTransparent->Checked;

					if (m_cbSrcBlend->getSelectedIndex()!=-1)
					{
						mtrl->SourceBlend = BlendConverter.Parse(
							m_cbSrcBlend->getItems()[m_cbSrcBlend->getSelectedIndex()]);
					}
					if (m_cbDstBlend->getSelectedIndex()!=-1)
					{
						mtrl->DestinationBlend = BlendConverter.Parse(
							m_cbDstBlend->getItems()[m_cbDstBlend->getSelectedIndex()]);
					}
					if (m_cbBlendFunction->getSelectedIndex() !=-1)
					{
						mtrl->BlendFunction = BlendFunctionConverter.Parse(
							m_cbBlendFunction->getItems()[m_cbBlendFunction->getSelectedIndex()]);
					}

					if (m_cbCull->getSelectedIndex() !=-1)
					{
						mtrl->Cull = CullModeConverter.Parse(
							m_cbCull->getItems()[m_cbCull->getSelectedIndex()]);
					}

					mtrl->setPassFlags(currentMtrl->getPassFlags());
					for (int i=0;i<MaxScenePass;i++)
					{
						mtrl->setPassEffectName(i, currentMtrl->getPassEffectName(i));
						//mtrl->getPassEffectName(i) = currentMtrl->getPassEffectName(i);
						if (mtrl->getPassEffectName(i).size())
						{
							mtrl->setPassEffect(i, EffectManager::getSingleton().getEffect(mtrl->getPassEffectName(i)));
						}
						else
						{
							mtrl->setPassEffect(i, 0);
						}
					}

					mtrl->ExternalReferenceName = currentMtrl->ExternalReferenceName;

					mtrl->Reload();
				}
			}
			
		}
	}
	void ModelDocument::BtnAddMtrl_Pressed(Button* ctrl)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();

			Material* newMtrl = new Material(*mtrls->getMaterial(partIdx, mtrls->getFrameCount(partIdx)-1));
			mtrls->AddFrame(newMtrl, partIdx);

			CBMeshPart_SelectionChanged(nullptr);
		}
	}
	void ModelDocument::BtnApplyMtrl_Pressed(Button* ctrl)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			ents[selMeshIdx]->setName(m_tbMeshName->getText());

			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				Material* mtrl = mtrls->getMaterial(partIdx, frameIndex);

				if (m_cbUseRef->Checked)
				{
					mtrl->ExternalReferenceName = m_tbRefMaterialName->getText();
				}
				else
				{
					mtrl->ExternalReferenceName = L"";
				
					mtrl->Ambient = Color4(m_cfAmbient->GetValue());
					mtrl->Diffuse = Color4(m_cfDiffuse->GetValue());
					mtrl->Specular = Color4(m_cfSpecular->GetValue());
					mtrl->Emissive = Color4(m_cfEmissive->GetValue());
					mtrl->Power = StringUtils::ParseSingle(m_tbShinness->getText());

					mtrl->setTextureName(0, m_tbTex1->getText());
					mtrl->setTextureName(1, m_tbTex2->getText());
					mtrl->setTextureName(2, m_tbTex3->getText());
					mtrl->setTextureName(3, m_tbTex4->getText());
					mtrl->setTextureName(4, m_tbTex5->getText());

					mtrl->setPriority(StringUtils::ParseUInt32(m_tbPriority->getText()));
					mtrl->AlphaReference = StringUtils::ParseUInt32(m_tbAlphaTest->getText());
				
					mtrl->DepthTestEnabled = m_cbDepthTest->Checked;
					mtrl->DepthWriteEnabled = m_cbDepthWrite->Checked;
				
					mtrl->IsBlendTransparent = m_cbTransparent->Checked;
				
					if (m_cbSrcBlend->getSelectedIndex()!=-1)
					{
						mtrl->SourceBlend = BlendConverter.Parse(
							m_cbSrcBlend->getItems()[m_cbSrcBlend->getSelectedIndex()]);
					}
					if (m_cbDstBlend->getSelectedIndex()!=-1)
					{
						mtrl->DestinationBlend = BlendConverter.Parse(
							m_cbDstBlend->getItems()[m_cbDstBlend->getSelectedIndex()]);
					}
					if (m_cbBlendFunction->getSelectedIndex() !=-1)
					{
						mtrl->BlendFunction = BlendFunctionConverter.Parse(
							m_cbBlendFunction->getItems()[m_cbBlendFunction->getSelectedIndex()]);
					}

					if (m_cbCull->getSelectedIndex() !=-1)
					{
						mtrl->Cull = CullModeConverter.Parse(
							m_cbCull->getItems()[m_cbCull->getSelectedIndex()]);
					}
				
				}
				mtrl->Reload();
			}
		}
	}
	void ModelDocument::BtnRemoveMtrl_Pressed(Button* ctrl)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
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
				CBMeshPart_SelectionChanged(nullptr);
			}
		}
	}

	void ModelDocument::RecenterModel_Pressed(Button* ctrl)
	{
		Vector3 center = Vector3::Zero;
		int totalVertexCount = 0;
		for (int i=0;i<m_modelSData->getEntities().getCount();i++)
		{
			Mesh* mesh = m_modelSData->getEntities()[i];
			center = Vector3::Add(center, Vector3::Multiply(mesh->getBoundingSphere().Center, (float)mesh->getVertexCount()));
			totalVertexCount+=mesh->getVertexCount();
		}

		center = Vector3::Divide(center, (float)totalVertexCount);

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
					vtxPosOfs[0] -= center.X;
					vtxPosOfs[1] -= center.Y;
					vtxPosOfs[2] -= center.Z;
				}
				mesh->getVertexBuffer()->Unlock();
			}

			BoundingSphere sphere = mesh->getBoundingSphere();
			sphere.Center = sphere.Center - center;
			mesh->setBoundingSphere(sphere);
		}
	}
	void ModelDocument::Transform(const Matrix& transform)
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
						float* dataOfs = reinterpret_cast<float*>(vtxData+posElem->getOffset()+j*mesh->getVertexSize());

						Vector3 pos = Vector3::Set(dataOfs);
						pos = Vector3::TransformCoordinate(pos, transform);

						pos.Store(dataOfs);

					}

					if (nrmElem &&  nrmElem->getType() == VEF_Vector3)
					{
						float* dataOfs = reinterpret_cast<float*>(vtxData+nrmElem->getOffset()+j*mesh->getVertexSize());

						Vector3 pos = Vector3::Set(dataOfs);
						pos = Vector3::TransformNormal(pos, transform);

						pos.Store(dataOfs);

					}

				}
				mesh->getVertexBuffer()->Unlock();
			}

			BoundingSphere sphere = mesh->getBoundingSphere();
			sphere.Center = Vector3::TransformCoordinate(sphere.Center, transform);


			mesh->setBoundingSphere(sphere);
		}
	}
	void ModelDocument::RevertYZ_Pressed(Button* ctrl)
	{
		Matrix rot;
		Matrix::CreateRotationX(rot, Math::Half_PI);
		Transform(rot);

	}
	void ModelDocument::RevertZ_Pressed(Button* ctrl)
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
			sphere.Center.Z = -sphere.Center.Z;
			mesh->setBoundingSphere(sphere);
		}
	}
	void ModelDocument::RotY_Pressed(Button* ctrl)
	{
		Matrix rot;
		Matrix::CreateRotationY(rot, Math::Half_PI);
		Transform(rot);
	}
	void ModelDocument::RotZ_Pressed(Button* ctrl)
	{
		Matrix rot;
		Matrix::CreateRotationZ(rot, Math::Half_PI);
		Transform(rot);
	}
	void ModelDocument::ZoomIn_Pressed(Button* ctrl)
	{
		m_distance -= 10;
	}
	void ModelDocument::ZoomOut_Pressed(Button* ctrl)
	{
		m_distance += 10;
	}

	void ModelDocument::SetSequenceImages_Pressed(Button* ctrl)
	{
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx !=-1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			int partIdx = m_cbMeshPart->getSelectedIndex();
		
			m_batchCopyMtrl->ShowModal(mtrls, partIdx);
		}
	}
	void ModelDocument::ApplyColorToAll_Pressed(Button* ctrl)
	{
		Material* currentMtrl = 0;
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				currentMtrl = mtrls->getMaterial(partIdx, frameIndex);
			}
		}


		for (int i=0;i<ents.getCount();i++)
		{
			MeshMaterialSet<Material*>* mtrls = ents[i]->getMaterials();

			for (int32 j=0;j<mtrls->getMaterialCount();j++)
			{
				for (int32 k=0;k<mtrls->getFrameCount(j);k++)
				{
					Material* mtrl = mtrls->getMaterial(j,k);

					mtrl->Ambient = currentMtrl->Ambient;
					mtrl->Diffuse = currentMtrl->Diffuse;
					mtrl->Specular = currentMtrl->Specular;
					mtrl->Emissive = currentMtrl->Emissive;
					mtrl->Power = currentMtrl->Power;
				}
			}

		}
	}
	void ModelDocument::ApplyFXToAll_Pressed(Button* ctrl)
	{
		Material* currentMtrl = 0;
		const List<Mesh*> ents = m_modelSData->getEntities();
		int selMeshIdx = m_cbMesh->getSelectedIndex();
		if (selMeshIdx != -1)
		{
			MeshMaterialSet<Material*>* mtrls = ents[selMeshIdx]->getMaterials();
			
			int partIdx = m_cbMeshPart->getSelectedIndex();
			int frameIndex = m_cbSubMtrl->getSelectedIndex();
			if (partIdx != -1 && frameIndex != -1)
			{
				currentMtrl = mtrls->getMaterial(partIdx, frameIndex);
			}
		}

		for (int i=0;i<ents.getCount();i++)
		{
			MeshMaterialSet<Material*>* mtrls = ents[i]->getMaterials();

			for (int32 j=0;j<mtrls->getMaterialCount();j++)
			{
				for (int32 k=0;k<mtrls->getFrameCount(j);k++)
				{
					Material* mtrl = mtrls->getMaterial(j,k);

					//uint64 passFlags = 0;
					for (int p=0;p<MaxScenePass;p++)
					{
						mtrl->setPassEffectName(p, currentMtrl->getPassEffectName(p));
						mtrl->setPassEffect(p, currentMtrl->getPassEffect(p));
					}
					
					mtrl->setPassFlags(currentMtrl->getPassFlags());
				}
			}
		}
	}
	void ModelDocument::AutoTex_Pressed(Button* ctrl)
	{
		for (Mesh* m : m_modelSData->getEntities())
		{
			MeshMaterialSet<Material*>* mtrls = m->getMaterials();

			for (int32 j=0;j<mtrls->getMaterialCount();j++)
			{
				for (int32 k=0;k<mtrls->getFrameCount(j);k++)
				{
					Material* mtrl = mtrls->getMaterial(j,k);

					bool processed = false;
					for (int p = 0; p < MaxTextures; p++)
					{
						const String& tn = mtrl->getTextureName(p);
						if (tn.size() && tn.find('.', 0) != String::npos)
						{
							String newName = PathUtils::GetFileNameNoExt(tn);
							newName.append(L".tex");

							mtrl->setTextureName(p, newName);
							processed = true;
						}
					}

					if (processed)
						mtrl->Reload();
				}
			}
		}
	}

	void ModelDocument::PassViewSelect_SelectionChanged(ComboBox* ctrl)
	{
		SceneProcedure* sp = m_sceneRenderer->getSelectedProc();
		if (sp)
		{
			int selId = m_passViewSelect->getSelectedIndex() - 1;

			for (int32 i=0;i<sp->getPassCount();i++)
			{
				sp->getPass(i)->setSelectorID(selId);
			}
		}
	}

	/************************************************************************/
	/*   CopyMaterialDialog                                                 */
	/************************************************************************/

	CopyMaterialDialog::CopyMaterialDialog(ModelDocument* parent, MainWindow* window, RenderDevice* device)
		: m_parent(parent), m_dialogResult(false)
	{
		const StyleSkin* skin = window->getUISkin();

		m_form = new Form(skin, device, FBS_Fixed, L"Name Pattern");

		Apoc3D::Math::Rectangle rect = SystemUI::GetUIArea(device);
		m_form->setSize(400, 250);
		m_form->Position = (rect.getSize() - m_form->getSize()) / 2;
		m_form->eventClosed.Bind(this, &CopyMaterialDialog::Form_Closed);

		int32 sy = skin->FormTitle->Height + 5;
		m_lblTextureName = new Label(skin, Point(15, sy), L"Texture Name (C style)", 180);
		m_form->getControls().Add(m_lblTextureName);

		m_tbTextureName = new TextBox(skin, Point(15 + 190, sy), 150);
		m_tbTextureName->eventContentChanged.Bind(this, &CopyMaterialDialog::Config_Changed);
		m_form->getControls().Add(m_tbTextureName);

		sy += 40;
		m_lblPreview = new Label(skin, Point(15, sy), L"[Preview] ", 350);
		m_form->getControls().Add(m_lblPreview);

		sy += 40;
		m_lblStartNumber = new Label(skin, Point(15, sy), L"Start No.", 120);
		m_form->getControls().Add(m_lblStartNumber);

		m_tbStartNumber = new TextBox(skin, Point(15 + 190, sy), 150, L"0");
		m_tbStartNumber->eventContentChanged.Bind(this, &CopyMaterialDialog::Config_Changed);
		m_form->getControls().Add(m_tbStartNumber);


		sy += 40;
		m_lblEndNumber = new Label(skin, Point(15, sy), L"End No.", 120);
		m_form->getControls().Add(m_lblEndNumber);

		m_tbEndNumber = new TextBox(skin, Point(15 + 190, sy), 150, L"0");
		m_form->getControls().Add(m_tbEndNumber);


		sy += 20;
		m_btnOK = new Button(skin, Point(m_form->getWidth() - 150 - 25, sy), 150, L"OK");
		m_btnOK->eventPress.Bind(this, &CopyMaterialDialog::ButtonOK_Pressed);
		m_form->getControls().Add(m_btnOK);

		SystemUI::Add(m_form);
	}

	CopyMaterialDialog::~CopyMaterialDialog()
	{
		m_form->eventClosed.Unbind(this, &CopyMaterialDialog::Form_Closed);

		SystemUI::Remove(m_form);
		
		delete m_form;
		delete m_lblTextureName;
		delete m_tbTextureName;
		delete m_lblPreview;
		delete m_lblStartNumber;
		delete m_tbStartNumber;
		delete m_lblEndNumber;
		delete m_tbEndNumber;
	}

	void CopyMaterialDialog::ShowModal(MeshMaterialSet<Material*>* mtrl, int selectedSet)
	{
		m_mtrl = mtrl;
		m_selectedSet = selectedSet;
		m_dialogResult = false;
		m_form->ShowModal();
	}

	void CopyMaterialDialog::ButtonOK_Pressed(Button* ctrl)
	{
		m_dialogResult = true;
		m_form->Close();
	}
	void CopyMaterialDialog::Config_Changed(TextBox* ctrl)
	{
		int startNo = StringUtils::ParseInt32(m_tbStartNumber->getText());
		std::string formatString = StringUtils::toPlatformNarrowString(m_tbTextureName->getText());

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));

		int ret;
#ifdef _MSC_VER
		ret = sprintf_s(buffer, sizeof(buffer), formatString.c_str(), startNo);
#else
		ret = sprintf(buffer, formatString.c_str(), startNo);
#endif

		if (ret>0)
		{
			String texName = StringUtils::toPlatformWideString(buffer);

			m_lblPreview->SetText(L"[Preview]" + texName);
		}
		else
		{
			m_lblPreview->SetText(L"[INVALID]");
		}
	}

	void CopyMaterialDialog::Form_Closed(Control* ctrl)
	{
		if (!m_dialogResult)
			return;

		// copy frames
		int startNo = StringUtils::ParseInt32(m_tbStartNumber->getText());
		int endNo = StringUtils::ParseInt32(m_tbEndNumber->getText());

		std::string formatString = StringUtils::toPlatformNarrowString(m_tbTextureName->getText());

		Material* baseMtrl = m_mtrl->getMaterial(m_selectedSet, m_mtrl->getFrameCount(m_selectedSet)-1);

		char buffer[256];
		for (int i=startNo;i<=endNo;i++)
		{
			memset(buffer, 0, sizeof(buffer));
			sprintf_s(buffer, formatString.c_str(), i);

			String texName = StringUtils::toPlatformWideString(buffer);

			Material* newMtrl = new Material(*baseMtrl);
			newMtrl->setTextureName(0, texName);
			newMtrl->Reload();

			m_mtrl->AddFrame(newMtrl, m_selectedSet);
		}

		m_mtrl = 0;

		m_parent->UpdateSelectedPart();
	}

	/************************************************************************/
	/*  SelectMaterialDialog                                                */
	/************************************************************************/

	SelectMaterialDialog::SelectMaterialDialog(ModelDocument* parent, MainWindow* window,RenderDevice* device)
		: m_parent(parent), m_dialogResult(false), m_mtrl(nullptr)
	{
		const StyleSkin* skin = window->getUISkin();

		m_form = new Form(skin, device, FBS_Fixed, L"Material Table");
		
		Apoc3D::Math::Rectangle rect = SystemUI::GetUIArea(device);
		m_form->setSize(600, 600);
		m_form->Position = Point(rect.Width, rect.Height) - m_form->getSize();
		m_form->Position.X /= 2;
		m_form->Position.Y /= 2;
		m_form->eventClosed.Bind(this, &SelectMaterialDialog::Form_Closed);

		const List<std::pair<String, String>>& mtrlList = window->getProjectMaterialDesc();
		List2D<String> items(2, mtrlList.getCount());

		for (int i=0;i<mtrlList.getCount();i++)
		{
			String line[2] = { mtrlList[i].first, mtrlList[i].second };
			items.AddRow(line);
		}

		int32 sy = skin->FormTitle->Height + 5;
		m_materialTable = new ListView(skin, Point(5, sy), m_form->getSize() - Point(10, sy + 50), items);
		m_materialTable->getColumnHeader().Add(ListView::Header(L"Name", m_materialTable->getWidth() * 2 / 5 - 5));
		m_materialTable->getColumnHeader().Add(ListView::Header(L"Description", m_materialTable->getWidth() * 3 / 5 - 5));
		m_materialTable->FullRowSelect = true;
		m_form->getControls().Add(m_materialTable);

		sy += m_materialTable->getHeight() + 5;
		m_btnOK = new Button(skin, Point(m_form->getWidth() - 150 - 25, sy), 150, L"OK");
		m_btnOK->eventPress.Bind(this, &SelectMaterialDialog::ButtonOK_Pressed);
		m_form->getControls().Add(m_btnOK);

		SystemUI::Add(m_form);
	}

	SelectMaterialDialog::~SelectMaterialDialog()
	{
		m_form->eventClosed.Unbind(this, &SelectMaterialDialog::Form_Closed);

		SystemUI::Remove(m_form);

		delete m_form;
		delete m_materialTable;
		delete m_btnOK;
	}

	void SelectMaterialDialog::ShowModal(Material* mtrl)
	{
		m_dialogResult = false;
		m_mtrl = mtrl;
		m_form->ShowModal();
	}

	void SelectMaterialDialog::ButtonOK_Pressed(Button* ctrl)
	{
		m_dialogResult = true;
		m_form->Close();
	}
	void SelectMaterialDialog::Form_Closed(Control* ctrl)
	{
		if (!m_dialogResult)
			return;

		m_mtrl->ExternalReferenceName = m_materialTable->getItems().at(m_materialTable->getSelectedRowIndex(),0) + L".mtrl";

		m_mtrl = nullptr;
		
		m_parent->UpdateSelectedMaterial();
	}
}