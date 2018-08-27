
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

#include "MaterialDocument.h"

#include "APDesigner/MainWindow.h"
#include "APDesigner/UIResources.h"
#include "APDesigner/CommonDialog/ChooseColor.h"

using namespace APDesigner::CommonDialog;

namespace APDesigner
{
	MaterialDocument::MaterialDocument(MainWindow* window, EditorExtension* ext, const String& name, const String& file)
		: Document(window, ext), m_filePath(file), 
		m_name(name), m_model(0), m_modelSData(0), m_material(0),
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
		m_camera->setFar(1000);
		m_camera->setNear(1);
		m_sceneRenderer->RegisterCamera(m_camera);

		
		fl = FileLocation(L"ball.mesh");
		m_modelSData = ModelManager::getSingleton().CreateInstanceUnmanaged(getMainWindow()->getDevice(), fl);


		m_model = new Model(new ResourceHandle<ModelSharedData>(m_modelSData),0);

		m_object.setmdl(m_model);

		m_scene.AddObject(&m_object);

		const StyleSkin* skin = window->getUISkin();

		m_modelViewer = new PictureBox(skin, Point(10,10 + 17), 1);
		m_modelViewer->setSize(512, 512);
		m_modelViewer->eventPictureDraw.Bind(this, &MaterialDocument::ModelView_Draw);

