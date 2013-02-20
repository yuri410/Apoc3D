#pragma once
#ifndef APOC3D_LIST_H
#define APOC3D_LIST_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Games
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */


#include "UICommon.h"

#include "Control.h"

#include "apoc3d/Collections/FastList.h"
#include "apoc3d/Collections/List2D.h"

using namespace Apoc3D::Collections;

namespace Apoc3D
{
	namespace UI
	{
		class APAPI ListBox : public Control
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
			UIEventHandler& eventSelect() { return m_eSelect; }
			UIEventHandler& eventSelectionChanged() { return m_eSelectionChanged; }

			bool getUseHorizontalScrollbar() const { return m_horizontalScrollbar; }
			void setUseHorizontalScrollbar(bool v) { m_horizontalScrollbar = v; }

			List<String>& getItems() { return m_items; }

			bool getIsSorted() const { return m_sorted; }
			void setIsSorted(bool v) { m_sorted = v; }

			int getSelectedIndex() const { return m_selectedIndex; }
			void setSelectedIndex(int i) { m_selectedIndex = i; }
			void SetSelectedByName(const String& name)
			{
				for (int i=0;i<m_items.getCount();i++)
					if (m_items[i] == name)
					{
						setSelectedIndex(i); 
						return;
					}
			}

			ListBox(const Point& position, int width, int height, const List<String>& items);
			~ListBox();

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

		};

		class APAPI TreeViewNode
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

		class APAPI TreeView : public Control
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
			UIEventHandler& eventSelect() { return m_eSelect; }
			UIEventHandler& eventSelectionChanged() { return m_eSelectionChanged; }

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

		typedef fastdelegate::FastDelegate2<int, int, void> ListViewSelectionHandler;

		class APAPI ListView : public Control
		{
		public:
			class Header
			{
			public:
				Header()
					:Width(0)
				{
				}
				Header(const String& text, int width)
					: Text(text), Width(width)
				{
				}

				UIEventHandler& eventPress() { return m_eOnPress; }
				UIEventHandler& enentRelease() { return m_eOnRelease; }

				String Text;
				int Width;

			private:
				UIEventHandler m_eOnPress;
				UIEventHandler m_eOnRelease;
				
			};
			enum ListViewHeaderStyle
			{
				LHSTYLE_Clickable,
				LHSTYLE_Nonclockable,
				LHSTYLE_None
			};

			ListView(const Point& position, const Point& size, const List2D<String>& items);
			~ListView();

			virtual void Initialize(RenderDevice* device);
			virtual void Update(const GameTime* const time);
			virtual void Draw(Sprite* sprite);

			List2D<String>& getItems() { return m_items; }
			List<Header>& getColumnHeader() { return m_columnHeader; }
			ListViewHeaderStyle getHeaderStyle() const { return m_headerStyle; }
			void setHeaderStyle(ListViewHeaderStyle s) { m_headerStyle = s; }

			bool getGridLines() const { return m_gridLines; }
			void setGridLines(bool v) { m_gridLines = v; }

			bool getFullRowSelect() const { return m_fullRowSelect; }
			void setFullRowSelect(bool v) { m_fullRowSelect = v; }

			bool getHoverSelection() const { return m_hoverSelection; }
			void setHoverSelection(bool v) { m_hoverSelection = v; }

			int getSelectedRowIndex() const { return m_selectedRow; }

			ListViewSelectionHandler& eventSelected() { return m_eSelect; }
		private:
			int GetVisibleItems();
			
			void UpdateSelection();
			void DrawBorder(Sprite* sprite);
			void DrawScrollbars(Sprite* sprite);
			void DrawColumnHeaders(Sprite* sprite);
			void UpdateHeaderSize(Apoc3D::Math::Rectangle headerArea, int index, Sprite* sprite);
			void DrawHeaderEnd(Sprite* sprite, int width);
			void DrawGridLines(Sprite* sprite);
			void DrawItems(Sprite* sprite);
			void DrawSelectionBox(Sprite* sprite, const Point& position, int x, int y);
			void DrawSelectedBox(Sprite* sprite, const Point& position);

			Apoc3D::Math::Rectangle m_sizeArea;
			int m_resizeIndex;

			List2D<String> m_items;

			List<Header> m_columnHeader;
			Apoc3D::Math::Rectangle m_headerArea;
			bool m_isResizing;

			Apoc3D::Math::Rectangle m_backArea;

			ScrollBar* m_hScrollBar;
			ScrollBar* m_vScrollBar;

			ListViewHeaderStyle m_headerStyle;
			int m_headerHoverIndex;
			bool m_gridLines;
			Apoc3D::Math::Rectangle m_lineRect;
			Point m_gridSize;

			int m_selectedRow;
			int m_selectedColumn;
			int m_hoverRowIndex;
			int m_hoverColumnIndex;
			Apoc3D::Math::Rectangle m_selectionArea;
			Apoc3D::Math::Rectangle m_selectionRect;
			bool m_fullRowSelect;
			bool m_hoverSelection;

			ListViewSelectionHandler m_eSelect;

			Apoc3D::Math::Rectangle m_srcRect;
		};
	}
}

#endif