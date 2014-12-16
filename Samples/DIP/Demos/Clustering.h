#pragma once

#include "../SubDemo.h"

namespace dip
{
	class DemoClustering : public SubDemo
	{
	public:
		static const int DMCount = 8;

		DemoClustering(DIP1* parent, RenderDevice* device, const StyleSkin* skin);
		~DemoClustering();

		virtual void Show() override;

	private:
		void WorkDM_Process(Button* ctrl);

		CheckBox* m_dmInitCluster[8];
		Texture* m_dmOriginals[8];
		Texture* m_dmSpecturm[8];
		List<PictureBox*> m_resultDisplays;
		PictureBox* m_dmPicBoxSpecturm[9];
		Form* m_frmDM;
		const StyleSkin* m_skin;
	};
}