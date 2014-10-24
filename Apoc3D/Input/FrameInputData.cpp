//========= Copyright (c) 2010-2014, Tao Xin, All rights reserved. ============

#include "FrameInputData.h"

#include <time.h>

#include "apoc3d/Math/RandomUtils.h"
#include "apoc3d/Math/Math.h"
#include "apoc3d/IOLib/BinaryReader.h"
#include "apoc3d/IOLib/BinaryWriter.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/GameTime.h"
#include "apoc3d/Utility/Hash.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/Library/lz4hc.h"
#include "apoc3d/Library/lz4.h"
#include "InputAPI.h"
#include "Mouse.h"
#include "Keyboard.h"

using namespace Apoc3D::Math;
using namespace Apoc3D::Utility;

namespace Apoc3D
{
	namespace Input
	{
		const int32 BlockFrameCount = 8192;
		const int32 MaxBufferedFrames = BlockFrameCount*2;
		const int32 DataID = 'PTSR';

		FrameInputData::FrameInputData(uint64 machineID, const String& version)
			: m_frames(MaxBufferedFrames), m_stream(nullptr),
			m_machineID(machineID), m_versionText(version)
		{

		}

		void FrameInputData::SetStream(Stream* strm)
		{
			if (strm->CanWrite())
			{
				m_time = time(0);
				m_sessionID = m_time;
				m_frameCount = 0;
				m_blockCount = 0;
				m_randomizerSeed = Randomizer::getSeed();
				Randomizer::Reset();

				m_stream = strm;

				BinaryWriter bw(strm);
				bw.SuspendStreamRelease();

				bw.WriteInt32(DataID);

				bw.WriteInt32(1);
				bw.WriteString(m_versionText);

				bw.WriteUInt64(m_machineID);
				bw.WriteUInt64(m_sessionID);

				bw.WriteInt32(m_blockCount);
				bw.WriteInt32(m_frameCount); 
				bw.WriteInt32(m_randomizerSeed); 
				bw.WriteInt64(m_time); 
			}
			else
			{
				BinaryReader br(strm);
				br.SuspendStreamRelease();

				int32 id = br.ReadInt32();
				if (id == DataID)
				{
					int32 formatVersion = br.ReadInt32();
					String gameVersion = br.ReadString();

					m_machineID = br.ReadUInt64();
					m_sessionID = br.ReadUInt64();

					assert(gameVersion == m_versionText);
					if (gameVersion == m_versionText)
					{
						m_blockCount = br.ReadInt32();
						m_frameCount = br.ReadInt32();
						m_randomizerSeed = br.ReadInt32();
						m_time = br.ReadInt64();

						Randomizer::setSeed(m_randomizerSeed, true);
						

						m_stream = strm;
					}
					else
					{
						ApocLog(LOG_Game, L"[Record] Data not matching game version. Ignored. " + gameVersion + L" expected " + m_versionText, LOGLVL_Warning);
					}
				}

				m_numBlocksRead = 0;
				m_numFramesRead = 0;
			}
			
		}

		bool FrameInputData::ReadFrame(FrameInfo& frame)
		{
			if (m_stream == nullptr)
				return false;


			if (m_frames.getCount()<BlockFrameCount && 
				m_numBlocksRead<m_blockCount && m_numFramesRead < m_frameCount)
			{
				// read blocks until BlockFrameCount is reached

				BinaryReader br(m_stream);
				br.SuspendStreamRelease();

				while (m_frames.getCount()<BlockFrameCount)
				{
					int32 blockFrameCount = br.ReadInt32();
					int32 compressedDataSize = br.ReadInt32();
					int32 decompressedDataSize = br.ReadInt32();
					int32 crc = br.ReadInt32();

					assert(compressedDataSize<1048576*5); // 5MB

					char* compressedBuffer = new char[compressedDataSize];
					br.ReadBytes(compressedBuffer, compressedDataSize);

					uint32 crc32 = Utility::CalculateCRC32(compressedBuffer, compressedDataSize);

					if (crc32 == crc)
					{
						char* decompressedBuffer = new char[decompressedDataSize];

						int32 ret = LZ4_decompress_safe(compressedBuffer, decompressedBuffer, compressedDataSize, decompressedDataSize);
						assert(ret == decompressedDataSize);

						MemoryStream ms(decompressedBuffer, decompressedDataSize);
						BinaryReader bbr(&ms);
						bbr.SuspendStreamRelease();

						for (int32 i=0;i<blockFrameCount;i++)
						{
							FrameInfo fi;
							fi.Read(&bbr);

							m_frames.Enqueue(fi);
						}
						delete[] decompressedBuffer;
					}
					else
					{
						ApocLog(LOG_Game, L"[Record] Invalid data. CRC32 test failed. ", LOGLVL_Warning);
					}

					delete[] compressedBuffer;

					m_numFramesRead += blockFrameCount;
					m_numBlocksRead ++;
				}
			}

			if (m_frames.getCount()>0)
			{
				frame = m_frames.Dequeue();
				return true;
			}
			return false;
		}
		void FrameInputData::WriteFrame(const FrameInfo& frame)
		{
			assert(m_stream);

			if (m_frames.getCount()>BlockFrameCount)
			{
				// write one full block
				ForceWrite();
			}

			m_frames.Enqueue(frame);
		}

