#pragma once
#ifndef APD_MODELDOCUMENT_H
#define APD_MODELDOCUMENT_H


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
	class UIDocument : public Document
	{
	public:
		UIDocument(MainWindow* window, EditorExtension* ext, const String& file);
		~UIDocument();

		virtual void LoadRes();
		virtual void SaveRes();
		virtual bool IsReadOnly() { return false; };

		virtual void Initialize(RenderDevice* device);
		virtual void Update(const GameTime* const time);
		virtual void Render();

	private:
		void UIViewer_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);

		String m_filePath;

		Form* m_editingForm;

		PictureBox* m_uiViewer;

	};
}


#endif