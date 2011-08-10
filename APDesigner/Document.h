#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "APDCommon.h"

using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Graphics;
using namespace Apoc3D::UI;

namespace APDesigner
{
	class Document
	{
	private:
		bool m_activated;
		Form* m_docForm;

	protected:
		Document();

		virtual void activate() { }
		virtual void deactivate() { }
	public:

		Form* getDocumentForm() const { return m_docForm; }

		bool isActivated() const { return m_activated; }

		virtual void Initialize(RenderDevice* device);

		virtual ObjectPropertyEditor* getRootPropertyEditor() { return 0; }
		virtual void LoadRes() = 0;
		virtual void SaveRes() = 0;
		virtual bool IsReadOnly() = 0;

		void DocActivate()
		{
			if (!m_activated)
			{
				activate();
				m_activated = true;
			}
		}
		void DocDeactivate()
		{
			if (m_activated)
			{
				deactivate();
				m_activated = false;
			}
		}
	};
}

#endif