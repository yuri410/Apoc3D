#include "WorkerPanel.h"
#include "apoc3d/UILib/StyleSkin.h"
#include "apoc3d/UILib/Button.h"
#include "apoc3d/UILib/FontManager.h"
#include "apoc3d/Graphics/RenderSystem/Sprite.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Math/MathCommon.h"

using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace UI
	{
		const int32 Margin = 5;

		WorkerPanel::WorkerPanel(const StyleSkin* skin, IWorkerAdapter* worker)
			: m_skin(skin)
			, m_worker(worker)
		{
			m_prog = new ProgressBar(skin, Point(0, 0), 150);
			m_prog->Visible = false;

			m_ctrlBtn = new Button(skin, Point(0, 0), Point(100, 20), L"Start");
			m_ctrlBtn->eventRelease += { this, &WorkerPanel::CtrlBtn_Released };
		}
		WorkerPanel::~WorkerPanel()
		{
			DELETE_AND_NULL(m_prog);
		}

		static void DrawString(Sprite* spr, const String& txt, const Point& pos, const StyleSkin* skin)
		{
			skin->ButtonFont->DrawString(spr, txt, pos + Point(1, 1), CV_Black);
			skin->ButtonFont->DrawString(spr, txt, pos, skin->TextColor);
		}

		void WorkerPanel::Draw(Sprite* spr)
		{
			static int32 LineHeight = m_skin->ButtonFont->getLineHeightInt();

			Rectangle area = getAbsoluteArea();

			spr->DrawRoundedRect(SystemUI::GetWhitePixel(), area, nullptr, 5, 2, m_skin->ControlFaceColor);

			Point basePt = area.getPosition() + Point(Margin, Margin);

			DrawString(spr, m_name, basePt, m_skin);

			m_ctrlBtn->Draw(spr);

			if (m_prog->Visible)
			{
				m_prog->Draw(spr);
			}
		}
		void WorkerPanel::Update(const AppTime* time)
		{
			m_prog->BaseOffset = BaseOffset;
			m_ctrlBtn->BaseOffset = BaseOffset;

			m_prog->IsInteractive = IsInteractive;
			m_ctrlBtn->IsInteractive = IsInteractive;

			m_prog->Update(time);
			m_ctrlBtn->Update(time);
		}

		void WorkerPanel::SetArea(const Rectangle& area)
		{
			WorkerAdapterStatus status = m_worker->WA_GetStatus();

			Position = area.getPosition();
			m_size = area.getSize();

			m_name = StringUtils::toPlatformWideString(status.m_name);

			m_prog->Position = Position + Point(Margin, Margin + 25);
			m_prog->setWidth(m_size.X - Margin*2);
			m_prog->Visible = status.m_isRunning;
			m_prog->Text = StringUtils::toASCIIWideString(fmt::format("{0} - [{1}/{2}]", status.m_currentItem, status.m_currentCount, status.m_totalCount));
			m_prog->CurrentValue = status.m_totalCount > 0 ? Math::Saturate((float)status.m_currentCount / status.m_totalCount) : 0;

			m_ctrlBtn->Position = area.getTopRight() - Point(m_ctrlBtn->getWidth() + Margin, -Margin - 2);

			m_ctrlBtn->Enabled = !status.m_isStopping;
			if (m_ctrlBtn->Enabled)
				m_ctrlBtn->SetText(status.m_isRunning ? L"Stop" : L"Start");
			else
				m_ctrlBtn->SetText(L"Stopping...");
		}

		void WorkerPanel::CtrlBtn_Released(Button* btn)
		{
			WorkerAdapterStatus status = m_worker->WA_GetStatus();

			if (!status.m_isRunning)
			{
				m_worker->WA_StartBackground();
			}
			else
			{
				m_worker->WA_StopBackgroundAsync();
			}
			
			btn->Enabled = false;
		}
	}
}