		void FrameInputData::ForceWrite()
		{
			int32 numFramesToWrite = Math::Min(m_frames.getCount(), BlockFrameCount);

			MemoryOutStream buffer(numFramesToWrite*sizeof(FrameInfo));

			BinaryWriter bbw(&buffer);
			bbw.SuspendStreamRelease();

			for (int32 i=0;i<numFramesToWrite;i++)
			{
				FrameInfo frame = m_frames.Dequeue();

				frame.Write(&bbw);
			}

			bbw.Close();

			//////////////////////////////////////////////////////////////////////////

			char* compressedData = new char[LZ4_COMPRESSBOUND((int32)buffer.getLength())];
			int32 compressedSize = LZ4_compressHC(buffer.getDataPointer(), compressedData, (int32)buffer.getLength());

			uint32 crc32 = Utility::CalculateCRC32(compressedData, compressedSize);

			BinaryWriter bw(m_stream);
			bw.SuspendStreamRelease();

			bw.WriteInt32(numFramesToWrite);
			bw.WriteInt32(compressedSize);
			bw.WriteInt32((int32)buffer.getLength());
			bw.WriteInt32(crc32);
			bw.Write(compressedData, compressedSize);

			delete[] compressedData;

			m_frameCount += numFramesToWrite;
			m_blockCount++;
		}
		
		void FrameInputData::WriteHeaderUpdateAndClose()
		{
			assert(m_stream);
			m_stream->setPosition(0);
			BinaryWriter bw(m_stream);
			bw.SuspendStreamRelease();

			bw.WriteInt32(DataID);

			bw.WriteInt32(1);
			bw.WriteString(m_versionText);

			bw.WriteUInt64(m_machineID);
			bw.WriteUInt64(m_sessionID);

			bw.WriteInt32(m_blockCount);
			bw.WriteInt32(m_frameCount); 
			bw.WriteInt32(m_randomizerSeed); 
			bw.WriteInt64(m_time); 
			bw.Close();
		}
		
		void FrameInputData::FrameInfo::Read(BinaryReader* br)
		{
			Index = br->ReadUInt32();

			ElapsedTime = br->ReadSingle();
			ElapsedRealTime = br->ReadSingle();
			TotalTime = br->ReadSingle();
			TotalRealTime = br->ReadSingle();
			FPS = br->ReadSingle();

			MouseStateSize = (byte)br->ReadByte();
			br->ReadBytes(MouseState, MouseStateSize);

			KeyboardStateSize = (byte)br->ReadByte();
			br->ReadBytes(KeyboardState, KeyboardStateSize);
		}
		void FrameInputData::FrameInfo::Write(BinaryWriter* bw)
		{
			bw->WriteUInt32(Index);

			bw->WriteSingle(ElapsedTime);
			bw->WriteSingle(ElapsedRealTime);
			bw->WriteSingle(TotalTime);
			bw->WriteSingle(TotalRealTime);
			bw->WriteSingle(FPS);

			bw->WriteByte((char)MouseStateSize);
			bw->Write(MouseState, MouseStateSize);

			bw->WriteByte((char)KeyboardStateSize);
			bw->Write(KeyboardState, KeyboardStateSize);
		}


		/************************************************************************/
		/*   InputRecorder                                                      */
		/************************************************************************/
		InputRecorder::InputRecorder(uint64 machineID, const String& clientVersion, bool isLiveStream, Stream* outStream)
			: m_content(machineID, clientVersion), m_isLiveStream(isLiveStream), m_outputStream(outStream)
		{

		}

