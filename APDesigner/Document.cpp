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
		//reinterpret_cast<int&>(m_docForm->UserData) = 0xffffffff;

		//m_eDocActivated.bind(this, &Document::DocActivate);
		//m_eDocDeactivated.bind(this, &Document::DocDeactivate);
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

	void Document::Update(const GameTime* const time)
	{
		if (!m_activated)
		{
			if (UIRoot::getTopMostForm() == m_docForm)
			{
				if (!m_eDocActivated.empty())
					m_eDocActivated(this);
				DocActivate(this);
			}
		}
		else
		{
			if (UIRoot::getTopMostForm() != m_docForm && UIRoot::getTopMostForm() != m_mainWindow->getToolsPane())
			{
				if (!m_eDocDeactivated.empty())
				{
					m_eDocDeactivated(this);
				}
				DocDeactivate(this);
			}
		}
	}
}