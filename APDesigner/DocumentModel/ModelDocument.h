#pragma once
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
#ifndef MODELDOCUMENT_H
#define MODELDOCUMENT_H

#include "APDesigner/Document.h"

#include "apoc3d/Scene/SimpleSceneManager.h"
#include "apoc3d/Scene/SceneObject.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/Graphics/ModelTypes.h"

using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Scene;

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
		virtual void Update(const GameTime* const time);
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
		
		FastList<Label*> m_labels;

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

		FastList<Label*> m_mtrlPanelLabels;


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
		CopyMaterialDialog* m_batchCopyMtrl;
		SelectMaterialDialog* m_selectMtrl;

		void PassButton_Pressed(Control* ctrl);
		void CBMesh_SelectionChanged(Control* ctrl);
		void CBMeshPart_SelectionChanged(Control* ctrl);
		void CBSubMtrl_SelectionChanged(Control* ctrl);
		void PBTime_Pressed(Control* ctrl);

		void BtnApplyAllMtrl_Pressed(Control* ctrl);
		void BtnApplyMtrl_Pressed(Control* ctrl);
		void BtnAddMtrl_Pressed(Control* ctrl);
		void BtnRemoveMtrl_Pressed(Control* ctrl);

		void ModelView_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void Timeline_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
		void PassFlags_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);

		void Transform(const Matrix& transform);

		void RecenterModel_Pressed(Control* ctrl);
		void RevertZ_Pressed(Control* ctrl);
		void RevertYZ_Pressed(Control* ctrl);
		void RotY_Pressed(Control* ctrl);
		void RotZ_Pressed(Control* ctrl);
		void ZoomIn_Pressed(Control* ctrl);
		void ZoomOut_Pressed(Control* ctrl);
		void SetSequenceImages_Pressed(Control* ctrl);
		void ApplyColorToAll_Pressed(Control* ctrl);
		void ApplyFXToAll_Pressed(Control* ctrl);
		void AutoTex_Pressed(Control* ctrl);

		void PassViewSelect_SelectionChanged(Control* ctrl);

		void CBUseRef_Checked(Control* ctrl);
		void BtnRefMtrl_Pressed(Control* ctrl);

		void DisplayMaterialEditor(Material* mtrl, bool usingRef);

	};

	class CopyMaterialDialog
	{
	public:
		CopyMaterialDialog(ModelDocument* parent, MainWindow* window, RenderDevice* device);
		~CopyMaterialDialog();

		void ShowModal(MeshMaterialSet<Material*>* mtrl, int selectedSet);

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

		MeshMaterialSet<Material*>* m_mtrl;
		int m_selectedSet;

		void ButtonOK_Pressed(Control* ctrl);
		void Form_Closed(Control* ctrl);
		void Config_Changed(Control* ctrl);
	};

	class SelectMaterialDialog
	{
	public:
		SelectMaterialDialog(ModelDocument* parent, MainWindow* window, RenderDevice* device);
		~SelectMaterialDialog();

		void ShowModal(Material* mtrl);

	private:
		ModelDocument* m_parent;
		Form* m_form;

		ListView* m_materialTable;

		Button* m_btnOK;

		Material* m_mtrl;
		bool m_dialogResult;

		void ButtonOK_Pressed(Control* ctrl);
		void Form_Closed(Control* ctrl);
	};
}

#endif