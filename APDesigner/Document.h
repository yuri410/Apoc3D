#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "APDCommon.h"

namespace APDesigner
{
	class Document
	{
	private:
		bool m_activated;

	protected:
		Document()
			: m_activated(false)
		{

		}

		virtual void activate() { }
		virtual void deactivate() { }
	public:
		bool isActivated() const { return m_activated; }

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