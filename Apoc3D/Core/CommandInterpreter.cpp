#include "CommandInterpreter.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/ResourceManager.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/FileLocateRule.h"

#include <strstream>
#include <sstream>
#include <vector>

using namespace std;

using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

SINGLETON_DECL(Apoc3D::Core::CommandInterpreter);

namespace Apoc3D
{
	namespace Core
	{
		/************************************************************************/
		/*  Engine Built-in Commands                                            */
		/************************************************************************/

		void ResCommand(CommandArgsConstRef args);
		void ResListCommand(CommandArgsConstRef args);
		void ResReloadCommand(CommandArgsConstRef args);
		void ExecCommand(CommandArgsConstRef args);

		void BatchReportCommand(CommandArgsConstRef args);

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		
		CommandInterpreter::CommandInterpreter()
		{
			{
				CommandDescription desc(L"res", 1, ResCommand, L"Resource Management Command Set", L"");
				desc.SubCommands.PushBack(CommandDescription(L"list", 0, ResListCommand, L"List ResourceManager", L""));
				desc.SubCommands.PushBack(CommandDescription(L"reload", 1, ResReloadCommand, L"Reload Resources", L""));

				RegisterCommand(desc);
			}


			RegisterCommand(CommandDescription(L"exec", 1, ExecCommand, L"Execute Script", L""));

			RegisterCommand(CommandDescription(L"batchreport", 0, BatchReportCommand, L"Report Render Batch Stats", L""));
		}

		CommandInterpreter::~CommandInterpreter()
		{
			DesturctCommandTree(m_rootNode.SubCommands);
		}

		bool CommandInterpreter::RunLine(const String& line, bool triggersEvent)
		{
			List<String> args;

			int lastPos = -1;

			bool isInQuote = false;
			bool isSingleQuote = false;
			
			bool previousIsQuoted = false;

			for (size_t i=0;i<line.size();i++)
			{
				wchar_t ch = line[i];
				if (ch == '\'')
				{
					if (!isInQuote)
					{
						isInQuote = true;
						isSingleQuote = true;
					}
					else if (isSingleQuote)
					{
						isInQuote = false;
						previousIsQuoted = true;
					}
				}
				else if (ch == '"')
				{
					if (!isInQuote)
					{
						isInQuote = true;
						isSingleQuote = false;
					}
					else if (!isSingleQuote)
					{
						isInQuote = false;
						previousIsQuoted = true;
					}
				}
				else if (ch == ' ' && !isInQuote)
				{
					if (i && (int)i-1 != lastPos)
					{
						int32 startBounds = previousIsQuoted ? 2 : 1;
						int32 endbounds = previousIsQuoted ? 3 : 1;

						if (previousIsQuoted)
						{
							previousIsQuoted = false;
						}

						args.Add(line.substr(lastPos+startBounds, i-lastPos-endbounds));
						lastPos = i;
					}
					else
					{
						lastPos = i;
					}

				}
			}
			if (lastPos != static_cast<int32>(line.size())-1)
			{
				int32 startBounds = previousIsQuoted ? 2 : 1;
				int32 endbounds = previousIsQuoted ? 3 : 1;

				args.Add(line.substr(lastPos+startBounds, (int32)line.size()-lastPos-endbounds));

				//args.Add(line.substr(lastPos+1, line.size()-lastPos-1));
			}

			if (args.getCount()>0)
			{
				CommandMatchingResult cmr = RunCommand(0, m_rootNode.SubCommands, args);
				if (cmr == CMR_NoSuchCommand)
				{
					LogManager::getSingleton().Write(LOG_CommandResponse, L"Command not supported.", LOGLVL_Error);

					return false;
				}
				else if (cmr == CMR_NoSuchArgument)
				{
					LogManager::getSingleton().Write(LOG_CommandResponse, L"Invalid amount of parameters.", LOGLVL_Error);

					return false;
				}

				//Console_CommandSubmited(cmd, &args);

				if (triggersEvent)
				{
					String cmd = args[0];
					StringUtils::ToLowerCase(cmd);

					m_eCommandSubmited.Invoke(cmd, &args);
				}
			}
			return true;
		}

