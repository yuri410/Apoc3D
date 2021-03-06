#pragma once
#ifndef APOC3D_INPUTFRAMERECORDING_H
#define APOC3D_INPUTFRAMERECORDING_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2010-2017 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "apoc3d/ApocCommon.h"
#include "apoc3d/Collections/Queue.h"
#include "apoc3d/Meta/EventDelegate.h"
#include "apoc3d/Core/AppTime.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::IO;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Input
	{
		class APAPI FrameInputData
		{
		public:
			struct FrameInfo
			{
				uint32 Index;

				Core::AppTime Time;

				byte MouseStateSize;
				char MouseState[128];

				byte KeyboardStateSize;
				char KeyboardState[128];

				void Write(BinaryWriter* bw);
				void Read(BinaryReader* br);
			};

			FrameInputData(uint64 machineID, const String& version);

			void SetStream(Stream* strm);

			bool ReadFrame(FrameInfo& frame);
			void WriteFrame(const FrameInfo& frame);

			void FlushWrite() { ForceWrite(); }
			void WriteHeaderUpdateAndClose();

			int32 getFrameCount() const { return m_frameCount; }

			uint64 getMachineID() const { return m_machineID; }
			uint64 getSessionID() const { return m_sessionID; }

		private:
			void ForceWrite();

			Stream* m_stream;
			Queue<FrameInfo> m_frames;

			String m_versionText;

			uint64 m_machineID;
			uint64 m_sessionID;

			int32 m_randomizerSeed;
			int32 m_frameCount;
			int32 m_blockCount;
			int64 m_time;


			int32 m_numFramesRead;
			int32 m_numBlocksRead;
		};

		class APAPI InputPlayer
		{
		public:
			InputPlayer(const String& file, const String& clientVersion);
			~InputPlayer();

			void Update(const AppTime* time, AppTime& newTime);

			bool isFinished() const;

			bool Enabled = true;
		private:
			FrameInputData m_content;

			int32 m_currentFrame;

			Stream* m_inputStream;
		};

		class APAPI InputRecorder
		{
		public:
			InputRecorder(uint64 machineID, const String& clientVersion, bool isLiveStream, Stream* outStream);
			InputRecorder(uint64 machineID, const String& clientVersion, bool isLiveStream);
			~InputRecorder();

			void Update(const AppTime* time);

			bool Enabled = true;
			EventDelegate<const FrameInputData&, const char*, int64> eventStreamData;
		private:
			void FlushLiveStreamData();

			FrameInputData m_content;

			int32 m_currentFrame = 0;

			Stream* m_outputStream;
			bool m_isLiveStream = false;
			bool m_isPipeStream = false;
		};


	}
}

#endif