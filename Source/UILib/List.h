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
#ifndef LIST_H
#define LIST_H

#include "UICommon.h"
#include "Control.h"

namespace Apoc3D
{
	namespace UI
	{
		class ListBox : public Control
		{
		private:
			List<String> m_items;
			bool m_sorted;
			bool m_isSorted;
			int m_visisbleItems;

			Point m_textOffset;

			Apoc3D::Math::Rectangle m_destRect[9];

			Apoc3D::Math::Rectangle m_selectionRect;
			int m_hoverIndex;
			int m_selectedIndex;

			ScrollBar* m_vscrollbar;
			ScrollBar* m_hscrollbar;
			bool m_horizontalScrollbar;
			int m_hScrollWidth;

			UIEventHandler m_eSelect;
			UIEventHandler m_eSelectionChanged;
			bool m_mouseOver;

			void InitScrollbars(RenderDevice* device);
			void UpdateHScrollbar();
			void RenderSelectionBox(Sprite* sprite, int index);
			void DrawBackground(Sprite* sprite);
			void DrawScrollbar(Sprite* sprite);
		public:
			bool getUseHorizontalScrollbar() const { return m_horizontalScrollbar; }
			void setUseHorizontalScrollbar(bool v) { m_horizontalScrollbar = v; }

			List<String>& getItems() { return m_items; }

			bool getIsSorted() const { return m_sorted; }
			void setIsSorted(bool v) { m_sorted = v; }

			int getSelectedIndex() const { return m_selectedIndex; }
			void setSelectedIndex(int i) { m_selectedIndex = i; }

			ListBox(const Point& position, int width, int height, const List<String>& items);
			~ListBox();

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

		};

		class TreeViewNode
		{
		private:
			String m_text;
			Texture* m_icon;
			FastList<TreeViewNode*> m_subNode;

			bool m_expanded;
		public:
			void* UserData;

			bool isExpanded() const { return m_expanded; }
			const String& getText() const { return m_text; }
			void setText(const String& txt) { m_text = txt; }

			Texture* getIcon() const { return m_icon; }
			void setIcon(Texture* texture) { m_icon = texture; }

			FastList<TreeViewNode*>& getNodes() { return m_subNode; }

			TreeViewNode(const String& text)
				: m_text(text), m_icon(0), m_expanded(false), UserData(0)
			{

			}
			TreeViewNode(const String& text, Texture* icon)
				: m_text(text), m_icon(icon), m_expanded(false), UserData(0)
			{

			}

			void Expand()
			{
				m_expanded = true;
			}
			void Close()
			{
				m_expanded = false;
			}
		};

		class TreeView : public Control
		{
		private:
			FastList<TreeViewNode*> m_nodes;

			int m_visisbleItems;

			Point m_textOffset;

			Apoc3D::Math::Rectangle m_destRect[9];

			Apoc3D::Math::Rectangle m_selectionRect;
			TreeViewNode* m_selectedNode;
			TreeViewNode* m_hoverNode;
			TreeViewNode* m_anyHoverNode;

			ScrollBar* m_vscrollbar;
			ScrollBar* m_hscrollbar;
			bool m_horizontalScrollbar;
			int m_hScrollWidth;

			UIEventHandler m_eSelect;
			UIEventHandler m_eSelectionChanged;
			bool m_mouseOver;

			int GetItemHeight() const;

			void InitScrollbars(RenderDevice* device);
			void UpdateHScrollbar();
			
			void RenderSelectionBox(Sprite* sprite, TreeViewNode* node);
			void DrawBackground(Sprite* sprite);
			void DrawScrollbar(Sprite* sprite);
			void DrawNodes(Sprite* sprite, const FastList<TreeViewNode*>& nodes, int depth, int& counter, int maxCount);

			int GetExpandedNodeCount() const { return GetExpandedNodeCount(m_nodes); }
			int GetExpandedNodeCount(const FastList<TreeViewNode*>& node) const;
			int GetExpandedNodeMaxRight(const FastList<TreeViewNode*>& node) const;
			int GetAllNodeCount(const FastList<TreeViewNode*>& node) const;
			int GetAllNodeCount() const;
			int MeasureExpandedNodeWidth() const;
			int MeasureExpandedModeHeight() const;
		public:
			bool getUseHorizontalScrollbar() const { return m_horizontalScrollbar; }
			void setUseHorizontalScrollbar(bool v) { m_horizontalScrollbar = v; }
			
			FastList<TreeViewNode*>& getNodes() { return m_nodes; }


			TreeViewNode* getSelectedNode() const { return m_selectedNode; }

			TreeView(const Point& position, int width, int height);
			~TreeView();

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			
			virtual void Draw(Sprite* sprite);

			void SetSize(const Point& newSize);
		};
	}
}

#endif