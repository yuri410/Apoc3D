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
#include "List.h"
#include "FontManager.h"
#include "StyleSkin.h"
#include "Input/Mouse.h"
#include "Input/InputAPI.h"
#include "Button.h"
#include "Graphics/RenderSystem/Texture.h"
#include "Graphics/RenderSystem/Sprite.h"
#include "Graphics/RenderSystem/RenderDevice.h"
#include "Graphics/RenderSystem/RenderStateManager.h"
#include "Scrollbar.h"
#include "Form.h"

using namespace Apoc3D::Input;

namespace Apoc3D
{
	namespace UI
	{
		ListBox::ListBox(const Point& position, int width, int height, const List<String>& items)
			: Control(position), m_sorted(false), m_isSorted(false), m_visisbleItems(0), m_textOffset(0,0),
			m_selectionRect(0,0,0,0), m_hoverIndex(-1), m_selectedIndex(-1), m_vscrollbar(0), m_hscrollbar(0), m_horizontalScrollbar(false), m_hScrollWidth(0),
			m_mouseOver(false), m_items(items)
		{
			Size = Point(width, height);
		}

		ListBox::~ListBox()
		{
			if(m_hscrollbar)
				delete m_hscrollbar;
			if (m_vscrollbar)
				delete m_vscrollbar;
		}

		void ListBox::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			m_textOffset = Point(5, m_skin->TextBox->getHeight()-m_fontRef->getLineHeight()/2);

			m_destRect[0] = Apoc3D::Math::Rectangle(0,0, 
				m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[1] = Apoc3D::Math::Rectangle(0,0, 
				Size.X - m_skin->TextBoxSrcRects[0].Width*2, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[2] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);

			m_destRect[3] = Apoc3D::Math::Rectangle(0,0, 
				m_skin->TextBoxSrcRects[0].Width, Size.Y - m_skin->TextBoxSrcRects[0].Height*2);
			m_destRect[4] = Apoc3D::Math::Rectangle(0,0,
				m_destRect[1].Width, m_destRect[3].Height);
			m_destRect[5] = Apoc3D::Math::Rectangle(0,0,
				m_skin->TextBoxSrcRects[1].Width, m_destRect[3].Height);

			m_destRect[6] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[7] = Apoc3D::Math::Rectangle(0,0, m_destRect[1].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[8] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);

			m_visisbleItems = (int)ceilf((float)Size.Y / m_fontRef->getLineHeight());
			Size.Y = m_visisbleItems * m_fontRef->getLineHeight();

			UpdateHScrollbar();
			InitScrollbars(device);

			if (m_sorted)
			{
				m_isSorted = true;
			}
		}

