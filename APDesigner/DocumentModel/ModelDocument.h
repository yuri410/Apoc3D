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

#ifndef MODELDOCUMENT_H
#define MODELDOCUMENT_H

#include "APDesigner/Document.h"

namespace APDesigner
{
	class CopyMaterialDialog;
	class SelectMaterialDialog;

	class ModelDocument : public Document
	{
	public:
		ModelDocument(MainWindow* window, EditorExtension* ext, const String& name, const String& file, const String& animationFile);
		~ModelDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const GameTime* time);
		virtual void Render();

		void UpdateSelectedPart();
		void UpdateSelectedMaterial();
	private:
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
		String m_animPath;

		ModelWrapper m_object;
		SimpleSceneManager m_scene;
		SceneRenderer* m_sceneRenderer;

		ModelSharedData* m_modelSData;
		Model* m_model;

		const AnimationData* m_animData;

		ChaseCamera* m_camera;
		float m_xang;
		float m_yang;
		float m_distance;

		PictureBox* m_modelViewer;
		ComboBox* m_passViewSelect;
		
		List<Label*> m_labels;

		PictureBox* m_pbTime;
		TextBox* m_tbMKeyTime;
		TextBox* m_tbMKeyIndex;
		Button* m_btnRefreshTimeLine;
		Button* m_btnModify;
		Button* m_btnAddMkey;
		Button* m_btnRemoveMKey;
		
		// utilities
		Button* m_recenterModel;
		Button* m_revertZ;
		Button* m_swapYZ;
		Button* m_rotateY;
		Button* m_rotateZ;
		Button* m_zoomIn;
		Button* m_zoomOut;
		Button* m_setSequenceImages;
		Button* m_applyColorToAll;
		Button* m_applyFXToAll;
		Button* m_autoTex;
		Button* m_applyTranspToAll;

		ComboBox* m_cbMesh;
		ComboBox* m_cbMeshPart;
		ComboBox* m_cbSubMtrl;

		Button* m_applyAllMtrl;
		Button* m_applyMtrl;
		Button* m_addMtrlFrame;
		Button* m_removeMtrlFrame;
		TextBox* m_tbMeshName;

		CheckBox* m_cbUseRef;
		TextBox* m_tbRefMaterialName;
		Button* m_btnRefMaterial;

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


		CheckBox* m_cbDepthTest;
		CheckBox* m_cbDepthWrite;


		CheckBox* m_cbTransparent;
		ComboBox* m_cbSrcBlend;
		ComboBox* m_cbDstBlend;
		ComboBox* m_cbBlendFunction;

		ComboBox* m_cbCull;
		PassFlagDialog* m_passEditor;
		CopyMaterialDialog* m_batchCopyMtrl;
		SelectMaterialDialog* m_selectMtrl;

		void PassButton_Pressed(Button* ctrl);
		void CBMesh_SelectionChanged(ComboBox* ctrl);
		void CBMeshPart_SelectionChanged(ComboBox* ctrl);
		void CBSubMtrl_SelectionChanged(ComboBox* ctrl);
		void PBTime_Pressed(Control* ctrl);

		void BtnApplyAllMtrl_Pressed(Button* ctrl);
		void BtnApplyMtrl_Pressed(Button* ctrl);
		void BtnAddMtrl_Pressed(Button* ctrl);
		void BtnRemoveMtrl_Pressed(Button* ctrl);

		void ModelView_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void Timeline_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void PassFlags_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);

		void Transform(const Matrix& transform);

		void RecenterModel_Pressed(Button* ctrl);
		void RevertZ_Pressed(Button* ctrl);
		void RevertYZ_Pressed(Button* ctrl);
		void RotY_Pressed(Button* ctrl);
		void RotZ_Pressed(Button* ctrl);
		void ZoomIn_Pressed(Button* ctrl);
		void ZoomOut_Pressed(Button* ctrl);
		void SetSequenceImages_Pressed(Button* ctrl);
		void ApplyColorToAll_Pressed(Button* ctrl);
		void ApplyFXToAll_Pressed(Button* ctrl);
		void AutoTex_Pressed(Button* ctrl);
		void TranspAll_Pressed(Button* ctrl);

		void PassViewSelect_SelectionChanged(ComboBox* ctrl);

		void CBUseRef_Checked(CheckBox* ctrl);
		void BtnRefMtrl_Pressed(Button* ctrl);

		void DisplayMaterialEditor(Material* mtrl, bool usingRef);

		Mesh* GetSelectedMesh();
		Material* GetSelectedMaterial();
		List<Material*>* GetSelectedMeshSubpartMaterials();

	};

	class CopyMaterialDialog
	{
	public:
		CopyMaterialDialog(ModelDocument* parent, MainWindow* window, RenderDevice* device);
		~CopyMaterialDialog();

		void ShowModal(List<Material*>* mtrls);

	private:
		ModelDocument* m_parent;
		Form* m_form;

		Label* m_lblTextureName;
		TextBox* m_tbTextureName;

		Label* m_lblPreview;

		Label* m_lblStartNumber;
		TextBox* m_tbStartNumber;
		Label* m_lblEndNumber;
		TextBox* m_tbEndNumber;

		Button* m_btnOK;

		bool m_dialogResult;

		//MeshMaterialSet<Material*>* m_mtrl;
		//int m_selectedSet;
		List<Material*>* m_mtrls;

		void ButtonOK_Pressed(Button* ctrl);
		void Form_Closed(Control* ctrl);
		void Config_Changed(TextBox* ctrl);
	};

	class SelectMaterialDialog
	{
	public:
		SelectMaterialDialog(ModelDocument* parent, MainWindow* window, RenderDevice* device);
		~SelectMaterialDialog();

		void ShowModal(Material* mtrl);

	private:
		void ButtonOK_Pressed(Button* ctrl);
		void Form_Closed(Control* ctrl);

		ModelDocument* m_parent;
		Form* m_form;

		ListView* m_materialTable;

		Button* m_btnOK;

		Material* m_mtrl;
		bool m_dialogResult;

	};
}

#endif