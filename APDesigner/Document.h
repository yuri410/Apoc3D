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