		{

			m_applyMtrl = new Button(skin, Point(21 + 522 + 100 + 220, 159), 150, L"Apply Changes");
			m_applyMtrl->eventRelease.Bind(this, &MaterialDocument::BtnApplyMtrl_Pressed);

		}
		{
			int sx = 21 + 522;
			int sy = 10+17;
			m_cfAmbient = new ColorField(skin, Point(sx, sy), L"Ambient", CV_Red);
			m_cfDiffuse = new ColorField(skin, Point(sx + 250, sy), L"Diffuse", CV_Red);
			sy += 30;

			m_cfSpecular = new ColorField(skin, Point(sx, sy), L"Specular", CV_Red);
			m_cfEmissive = new ColorField(skin, Point(sx + 250, sy), L"Emissive", CV_Red);
			sy += 30;

			Label* lbl = new Label(skin, Point(sx, sy), L"Shininess", 100);
			m_mtrlPanelLabels.Add(lbl);

			m_tbShinness = new TextBox(skin, Point(sx + 100, sy), 200, L"");

			sy += 40;
			lbl = new Label(skin, Point(sx, sy), L"Texture1", 100);
			m_mtrlPanelLabels.Add(lbl);

			m_tbTex1 = new TextBox(skin, Point(sx + 100, sy), 200, L"");

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Texture2", 100);
			m_mtrlPanelLabels.Add(lbl);

			m_tbTex2 = new TextBox(skin, Point(sx + 100, sy), 200, L"");
			
			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Texture3", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex3 = new TextBox(skin, Point(sx + 100, sy), 200, L"");


			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Texture4", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex4 = new TextBox(skin, Point(sx + 100, sy), 200, L"");


			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Texture5", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_tbTex5 = new TextBox(skin, Point(sx + 100, sy), 200, L"");


			//sy += 25;
			//lbl = new Label(Point(sx, sy), L"Texture6", 100);
			//lbl->SetSkin(window->getUISkin());
			//m_mtrlPanelLabels.Add(lbl);
			//m_tbTex6 = new TextBox(Point(sx + 100, sy), 200, L"");
			//m_tbTex6->SetSkin(window->getUISkin());


			sy += 35;
			lbl = new Label(skin, Point(sx, sy), L"Priority[0,127]", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_tbPriority = new TextBox(skin, Point(sx + 100, sy), 200, L"");

			m_cbDepthTest = new CheckBox(skin, Point(sx+ 250 + 100, sy), L"DepthTest", false);


			sy += 25;
			List<String> items;
			items.Add(CullModeConverter[CullMode::None]);
			items.Add(CullModeConverter[CullMode::Clockwise]);
			items.Add(CullModeConverter[CullMode::CounterClockwise]);
			lbl = new Label(skin, Point(sx, sy), L"Cull Mode", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_cbCull = new ComboBox(skin, Point(sx + 100, sy), 200, items);

			m_cbDepthWrite = new CheckBox(skin, Point(sx+ 250 + 100, sy), L"DepthWrite", false);

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"AlphaTest[0,1]", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_tbAlphaTest = new TextBox(skin, Point(sx + 100, sy), 200, L"");
			

			sy += 25;

			m_cbTransparent = new CheckBox(skin, Point(sx + 250 + 100, sy), L"Transparent", false);

			items.Clear();
			items.Add(BlendFunctionConverter[BlendFunction::Add]);
			items.Add(BlendFunctionConverter[BlendFunction::Subtract]);
			items.Add(BlendFunctionConverter[BlendFunction::ReverseSubtract]);
			items.Add(BlendFunctionConverter[BlendFunction::Max]);
			items.Add(BlendFunctionConverter[BlendFunction::Min]);
			lbl = new Label(skin, Point(sx, sy), L"Blend Func", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_cbBlendFunction = new ComboBox(skin, Point(sx + 100, sy), 200, items);

			items.Clear();
			items.Add(BlendConverter[Blend::Zero]);
			items.Add(BlendConverter[Blend::One]);
			items.Add(BlendConverter[Blend::SourceColor]);
			items.Add(BlendConverter[Blend::InverseSourceColor]);
			items.Add(BlendConverter[Blend::SourceAlpha]);
			items.Add(BlendConverter[Blend::InverseSourceAlpha]);
			items.Add(BlendConverter[Blend::DestinationAlpha]);
			items.Add(BlendConverter[Blend::InverseDestinationAlpha]);
			items.Add(BlendConverter[Blend::DestinationColor]);
			items.Add(BlendConverter[Blend::InverseDestinationColor]);

			items.Add(BlendConverter[Blend::SourceAlphaSaturation]);
			items.Add(BlendConverter[Blend::BlendFactor]);
			
			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Src Blend", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_cbSrcBlend = new ComboBox(skin, Point(sx + 100, sy), 200, items);

			sy += 25;
			lbl = new Label(skin, Point(sx, sy), L"Dest Blend", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_cbDstBlend = new ComboBox(skin, Point(sx + 100, sy), 200, items);

			sy += 35;
			lbl = new Label(skin, Point(sx, sy), L"Pass Flags", 100);
			m_mtrlPanelLabels.Add(lbl);
			m_pbPassFlag = new PictureBox(skin, Point(sx+100, sy),1);
			//m_pbPassFlag->Size.Y = getDocumentForm()->getFontRef()->getLineHeight();
			m_pbPassFlag->setWidth(256);
			m_pbPassFlag->eventPictureDraw.Bind(this, &MaterialDocument::PassFlags_Draw);

			m_btnPassFlag = new Button(skin, Point(sx+100+300, sy), L"Edit");
			m_btnPassFlag->eventPress.Bind(this, &MaterialDocument::PassButton_Pressed);
		}
		
		getDocumentForm()->setMinimumSize(Point(1070,512+137));
		
		//getDocumentForm()->setMaximumSize(Point(1071,512+138));
		getDocumentForm()->setTitle(file);

		m_passEditor = new PassFlagDialog(window, window->getDevice());
	}

	MaterialDocument::~MaterialDocument()
	{
		delete m_modelViewer;
		AssignDisplayMaterial(nullptr);

		delete m_model;
		
		if (m_material)
		{
			delete m_material;
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
		
		delete m_applyMtrl;

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
	

	void MaterialDocument::LoadRes()
	{
		if (m_material)
		{
			delete m_material;
		}

		FileLocation fl(m_filePath);
		MaterialData md;
		md.Load(fl);
		m_material->Load(md);
		
		AssignDisplayMaterial(m_material);
		m_model->RebuildROPCache();

		m_distance = 12;

		DisplayMaterialEditor(m_material);
	}
	void MaterialDocument::SaveRes()
	{
		MaterialData data;
		m_material->Save(data);
		data.Save(FileOutStream(m_filePath));
	}
	void MaterialDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_modelViewer);
		//getDocumentForm()->getControls().Add(m_btnZoomIn);
		//getDocumentForm()->getControls().Add(m_btnZoomOut);

		getDocumentForm()->getControls().Add(m_applyMtrl);
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
	void MaterialDocument::Update(const GameTime* time)
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
	void MaterialDocument::Render()
	{
		m_sceneRenderer->RenderScene(&m_scene);
	}

	void MaterialDocument::AssignDisplayMaterial(Material* mtrl)
	{
		for (Mesh* mesh : m_modelSData->getEntities())
		{
			for (Material*& m : mesh->getMaterials())
			{
				m = mtrl;
			}
		}
	}

	void MaterialDocument::PassButton_Pressed(Button* ctrl)
	{
		m_passEditor->ShowModal(m_material);
	}
	void MaterialDocument::ModelView_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		SceneProcedure* proc = m_sceneRenderer->getSelectedProc();

		if (proc)
		{
			RenderTarget* rt = proc->FindRenderTargetVar(L"View");

			Texture* texture = rt->GetColorTexture();
			sprite->Draw(texture, *dstRect, 0, CV_White);
		}
	}
	void MaterialDocument::PassFlags_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		Material* mtrl = m_material;
		if (!mtrl)
			return;

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
	void MaterialDocument::BtnApplyMtrl_Pressed(Button* ctrl)
	{
		Material* mtrl = m_material;
		if (!mtrl)
			return;

		mtrl->Ambient = Color4(m_cfAmbient->GetValue());
		mtrl->Diffuse = Color4(m_cfDiffuse->GetValue());
		mtrl->Specular = Color4(m_cfSpecular->GetValue());
		mtrl->Emissive = Color4(m_cfEmissive->GetValue());
		mtrl->Power = StringUtils::ParseSingle(m_tbShinness->getText());

		mtrl->SetTextureName(0, m_tbTex1->getText());
		mtrl->SetTextureName(1, m_tbTex2->getText());
		mtrl->SetTextureName(2, m_tbTex3->getText());
		mtrl->SetTextureName(3, m_tbTex4->getText());
		mtrl->SetTextureName(4, m_tbTex5->getText());

		mtrl->setPriority(StringUtils::ParseUInt32(m_tbPriority->getText()));
		mtrl->AlphaReference = StringUtils::ParseUInt32(m_tbAlphaTest->getText());

		mtrl->DepthTestEnabled = m_cbDepthTest->Checked;
		mtrl->DepthWriteEnabled = m_cbDepthWrite->Checked;

		mtrl->IsBlendTransparent = m_cbTransparent->Checked;

		if (m_cbSrcBlend->getSelectedIndex()!=-1)
		{
			mtrl->SourceBlend = BlendConverter[m_cbSrcBlend->getItems()[m_cbSrcBlend->getSelectedIndex()]];
		}
		if (m_cbDstBlend->getSelectedIndex()!=-1)
		{
			mtrl->DestinationBlend = BlendConverter[m_cbDstBlend->getItems()[m_cbDstBlend->getSelectedIndex()]];
		}
		if (m_cbBlendFunction->getSelectedIndex() !=-1)
		{
			mtrl->BlendFunction = BlendFunctionConverter[m_cbBlendFunction->getItems()[m_cbBlendFunction->getSelectedIndex()]];
		}

		if (m_cbCull->getSelectedIndex() !=-1)
		{
			mtrl->Cull = CullModeConverter[m_cbCull->getItems()[m_cbCull->getSelectedIndex()]];
		}

		mtrl->Reload();
	}

	void MaterialDocument::DisplayMaterialEditor(Material* mtrl)
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

			m_tbShinness->SetText(StringUtils::SingleToString(mtrl->Power));

			m_tbTex1->SetText(mtrl->GetTextureName(0));
			m_tbTex2->SetText(mtrl->GetTextureName(1));
			m_tbTex3->SetText(mtrl->GetTextureName(2));
			m_tbTex4->SetText(mtrl->GetTextureName(3));
			m_tbTex5->SetText(mtrl->GetTextureName(4));

			m_tbPriority->SetText(StringUtils::UIntToString(mtrl->getPriority()));
			m_tbAlphaTest->SetText(StringUtils::UIntToString(mtrl->AlphaReference));

			m_cbDepthTest->Checked = mtrl->DepthTestEnabled;
			m_cbDepthWrite->Checked = mtrl->DepthWriteEnabled;

			m_cbTransparent->Checked = mtrl->IsBlendTransparent;

			m_cbSrcBlend->SetSelectionByName(BlendConverter[mtrl->SourceBlend]);
			m_cbDstBlend->SetSelectionByName(BlendConverter[mtrl->DestinationBlend]);
			m_cbBlendFunction->SetSelectionByName(BlendFunctionConverter[mtrl->BlendFunction]);
			m_cbCull->SetSelectionByName(CullModeConverter[mtrl->Cull]);
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

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	ColorField::ColorField(const StyleSkin* skin, const Point& position, const String& text, ColorValue defaultColor)
		: Control(skin, position),  m_color(defaultColor), m_skin(skin)
	{
		Initialize(skin);

		m_lblAmbient->SetText(text);
	}
	ColorField::~ColorField()
	{
		DELETE_AND_NULL(m_lblAmbient);
		DELETE_AND_NULL(m_pbAmbient);
		DELETE_AND_NULL(m_btnAmbient);
	}

	void ColorField::Initialize(const StyleSkin* skin)
	{
		assert(m_skin);
		int32 lineHeight = m_skin->ContentTextFont->getLineHeightInt();

		m_lblAmbient = new Label(skin, Position, L"", 80, lineHeight);
		
		m_pbAmbient = new PictureBox(skin, Position + Point(100, 0), 1);
		m_pbAmbient->setSize(50, lineHeight);
		m_pbAmbient->eventPictureDraw.Bind(this, &ColorField::PictureBox_Draw);

		m_btnAmbient = new Button(skin, Position + Point(160, 0), L"...");
		
		m_size.Y = m_lblAmbient->getHeight();
		m_size.X = m_btnAmbient->Position.X + m_btnAmbient->getWidth();
		
		m_btnAmbient->SetSizeY(lineHeight);
		m_btnAmbient->Position.Y += (m_lblAmbient->getHeight() - m_btnAmbient->getHeight())/2;
		m_btnAmbient->eventPress.Bind(this, &ColorField::Button_Press);
	}
	void ColorField::Draw(Sprite* sprite)
	{
		m_lblAmbient->Draw(sprite);
		m_pbAmbient->Draw(sprite);
		m_btnAmbient->Draw(sprite);
	}
	void ColorField::Update(const GameTime* time)
	{
		m_lblAmbient->Position = Position;
		m_pbAmbient->Position = Position + Point(100, 0);
		m_btnAmbient->Position = Position + Point(160, 0);
		m_btnAmbient->Position.Y += (m_lblAmbient->getHeight() - m_btnAmbient->getHeight())/2;

		SetControlBasicStates({m_lblAmbient, m_pbAmbient, m_btnAmbient});

		m_lblAmbient->Update(time);
		m_pbAmbient->Update(time);
		m_btnAmbient->Update(time);
	}

	void ColorField::PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		sprite->Draw(SystemUI::GetWhitePixel(), *dstRect, 0, m_color);
	}
	void ColorField::Button_Press(Button* ctrl)
	{
		ChooseColorDialog dlg(m_color);
		if (dlg.ShowDialog() == DLGRES_OK)
		{
			m_color = dlg.getSelectedColor();

			eventColorSelected.Invoke(this);
		}
	}

	void ColorField::SetValue(const Color4& color)
	{
		m_color = color.ToArgb();
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	PassFlagDialog::PassFlagDialog(MainWindow* window, RenderDevice* device)
	{
		const StyleSkin* skin = window->getUISkin();

		m_form = new Form(skin, device, FBS_Sizable, L"Pass Editor");

		Apoc3D::Math::Rectangle rect = SystemUI::GetUIArea(device);
		m_form->setSize(980, 450);
		m_form->Position = (Point(rect.getSize()) - m_form->getSize()) / 2;

		m_form->eventClosed.Bind(this, &PassFlagDialog::Form_Closed);
		int ofsX = 10;
		int ofsY = 27;
		int counter = 0;
		for (int i=0;i<4;i++)
		{
			for (int j=0;j<16;j++)
			{
				Label* lbl = new Label(skin, Point(i * 250+ofsX,j * 25+ofsY), L"Pass " + StringUtils::IntToString(counter++), 50);
				m_lblTable.Add(lbl);

				TextBox* text = new TextBox(skin, Point(i * 250 + ofsX + lbl->getWidth(), j * 25 + ofsY), 150, L"");
				m_tbTable.Add(text);
			}
		}

		for (int i=0;i<m_lblTable.getCount();i++)
		{
			m_form->getControls().Add(m_lblTable[i]);
			m_form->getControls().Add(m_tbTable[i]);
		}

		SystemUI::Add(m_form);
	}

	PassFlagDialog::~PassFlagDialog()
	{
		m_form->eventClosed.Unbind(this, &PassFlagDialog::Form_Closed);
		SystemUI::Remove(m_form);
		delete m_form;
		for (int i=0;i<m_lblTable.getCount();i++)
		{
			delete m_lblTable[i];
			delete m_tbTable[i];
		}
	}

	void PassFlagDialog::ShowModal(Material* mtrl)
	{
		m_mtrl = mtrl;
		for (int i=0;i<MaxScenePass;i++)
		{
			m_tbTable[i]->SetText(mtrl->GetPassEffectName(i));
		}
		m_form->ShowModal();
	}

	void PassFlagDialog::Form_Closed(Control* ctrl)
	{
		uint64 passFlags = 0;
		for (int i=0;i<MaxScenePass;i++)
		{
			const String& tv = m_tbTable[i]->getText();

			if (tv.size())
			{
				passFlags |= (uint64)1<<i;
			}
			//passFlags = passFlags << (uint)1;
			m_mtrl->SetPassEffectName(i, tv);
			if (tv.size())
			{
				m_mtrl->SetPassEffect(i, EffectManager::getSingleton().getEffect(m_mtrl->GetPassEffectName(i)));
			}
			else
			{
				m_mtrl->SetPassEffect(i, 0);
			}
			
		}
		if (!passFlags)
			passFlags = 1;
		m_mtrl->setPassFlags(passFlags);
		m_mtrl = 0;
	}
}