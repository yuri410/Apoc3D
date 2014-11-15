#include "Document.h"

#include "MainWindow.h"

namespace APDesigner
{
	Document::Document(MainWindow* window, EditorExtension* ext)
		: m_mainWindow(window), m_extension(ext)
	{
		m_docForm = new Form(window->getUISkin(), window->getDevice(), FBS_Sizable, L"New document");
		m_docForm->Position.Y = 25;
		//reinterpret_cast<int&>(m_docForm->UserData) = 0xffffffff;

		//m_eDocActivated.bind(this, &Document::DocActivate);
		//m_eDocDeactivated.bind(this, &Document::DocDeactivate);
	}

	Document::~Document()
	{
		SystemUI::Remove(m_docForm);
		delete m_docForm;
	}

	void Document::Initialize(RenderDevice* device)
	{
		m_docForm->Show();
	}

	void Document::Update(const GameTime* time)
	{
		if (!m_activated)
		{
			if (SystemUI::TopMostForm == m_docForm)
			{
				eventDocumentActivated.Invoke(this);
				DocActivate(this);
			}
		}
		else
		{
			if (SystemUI::TopMostForm != m_docForm)
			{
				eventDocumentDeactivated.Invoke(this);
				DocDeactivate(this);
			}
		}

		if (!m_docForm->Visible)
		{
			if (!m_closed)
			{
				m_closed = true;

				eventDocumentClosed.Invoke(this);
			}
		}
	}
}