/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Xin

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

		void Update(const GameTime* const time);

		void UpdateToNewProject(Project* prj);

		void Show();
		void Hide();
	private:
		void UpdateAtomListView();

		void BtnAdd_Release(Control* ctrl);
		void BtnRemove_Release(Control* ctrl);
		void BtnEdit_Release(Control* ctrl);


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