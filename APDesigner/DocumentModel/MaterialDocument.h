#pragma once

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

#ifndef MATERIALDOCUMENT_H
#define MATERIALDOCUMENT_H

#include "APDesigner/Document.h"

namespace APDesigner
{
	class MaterialDocument : public Document
	{
	public:
		MaterialDocument(MainWindow* window, EditorExtension* ext, const String& name, const String& file);
		~MaterialDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const AppTime* time);
		virtual void Render();

	private:
		void PassButton_Pressed(Button* ctrl);
		void BtnApplyMtrl_Pressed(Button* ctrl);

		void ModelView_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void PassFlags_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);

		void DisplayMaterialEditor(Material* mtrl);
		
		void AssignDisplayMaterial(Material* mtrl);

		class ModelWrapper : public Entity 
		{
		public:
			ModelWrapper() 
			{
				m_position = Vector3::Zero;
				m_transformation.LoadIdentity();
			}

			void setmdl(Model* mdl) 
			{
				m_models[0] = mdl;
			}

		};
		
		String m_name;
		String m_filePath;

		ModelWrapper m_object;
		SimpleSceneManager m_scene;
		SceneRenderer* m_sceneRenderer;

		ModelSharedData* m_modelSData;
		Model* m_model;
		Material* m_material;


		ChaseCamera* m_camera;
		float m_xang;
		float m_yang;
		float m_distance;

		PictureBox* m_modelViewer;

		List<Label*> m_labels;

		Button* m_applyMtrl;

		// material specific parameters

		ColorField* m_cfAmbient;
		ColorField* m_cfDiffuse;
		ColorField* m_cfSpecular;
		ColorField* m_cfEmissive;
		TextBox* m_tbShinness;
		

		PictureBox* m_pbPassFlag;
		Button* m_btnPassFlag;

		List<Label*> m_mtrlPanelLabels;


		TextBox* m_tbTex1;
		TextBox* m_tbTex2;
		TextBox* m_tbTex3;
		TextBox* m_tbTex4;
		TextBox* m_tbTex5;

		//ListBox* m_customParameterList;

		TextBox* m_tbPriority;
		TextBox* m_tbAlphaTest;


		CheckBox* m_cbDepthTest;
		CheckBox* m_cbDepthWrite;


		CheckBox* m_cbTransparent;
		ComboBox* m_cbSrcBlend;
		ComboBox* m_cbDstBlend;
		ComboBox* m_cbBlendFunction;

		ComboBox* m_cbCull;
		PassFlagDialog* m_passEditor;

	};

	class APDAPI PassFlagDialog
	{
	public:
		PassFlagDialog(MainWindow* window, RenderDevice* device);
		~PassFlagDialog();

		void ShowModal(Material* mtrl);

	private:
		void Form_Closed(Control* ctrl);

		Form* m_form;
		List<Label*> m_lblTable;
		List<TextBox*> m_tbTable;

		Material* m_mtrl;
	};

	class APDAPI ColorField : public Control
	{
	public:
		ColorField(const StyleSkin* skin, const Point& position, const String& text, ColorValue defaultColor);
		~ColorField();

		virtual void Draw(Sprite* sprite);
		virtual void Update(const AppTime* time);

		void SetValue(const Color4& color);
		ColorValue GetValue() const { return m_color; }

		void SetText(const String& txt) const { m_lblAmbient->SetText(txt); }

		UIEventHandler eventColorSelected;

	private:
		void Initialize(const StyleSkin* skin);

		void PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void Button_Press(Button* ctrl);

		Label* m_lblAmbient = nullptr;
		PictureBox* m_pbAmbient = nullptr;
		Button* m_btnAmbient = nullptr;
		ColorValue m_color;

		const StyleSkin* m_skin;
	};
}

#endif