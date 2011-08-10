#include "Panes.h"

#include "Graphics/RenderSystem/Sprite.h"
#include "MainWindow.h"
#include "UILib/Form.h"
#include "UILib/List.h"

namespace APDesigner
{
	ResourcePane::ResourcePane(MainWindow* window)
		: m_mainWindow(window)
	{
		m_form = new Form(FBS_Pane);
		m_form->SetSkin(window->getUISkin());


		m_form->Size = Point(225,600);
		m_form->Position = Point(0, 17);
		
		m_form->Text = L"Resource Explorer.";
		m_form->setTitle(m_form->Text);


		{
			TreeView* treeview = new TreeView(Point(5, 25), 200, 300);
			treeview->SetSkin(window->getUISkin());
			treeview->setUseHorizontalScrollbar(true);

			TreeViewNode* node1 = new TreeViewNode(L"Root");
			treeview->getNodes().Add(node1);

			TreeViewNode* node2 = new TreeViewNode(L"Root2");
			treeview->getNodes().Add(node2);
			TreeViewNode* node3 = new TreeViewNode(L"Root3 the long one. Project Hosting will be READ-ONLY Tuesday at 9:00am CDT for brief maintenance.");
			treeview->getNodes().Add(node3);

			node2->getNodes().Add(new TreeViewNode(L"SubNode1"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode2"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode3"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode4"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode5"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode6"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode7"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode8"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode9"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode10"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode11"));
			node2->getNodes().Add(new TreeViewNode(L"SubNode12"));

			node3->getNodes().Add(new TreeViewNode(L"SubNode1"));
			node3->getNodes().Add(new TreeViewNode(L"SubNode2"));
			node3->Expand();


			m_form->getControls().Add(treeview);
			m_resourceView = treeview;
		}
	}
	ResourcePane::~ResourcePane()
	{
		delete m_form;



		delete m_resourceView;
	}

	void ResourcePane::Initialize(RenderDevice* device)
	{
		m_form->Initialize(device);

		UIRoot::Add(m_form);
		m_form->Show();
	}

	void ResourcePane::Update(const GameTime* const time)
	{
		m_form->Size.Y = m_mainWindow->getUIAreaSize().Y - 17;
		Point newSize = m_form->Size;
		newSize.X -= 10;
		newSize.Y -= 30+300;
		m_resourceView->SetSize(newSize);
		
	}
}