		CommandInterpreter::CommandMatchingResult CommandInterpreter::RunCommand(int32 startingIndex, const CommandRecord::SubCommandTable& table, const List<String>& args)
		{
			if (args.getCount()>startingIndex)
			{
				String cmdName = args[startingIndex];
				StringUtils::ToLowerCase(cmdName);

				CommandRecord* cmdRec;
				if (table.TryGetValue(cmdName, cmdRec))
				{
					CommandMatchingResult cmr = RunCommand(startingIndex+1, cmdRec->SubCommands, args);
					if (cmr != CMR_OK)
					{
						if (cmdRec->Cmd)
						{
							int32 remainingArgCount = args.getCount() - startingIndex - 1;
							if (cmdRec->Cmd->NumOfParameters == remainingArgCount)
							{
								List<String> finalArgs(args.getCount() - startingIndex);

								for (int32 i=startingIndex+1;i<args.getCount();i++)
									finalArgs.Add(args[i]);

								cmdRec->Cmd->Handler(finalArgs);

								return CMR_OK;
							}
							return CMR_NoSuchArgument;
						}
					}
					return CMR_OK;
				}
			}
			return CMR_NoSuchCommand;
		}

		void CommandInterpreter::RegisterCommand(const CommandDescription& cmd)
		{
			AddCommandTree(cmd, m_rootNode.SubCommands);
		}
		void CommandInterpreter::UnregisterCommand(const CommandDescription& cmd)
		{
			DestoryCommandTree(cmd, m_rootNode.SubCommands);
		}

		
		void CommandInterpreter::AddCommandTree(const CommandDescription& cmd, CommandRecord::SubCommandTable& table)
		{
			CommandRecord* rec = new CommandRecord();
			rec->Cmd = new CommandDescription(cmd);

			for (LinkedList<CommandDescription>::Iterator iter = cmd.SubCommands.Begin(); iter != cmd.SubCommands.End(); ++iter)
			{
				const CommandDescription& subCmd = *iter;
				AddCommandTree(subCmd, rec->SubCommands);
			}

			String cmdName = cmd.CommandName;
			StringUtils::ToLowerCase(cmdName);
			table.Add(cmdName, rec);
		}
		void CommandInterpreter::DestoryCommandTree(const CommandDescription& cmd, CommandRecord::SubCommandTable& table)
		{
			String cmdName = cmd.CommandName;
			StringUtils::ToLowerCase(cmdName);

			CommandRecord* rec;
			if (table.TryGetValue(cmdName, rec))
			{
				for (LinkedList<CommandDescription>::Iterator iter = cmd.SubCommands.Begin(); iter != cmd.SubCommands.End(); ++iter)
				{
					const CommandDescription& subCmd = *iter;
					DestoryCommandTree(subCmd, rec->SubCommands);
				}

				table.Remove(cmdName);
				delete rec;
			}
		}

		void CommandInterpreter::DesturctCommandTree(CommandRecord::SubCommandTable& table)
		{
			for (CommandRecord::SubCommandTable::Enumerator e = table.GetEnumerator(); e.MoveNext(); )
			{
				CommandRecord* rec = *e.getCurrentValue();
				DesturctCommandTree(rec->SubCommands);
				if (rec->Cmd)
				{
					delete rec->Cmd;
				}

				delete rec;
			}
		}

		/************************************************************************/
		/*  Engine Built-in Commands                                            */
		/************************************************************************/

