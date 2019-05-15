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

#ifndef DLGATOMMANAGER_H
#define DLGATOMMANAGER_H

#include "APDesigner/APDCommon.h"

namespace APDesigner
{
	class AtomManagerDialog
	{
	public:

		AtomManagerDialog(MainWindow* window);
		~AtomManagerDialog();

		void Initialize(RenderDevice* device);

		void Update(const AppTime* time);

		void UpdateToNewProject(Project* prj);

		void Show();
		void Hide();
	private:
		void UpdateAtomListView();

		void BtnAdd_Release(Button* ctrl);
		void BtnRemove_Release(Button* ctrl);
		void BtnEdit_Release(Button* ctrl);


		MainWindow* m_mainWindow;
		const StyleSkin* m_skin;

		Form* m_form;

		ListView* m_atomList;
		Button* m_addItem;
		Button* m_removeItem;
		Button* m_editItem;

		Project* m_currentProject;
	};
}

#endif