		void ListBox::InitScrollbars(RenderDevice* device)
		{
			if (m_vscrollbar)
			{
				delete m_vscrollbar;
				m_vscrollbar = 0;
			}
			if (m_hscrollbar)
			{
				delete m_hscrollbar;
				m_hscrollbar = 0;
			}

			if (getUseHorizontalScrollbar())
			{
				if (m_items.getCount()>m_visisbleItems)
				{
					m_hscrollbar = new ScrollBar(Point(Position.X+1,Position.Y+Size.Y-13),ScrollBar::SCRBAR_Horizontal,Size.X -14);
				}
				else
				{
					m_hscrollbar = new ScrollBar(Point(Position.X+1,Position.Y+Size.Y-13),ScrollBar::SCRBAR_Horizontal,Size.X -2);
				}

				m_hscrollbar->setOwner(getOwner());
				m_hscrollbar->SetSkin(m_skin);
				m_hscrollbar->Initialize(device);
			}

			int vScrollbarHeight = 0;
			if (getUseHorizontalScrollbar() && m_hscrollbar->Visible && m_hscrollbar->getMax()>0)
			{
				vScrollbarHeight = Size.Y - 14;
			}
			else
			{
				vScrollbarHeight = Size.Y - 2;
			}

			m_vscrollbar = new ScrollBar(Point(Position.X+Size.X - 13,Position.Y+1),ScrollBar::SCRBAR_Vertical,vScrollbarHeight);
			m_vscrollbar->setOwner(getOwner());
			m_vscrollbar->SetSkin(m_skin);
			m_vscrollbar->Initialize(device);
		}
		void ListBox::Update(const GameTime* const time)
		{
			if (m_vscrollbar && m_vscrollbar->Visible)
			{
				m_vscrollbar->Update(time);
			}
			if (m_hscrollbar && m_hscrollbar->Visible)
			{
				m_hscrollbar->Update(time);
			}

			if (!m_isSorted && m_sorted)
			{

			}

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (UIRoot::getTopMostForm() == getOwner())
			{
				if (m_hoverIndex != -1 && mouse->IsLeftPressed())
				{
					int previousIndex = m_selectedIndex;
					m_selectedIndex = m_hoverIndex;
					if (!m_eSelect.empty())
						m_eSelect(this);
					if (m_selectedIndex != previousIndex && !m_eSelectionChanged.empty())
						m_eSelectionChanged(this);
				}

				if (getAbsoluteArea().Contains(mouse->GetCurrentPosition()))
				{
					if (!m_mouseOver)
					{
						m_mouseOver = true;
						OnMouseOver();
					}

					if (mouse->IsLeftPressed())
						OnPress();
					else if (mouse->IsLeftUp())
						OnRelease();
				}
				else if (m_mouseOver)
				{
					m_mouseOver = false;
					OnMouseOut();
				}
			}
		}

		void ListBox::UpdateHScrollbar()
		{
			// measure the max width among all the item
			int ew = 0;
			for (int i=0;i<m_items.getCount();i++)
			{
				int w;
				if (!m_vscrollbar || m_vscrollbar->getMax()==0)
					w = m_fontRef->MeasureString(m_items[i]).X - Size.X + 12;
				else
					w = m_fontRef->MeasureString(m_items[i]).X - Size.X + 30;

				if (w > ew)
					ew = w;
			}

			m_hScrollWidth = ew;
			if (m_hscrollbar)
				m_hscrollbar->setMax(m_hScrollWidth);
		}

		void ListBox::Draw(Sprite* sprite)
		{
			DrawBackground(sprite);

			sprite->Flush();
			Matrix trans;
			Matrix::CreateTranslation(trans, (float)Position.X, (float)Position.Y ,0);
			sprite->MultiplyTransform(trans);

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

			m_hoverIndex = -1;
			for (int i=m_vscrollbar->getValue();
				i<min(m_items.getCount(), m_vscrollbar->getValue()+m_visisbleItems);i++)
			{
				if (m_hscrollbar)
					m_textOffset.X = -m_hscrollbar->getValue() + 2;
				else
					m_textOffset.X = 2;

				m_textOffset.Y = (i - m_vscrollbar->getValue()) * m_fontRef->getLineHeight();
				if (UIRoot::getTopMostForm() == getOwner())
					RenderSelectionBox(sprite,i);

				m_fontRef->DrawString(sprite, m_items[i], m_textOffset, m_skin->ForeColor);
			}
			sprite->Flush();
			if (shouldRestoreScissorTest)
			{
				dev->getRenderState()->setScissorTest(true,&oldScissorRect);
			}
			else
			{
				dev->getRenderState()->setScissorTest(false,0);
			}
			if (sprite->isUsingStack())
				sprite->PopTransform();
			else
				sprite->SetTransform(Matrix::Identity);
			
			UpdateHScrollbar();
			DrawScrollbar(sprite);
		}

		
		void ListBox::RenderSelectionBox(Sprite* sprite, int index)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			m_selectionRect.X = 0;
			m_selectionRect.Y = m_textOffset.Y;
			m_selectionRect.Height = m_fontRef->getLineHeight();

			if (m_vscrollbar->getMax()>0)
				m_selectionRect.Width = Size.X - 13;
			else
				m_selectionRect.Width = Size.X - 1;

