#pragma once

#include "Apoc3D.Essentials/EssentialCommon.h"

namespace Apoc3D
{
	namespace UI
	{
		struct ChartDataPoint
		{
			double m_x = 0;
			double m_y = 0;
			double m_y_low = 0;
			double m_y_high = 0;
			double m_y_open = 0;
		};

		enum struct LayoutDirection
		{
			Horizontal,
			Vertical
		};

		enum struct LayoutLocation
		{
			Top,
			Left
		};

		enum struct LoadingState
		{
			Unloaded,
			Loading,
			Loaded
		};

		struct LoadingStatus
		{
			LoadingState m_state = LoadingState::Loaded;
			float m_progress = 0;

			LoadingStatus() { }
			LoadingStatus(LoadingState state, float prg) : m_state(state), m_progress(prg) { }
		};

		class AnimatedIconInstance
		{
		public:
			AnimatedIconInstance() { }
			AnimatedIconInstance(int32 frameCount, float playbackRate)
				: m_frameCount(frameCount), m_playbackRate(playbackRate) { }

			void Advance();

			int32 getFrameCount() const { return (int32)m_frameIndex; }
			float getPlaybackRate() const { return m_playbackRate; }

		private:
			float m_frameIndex = 0;
			int32 m_frameCount = 0;
			float m_playbackRate = 1;
		};

	}
}
