#include "Document.h"

#include "UILib/Form.h"

namespace APDesigner
{
	Document::Document()
		: m_activated(false)
	{
		m_docForm = new Form(FBS_Sizable, L"New document");

		
	}

	void Document::Initialize(RenderDevice* device)
	{
		m_docForm->Initialize(device);
		m_docForm->Show();
	}
}