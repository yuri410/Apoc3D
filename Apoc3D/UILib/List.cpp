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
#include "List.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "Button.h"
#include "Scrollbar.h"
#include "Form.h"

#include "apoc3d/Input/Mouse.h"
#include "apoc3d/Input/InputAPI.h"

#include "apoc3d/Graphics/RenderSystem/Texture.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/RenderSystem/RenderStateManager.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		ListBox::ListBox(const StyleSkin* skin, const Point& position, int width, int height, const List<String>& items)
			: ScrollableControl(skin, position, Point(width, height)),  m_items(items)
		{
			Initialize(skin);
		}

		ListBox::~ListBox()
		{ }

		void ListBox::Initialize(const StyleSkin* skin)
		{
			BackgroundGraphic = UIGraphic(skin->SkinTexture, skin->ListBoxBackground);
			Margin = skin->ListBoxMargin;

			ItemSettings.HorizontalAlignment = TextHAlign::Left;
			ItemSettings.TextColor = skin->TextColor;

			m_visisbleItems = (int)ceilf((float)m_size.Y / getItemHeight());
			m_size.Y = m_visisbleItems * getItemHeight();

			InitScrollbars(skin, false);
		}

		void ListBox::Update(const GameTime* time)
		{
			if (m_selectedIndex >= m_items.getCount())
				m_selectedIndex = -1;

			UpdateScrollBarsGeneric(getArea(), time);
			UpdateHScrollbar();

			Apoc3D::Math::Rectangle cntArea = GetContentArea();

			m_visisbleItems = (int)ceilf((float)cntArea.Height / getItemHeight());

			m_vscrollbar->Maximum = Math::Max(0, m_items.getCount() - m_visisbleItems);
			m_vscrollbar->Step = Math::Max(1, m_vscrollbar->Maximum / 15);

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (ParentFocused)
			{
				if (m_hoverIndex != -1 && mouse->IsLeftPressed())
				{
					int previousIndex = m_selectedIndex;
					m_selectedIndex = m_hoverIndex;
					
					eventSelect.Invoke(this);

					if (m_selectedIndex != previousIndex)
						eventSelectionChanged.Invoke(this);
				}

				if (getAbsoluteArea().Contains(mouse->GetPosition()))
				{
					if (!m_mouseHover)
					{
						m_mouseHover = true;
						OnMouseHover();
					}

					if (mouse->getDZ())
					{
						m_vscrollbar->SetValue(m_vscrollbar->getValue() - mouse->getDZ() / 60);
					}

					if (mouse->IsLeftPressed())
						OnPress();
					else if (mouse->IsLeftUp())
						OnRelease();
				}
				else if (m_mouseHover)
				{
					m_mouseHover = false;
					OnMouseOut();
				}
			}
		}

		void ListBox::UpdateHScrollbar()
		{
			// measure the max width among all the item
			int ew = 0;
			for (const String& item : m_items)
			{
				int w;
				if (!m_vscrollbar || m_vscrollbar->Maximum == 0)
					w = m_fontRef->MeasureString(item).X - m_size.X + 12;
				else
					w = m_fontRef->MeasureString(item).X - m_size.X + 30;

				if (w > ew)
					ew = w;
			}

			m_hScrollWidth = ew;
			if (m_hscrollbar)
				m_hscrollbar->Maximum = m_hScrollWidth;
		}

		void ListBox::Draw(Sprite* sprite)
		{
			DrawBackground(sprite);

			sprite->Flush();

			bool shouldRestoreScissorTest = false;
			Apoc3D::Math::Rectangle oldScissorRect;
			RenderDevice* dev = sprite->getRenderDevice();
			if (dev->getRenderState()->getScissorTestEnabled())
			{
				shouldRestoreScissorTest = true;
				oldScissorRect = dev->getRenderState()->getScissorTestRect();
			}
			Apoc3D::Math::Rectangle scissorRect = getAbsoluteArea();
			dev->getRenderState()->setScissorTest(true, &scissorRect);

			Apoc3D::Math::Rectangle cntArea = GetContentArea();

			m_hoverIndex = -1;
			for (int i = m_vscrollbar->getValue();
				i < Math::Min(m_items.getCount(), m_vscrollbar->getValue() + m_visisbleItems); i++)
			{
				Point textOffset;

				if (m_hscrollbar)
					textOffset.X = -m_hscrollbar->getValue() + 2;
				else
					textOffset.X = 2;

				textOffset.Y = (i - m_vscrollbar->getValue()) * getItemHeight();

				textOffset += cntArea.getPosition();

				if (ParentFocused)
					RenderSelectionBox(sprite, i, textOffset);

				ItemSettings.Draw(sprite, m_fontRef, m_items[i], textOffset, Point(m_size.X, getItemHeight()), 0xff);
			}
			sprite->Flush();
			if (shouldRestoreScissorTest)
			{
				dev->getRenderState()->setScissorTest(true, &oldScissorRect);
			}
			else
			{
				dev->getRenderState()->setScissorTest(false, 0);
			}

			DrawScrollBars(sprite);
		}

		void ListBox::RenderSelectionBox(Sprite* sprite, int index, const Point& txtPos)
		{
			Texture* whitePix = SystemUI::GetWhitePixel();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Apoc3D::Math::Rectangle cntArea = GetContentArea();

			Apoc3D::Math::Rectangle selectionRect;
			selectionRect.X = txtPos.X;
			selectionRect.Y = txtPos.Y;
			selectionRect.Height = getItemHeight();
			selectionRect.Width = cntArea.Width;

			if (index == m_selectedIndex)
				sprite->Draw(whitePix, selectionRect, CV_LightGray);
			else if (selectionRect.Contains(mouse->GetPosition()))
			{
				m_hoverIndex = index;
				sprite->Draw(whitePix, selectionRect, CV_Silver);
			}
		}

		void ListBox::DrawBackground(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle graphicalArea = getAbsoluteArea();
			graphicalArea = Margin.InflateRect(graphicalArea);

			BackgroundGraphic.Draw(sprite, graphicalArea);
		}
		
		int32 ListBox::FindEntry(const String& v) { return m_items.IndexOf(v); }
		int32 ListBox::getItemHeight() const { return m_fontRef->getLineHeightInt(); }

		void ListBox::OnMouseHover() { }
		void ListBox::OnMouseOut() { }
		void ListBox::OnPress() { eventPress.Invoke(this); } 
		void ListBox::OnRelease() { eventRelease.Invoke(this); }

		/************************************************************************/
		/* Tree view                                                            */
		/************************************************************************/

		const int TreeViewIntent = 20;

		TreeView::TreeView(const StyleSkin* skin, const Point& position, int width, int height)
			: ScrollableControl(skin, position, Point(width, height))
		{
			Initialize(skin);
		}

		TreeView::~TreeView()
		{
			NukeTreeViewNodes();
		}

		void TreeView::Initialize(const StyleSkin* skin)
		{
			BackgroundGraphic = UIGraphic(skin->SkinTexture, skin->ListBoxBackground);
			Margin = skin->ListBoxMargin;

			ItemSettings.TextColor = skin->TextColor;
			ItemSettings.HorizontalAlignment = TextHAlign::Left;

			m_visisbleItems = (int)ceilf((float)m_size.Y / GetItemHeight());
			m_size.Y = m_visisbleItems * GetItemHeight();

			UpdateHScrollbar();
			InitScrollbars(skin, false);
		}


		void TreeView::Update(const GameTime* time)
		{
			UpdateScrollBarsGeneric(getArea(), time);
			UpdateHScrollbar();

			Apoc3D::Math::Rectangle cntArea = GetContentArea();
			m_visisbleItems = (int)ceilf((float)cntArea.Height / GetItemHeight());

			m_vscrollbar->Maximum = Math::Max(0, GetAllVisibleNodeCount() - m_visisbleItems);
			m_vscrollbar->Step = Math::Max(1, m_vscrollbar->Maximum / 15);

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (ParentFocused)
			{
				if (m_hoverNode && mouse->IsLeftPressed())
				{
					TreeViewNode* previousNode = m_selectedNode;
					m_selectedNode = m_hoverNode;
					
					eventSelect.Invoke(this);
					if (m_selectedNode != previousNode)
						eventSelectionChanged.Invoke(this);
				}

				if (getAbsoluteArea().Contains(mouse->GetPosition()))
				{
					if (!m_mouseOver)
					{
						m_mouseOver = true;
						OnMouseHover();
					}
					
					if (mouse->getDZ())
					{
						m_vscrollbar->SetValue(m_vscrollbar->getValue() - mouse->getDZ() / 60);
					}

					if (mouse->IsLeftPressed())
					{
						OnPress();
					}
					else if (mouse->IsLeftUp())
					{
						OnRelease();
						if (m_selectedNode && !m_hoverNode && m_anyHoverNode == m_selectedNode)
						{
							if (!m_selectedNode->isExpanded())
								m_selectedNode->Expand();
							else
								m_selectedNode->Close();
						}
					}
				}
				else if (m_mouseOver)
				{
					m_mouseOver = false;
					OnMouseOut();
				}
				
			}
		}

		void TreeView::UpdateHScrollbar()
		{
			m_hScrollWidth = MeasureExpandedNodeWidth();
			int barmax = m_hScrollWidth - m_size.X;
			if (barmax<0) barmax = 0;
			if (m_hscrollbar)
				m_hscrollbar->Maximum = barmax;
		}

		void TreeView::DrawNodes(Sprite* sprite, const List<TreeViewNode*>& nodes, int depth, int& counter, int maxCount)
		{
			
			//Apoc3D::Math::Rectangle absArea = getAbsoluteArea();
			Apoc3D::Math::Rectangle cntArea = GetContentArea();

			for (TreeViewNode* nde : nodes)
			{
				counter++;
				if (counter > maxCount)
					return;

				Point itemOffset;

				if (m_hscrollbar)
					itemOffset.X = -m_hscrollbar->getValue() + 2;
				else
					itemOffset.X = 2;
				itemOffset.X += depth * TreeViewIntent;

				itemOffset.Y = (counter - 1 - m_vscrollbar->getValue()) * GetItemHeight();

				itemOffset += cntArea.getTopLeft();

				if (ParentFocused)
					RenderSelectionBox(sprite, nde, cntArea, itemOffset);

				if (nde->getIcon())
				{
					//Apoc3D::Math::Rectangle iconRect(m_textOffset.X, m_textOffset.Y, m_nodes[i]->getIcon()->getWidth(), m_nodes[i]->getIcon()->getHeight());
					sprite->Draw(nde->getIcon(), itemOffset.X, itemOffset.Y, CV_White);

					itemOffset.X += nde->getIcon()->getWidth();
				}

				ItemSettings.Draw(sprite, m_fontRef, nde->getText(), itemOffset, Point(0, GetItemHeight()), 0xff);
				//m_fontRef->DrawString(sprite, nodes[i]->getText(), to, m_skin->TextColor);

				if (nde->isExpanded() && nde->getNodes().getCount())
				{
					DrawNodes(sprite, nde->getNodes(), depth + 1, counter, maxCount);
				}
			}
		}

		void TreeView::Draw(Sprite* sprite)
		{
			DrawBackground(sprite);

			sprite->Flush();

			bool shouldRestoreScissorTest = false;
			Apoc3D::Math::Rectangle oldScissorRect;
			RenderDevice* dev = sprite->getRenderDevice();
			if (dev->getRenderState()->getScissorTestEnabled())
			{
				shouldRestoreScissorTest = true;
				oldScissorRect = dev->getRenderState()->getScissorTestRect();
			}
			Apoc3D::Math::Rectangle scissorRect = getAbsoluteArea();
			dev->getRenderState()->setScissorTest(true, &scissorRect);

			m_hoverNode = 0;
			m_anyHoverNode = 0;
			int counter = 0;
			DrawNodes(sprite, m_nodes, 0, counter, m_visisbleItems + m_vscrollbar->getValue());

			sprite->Flush();
			if (shouldRestoreScissorTest)
			{
				dev->getRenderState()->setScissorTest(true, &oldScissorRect);
			}
			else
			{
				dev->getRenderState()->setScissorTest(false, 0);
			}

			DrawScrollBars(sprite);
		}

		void TreeView::RenderSelectionBox(Sprite* sprite, TreeViewNode* node, const Apoc3D::Math::Rectangle& contentArea, const Point& txtPos)
		{
			Texture* whitePix = SystemUI::GetWhitePixel();

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			Apoc3D::Math::Rectangle selectionRect;

			selectionRect.X = txtPos.X;
			selectionRect.Y = txtPos.Y;
			selectionRect.Height = GetItemHeight();

			if (m_vscrollbar->Maximum>0)
				selectionRect.Width = m_size.X - 13;
			else
				selectionRect.Width = m_size.X - 1;

			
			if (node == m_selectedNode)
				sprite->Draw(whitePix, selectionRect, CV_LightGray);
			if (contentArea.Contains(mouse->GetPosition()) &&
				selectionRect.Contains(mouse->GetPosition()))
			{
				m_anyHoverNode = node;
				if (node != m_selectedNode)
				{
					m_hoverNode = node;
					sprite->Draw(whitePix, selectionRect, CV_Silver);
				}
			}

		}

		void TreeView::DrawBackground(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle graphicalArea = getAbsoluteArea();
			graphicalArea = Margin.InflateRect(graphicalArea);

			BackgroundGraphic.Draw(sprite, graphicalArea);
		}
		
		void TreeView::SetSize(const Point& newSize)
		{
			if (newSize == m_size)
				return;

			m_size = newSize;

			UpdateScrollBarsLength(getArea());
		}

		void TreeView::NukeTreeViewNodes()
		{
			NukeTreeViewNodes(m_nodes);
			
			m_selectedNode = nullptr;
			m_hoverNode = nullptr;
			m_anyHoverNode = nullptr;
		}

		void TreeView::NukeTreeViewNodes(List<TreeViewNode*>& nodes)
		{
			for (int i=0;i<nodes.getCount();i++)
			{
				if (nodes[i]->getNodes().getCount())
				{
					NukeTreeViewNodes(nodes[i]->getNodes());
				}
				delete nodes[i];
			}
			nodes.Clear();
		}

		int TreeView::GetAllVisibleNodeCount(const List<TreeViewNode*>& nodes) const
		{
			int result = nodes.getCount();

			for (TreeViewNode* n : nodes)
			{
				if (n->isExpanded())
					result += GetAllVisibleNodeCount(n->getNodes());
			}

			return result;
		}
		int TreeView::GetAllVisibleNodeCount() const { return GetAllVisibleNodeCount(m_nodes); }
		int TreeView::GetExpandedNodeMaxRight(const List<TreeViewNode*>& nodes) const
		{
			int r = 0;
			for (TreeViewNode* n : nodes)
			{
				if (n->isExpanded())
				{
					if (n->getNodes().getCount())
					{
						int res = GetExpandedNodeMaxRight(n->getNodes()) + TreeViewIntent;
						if (res > r)
							r = res;
					}
				}
				int w;
				if (!m_vscrollbar || m_vscrollbar->Maximum == 0)
					w = m_fontRef->MeasureString(n->getText()).X - m_size.X + 12;
				else
					w = m_fontRef->MeasureString(n->getText()).X - m_size.X + 30;
				if (w > r)
					r = w;
			}
			return r;
		}
		int TreeView::GetExpandedNodeCount(const List<TreeViewNode*>& nodes) const
		{
			int r = 0;
			for (TreeViewNode* n : nodes)
			{
				r++;
				if (n->isExpanded())
				{
					if (n->getNodes().getCount())
					{
						r += GetExpandedNodeCount(n->getNodes());
					}
				}
			}
			return r;
		}

		int TreeView::GetItemHeight() const { return (int)(1.05f * m_fontRef->getLineHeight()); }
		int TreeView::MeasureExpandedModeHeight() const { return GetExpandedNodeCount() * GetItemHeight(); }

		// measure the width of space taken by all the expanded nodes
		int TreeView::MeasureExpandedNodeWidth() const { return GetExpandedNodeMaxRight(m_nodes); }

		void TreeView::OnMouseHover() { }
		void TreeView::OnMouseOut() { }
		void TreeView::OnPress() { }
		void TreeView::OnRelease() { }

		/************************************************************************/
		/*  ListView                                                            */
		/************************************************************************/

		ListView::ListView(const StyleSkin* skin, const Point& position, const Point& size, const List2D<String>& items)
			: ScrollableControl(skin, position, size), m_items(items)
		{
			Initialize(skin);
		}

		ListView::~ListView()
		{
		}

		void ListView::Initialize(const StyleSkin* skin)
		{
			// init scrollbars
			InitScrollbars(skin, true);

			m_headerHeight = m_rowHeight = m_fontRef->getLineHeightInt();
		}

		void ListView::Update(const GameTime* time)
		{
			UpdateScrollBarsGeneric(getArea(), time);
			
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (m_isResizingHeaders && mouse->IsLeftReleasedState())
				m_isResizingHeaders = false;

			if (getAbsoluteArea().Contains(mouse->GetPosition()))
			{
				if (mouse->getDZ())
				{
					m_vscrollbar->SetValue(m_vscrollbar->getValue() - mouse->getDZ() / 60);
				}
			}

			m_contentArea = GetContentArea();

			if (m_headerStyle != LHSTYLE_None)
			{
				m_contentArea.Y += m_headerHeight;
				m_contentArea.Height -= m_headerHeight;
			}

			UpdateScrollBars();
			UpdateColumnHeaders();
			
			UpdateSelection();

			if (m_headerStyle == LHSTYLE_Clickable && m_headerHoverIndex != -1 &&
				mouse->IsLeftUp())
			{
				m_columnHeader[m_headerHoverIndex].enentRelease.Invoke(this);
			}
		}

		void ListView::UpdateSelection()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (mouse->IsLeftPressed())
			{
				m_selectedRow = m_hoverRowIndex;
				m_selectedColumn = m_hoverColumnIndex;

				if (FullRowSelect)
					eventSelected.Invoke(m_selectedRow, 0);
				else
					eventSelected.Invoke(m_selectedRow, m_selectedColumn);
			}
			m_hoverRowIndex = -1;
			m_hoverColumnIndex = -1;

			if (!ParentFocused || !m_contentArea.Contains(mouse->GetPosition()))
				return;

			if (FullRowSelect)
			{
				for (int y = 0; y < m_items.getCount(); y++)
				{
					Apoc3D::Math::Rectangle selectionRect;
					selectionRect.setPosition(GetCellOffset() + m_contentArea.getTopLeft());
					selectionRect.Y += m_rowHeight * y;
					selectionRect.Width = m_contentArea.Width;
					selectionRect.Height = m_rowHeight;

					if (selectionRect.Contains(mouse->GetPosition()))
					{
						m_hoverColumnIndex = 0;
						m_hoverRowIndex = y;
					}
				}
			}
			else
			{
				Apoc3D::Math::Rectangle cellArea;
				cellArea.setPosition(GetCellOffset() + m_contentArea.getTopLeft());
				cellArea.Height = m_rowHeight;

				for (int y = 0; y < m_items.getCount(); y++)
				{
					int32 baseX = cellArea.X;
					for (int x = 0; x<m_items.getWidth(); x++)
					{
						cellArea.Width = m_columnHeader[x].Width;

						if (y > m_vscrollbar->getValue() - 1)
						{
							if (cellArea.Contains(mouse->GetPosition()))
							{
								m_hoverColumnIndex = 0;
								m_hoverRowIndex = y;
							}
						}

						cellArea.X += cellArea.Width;
					}

					cellArea.X = baseX;
					cellArea.Y += m_rowHeight;
				}
			}

		}

		void ListView::UpdateColumnHeaders()
		{
			m_headerHoverIndex = -1;

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Point mousePos = mouse->GetPosition();

			Apoc3D::Math::Rectangle maxPossibleArea = getAbsoluteArea();
			maxPossibleArea.Width = m_contentArea.Width;// excluding possible VScrollBar

			if (ParentFocused && maxPossibleArea.Contains(mousePos))
			{
				Point absP = GetAbsolutePosition();

				Apoc3D::Math::Rectangle headerArea;
				headerArea.setPosition(GetColumnHeaderOffset() + absP);
				headerArea.Height = m_headerHeight;

				for (int i = 0; i < m_columnHeader.getCount(); i++)
				{
					headerArea.Width = m_columnHeader[i].Width;

					Apoc3D::Math::Rectangle sizeArea;
					sizeArea.X = headerArea.X + headerArea.Width - 5;
					sizeArea.Y = headerArea.Y;
					sizeArea.Width = 10;
					sizeArea.Height = headerArea.Height;

					if (sizeArea.Contains(mousePos))
					{
						if (mouse->IsLeftPressed())
						{
							m_isResizingHeaders = true;
							m_resizeIndex = i;
						}
					}

					if (m_headerStyle == LHSTYLE_Clickable &&
						headerArea.Contains(mouse->GetPosition()))
					{
						m_headerHoverIndex = i;

						if (mouse->IsLeftPressed())
						{
							m_columnHeader[i].eventPress.Invoke(this);
						}
						if (mouse->IsLeftUp())
						{
							m_columnHeader[i].enentRelease.Invoke(this);
						}
					}

					headerArea.X += headerArea.Width;
				}
			}
		}
		void ListView::UpdateScrollBars()
		{
			int totalWidth = 0;
			if (m_columnHeader.getCount() > 0)
			{
				for (const ListView::Header& hdr : m_columnHeader)
					totalWidth += hdr.Width;
			}

			if (!m_isResizingHeaders)
				m_hscrollbar->Maximum = Math::Max(0, totalWidth - m_contentArea.Width);

			m_hscrollbar->Step = Math::Max(1, m_hscrollbar->Maximum / 15);

			int rowCount = m_items.getCount();
			int visiCount = GetVisibleItems();
			m_vscrollbar->Maximum = Math::Max(0, rowCount - visiCount);
		}


		void ListView::Draw(Sprite* sprite)
		{
			DrawBackground(sprite);

			{
				sprite->Flush();

				bool shouldRestoreScissorTest = false;
				Apoc3D::Math::Rectangle oldScissorRect;
				RenderDevice* dev = sprite->getRenderDevice();
				if (dev->getRenderState()->getScissorTestEnabled())
				{
					shouldRestoreScissorTest = true;
					oldScissorRect = dev->getRenderState()->getScissorTestRect();
				}
				Apoc3D::Math::Rectangle scissorRect = getAbsoluteArea();
				dev->getRenderState()->setScissorTest(true, &scissorRect);

				// draw content
				DrawColumnHeaders(sprite);
				DrawItems(sprite);
				DrawGridLines(sprite);


				sprite->Flush();
				if (shouldRestoreScissorTest)
				{
					dev->getRenderState()->setScissorTest(true, &oldScissorRect);
				}
				else
				{
					dev->getRenderState()->setScissorTest(false, 0);
				}
			}
			
			DrawScrollBars(sprite);
		}

		void ListView::DrawBackground(Sprite* sprite)
		{
			Texture* whitePix = SystemUI::GetWhitePixel();
			
			Apoc3D::Math::Rectangle m_backArea = getAbsoluteArea();
			sprite->Draw(whitePix, m_backArea, nullptr, CV_Black);

			m_backArea.Inflate(-1, -1);
			sprite->Draw(whitePix, m_backArea, nullptr, CV_White);
		}


		void ListView::DrawColumnHeaders(Sprite* sprite)
		{
			Point drawPos = GetAbsolutePosition();
			Texture* whitePix = SystemUI::GetWhitePixel();

			Apoc3D::Math::Rectangle headerArea;
			headerArea.setPosition(GetColumnHeaderOffset() + drawPos);
			headerArea.Height = m_headerHeight;

			if (m_headerStyle != LHSTYLE_None)
			{
				int totalWidth = 0;

				for (int i = 0; i < m_columnHeader.getCount(); i++)
				{
					headerArea.Width = m_columnHeader[i].Width;

					sprite->Draw(whitePix, headerArea, CV_Black);

					if (m_headerHoverIndex == i)
					{
						// MouseOver Bevel Effect
						Apoc3D::Math::Rectangle r2 = headerArea;

						r2.Width--;
						r2.Height--;

						sprite->Draw(whitePix, r2, nullptr, CV_DarkGray);

						r2.X++;
						r2.Y++;
						r2.Width--;
						r2.Height--;

						sprite->Draw(whitePix, r2, nullptr, CV_LightGray);
					}
					else
					{
						Apoc3D::Math::Rectangle r2 = headerArea;

						//Draw normal header
						r2.Width--;
						r2.Height--;
						sprite->Draw(whitePix, r2, nullptr, CV_White);

						r2.X++;
						r2.Y++;
						r2.Width--;
						r2.Height--;
						sprite->Draw(whitePix, r2, CV_LightGray);

						r2.X++;
						r2.Y--;
					}

					// =========================================

					Point textPos = headerArea.getTopLeft();
					String text = m_columnHeader[i].Text;
					guiOmitLineText(m_fontRef, headerArea.Width, text);

					Point mts = m_fontRef->MeasureString(text);
					textPos.X += (headerArea.Width - mts.X) / 2;
					m_fontRef->DrawString(sprite, text, textPos, CV_Black);

					headerArea.X += headerArea.Width;
					totalWidth += headerArea.Width;
				}

				if (m_hscrollbar->Maximum == 0)
					DrawHeaderEnd(sprite, headerArea.getPosition(), m_size.X - totalWidth);
			}
		}

		void ListView::DrawHeaderEnd(Sprite* sprite, const Point& pos, int32 width)
		{
			Apoc3D::Math::Rectangle headerArea;
			headerArea.setPosition(pos);

			Texture* whitePix = SystemUI::GetWhitePixel();

			headerArea.Width = width;
			headerArea.Height = m_headerHeight;
			sprite->Draw(whitePix, headerArea, CV_Black);

			headerArea.Width--;
			headerArea.Height--;
			sprite->Draw(whitePix, headerArea, CV_White);

			headerArea.X++;
			headerArea.Y++;
			headerArea.Width--;
			headerArea.Height--;
			sprite->Draw(whitePix, headerArea, CV_LightGray);
		}

		void ListView::DrawGridLines(Sprite* sprite)
		{
			Texture* whitePix = SystemUI::GetWhitePixel();

			int32 rowCount = GetVisibleItems();

			Apoc3D::Math::Rectangle lineRect;
			lineRect.setPosition(m_contentArea.getTopLeft());
			lineRect.Width = m_contentArea.Width;
			lineRect.Height = 1;

			// rows
			for (int y = 0; y <= rowCount; y++)
			{
				sprite->Draw(whitePix, lineRect, CV_LightGray);

				lineRect.Y += m_rowHeight;
			}

			// columns
			lineRect.Width = 1;
			lineRect.Height = m_contentArea.Height;

			lineRect.setPosition(GetColumnHeaderOffset() + m_contentArea.getTopLeft());
			lineRect.X--;

			for (const ListView::Header& hdr : m_columnHeader)
			{
				sprite->Draw(whitePix, lineRect, CV_LightGray);

				lineRect.X += hdr.Width;
			}

			sprite->Draw(whitePix, lineRect, CV_LightGray);
		}

		void ListView::DrawItems(Sprite* sprite)
		{
			Apoc3D::Math::Rectangle cellArea;
			cellArea.setPosition(GetCellOffset() + m_contentArea.getTopLeft());
			cellArea.Height = m_rowHeight;

			int32 baseX = cellArea.X;
			for (int y = 0; y < m_items.getCount(); y++)
			{
				for (int x = 0; x<m_items.getWidth(); x++)
				{
					cellArea.Width = m_columnHeader[x].Width;

					if (y > m_vscrollbar->getValue() - 1)
					{
						if (!m_isResizingHeaders)
						{
							if (HoverSelectionMode && x == m_hoverColumnIndex && y == m_hoverRowIndex)
								DrawHoverBox(sprite, cellArea);
						}

						if (FullRowSelect && x == 0 && y == m_selectedRow)
							DrawSelectedBox(sprite, cellArea);
						else if (!FullRowSelect && x == m_selectedRow && y == m_selectedColumn)
							DrawSelectedBox(sprite, cellArea);

						{
							// truncate text
							String text = m_items.at(y, x);

							guiOmitLineText(m_fontRef, cellArea.Width, text);

							//m_fontRef->DrawString(sprite, text, cellArea.getTopLeft(), CV_Black);
							TextSettings.Draw(sprite, m_fontRef, text, cellArea, 0xff);
						}
					}
					cellArea.X += cellArea.Width;
				}
				cellArea.X = baseX;
				cellArea.Y += m_rowHeight;
			}
		}

		void ListView::DrawSelectedBox(Sprite* sprite, const Apoc3D::Math::Rectangle& area) { DrawItemBox(sprite, area, CV_Silver); }
		void ListView::DrawHoverBox(Sprite* sprite, const Apoc3D::Math::Rectangle& area) { DrawItemBox(sprite, area, CV_LightGray); }
		void ListView::DrawItemBox(Sprite* sprite, const Apoc3D::Math::Rectangle& area, ColorValue cv)
		{
			Apoc3D::Math::Rectangle selectionRect;

			if (FullRowSelect)
			{
				selectionRect.X = GetAbsolutePosition().X;
				selectionRect.Width = m_size.X;
			}
			else
			{
				selectionRect.X = area.X - 4;
				selectionRect.Width = area.Width;
			}
			selectionRect.Height = area.Height;
			selectionRect.Y = area.Y;

			sprite->Draw(SystemUI::GetWhitePixel(), selectionRect, cv);
		}

		Point ListView::GetColumnHeaderOffset() const
		{
			Point r;
			r.X = -m_hscrollbar->getValue();
			r.Y = 0;
			return r;
		}
		Point ListView::GetCellOffset() const
		{
			Point r;
			r.X = -m_hscrollbar->getValue();
			r.Y = -m_vscrollbar->getValue() * m_rowHeight;
			return r;
		}

		int ListView::GetVisibleItems()
		{
			return (int)ceilf((float)(m_contentArea.Height / m_rowHeight));
		}
	}
}