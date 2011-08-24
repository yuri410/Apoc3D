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
#ifndef MODELDOCUMENT_H
#define MODELDOCUMENT_H

#include "Document.h"
#include "Scene/SimpleSceneManager.h"
#include "Scene/SceneObject.h"
#include "UILib/Control.h"

using namespace Apoc3D;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::Animation;
using namespace Apoc3D::Scene;

namespace APDesigner
{
	class ModelDocument : public Document
	{
	public:
		ModelDocument(MainWindow* window, const String& name, const String& file, const String& animationFile);
		~ModelDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const GameTime* const time);
		virtual void Render();
	private:
		class ModelWrapper : public Entity 
		{
		public:
			ModelWrapper() 
			{
				m_position = Vector3Utils::Zero;
				m_transformation.LoadIdentity();
			}

			void setmdl(Model* mdl) 
			{
				m_models[0] = mdl;
			}

		};
		class ColorField : public Control
		{
		public:
			ColorField(const Point& position, ColorValue defaultColor);
			~ColorField();

			virtual void Initialize(RenderDevice* device);
			virtual void Draw(Sprite* sprite);
			virtual void Update(const GameTime* const time);
		private:
			Label* m_lblAmbient;
			PictureBox* m_pbAmbient;
			Button* m_btnAmbient;
			ColorValue m_color;

			void PictureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
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

		List<Label*> m_labels;

		//Label* m_lblTimeline;
		//Label* m_lblKeyframeParam;
		//Label* m_lblMtrlKeyTime;
		//Label* m_lblMtrlKeyIndex;

		//Label* m_lblMesh;
		//Label* m_lblMtrlPart;
		PictureBox* m_pbTime;
		TextBox* m_tbMKeyTime;
		TextBox* m_tbMKeyIndex;
		Button* m_btnRefreshTimeLine;
		Button* m_btnModify;
		Button* m_btnAddMkey;
		Button* m_btnRemoveMKey;

		ComboBox* m_cbMesh;
		ComboBox* m_cbMtrlPart;

		Button* m_applyMtrl;
		Button* m_addMtrlFrame;
		Button* m_removeMtrlFrame;



		ColorField* m_cfAmbient;
		ColorField* m_cfDiffuse;
		ColorField* m_cfSpecular;
		ColorField* m_cfEmissive;

		//Label* m_lblShinness;
		TextBox* m_cbShinness;
		
		//Label* m_lblPassFlag;
		PictureBox* m_pbPassFlag;
		Button* m_btnPassFlag;

		List<Label*> m_mtrlPanelLabels;

		//Label* m_lblTex1;
		//Label* m_lblTex2;
		//Label* m_lblTex3;
		//Label* m_lblTex4;
		//Label* m_lblTex5;
		//Label* m_lblTex6;

		TextBox* m_tbTex1;
		TextBox* m_tbTex2;
		TextBox* m_tbTex3;
		TextBox* m_tbTex4;
		TextBox* m_tbTex5;
		TextBox* m_tbTex6;

		ListBox* m_customParameterList;

		//Label* m_lblPriority;
		TextBox* m_tbPriority;

		CheckBox* m_cbDepthTest;
		CheckBox* m_cbDepthWrite;

		//Label* m_lblAlphaTest;
		TextBox* m_tbAlphaTest;

		CheckBox* m_cbTransparent;
		//Label* m_lblSrcBlend;
		//Label* m_lblDstBlend;
		//Label* m_lblBlendFunction;
		ComboBox* m_cbSrcBlend;
		ComboBox* m_cbDstBlend;
		ComboBox* m_cbBlendFunction;

		void PixtureBox_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);
	};
	
}

#endif