		InputRecorder::InputRecorder(uint64 machineID, const String& clientVersion, bool isLiveStream)
			: m_content(machineID, clientVersion), m_isLiveStream(isLiveStream), m_outputStream(nullptr)
		{
			if (isLiveStream)
			{
				m_outputStream = new PipeOutStream(1048576);
				m_isPipeStream = true;
			}
			else
			{
				time_t tim = time(0);

				tm* tim2 = localtime(&tim);

				String s = L"plays";
				s.append(StringUtils::IntToString(tim2->tm_year + 1900));
				s.append(L"-");
				s.append(StringUtils::IntToString(tim2->tm_mon + 1));
				s.append(L"-");
				s.append(StringUtils::IntToString(tim2->tm_mday));
				s.append(L"-");
				s.append(StringUtils::IntToString(tim2->tm_hour, StrFmt::a<2, '0'>::val));
				s.append(StringUtils::IntToString(tim2->tm_min, StrFmt::a<2, '0'>::val));
				s.append(StringUtils::IntToString(tim2->tm_sec, StrFmt::a<2, '0'>::val));
				s.append(L".dat");

				m_outputStream = new FileOutStream(s);
			}
			m_content.SetStream(m_outputStream);
		}


		InputRecorder::~InputRecorder()
		{
			m_content.FlushWrite();

			if (m_isLiveStream)
			{
				FlushLiveStreamData();
			}
			else
			{
				m_content.WriteHeaderUpdateAndClose();
			}
			
			delete m_outputStream;
		}

		void InputRecorder::Update(const GameTime* time)
		{
			if (!Enabled)
				return;

			FrameInputData::FrameInfo frame;

			frame.Index = m_currentFrame;
			frame.ElapsedTime = time->getElapsedTime();
			frame.ElapsedRealTime = time->getElapsedRealTime();
			frame.FPS = time->getFPS();
			frame.TotalRealTime = time->getTotalRealTime();
			frame.TotalTime = time->getTotalTime();

			frame.MouseStateSize = 0;
			frame.KeyboardStateSize = 0;

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();

			if (mouse)
			{
				MemoryStream ms(frame.MouseState, countof(frame.MouseState));
				BinaryWriter bw(&ms);
				bw.SuspendStreamRelease();
				mouse->Serialize(&bw);

				int64 size = ms.getPosition();
				assert(size<countof(frame.MouseState));
				frame.MouseStateSize = (byte)size;
			}

			if (kb)
			{
				MemoryStream ms(frame.KeyboardState, countof(frame.KeyboardState));
				BinaryWriter bw(&ms);
				bw.SuspendStreamRelease();
				kb->Serialize(&bw);

				int64 size = ms.getPosition();
				assert(size<countof(frame.KeyboardState));
				frame.KeyboardStateSize = (byte)size;
			}

			m_content.WriteFrame(frame);
			m_currentFrame++;

			if (m_isLiveStream)
			{
				FlushLiveStreamData();
			}
		}

		void InputRecorder::FlushLiveStreamData()
		{
			if (m_isPipeStream)
			{
				PipeOutStream* ps = static_cast<PipeOutStream*>(m_outputStream);
				if (ps->getLength() > 0)
				{
					int64 dataLen = (int64)ps->getLength();
					ps->PopAll(nullptr);

					eventStreamData.Invoke(m_content, ps->getDataPointer(), dataLen);
				}
			}
		}
		

		/************************************************************************/
		/*  InputPlayer                                                         */
		/************************************************************************/

		InputPlayer::InputPlayer(const String& file, const String& clientVersion)
			: m_content(0, clientVersion), m_currentFrame(0)
		{
			m_inputStream = new FileStream(file);
			m_content.SetStream(m_inputStream);
		}

		InputPlayer::~InputPlayer()
		{
			delete m_inputStream;
		}
		void InputPlayer::Update(const GameTime* time, GameTime& newTime)
		{
			if (!Enabled)
				return;

			FrameInputData::FrameInfo fi;
			m_content.ReadFrame(fi);

			newTime = GameTime(fi.ElapsedTime, fi.TotalTime, fi.ElapsedRealTime, fi.TotalRealTime, fi.FPS, false);

			Mouse* mouse = InputAPIManager::getSingleton().getMouse();
			Keyboard* kb = InputAPIManager::getSingleton().getKeyboard();

			if (mouse)
			{
				MemoryStream ms(fi.MouseState, fi.MouseStateSize);
				BinaryReader br(&ms);
				br.SuspendStreamRelease();
				mouse->Deserialize(&br);
			}
			
			if (kb)
			{
				MemoryStream ms(fi.KeyboardState, fi.KeyboardStateSize);
				BinaryReader br(&ms);
				br.SuspendStreamRelease();
				kb->Deserialize(&br);
			}
			
			m_currentFrame++;
		}

		bool InputPlayer::isFinished() const
		{
			return m_currentFrame >= m_content.getFrameCount();
		}


	}
}