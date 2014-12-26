#include "CommandInterpreter.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/ResourceManager.h"
#include "apoc3d/Graphics/RenderSystem/RenderDevice.h"
#include "apoc3d/Graphics/EffectSystem/EffectManager.h"
#include "apoc3d/Graphics/EffectSystem/Effect.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/IOLib/TextData.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/FileLocateRule.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Graphics::EffectSystem;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

namespace Apoc3D
{
	namespace Core
	{
		SINGLETON_IMPL(CommandInterpreter);

		/************************************************************************/
		/*  Engine Built-in Commands                                            */
		/************************************************************************/

		void ResCommand(CommandArgsConstRef args);
		void ResListCommand(CommandArgsConstRef args);
		void ResReloadCommand(CommandArgsConstRef args);
		void ExecCommand(CommandArgsConstRef args);

		void FxListCommand(CommandArgsConstRef args);
		void FxReloadCommand(CommandArgsConstRef args);

		void BatchReportCommand(CommandArgsConstRef args);

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		
		CommandInterpreter::CommandInterpreter()
		{
			RegisterCommand(
			{
				L"res", 1, ResCommand, L"Resource Management Command Set",
				{
					{ L"list", 0, ResListCommand, L"List ResourceManager" },
					{ L"reload", 1, ResReloadCommand, L"Reload Resources" }
				}
			});


			RegisterCommand(
			{
				L"fx", 0, nullptr, L"Effect Management Command Set",
				{
					{ L"list", 0, FxListCommand, L"List Effects" },
					{ L"reload", 0, FxReloadCommand, L"Reload All Automatic Effects" }
				}
			});


			RegisterCommand({ L"exec", 1, ExecCommand, L"Execute Script" });

			RegisterCommand({ L"batchreport", 0, BatchReportCommand, L"Report Render Batch Stats" });
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

			for (size_t i = 0; i < line.size(); i++)
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
					if (i && (int)i - 1 != lastPos)
					{
						int32 startBounds = previousIsQuoted ? 2 : 1;
						int32 endbounds = previousIsQuoted ? 3 : 1;

						if (previousIsQuoted)
						{
							previousIsQuoted = false;
						}

						args.Add(line.substr(lastPos + startBounds, i - lastPos - endbounds));
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

			if (args.getCount() > 0)
			{
				CommandMatchingResult cmr = RunCommand(0, m_rootNode.SubCommands, args);
				if (cmr == CommandMatchingResult::NoSuchCommand)
				{
					LogManager::getSingleton().Write(LOG_CommandResponse, L"Command not supported.", LOGLVL_Error);

					return false;
				}
				else if (cmr == CommandMatchingResult::NoSuchArgument)
				{
					LogManager::getSingleton().Write(LOG_CommandResponse, L"Invalid amount of parameters.", LOGLVL_Error);

					return false;
				}
				else if (cmr == CommandMatchingResult::SubcommandNeeded)
				{
					LogManager::getSingleton().Write(LOG_CommandResponse, L"Sub command needed.", LOGLVL_Error);
				}


				if (triggersEvent)
				{
					String cmd = args[0];
					StringUtils::ToLowerCase(cmd);

					m_eCommandSubmited.Invoke(cmd, &args);
				}
			}
			return true;
		}

		void PrintSubCommands(CommandDescription* cmd)
		{
			LogManager::getSingleton().Write(LOG_CommandResponse, L"Sub commands:", LOGLVL_Infomation);

			for (CommandDescription& cd : cmd->SubCommands)
			{
				String msg = L"  " + cd.CommandName + L"(" + StringUtils::IntToString(cd.NumOfParameters) + L"): " + cd.Name;

				LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);
			}
		}

		CommandInterpreter::CommandMatchingResult CommandInterpreter::RunCommand(int32 startingIndex, const CommandRecord::SubCommandTable& table, const List<String>& args)
		{
			if (args.getCount() > startingIndex)
			{
				String cmdName = args[startingIndex];
				StringUtils::ToLowerCase(cmdName);

				CommandRecord* cmdRec;
				if (table.TryGetValue(cmdName, cmdRec))
				{
					CommandMatchingResult cmr = RunCommand(startingIndex + 1, cmdRec->SubCommands, args);
					if (cmr != CommandMatchingResult::OK)
					{
						if (cmdRec->Cmd)
						{
							int32 remainingArgCount = args.getCount() - startingIndex - 1;
							if (cmdRec->Cmd->NumOfParameters == remainingArgCount)
							{
								if (cmdRec->Cmd->Handler.empty() && cmdRec->Cmd->SubCommands.getCount() > 0)
								{
									PrintSubCommands(cmdRec->Cmd);
									return CommandMatchingResult::SubcommandNeeded;
								}

								List<String> finalArgs(args.getCount() - startingIndex);

								for (int32 i = startingIndex + 1; i < args.getCount(); i++)
									finalArgs.Add(args[i]);

								cmdRec->Cmd->Handler(finalArgs);

								return CommandMatchingResult::OK;
							}
							return CommandMatchingResult::NoSuchArgument;
						}
					}
					return CommandMatchingResult::OK;
				}
			}
			return CommandMatchingResult::NoSuchCommand;
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
			
			for (const CommandDescription& subCmd : cmd.SubCommands)
			{
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
				for (const CommandDescription& subCmd : cmd.SubCommands)
				{
					DestoryCommandTree(subCmd, rec->SubCommands);
				}

				table.Remove(cmdName);
				delete rec;
			}
		}

