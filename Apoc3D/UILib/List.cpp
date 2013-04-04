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
			m_textOffset = Point(5, m_skin->TextBox->getHeight()-m_fontRef->getLineHeightInt()/2);

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
				m_skin->TextBoxSrcRects[0].Width, m_destRect[3].Height);

			m_destRect[6] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[7] = Apoc3D::Math::Rectangle(0,0, m_destRect[1].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[8] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);

			m_visisbleItems = (int)ceilf((float)Size.Y / m_fontRef->getLineHeightInt());
			Size.Y = m_visisbleItems * m_fontRef->getLineHeightInt();

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
			if (m_selectedIndex>=m_items.getCount())
				m_selectedIndex=-1;

			if (m_hscrollbar)
				m_hscrollbar->setOwner(getOwner());
			if (m_vscrollbar)
				m_vscrollbar->setOwner(getOwner());

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

					if (mouse->getDZ())
					{
						m_vscrollbar->setValue(Math::Clamp(m_vscrollbar->getValue() - mouse->getDZ() / 60, 0, m_vscrollbar->getMax()));
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

				m_textOffset.Y = (i - m_vscrollbar->getValue()) * m_fontRef->getLineHeightInt();
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
			m_selectionRect.Height = m_fontRef->getLineHeightInt();

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
				m_vscrollbar->setStep(max(1, m_vscrollbar->getMax()/15));
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

		int TreeView::GetAllVisibleNodeCount(const FastList<TreeViewNode*>& node) const
		{
			int result = node.getCount();

			for (int i=0;i<node.getCount();i++)
			{
				if (node[i]->isExpanded())
					result += GetAllVisibleNodeCount( node[i]->getNodes());
			}
			
			return result;
		}
		int TreeView::GetAllVisibleNodeCount() const
		{
			return GetAllVisibleNodeCount(m_nodes);
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
				m_skin->TextBoxSrcRects[0].Width, m_destRect[3].Height);

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
				if (GetAllVisibleNodeCount()>m_visisbleItems)
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
			m_visisbleItems = (int)ceilf((float)Size.Y / GetItemHeight());

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
					
					if (mouse->getDZ())
					{
						m_vscrollbar->setValue(Math::Clamp(m_vscrollbar->getValue() - mouse->getDZ() / 60, 0, m_vscrollbar->getMax()));
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
			int barmax = m_hScrollWidth - Size.X;
			if (barmax<0) barmax = 0;
			if (m_hscrollbar)
				m_hscrollbar->setMax(barmax);
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
			if (m_hscrollbar && m_vscrollbar->getMax()>0)
			{
				m_vscrollbar->setMax(max(0, GetAllVisibleNodeCount()-m_visisbleItems+1));
			}
			else
			{
				m_vscrollbar->setMax(max(0, GetAllVisibleNodeCount()-m_visisbleItems));
			}

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
				m_vscrollbar->setStep(max(1, m_vscrollbar->getMax()/15));
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
				//m_visisbleItems = (int)ceilf((float)Size.Y / GetItemHeight())-1;
			}
			else if ((!m_hscrollbar || !m_hscrollbar->getMax()) && m_vscrollbar->getHeight() != Size.Y-2)
			{
				m_vscrollbar->setHeight(Size.Y - 2-12);
				//m_visisbleItems++;
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
				m_skin->TextBoxSrcRects[0].Width, m_destRect[3].Height);

			m_destRect[6] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[7] = Apoc3D::Math::Rectangle(0,0, m_destRect[1].Width, m_skin->TextBoxSrcRects[0].Height);
			m_destRect[8] = Apoc3D::Math::Rectangle(0,0, m_skin->TextBoxSrcRects[0].Width, m_skin->TextBoxSrcRects[0].Height);


			if (getUseHorizontalScrollbar())
			{
				if (GetAllVisibleNodeCount()>m_visisbleItems)
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

		/************************************************************************/
		/*  ListView                                                            */
		/************************************************************************/

		ListView::ListView(const Point& position, const Point& size, const List2D<String>& items)
			: Control(position, L"", size), m_headerArea(0,0,0,0), m_isResizing(false),
			m_headerStyle(LHSTYLE_Clickable), m_headerHoverIndex(-1), m_gridLines(false),
			m_lineRect(0,0,0,0), m_gridSize(0,0), m_selectedRow(-1), m_selectedColumn(-1),
			m_hoverRowIndex(-1), m_hoverColumnIndex(-1), m_selectionArea(0,0,0,0),
			m_selectionRect(0,0,0,0), m_fullRowSelect(false), m_hoverSelection(false),
			m_resizeIndex(-1), m_sizeArea(0,0,0,0),
			m_items(items),
			m_hScrollBar(0), m_vScrollBar(0),
			m_srcRect(0,0,0,0)
		{

		}
		ListView::~ListView()
		{
			if (m_hScrollBar)
				delete m_hScrollBar;
			if (m_vScrollBar)
				delete m_vScrollBar;
		}

		void ListView::Initialize(RenderDevice* device)
		{
			Control::Initialize(device);

			//Size.Y = (Size.Y - 2 + 12) / m_fontRef->getLineHeight();

			m_backArea.Width = Size.X;
			m_backArea.Height = Size.Y;
			m_backArea.X = Position.X;
			m_backArea.Y = Position.Y;

			// init scrollbars
			m_vScrollBar = new ScrollBar(Position+Point(Size.X-13, 1), ScrollBar::SCRBAR_Vertical, Size.Y - 14);
			m_vScrollBar->setOwner(getOwner());
			m_vScrollBar->SetSkin(m_skin);
			m_vScrollBar->Initialize(device);

			m_hScrollBar = new ScrollBar(Position+Point(1, Size.Y-13), ScrollBar::SCRBAR_Horizontal, Size.X -14);
			m_hScrollBar->setOwner(getOwner());
			m_hScrollBar->SetSkin(m_skin);
			m_hScrollBar->Initialize(device);

		}

		void ListView::Update(const GameTime* const time)
		{
			m_hScrollBar->Update(time);
			m_vScrollBar->Update(time);

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (m_isResizing && mouse->IsLeftReleasedState())
				m_isResizing = false;

			m_selectionArea = getAbsoluteArea();

			if (m_headerStyle != LHSTYLE_None)
			{
				m_selectionArea.Y += m_fontRef->getLineHeightInt();
				m_selectionArea.Height -= m_fontRef->getLineHeightInt();
			}

			if (m_vScrollBar->getMax()>0)
				m_selectionArea.Width -= 12;
			if (m_hScrollBar->getMax()>0)
				m_selectionArea.Height -= 12;

			if (m_hoverRowIndex != -1)
				UpdateSelection();

			if (m_headerStyle == LHSTYLE_Clickable && m_headerHoverIndex != -1 &&
				mouse->IsLeftUp() && !m_columnHeader[m_headerHoverIndex].enentRelease().empty())
			{
				m_columnHeader[m_headerHoverIndex].enentRelease()(this);
				m_headerHoverIndex = -1;
			}
		}

		void ListView::UpdateSelection()
		{
			Mouse* mouse = InputAPIManager::getSingleton().getMouse();

			if (mouse->IsLeftPressed())
			{
				m_selectedRow = m_hoverRowIndex;
				m_selectedColumn = m_hoverColumnIndex;

				if (!m_eSelect.empty())
				{
					if (m_fullRowSelect)
						m_eSelect(m_selectedRow, 0);
					else
						m_eSelect(m_selectedRow, m_selectedColumn);
				}
			}
			m_hoverRowIndex = -1;
			m_hoverColumnIndex = -1;
		}

		void ListView::Draw(Sprite* sprite)
		{
			DrawBorder(sprite);

			{
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

				// draw content
				DrawColumnHeaders(sprite);
				DrawItems(sprite);
				DrawGridLines(sprite);


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

				//if (m_hScrollBar->getMax()>0)
				//m_srcRect.Height = mar
			}
			

			DrawScrollbars(sprite);
		}

		void ListView::DrawBorder(Sprite* sprite)
		{
			m_backArea.X = Position.X;
			m_backArea.Y = Position.Y;
			m_backArea.Width = Size.X;
			m_backArea.Height = Size.Y;

			sprite->Draw(m_skin->WhitePixelTexture, m_backArea,0,CV_Black);

			m_backArea.X++;
			m_backArea.Y++;
			m_backArea.Width -=2;
			m_backArea.Height -=2;
			sprite->Draw(m_skin->WhitePixelTexture, m_backArea, 0, CV_White);
		}
		void ListView::DrawScrollbars(Sprite* sprite)
		{
			int totalWidth = 0;
			if (m_columnHeader.getCount()>0)
			{
				for (int i=0;i<m_columnHeader.getCount();i++)
					totalWidth += m_columnHeader[i].Width;
			}

			if (!m_isResizing)
			{
				if (m_vScrollBar->getMax() ==0)
					m_hScrollBar->setMax(totalWidth - Size.X);
				else
					m_hScrollBar->setMax(totalWidth-Size.X+12);
			}
			m_hScrollBar->setStep(max(1, m_hScrollBar->getMax()/15));

			if (m_hScrollBar->getMax()>0)
			{
				if (m_vScrollBar->getMax()>0 && m_hScrollBar->getWidth() != Size.X - 14)
					m_hScrollBar->setWidth(Size.X - 14);
				else if (m_vScrollBar->getMax() ==0 && m_hScrollBar->getWidth() != Size.X - 2)
					m_hScrollBar->setWidth(Size.X - 2);

				m_hScrollBar->Draw(sprite);
			}

			int rowCount = m_items.getCount();
			int visiCount = GetVisibleItems();
			if (rowCount > visiCount)
				m_vScrollBar->setMax(rowCount-visiCount);
			else
				m_vScrollBar->setMax(0);

			if (m_vScrollBar->getMax()>0)
			{
				if (m_hScrollBar->getMax()>0 && m_vScrollBar->getHeight() != Size.Y -14)
					m_vScrollBar->setHeight(Size.Y - 14);
				else if (m_hScrollBar->getMax() ==0 && m_vScrollBar->getHeight() != Size.Y - 2)
					m_vScrollBar->setHeight(Size.Y - 2);

				m_vScrollBar->Draw(sprite);
			}
		}

		void ListView::DrawColumnHeaders(Sprite* sprite)
		{
			if (m_headerStyle != LHSTYLE_None)
			{
				m_headerArea.X = -m_hScrollBar->getValue();
				m_headerArea.Y = 0;

				Point textPos(0,0);
				int totalWidth = 0;

				for (int i=0;i<m_columnHeader.getCount();i++)
				{
					m_headerArea.Width = m_columnHeader[i].Width;
					m_headerArea.Height = m_fontRef->getLineHeightInt();
					totalWidth += m_headerArea.Width;
					
					UpdateHeaderSize(m_headerArea, i, sprite);

					sprite->Draw(m_skin->WhitePixelTexture, m_headerArea, CV_Black);

					// MouseOver Bevel Effect
					Mouse* mouse = InputAPIManager::getSingleton().getMouse();
					Apoc3D::Math::Rectangle hoverArea(m_headerArea);
					Point pos = GetAbsolutePosition();
					//m_headerArea.X += Position.X;
					//m_headerArea.Y += Position.Y;

					if (m_headerStyle == LHSTYLE_Clickable && 
						m_headerArea.Contains(mouse->GetCurrentPosition()-pos) &&
						UIRoot::getActiveForm() == getOwner())
					{
						m_headerHoverIndex = i;

						if (mouse->IsLeftPressed() && !m_columnHeader[i].eventPress().empty())
						{
							m_columnHeader[i].eventPress()(this);
						}

						if (mouse->IsLeftReleasedState())
						{
							m_headerArea.Width--;
							m_headerArea.Height--;

							sprite->Draw(m_skin->WhitePixelTexture, m_headerArea, 0, CV_DarkGray);

							m_headerArea.X ++;
							m_headerArea.Y ++;
							m_headerArea.Width--;
							m_headerArea.Height--;

							sprite->Draw(m_skin->WhitePixelTexture, m_headerArea, 0, CV_LightGray);

							m_headerArea.X++;
							m_headerArea.Y--;
						}
						else
						{
							// Draw normal header
							m_headerArea.Width--;
							m_headerArea.Height--;
							sprite->Draw(m_skin->WhitePixelTexture, m_headerArea,0, CV_White);

							m_headerArea.X++;
							m_headerArea.Y++;
							m_headerArea.Width--;
							m_headerArea.Height--;
							sprite->Draw(m_skin->WhitePixelTexture, m_headerArea, CV_LightGray);

							m_headerArea.X++;
							m_headerArea.Y--;
						}
					}
					else
					{
						//Draw normal header
						m_headerArea.Width--;
						m_headerArea.Height--;
						sprite->Draw(m_skin->WhitePixelTexture, m_headerArea,0, CV_White);

						m_headerArea.X++;
						m_headerArea.Y++;
						m_headerArea.Width--;
						m_headerArea.Height--;
						sprite->Draw(m_skin->WhitePixelTexture, m_headerArea, CV_LightGray);

						m_headerArea.X++;
						m_headerArea.Y--;
					}




					// ===============
					Point mts = m_fontRef->MeasureString(m_columnHeader[i].Text);

					if (mts.X > m_headerArea.Width - 20)
					{
						//truncate text
						String text = m_columnHeader[i].Text;
						for (size_t j=0;j<Text.size();j++)
						{
							if (m_fontRef->MeasureString(text.substr(0,j)).X > m_headerArea.Width - 20)
							{
								if (j>0)
									text = text.substr(0,j-1) + L"..";
								else
									text = L"";
								break;
							}
						}

						textPos.X = m_headerArea.X + (m_headerArea.Width - m_fontRef->MeasureString(text).X)/2;
						textPos.Y = 0;
						m_fontRef->DrawString(sprite, text, textPos, CV_Black);
					}
					else
					{
						textPos.X = m_headerArea.X + 
							(m_headerArea.Width - mts.X)/2;
						m_fontRef->DrawString(sprite, m_columnHeader[i].Text, textPos, CV_Black);
					}

					UpdateHeaderSize(m_headerArea, i, sprite);

					m_headerArea.X += m_headerArea.Width;
				}

				if (m_hScrollBar->getMax() == 0)
					DrawHeaderEnd(sprite, Size.X - totalWidth);
			}
		}

		void ListView::UpdateHeaderSize(Apoc3D::Math::Rectangle headerArea, int index, Sprite* sprite)
		{
			if (getOwner() == UIRoot::getTopMostForm())
			{
				m_sizeArea.X = m_headerArea.X + m_headerArea.Width - 5;
				m_sizeArea.Y = m_headerArea.Y;
				m_sizeArea.Width = 10;
				m_sizeArea.Height =  m_headerArea.Height;

				Point absP = GetAbsolutePosition();
				//m_sizeArea.X += absP.X;
				//m_sizeArea.Y += absP.Y;
				
				Mouse* mouse = InputAPIManager::getSingleton().getMouse();
				Point mousePos = mouse->GetCurrentPosition();
				mousePos = mousePos - absP;
				if (m_sizeArea.Contains(mousePos))
				{
					if (mouse->IsLeftPressed())
					{
						m_isResizing = true;
						m_resizeIndex = index;
					}
				}

			}
		}
		void ListView::DrawHeaderEnd(Sprite* sprite, int width)
		{
			m_headerArea.Width = width;
			m_headerArea.Height = m_fontRef->getLineHeightInt();
			sprite->Draw(m_skin->WhitePixelTexture, m_headerArea, CV_Black);

			m_headerArea.Width--;
			m_headerArea.Height--;
			sprite->Draw(m_skin->WhitePixelTexture, m_headerArea, CV_White);

			m_headerArea.X++;
			m_headerArea.Y++;
			m_headerArea.Width--;
			m_headerArea.Height--;
			sprite->Draw(m_skin->WhitePixelTexture, m_headerArea, CV_LightGray);
		}

		void ListView::DrawGridLines(Sprite* sprite)
		{
			m_lineRect.X = 0;
			m_lineRect.Y = 0;

			m_gridSize.X = m_columnHeader.getCount()+1;
			m_gridSize.Y = Size.Y / m_fontRef->getLineHeightInt();

			if (m_headerStyle != LHSTYLE_None)
				m_gridSize.Y --;

			m_lineRect.Width = Size.X;
			m_lineRect.Height = 1;

			for (int y=1;y<m_gridSize.Y;y++)
			{
				if (m_headerStyle != LHSTYLE_None)
					m_lineRect.Y = y * m_fontRef->getLineHeightInt() + m_fontRef->getLineHeightInt();
				else
					m_lineRect.Y = y * m_fontRef->getLineHeightInt();

				sprite->Draw(m_skin->WhitePixelTexture, m_lineRect, CV_LightGray);
			}

			m_lineRect.Width = 1;
			m_lineRect.Height = Size.Y;
			if (m_headerStyle != LHSTYLE_None)
				m_lineRect.Y = m_fontRef->getLineHeightInt();
			else
				m_lineRect.Y = 0;

			m_lineRect.X = -1-m_hScrollBar->getValue();
			for (int x=0;x<m_columnHeader.getCount();x++)
			{
				sprite->Draw(m_skin->WhitePixelTexture, m_lineRect, CV_LightGray);
				m_lineRect.X += m_columnHeader[x].Width;
			}

			sprite->Draw(m_skin->WhitePixelTexture, m_lineRect, CV_LightGray);
		}

		void ListView::DrawItems(Sprite* sprite)
		{
			Point textPos(0,0);

			for (int y=0;y<m_items.getCount();y++)
			{
				textPos.X = -m_hScrollBar->getValue() + 4;
				for (int x=0;x<m_items.getWidth();x++)
				{
					if (y > m_vScrollBar->getValue() -1)
					{
						if (m_headerStyle == LHSTYLE_None)
							textPos.Y = (y - m_vScrollBar->getValue()) * m_fontRef->getLineHeightInt();
						else
							textPos.Y = (y - m_vScrollBar->getValue()) * m_fontRef->getLineHeightInt() + m_fontRef->getLineHeightInt();

						if (m_fullRowSelect && x ==0 && y == m_selectedRow)
							DrawSelectedBox(sprite, Point(0, textPos.Y));
						else if (!m_fullRowSelect && x == m_selectedRow && y == m_selectedColumn)
							DrawSelectedBox(sprite, textPos);

						if (!m_isResizing)
							DrawSelectionBox(sprite, textPos, x, y);

						if (m_columnHeader.getCount()==0 || (x<m_columnHeader.getCount() &&
							m_fontRef->MeasureString(m_items.at(y,x)).X > m_columnHeader[x].Width - 15))
						{
							// truncate text
							String text = m_items.at(y,x);
							for (size_t i=0;i<text.size();i++)
								if (m_columnHeader.getCount()==0 ||
									(x<m_columnHeader.getCount() && m_fontRef->MeasureString(text.substr(0,i)).X > m_columnHeader[x].Width-10))
								{
									if (i)
										text = text.substr(0,i-1)+L"..";
									else
										text = L"";
								}

							if (x <m_columnHeader.getCount())
								m_fontRef->DrawString(sprite, text, textPos, CV_Black);
						}
						else if (x<m_columnHeader.getCount())
							m_fontRef->DrawString(sprite, m_items.at(y,x), textPos, CV_Black);
					}

					if (m_columnHeader.getCount()>0 && x<m_columnHeader.getCount())
						textPos.X += m_columnHeader[x].Width;
				}
			}
		}

		void ListView::DrawSelectionBox(Sprite* sprite, const Point& position, int x, int y)
		{
			if (m_fullRowSelect)
				m_selectionRect.Width = Size.X;//m_srcRect.Width + 2;
			else
				m_selectionRect.Width = m_columnHeader[x].Width + 2;

			m_selectionRect.Height = m_fontRef->getLineHeightInt();

			Point absp = GetAbsolutePosition();
			m_selectionRect.X = position.X + absp.X - 4;
			m_selectionRect.Y = position.Y + absp.Y;

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			if (m_selectionArea.Contains(mouse->GetCurrentPosition()) &&
				m_selectionRect.Contains(mouse->GetCurrentPosition()) &&
				UIRoot::getTopMostForm() == getOwner())
			{
				//if (m_fullRowSelect)
				//{
				//	m_hoverRowIndex = y;
				//	m_hoverColumnIndex = x;
				//}
				//else
				//{
				//}

				m_hoverRowIndex = y;
				m_hoverColumnIndex = x;

				if (m_hoverSelection)
				{
					m_selectionRect.X = position.X - 4;
					m_selectionRect.Y = position.Y;
					sprite->Draw(m_skin->WhitePixelTexture, m_selectionRect, CV_LightGray);
				}
			}
		}

		void ListView::DrawSelectedBox(Sprite* sprite, const Point& position)
		{
			if (m_fullRowSelect)
			{
				m_selectionRect.X = 0;
				m_selectionRect.Width = Size.X;
			}
			else
			{
				m_selectionRect.X = position.X - 4;
				m_selectionRect.Width = m_columnHeader[m_selectedRow].Width;
			}
			m_selectionRect.Height = m_fontRef->getLineHeightInt();
			m_selectionRect.Y = position.Y;

			sprite->Draw(m_skin->WhitePixelTexture, m_selectionRect, CV_Silver);
		}

		int ListView::GetVisibleItems()
		{
			int visibleItems = (int)floor((float)(Size.Y / m_fontRef->getLineHeight()));

			if (m_hScrollBar->getMax()>0)
				visibleItems--;
			if (m_headerStyle != LHSTYLE_None)
				visibleItems--;

			return visibleItems;
		}
	}
}