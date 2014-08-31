#include "Document.h"

#include "MainWindow.h"

namespace APDesigner
{
	Document::Document(MainWindow* window, EditorExtension* ext)
		: m_activated(false), m_mainWindow(window), m_extension(ext)
	{
		m_docForm = new Form(FBS_Sizable, L"New document");
		m_docForm->Position.Y = 25;
		m_docForm->SetSkin(window->getUISkin());
		//reinterpret_cast<int&>(m_docForm->UserData) = 0xffffffff;

		//m_eDocActivated.bind(this, &Document::DocActivate);
		//m_eDocDeactivated.bind(this, &Document::DocDeactivate);
	}

	Document::~Document()
	{
		UIRoot::Remove(m_docForm);
		delete m_docForm;
	}

	void Document::Initialize(RenderDevice* device)
	{
		m_docForm->Initialize(device);
		m_docForm->Show();
	}

	void Document::Update(const GameTime* time)
	{
		if (!m_activated)
		{
			if (UIRoot::getTopMostForm() == m_docForm)
			{
				m_eDocActivated.Invoke(this);
				DocActivate(this);
			}
		}
		else
		{
			if (UIRoot::getTopMostForm() != m_docForm && UIRoot::getTopMostForm() != m_mainWindow->getToolsPane())
			{
				m_eDocDeactivated.Invoke(this);
				DocDeactivate(this);
			}
		}
	}
}