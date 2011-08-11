/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/
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
		Document(MainWindow* window);

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