			if (index == m_selectedIndex)
				sprite->Draw(m_skin->WhitePixelTexture, m_selectionRect, CV_LightGray);
			else if (//getOwner()->getArea().Contains(mouse->GetCurrentPosition()) &&
				m_selectionRect.Contains(Point(mouse->GetCurrentPosition().X-getOwner()->Position.X-Position.X, 
				mouse->GetCurrentPosition().Y-getOwner()->Position.Y-Position.Y)))
			{
				m_hoverIndex = index;
				sprite->Draw(m_skin->WhitePixelTexture, m_selectionRect, CV_Silver);
			}
				
		}
		void ListBox::DrawBackground(Sprite* sprite)
		{
			m_destRect[0].X = Position.X;
			m_destRect[0].Y = Position.Y;
			sprite->Draw(m_skin->TextBox, m_destRect[0], &m_skin->TextBoxSrcRects[0], CV_White);
			m_destRect[1].X = m_destRect[0].X + m_destRect[0].Width;
			m_destRect[1].Y = m_destRect[0].Y;
			sprite->Draw(m_skin->TextBox, m_destRect[1], &m_skin->TextBoxSrcRects[1], CV_White);
			m_destRect[2].X = m_destRect[1].X + m_destRect[1].Width;
			m_destRect[2].Y = m_destRect[0].Y;
			sprite->Draw(m_skin->TextBox, m_destRect[2], &m_skin->TextBoxSrcRects[2], CV_White);

			m_destRect[3].X = m_destRect[0].X;
			m_destRect[3].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[3], &m_skin->TextBoxSrcRects[3], CV_White);
			m_destRect[4].X = m_destRect[1].X;
			m_destRect[4].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[4], &m_skin->TextBoxSrcRects[4], CV_White);
			m_destRect[5].X = m_destRect[2].X;
			m_destRect[5].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[5], &m_skin->TextBoxSrcRects[5], CV_White);

			m_destRect[6].X = m_destRect[0].X;
			m_destRect[6].Y = m_destRect[3].Y + m_destRect[3].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[6], &m_skin->TextBoxSrcRects[6], CV_White);
			m_destRect[7].X = m_destRect[1].X;
			m_destRect[7].Y = m_destRect[4].Y + m_destRect[4].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[7], &m_skin->TextBoxSrcRects[7], CV_White);
			m_destRect[8].X = m_destRect[2].X;
			m_destRect[8].Y = m_destRect[5].Y + m_destRect[5].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[8], &m_skin->TextBoxSrcRects[8], CV_White);
		}
		void ListBox::DrawScrollbar(Sprite* sprite)
		{
			m_vscrollbar->setMax(max(0, m_items.getCount()-m_visisbleItems));
			if (m_vscrollbar->getValue()>m_vscrollbar->getMax())
				m_vscrollbar->setValue(m_vscrollbar->getMax());

			if (m_vscrollbar->getMax()>0)
			{
				m_vscrollbar->Draw(sprite);
				if (m_hscrollbar)
				{
					m_hscrollbar->setWidth( Size.X - 14);
					UpdateHScrollbar();
				}
			}
			else if (m_hscrollbar)
			{
				m_hscrollbar->setWidth(Size.X-2);
				UpdateHScrollbar();
			}

			if (getUseHorizontalScrollbar() && m_hscrollbar && m_hscrollbar->getMax()>0)
			{
				m_vscrollbar->setHeight(Size.Y - 12);
				m_hscrollbar->setMax(m_hScrollWidth);
				m_hscrollbar->Draw(sprite);
				m_visisbleItems = (int)ceilf((float)Size.Y / m_fontRef->getLineHeight())-1;
			}
			else if ((!m_hscrollbar || !m_hscrollbar->getMax()) && m_vscrollbar->getHeight() != Size.Y-2)
			{
				m_vscrollbar->setHeight(Size.Y - 2);
				m_visisbleItems++;
			}
		}

		/************************************************************************/
		/* Tree view                                                            */
		/************************************************************************/

		int TreeViewIntent = 25;

		TreeView::TreeView(const Point& position, int width, int height)
			: Control(position), m_visisbleItems(0), m_textOffset(0,0),
			m_selectionRect(0,0,0,0), m_selectedNode(0), m_vscrollbar(0), m_hscrollbar(0), m_horizontalScrollbar(false), m_hScrollWidth(0),
			m_mouseOver(false)
		{
			Size = Point(width, height);
		}

		TreeView::~TreeView()
		{
			if(m_hscrollbar)
				delete m_hscrollbar;
			if (m_vscrollbar)
				delete m_vscrollbar;
		}

		int TreeView::GetAllNodeCount(const FastList<TreeViewNode*>& node) const
		{
			int result = node.getCount();

			for (int i=0;i<node.getCount();i++)
			{
				result+=GetAllNodeCount( node[i]->getNodes());
			}
			
			return result;
		}
		int TreeView::GetAllNodeCount() const
		{
			return GetAllNodeCount(m_nodes);
		}
		int TreeView::GetExpandedNodeMaxRight(const FastList<TreeViewNode*>& node) const
		{
			int r = 0;
			for (int i=0;i<node.getCount();i++)
			{
				if (node[i]->isExpanded())
				{
					if (node[i]->getNodes().getCount())
					{
						int res = GetExpandedNodeMaxRight(node[i]->getNodes()) + TreeViewIntent;
						if (res>r)
							r = res;
					}
				}
				int w;
				if (!m_vscrollbar || m_vscrollbar->getMax()==0)
					w = m_fontRef->MeasureString(node[i]->getText()).X - Size.X + 12;
				else
					w = m_fontRef->MeasureString(node[i]->getText()).X - Size.X + 30;
				if (w>r)
					r= w;
			}
			return r;
		}
		int TreeView::GetExpandedNodeCount(const FastList<TreeViewNode*>& node) const
		{
			int r = 0;
			for (int i=0;i<node.getCount();i++)
			{
				r++;
				if (node[i]->isExpanded())
				{
					if (node[i]->getNodes().getCount())
					{
						r+=GetExpandedNodeCount(node[i]->getNodes());
					}
				}
			}
			return r;
		}
		int TreeView::GetItemHeight() const
		{
			return (int)(1.2f * m_fontRef->getLineHeight());
		}
		int TreeView::MeasureExpandedModeHeight() const
		{
			return GetExpandedNodeCount() * GetItemHeight();
		}
		int TreeView::MeasureExpandedNodeWidth() const
		{
			// measure the width of space taken by all the expanded nodes
			return GetExpandedNodeMaxRight(m_nodes);
		}
		void TreeView::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);
			m_textOffset = Point(5, m_skin->TextBox->getHeight()-GetItemHeight()/2);

			m_destRect[0] = Apoc3D::Math::Rectangle(0,0, 
				m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[1] = Apoc3D::Math::Rectangle(0,0, 
				Size.X - m_skin->TextBoxSrcRects[0].Width*2, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[2] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);

			m_destRect[3] = Apoc3D::Math::Rectangle(0,0, 
				m_skin->TextBoxSrcRects[0].Width, Size.Y - m_skin->TextBoxSrcRects[0].Height*2);
			m_destRect[4] = Apoc3D::Math::Rectangle(0,0,
				m_destRect[1].Width, m_destRect[3].Height);
			m_destRect[5] = Apoc3D::Math::Rectangle(0,0,
				m_skin->TextBoxSrcRects[1].Width, m_destRect[3].Height);

			m_destRect[6] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[7] = Apoc3D::Math::Rectangle(0,0, m_destRect[1].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[8] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);

			m_visisbleItems = (int)ceilf((float)Size.Y / GetItemHeight());
			Size.Y = m_visisbleItems * GetItemHeight();

			UpdateHScrollbar();
			InitScrollbars(device);

		}

		void TreeView::InitScrollbars(RenderDevice* device)
		{
			if (m_vscrollbar)
			{
				delete m_vscrollbar;
				m_vscrollbar = 0;
			}
			if (m_hscrollbar)
			{
				delete m_hscrollbar;
				m_hscrollbar = 0;
			}

			if (getUseHorizontalScrollbar())
			{
				if (GetAllNodeCount()>m_visisbleItems)
				{
					m_hscrollbar = new ScrollBar(Point(Position.X+1,Position.Y+Size.Y-13-12),ScrollBar::SCRBAR_Horizontal,Size.X -14);
				}
				else
				{
					m_hscrollbar = new ScrollBar(Point(Position.X+1,Position.Y+Size.Y-13-12),ScrollBar::SCRBAR_Horizontal,Size.X -2);
				}

				m_hscrollbar->setOwner(getOwner());
				m_hscrollbar->SetSkin(m_skin);
				m_hscrollbar->Initialize(device);
			}

			int vScrollbarHeight = 0;
			if (getUseHorizontalScrollbar() && m_hscrollbar->Visible && m_hscrollbar->getMax()>0)
			{
				vScrollbarHeight = Size.Y - 14;
			}
			else
			{
				vScrollbarHeight = Size.Y - 2;
			}
			vScrollbarHeight-=12;

			m_vscrollbar = new ScrollBar(Point(Position.X+Size.X - 13,Position.Y+1),ScrollBar::SCRBAR_Vertical,vScrollbarHeight);
			m_vscrollbar->setOwner(getOwner());
			m_vscrollbar->SetSkin(m_skin);
			m_vscrollbar->Initialize(device);
		}
		void TreeView::Update(const GameTime* const time)
		{
			if (m_vscrollbar && m_vscrollbar->Visible)
			{
				m_vscrollbar->Update(time);
			}
			if (m_hscrollbar && m_hscrollbar->Visible)
			{
				m_hscrollbar->Update(time);
			}


			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (UIRoot::getTopMostForm() == getOwner())
			{
				if (m_hoverNode && mouse->IsLeftPressed())
				{
					TreeViewNode* previousNode = m_selectedNode;
					m_selectedNode = m_hoverNode;
					if (!m_eSelect.empty())
						m_eSelect(this);
					if (m_selectedNode != previousNode && !m_eSelectionChanged.empty())
						m_eSelectionChanged(this);
				}
				if (getAbsoluteArea().Contains(mouse->GetCurrentPosition()))
				{
					if (!m_mouseOver)
					{
						m_mouseOver = true;
						OnMouseOver();
					}

					if (mouse->IsLeftPressed())
						OnPress();
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
			if (m_hscrollbar)
				m_hscrollbar->setMax(m_hScrollWidth);
		}

		void TreeView::DrawNodes(Sprite* sprite, const FastList<TreeViewNode*>& nodes, int depth, int& counter, int maxCount)
		{
			for (int i=0;i<nodes.getCount();i++)
			{
				counter++;
				if (counter>maxCount)
					return;

				if (m_hscrollbar)
					m_textOffset.X = -m_hscrollbar->getValue() + 2;
				else
					m_textOffset.X = 2;
				m_textOffset.X += depth * TreeViewIntent;

				m_textOffset.Y = (counter-1 - m_vscrollbar->getValue()) * GetItemHeight();

				if (UIRoot::getTopMostForm() == getOwner())
					RenderSelectionBox(sprite,nodes[i]);

				if (nodes[i]->getIcon())
				{
					//Apoc3D::Math::Rectangle iconRect(m_textOffset.X, m_textOffset.Y, m_nodes[i]->getIcon()->getWidth(), m_nodes[i]->getIcon()->getHeight());
					sprite->Draw(nodes[i]->getIcon(),m_textOffset.X,m_textOffset.Y,CV_White);

					Point to(m_textOffset);
					to.X+=nodes[i]->getIcon()->getWidth();
					m_fontRef->DrawString(sprite, nodes[i]->getText(), to, m_skin->ForeColor);
				}
				else
				{
					m_fontRef->DrawString(sprite, nodes[i]->getText(), m_textOffset, m_skin->ForeColor);
				}
				

				if (nodes[i]->isExpanded() && nodes[i]->getNodes().getCount())
				{
					DrawNodes(sprite, nodes[i]->getNodes(), depth+1, counter, maxCount);
				}
			}
		}
		void TreeView::Draw(Sprite* sprite)
		{
			DrawBackground(sprite);

			sprite->Flush();
			Matrix trans;
			Matrix::CreateTranslation(trans, (float)Position.X, (float)Position.Y ,0);
			sprite->MultiplyTransform(trans);

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

			//for (int i=m_vscrollbar->getValue();
			//	i<min(m_nodes.getCount(), m_vscrollbar->getValue()+m_visisbleItems);i++)
			//{
			//	if (m_hscrollbar)
			//		m_textOffset.X = -m_hscrollbar->getValue() + 2;
			//	else
			//		m_textOffset.X = 2;

			//	m_textOffset.Y = (i - m_vscrollbar->getValue()) * m_fontRef->getLineHeight();
			//	if (UIRoot::getTopMostForm() == getOwner())
			//		RenderSelectionBox(sprite,i);

			//	m_fontRef->DrawString(sprite, m_items[i], m_textOffset, m_skin->ForeColor);
			//}
			sprite->Flush();
			if (shouldRestoreScissorTest)
			{
				dev->getRenderState()->setScissorTest(true,&oldScissorRect);
			}
			else
			{
				dev->getRenderState()->setScissorTest(false,0);
			}
			if (sprite->isUsingStack())
				sprite->PopTransform();
			else
				sprite->SetTransform(Matrix::Identity);

			UpdateHScrollbar();
			DrawScrollbar(sprite);
		}



		void TreeView::RenderSelectionBox(Sprite* sprite, TreeViewNode* node)
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			m_selectionRect.X = 0;
			m_selectionRect.Y = m_textOffset.Y;
			m_selectionRect.Height = GetItemHeight();

			if (m_vscrollbar->getMax()>0)
				m_selectionRect.Width = Size.X - 13;
			else
				m_selectionRect.Width = Size.X - 1;

			if (node == m_selectedNode)
				sprite->Draw(m_skin->WhitePixelTexture, m_selectionRect, CV_LightGray);
			if (getOwner()->getArea().Contains(mouse->GetCurrentPosition()) &&
				m_selectionRect.Contains(Point(mouse->GetCurrentPosition().X-getOwner()->Position.X-Position.X, 
				mouse->GetCurrentPosition().Y-getOwner()->Position.Y-Position.Y)))
			{
				m_anyHoverNode = node;
				if (node != m_selectedNode)
				{
					m_hoverNode = node;
					sprite->Draw(m_skin->WhitePixelTexture, m_selectionRect, CV_Silver);
				}
			}

		}
		void TreeView::DrawBackground(Sprite* sprite)
		{
			m_destRect[0].X = Position.X;
			m_destRect[0].Y = Position.Y;
			sprite->Draw(m_skin->TextBox, m_destRect[0], &m_skin->TextBoxSrcRects[0], CV_White);
			m_destRect[1].X = m_destRect[0].X + m_destRect[0].Width;
			m_destRect[1].Y = m_destRect[0].Y;
			sprite->Draw(m_skin->TextBox, m_destRect[1], &m_skin->TextBoxSrcRects[1], CV_White);
			m_destRect[2].X = m_destRect[1].X + m_destRect[1].Width;
			m_destRect[2].Y = m_destRect[0].Y;
			sprite->Draw(m_skin->TextBox, m_destRect[2], &m_skin->TextBoxSrcRects[2], CV_White);

			m_destRect[3].X = m_destRect[0].X;
			m_destRect[3].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[3], &m_skin->TextBoxSrcRects[3], CV_White);
			m_destRect[4].X = m_destRect[1].X;
			m_destRect[4].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[4], &m_skin->TextBoxSrcRects[4], CV_White);
			m_destRect[5].X = m_destRect[2].X;
			m_destRect[5].Y = m_destRect[0].Y + m_destRect[0].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[5], &m_skin->TextBoxSrcRects[5], CV_White);

			m_destRect[6].X = m_destRect[0].X;
			m_destRect[6].Y = m_destRect[3].Y + m_destRect[3].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[6], &m_skin->TextBoxSrcRects[6], CV_White);
			m_destRect[7].X = m_destRect[1].X;
			m_destRect[7].Y = m_destRect[4].Y + m_destRect[4].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[7], &m_skin->TextBoxSrcRects[7], CV_White);
			m_destRect[8].X = m_destRect[2].X;
			m_destRect[8].Y = m_destRect[5].Y + m_destRect[5].Height;
			sprite->Draw(m_skin->TextBox, m_destRect[8], &m_skin->TextBoxSrcRects[8], CV_White);
		}
		void TreeView::DrawScrollbar(Sprite* sprite)
		{
			m_vscrollbar->setMax(max(0, GetAllNodeCount()-m_visisbleItems));
			if (m_vscrollbar->getValue()>m_vscrollbar->getMax())
				m_vscrollbar->setValue(m_vscrollbar->getMax());

			if (m_vscrollbar->getMax()>0)
			{
				m_vscrollbar->Draw(sprite);
				if (m_hscrollbar)
				{
					m_hscrollbar->setWidth( Size.X - 14);
					UpdateHScrollbar();
				}
			}
			else if (m_hscrollbar)
			{
				m_hscrollbar->setWidth(Size.X-2);
				UpdateHScrollbar();
			}

			if (getUseHorizontalScrollbar() && m_hscrollbar && m_hscrollbar->getMax()>0)
			{
				m_vscrollbar->setHeight(Size.Y - 12-12);
				m_hscrollbar->setMax(m_hScrollWidth);
				m_hscrollbar->Draw(sprite);
				m_visisbleItems = (int)ceilf((float)Size.Y / GetItemHeight())-1;
			}
			else if ((!m_hscrollbar || !m_hscrollbar->getMax()) && m_vscrollbar->getHeight() != Size.Y-2)
			{
				m_vscrollbar->setHeight(Size.Y - 2-12);
				m_visisbleItems++;
			}
		}

		void TreeView::SetSize(const Point& newSize)
		{
			if (newSize==Size)
				return;

			Size = newSize;
			m_destRect[0] = Apoc3D::Math::Rectangle(0,0, 
				m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[1] = Apoc3D::Math::Rectangle(0,0, 
				Size.X - m_skin->TextBoxSrcRects[0].Width*2, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[2] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);

			m_destRect[3] = Apoc3D::Math::Rectangle(0,0, 
				m_skin->TextBoxSrcRects[0].Width, Size.Y - m_skin->TextBoxSrcRects[0].Height*2);
			m_destRect[4] = Apoc3D::Math::Rectangle(0,0,
				m_destRect[1].Width, m_destRect[3].Height);
			m_destRect[5] = Apoc3D::Math::Rectangle(0,0,
				m_skin->TextBoxSrcRects[1].Width, m_destRect[3].Height);

			if (getUseHorizontalScrollbar())
			{
				if (GetAllNodeCount()>m_visisbleItems)
				{
					m_hscrollbar->setPosition( Point(Position.X+1,Position.Y+Size.Y-13));
				}
				else
				{
					m_hscrollbar->setPosition( Point(Position.X+1,Position.Y+Size.Y-13));
				}
			}

			int vScrollbarHeight = 0;
			if (getUseHorizontalScrollbar() && m_hscrollbar->Visible && m_hscrollbar->getMax()>0)
			{
				vScrollbarHeight = Size.Y - 14;
			}
			else
			{
				vScrollbarHeight = Size.Y - 2;
			}
			vScrollbarHeight-=12;

			m_vscrollbar->setPosition( Point(Position.X+Size.X - 13,Position.Y+1));

			if (m_hscrollbar)
				m_hscrollbar->setWidth(newSize.X);
			if (m_vscrollbar)
				m_vscrollbar->setHeight(newSize.Y);
		}

	}
}