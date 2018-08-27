#pragma once

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "APDCommon.h"

namespace APDesigner
{
	typedef Apoc3D::EventDelegate<Document*> DocumentActivationHandler;

	class APDAPI Document
	{
	public:
		virtual ~Document();

		
		/** Initializes the graphical user interface. File loading might be performed before this init.
		*/
		virtual void Initialize(RenderDevice* device);

		virtual void LoadRes() = 0;
		virtual void SaveRes() = 0;
		virtual bool IsReadOnly() = 0;

		virtual void Render() { }
		virtual void Update(const GameTime* time);

		virtual ObjectPropertyEditor* getRootPropertyEditor() { return nullptr; }

		Form* getDocumentForm() const { return m_docForm; }

		bool isActivated() const { return m_activated; }
		EditorExtension* getExtension() const { return m_extension; }

	protected:
		MainWindow* getMainWindow() const { return m_mainWindow; }

		Document(MainWindow* window, EditorExtension* ext);


		virtual void activate() { }
		virtual void deactivate() { }

		void DocActivate(Document* doc)
		{
			if (!m_activated)
			{
				activate();
				m_activated = true;
			}
		}
		void DocDeactivate(Document* doc)
		{
			if (m_activated)
			{
				deactivate();
				m_activated = false;
			}
		}

	private:
		Form* m_docForm;
		MainWindow* m_mainWindow;

		EditorExtension* m_extension;

		bool m_activated = false;
		bool m_closed = false;

	public:
		DocumentActivationHandler eventDocumentActivated;
		DocumentActivationHandler eventDocumentDeactivated;
		DocumentActivationHandler eventDocumentClosed;
	};
}

#endif