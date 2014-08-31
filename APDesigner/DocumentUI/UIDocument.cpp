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

		m_uiViewer = new PictureBox(Point(15, 27), 1);
		m_uiViewer->Size = Point(512,512);
		m_uiViewer->SetSkin(window->getUISkin());
		m_uiViewer->eventPictureDraw.Bind(this, &UIDocument::UIViewer_Draw);


		getDocumentForm()->setMinimumSize(Point(1000,600));

		getDocumentForm()->setTitle(file);
	}

	UIDocument::~UIDocument()
	{
		delete m_uiViewer;
	}


	void UIDocument::LoadRes()
	{
		// create virtual form here
		m_editingForm = new Form();
		m_editingForm->SetSkin(getMainWindow()->getUISkin());
		m_editingForm->Initialize(getMainWindow()->getDevice());
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

		m_uiViewer->Size = getDocumentForm()->Size - Point(30, 50);
	}
	void UIDocument::Render()
	{

	}

	void UIDocument::UIViewer_Draw(Sprite* sprite, Apoc3D::Math::Rectangle* dstRect)
	{
		sprite->Draw(getMainWindow()->getUISkin()->WhitePixelTexture, *dstRect, nullptr, CV_LightGray);

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