#include "Document.h"

#include "UILib/Form.h"
#include "MainWindow.h"

namespace APDesigner
{
	Document::Document(MainWindow* window)
		: m_activated(false), m_mainWindow(window)
	{
		m_docForm = new Form(FBS_Sizable, L"New document");
		m_docForm->Position.Y = 20;
		m_docForm->SetSkin(window->getUISkin());
		
	}

	Document::~Document()
	{
		delete m_docForm;
	}

	void Document::Initialize(RenderDevice* device)
	{
		m_docForm->Initialize(device);
		m_docForm->Show();
	}
}