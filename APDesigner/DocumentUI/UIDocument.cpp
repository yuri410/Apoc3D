#include "UIDocument.h"
#include "APDesigner/MainWindow.h"
#include "APDesigner/UIResources.h"
#include "APDesigner/CommonDialog/ChooseColor.h"

using namespace APDesigner::CommonDialog;

namespace APDesigner
{
	UIDocument::UIDocument(MainWindow* window, EditorExtension* ext, const String& file)
		: Document(window, ext), m_editingForm(nullptr), m_filePath(file)
	{
		m_uiViewer = new PictureBox(window->getUISkin(), Point(15, 27), 1);
		m_uiViewer->setSize(512, 512);
		m_uiViewer->eventPictureDraw.Bind(this, &UIDocument::UIViewer_Draw);

		getDocumentForm()->setMinimumSize(1000, 600);

		getDocumentForm()->setTitle(file);
	}

	UIDocument::~UIDocument()
	{
		delete m_uiViewer;
	}


	void UIDocument::LoadRes()
	{
		// create virtual form here
		m_editingForm = new Form(getMainWindow()->getUISkin(), getMainWindow()->getDevice());
		m_editingForm->Visible = true;
	}
	void UIDocument::SaveRes()
	{

	}

	void UIDocument::Initialize(RenderDevice* device)
	{
		getDocumentForm()->getControls().Add(m_uiViewer);

		Document::Initialize(device);

	}
	void UIDocument::Update(const GameTime* time)
	{
		Document::Update(time);

		m_uiViewer->setSize(getDocumentForm()->getSize() - Point(30, 50));
	}
	void UIDocument::Render()
	{

	}

	void UIDocument::UIViewer_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		sprite->Draw(SystemUI::GetWhitePixel(), *dstRect, nullptr, CV_LightGray);

		Matrix orginalTrans;
		if (!sprite->isUsingStack())
		{
			orginalTrans = sprite->getTransform();
		}


		Matrix offTrans;
		Matrix::CreateTranslation(offTrans, (float)dstRect->X + 7, (float)dstRect->Y + 7, 0);

		sprite->MultiplyTransform(offTrans);
		if (m_editingForm)
		{
			m_editingForm->Draw(sprite);
		}

		if (sprite->isUsingStack())
		{
			sprite->PopTransform();
		}
		else
		{
			sprite->SetTransform(orginalTrans);
		}
	}

}