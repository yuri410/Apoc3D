#pragma once
#ifndef APD_MODELDOCUMENT_H
#define APD_MODELDOCUMENT_H


#include "APDesigner/Document.h"

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
		virtual void Update(const GameTime* time);
		virtual void Render();

	private:
		void UIViewer_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect);

		String m_filePath;

		Form* m_editingForm;

		PictureBox* m_uiViewer;

	};
}


#endif