		void CommandInterpreter::DesturctCommandTree(CommandRecord::SubCommandTable& table)
		{
			for (CommandRecord* rec : table.getValueAccessor())
			{
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
			int32 index = StringUtils::ParseInt32(args[0]) - 1;
			const ResourceManager::ManagerList& list = ResourceManager::getManagerInstances();
			if (index >= 0 && index < list.getCount())
			{
				ResourceManager* mgr = list[index];
				String msg = mgr->getName();
				msg.append(L"  |  ");
				if (mgr->usesAsync())
				{
					msg.append(L"[");
					msg.append(StringUtils::SingleToString(mgr->getUsedCacheSize() / 1048576.0f, StrFmt::fpdec<1>::val));
					msg.append(L"MB / ");
					msg.append(StringUtils::SingleToString(mgr->getTotalCacheSize() / 1048576.0f, StrFmt::fpdec<1>::val));
					msg.append(L"MB]");

					msg.append(L" [OP = ");
					msg.append(StringUtils::IntToString(mgr->GetCurrentOperationCount()));
					msg.append(L"] [Async]");
				}
				else
				{
					int64 sz = mgr->CalculateTotalResourceSize();
					msg.append(L"[Used = ");
					msg.append(StringUtils::SingleToString(sz / 1048576.0f, StrFmt::fpdec<1>::val));
					msg.append(L"MB]");
				}

				LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);

				msg = L"Managing ";
				msg.append(StringUtils::IntToString(mgr->getResourceCount()));
				msg.append(L" resources.");

				LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);
			}
			else LogManager::getSingleton().Write(LOG_CommandResponse, L"No such ordinal.", LOGLVL_Error);
		}
		void ResListCommand(CommandArgsConstRef args)
		{
			LogManager::getSingleton().Write(LOG_CommandResponse, L"Listing ResourceManagers...", LOGLVL_Infomation);

			const ResourceManager::ManagerList& list = ResourceManager::getManagerInstances();
			for (int32 i = 0; i < list.getCount(); i++)
			{
				LogManager::getSingleton().Write(LOG_CommandResponse, L"  " + StringUtils::IntToString(i + 1) + L". " + list[i]->getName(), LOGLVL_Infomation);
			}
		}
		void ResReloadCommand(CommandArgsConstRef args)
		{
			int32 index = StringUtils::ParseInt32(args[0]) - 1;
			const ResourceManager::ManagerList& list = ResourceManager::getManagerInstances();
			if (index >= 0 && index < list.getCount())
			{
				LogManager::getSingleton().Write(LOG_CommandResponse, L"Reloading " + list[index]->getName() + L" ...", LOGLVL_Infomation);

				list[index]->ReloadAll();
			}
			else LogManager::getSingleton().Write(LOG_CommandResponse, L"No such ordinal.", LOGLVL_Error);
		}

		void ExecCommand(CommandArgsConstRef args)
		{
			FileLocation fl;
			
			if (FileSystem::getSingleton().TryLocate(args[0], FileLocateRule::Default, fl))
			{
				String msg = L"Executing ";
				msg.append(args[0]);
				msg.append(L" ...");

				LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);

				String code = IO::Encoding::ReadAllText(fl, IO::Encoding::TEC_Unknown);

				List<String> lines(50);
				StringUtils::Split(code, lines, L"\n\r");

				for (int32 i = 0; i < lines.getCount(); i++)
				{
					String& lineW = lines[i];
					StringUtils::Trim(lineW);

					CommandInterpreter::getSingleton().RunLine(lineW, false);
				}
			}
			else LogManager::getSingleton().Write(LOG_CommandResponse, String(L"Script '") + args[0] + String(L"'not found."), LOGLVL_Error);
		}


		void FxListCommand(CommandArgsConstRef args)
		{
			List<Effect*> fxList;
			EffectManager::getSingleton().FillEffects(fxList);

			LogManager::getSingleton().Write(LOG_CommandResponse, L"Effect Listing:", LOGLVL_Infomation);

			int32 id = 1;
			for (Effect* fx : fxList)
			{
				AutomaticEffect* ae = up_cast<AutomaticEffect*>(fx);

				String msg = L"  " + StringUtils::IntToString(id) + L". ";
				msg.append(ae ? L"[Auto] " : L"[Custom] ");
				msg.append(fx->getName());

				if (fx->IsUnsupported())
					msg.append(L" [Unsupported] ");

				if (fx->SupportsInstancing())
					msg.append(L" [Instancing] ");

				LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);

				id++;
			}
		}
		void FxReloadCommand(CommandArgsConstRef args)
		{
			EffectManager::getSingleton().ReloadAutomaticEffects();

			List<Effect*> fxList;
			EffectManager::getSingleton().FillEffects(fxList);
			int32 counter = 0;
			for (Effect* fx : fxList)
			{
				AutomaticEffect* ae = up_cast<AutomaticEffect*>(fx);
				if (ae) counter++;
			}

			LogManager::getSingleton().Write(LOG_CommandResponse, StringUtils::IntToString(counter) +  L" automatic effects reloaded.", LOGLVL_Infomation);
		}

		void BatchReportCommand(CommandArgsConstRef args)
		{
			Apoc3D::Graphics::RenderSystem::RenderDevice::HasBatchReportRequest = true;
		}
		
	}
}