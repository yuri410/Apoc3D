#include "OrphanDetector.h"

#include "APDesigner/MainWindow.h"
#include "APDesigner/BuildService/BuildService.h"
#include "APDesigner/UIResources.h"
#include "APDesigner/Document.h"
#include "apoc3d.Win32/Win32Common.h"

#include <windows.h>
#include <shellapi.h>

namespace APDesigner
{
	OrphanDetector::OrphanDetector(MainWindow* window)
		: m_mainWindow(window), m_skin(window->getUISkin())
	{
		RenderDevice* device = window->getDevice();

		m_form = new Form(m_skin, device, FBS_Fixed);

		m_form->setSize(600, 600);
		m_form->Position = Point(100, 100);

		m_form->setTitle(L"Orphaned Files Detector");

		{
			List2D<String> emptyList(1, 2);

			ListView* listView = new ListView(m_skin, Point(5, 25), Point(590, 500), emptyList);

			listView->getColumnHeader().Add(ListView::Header(L"FileName", 550));
			listView->FullRowSelect = true;

			m_form->getControls().Add(listView);
			m_orphanList = listView;
		}

		m_removeItem = new Button(m_skin, Point(10, 550), 100, L"Delete");
		m_removeItem->eventRelease.Bind(this, &OrphanDetector::BtnRemove_Release);
		m_form->getControls().Add(m_removeItem);
	}

	OrphanDetector::~OrphanDetector()
	{
		delete m_form;
		delete m_removeItem;
		delete m_orphanList;
	}

	void OrphanDetector::Initialize(RenderDevice* device)
	{
		SystemUI::Add(m_form);

		//UpdateListView();
	}
	void OrphanDetector::UpdateToNewProject(Project* prj)
	{
		if (m_currentProject == prj)
			return;

		m_currentProject = prj;
		UpdateListView();
	}
	void OrphanDetector::UpdateListView()
	{
		m_orphanList->getItems().Clear();

		if (m_currentProject)
		{
			String baseOutput = m_currentProject->getOutputPath();

			HashSet<String, StringEqualityComparerNoCase> allOutputSet;
			PassProject(m_currentProject->getItems(), allOutputSet);

			List<String> allOutput;
			allOutputSet.FillItems(allOutput);
			allOutputSet.Clear();

			for (String& fn : allOutput)
			{
				if (StringUtils::StartsWith(fn, baseOutput))
				{
					fn = fn.substr(baseOutput.size());
					if (StringUtils::StartsWith(fn, L"\\"))
					{
						fn = fn.substr(1);
					}
				}
				
				allOutputSet.Add(fn);
			}

			List<String> allFiles;
			File::ListDirectoryFilesRecursive(baseOutput, allFiles);

			for (const String& fn : allFiles)
			{
				if (!allOutput.Contains(fn))
				{
					m_orphanList->getItems().AddRow(&fn);
				}
			}
		}
	}
	void OrphanDetector::PassProject(const List<ProjectItem*>& items, HashSet<String, StringEqualityComparerNoCase>& allOutputFiles)
	{
		for (ProjectItem* itm : items)
		{
			if (itm->getData() == nullptr)
				continue;

			if (itm->getType() == ProjectItemType::Folder)
			{
				ProjectFolder* fld = up_cast<ProjectFolder*>(itm->getData());
				PassProject(fld->SubItems, allOutputFiles);
			}
			else
			{
				for (const String& op : itm->getData()->GetAllOutputFiles())
				{
					if (!allOutputFiles.Contains(op))
						allOutputFiles.Add(op);
				}
			}
		}
	}

	void OrphanDetector::Show()
	{
		m_form->Show();
	}
	void OrphanDetector::Hide()
	{
		m_form->Close();
	}

	void OrphanDetector::BtnRemove_Release(Button* ctrl)
	{
		if (m_currentProject == nullptr)
			return;

		int index = m_orphanList->getSelectedRowIndex();

		if (index != -1)
		{
			String name = m_orphanList->getItems().at(index, 0);

			String path = PathUtils::Combine(m_currentProject->getOutputPath(), name);
			if (File::FileExists(path))
			{
				wchar_t* pathBuffer = new wchar_t[path.size() + 2];
				memset(pathBuffer, 0, sizeof(wchar_t)*(path.size() + 2));
				memcpy(pathBuffer, path.c_str(), sizeof(wchar_t)*path.size());

				SHFILEOPSTRUCT fo;
				memset(&fo, 0, sizeof(fo));

				fo.hwnd = FindWindow(Apoc3D::Win32::WindowClass, m_mainWindow->getWindow()->getTitle().c_str());
				fo.wFunc = FO_DELETE;
				fo.pFrom = pathBuffer;
				fo.fFlags = FOF_ALLOWUNDO;

				if (SHFileOperation(&fo)==0 && !fo.fAnyOperationsAborted)
				{
					m_orphanList->getItems().RemoveRow(index);
				}
				delete[]pathBuffer;
			}
		}
	}
}