		void ResCommand(CommandArgsConstRef args)
		{
			int32 index = StringUtils::ParseInt32(args[0])-1;
			const ResourceManager::ManagerList& list = ResourceManager::getManagerInstances();
			if (index >=0 && index < list.getCount())
			{
				ResourceManager* mgr = list[index];
				String msg = mgr->getName();
				msg.append(L"  |  [");
				msg.append(StringUtils::ToString(mgr->getUsedCacheSize() / 1048576.0f, 1, 0, ' '));
				msg.append(L"MB / ");
				msg.append(StringUtils::ToString(mgr->getTotalCacheSize() / 1048576.0f, 1, 0, ' '));
				msg.append(L"MB]");
				if (mgr->usesAsync())
				{
					msg.append(L" [OP = ");
					msg.append(StringUtils::ToString(mgr->GetCurrentOperationCount()));
					msg.append(L"] [Async]");
				}

				LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);

				msg = L"Managing ";
				msg.append(StringUtils::ToString(mgr->getResourceCount()));
				msg.append(L" resources.");

				LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);
			}
			else LogManager::getSingleton().Write(LOG_CommandResponse, L"No such ordinal.", LOGLVL_Error);
		}
		void ResListCommand(CommandArgsConstRef args)
		{
			LogManager::getSingleton().Write(LOG_CommandResponse, L"Listing ResourceManagers...", LOGLVL_Infomation);

			const ResourceManager::ManagerList& list = ResourceManager::getManagerInstances();
			for (int32 i=0;i<list.getCount();i++)
			{
				LogManager::getSingleton().Write(LOG_CommandResponse, L"  " + StringUtils::ToString(i+1) + L". " + list[i]->getName(), LOGLVL_Infomation);
			}
		}
		void ResReloadCommand(CommandArgsConstRef args)
		{
			int32 index = StringUtils::ParseInt32(args[0])-1;
			const ResourceManager::ManagerList& list = ResourceManager::getManagerInstances();
			if (index >=0 && index < list.getCount())
			{
				LogManager::getSingleton().Write(LOG_CommandResponse, L"Reloading " + list[index]->getName() + L" ...", LOGLVL_Infomation);

				list[index]->ReloadAll();
			}
			else LogManager::getSingleton().Write(LOG_CommandResponse, L"No such ordinal.", LOGLVL_Error);	
		}

		void ExecCommand(CommandArgsConstRef args)
		{
			FileLocation* fl = FileSystem::getSingleton().TryLocate(args[0], FileLocateRule::Default);

			if (fl)
			{
				String msg = L"Executing ";
				msg.append(args[0]);
				msg.append(L" ...");

				LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);

				Stream* inStrm = fl->GetReadStream();
				int32 streamLength = static_cast<int32>(inStrm->getLength());
				char* binaryBuffer = new char[streamLength+1];
				inStrm->Read(binaryBuffer, inStrm->getLength());
				inStrm->Close();
				delete inStrm;
				delete fl;

				binaryBuffer[streamLength] = 0;

				istringstream readStrm(binaryBuffer, std::ios::in);
				readStrm.exceptions(std::ios::failbit);


				std::vector<char> buffer = std::vector<char>( std::istreambuf_iterator<char>(readStrm), std::istreambuf_iterator<char>( ) );
				buffer.push_back('\0');
				delete[] binaryBuffer;

				const char* allContent = &buffer[0];
				String allContentStr = StringUtils::toWString(allContent);
				List<String> lines(50);
				StringUtils::Split(allContentStr, lines, L"\n\r");

				for (int32 i=0;i<lines.getCount();i++)
				{
					String& lineW = lines[i];
					StringUtils::Trim(lineW);

					CommandInterpreter::getSingleton().RunLine(lineW, false);
				}

			}
			else LogManager::getSingleton().Write(LOG_CommandResponse, String(L"Script '") + args[0] + String(L"'not found."), LOGLVL_Error);
		}

		void BatchReportCommand(CommandArgsConstRef args)
		{
			Apoc3D::Graphics::RenderSystem::RenderDevice::HasBatchReportRequest = true;
		}
		
	}
}