#pragma once

#include "UICommon.h"
#include "apoc3d/UILib/UICommon.h"
#include "apoc3d/UILib/Control.h"
#include "apoc3d/UILib/Bar.h"

namespace Apoc3D
{
	namespace UI
	{
		struct WorkerAdapterStatus
		{
			std::string m_name;
			std::string m_currentItem;

			int32 m_currentCount = 0;
			int32 m_totalCount = 0;

			bool m_isRunning = false;
			bool m_isStopping = false;
		};

		class IWorkerAdapter
		{
			RTTI_BASE;
		public:
			virtual WorkerAdapterStatus WA_GetStatus() = 0;
			virtual void WA_StartBackground() = 0;
			virtual void WA_StopBackgroundAsync() = 0;

			virtual ~IWorkerAdapter() { }

		};

		class WorkerPanel : public Control
		{
		public:
			static const int32 PanelHeight = 50;

			WorkerPanel(const StyleSkin* skin, IWorkerAdapter* worker);
			~WorkerPanel();

			void Draw(Sprite* spr) override;
			void Update(const AppTime* time) override;
			
			void SetArea(const Rectangle& area);

			IWorkerAdapter* GetWorker() const { return m_worker; }

		private:

			void CtrlBtn_Released(Button* btn);

			const StyleSkin* m_skin;
			IWorkerAdapter* m_worker;

			ProgressBar* m_prog;
			Button* m_ctrlBtn;

			String m_name;
		};
	}
}