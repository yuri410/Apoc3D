#include "CommandInterpreter.h"

#include "apoc3d/Core/Logging.h"
#include "apoc3d/Core/ResourceManager.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3d/IOLib/Streams.h"
#include "apoc3d/Vfs/FileSystem.h"
#include "apoc3d/Vfs/ResourceLocation.h"
#include "apoc3d/Vfs/FileLocateRule.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Utility;
using namespace Apoc3D::VFS;

SINGLETON_DECL(Apoc3D::Core::CommandInterpreter);

namespace Apoc3D
{
	namespace Core
	{
		/************************************************************************/
		/* Engine Built-in Commands                                             */
		/************************************************************************/

		class ResourceManagementCommands : public Command
		{
		public:
			class ListCommand : public Command
			{
			public:
				ListCommand()
				{
					m_desc.Name = L"List ResourceManager";
					m_desc.CommandName = L"list";
					m_desc.NumOfParameters = 0;
				}

				virtual void Execute(const Apoc3D::Collections::List<String>& args)
				{
					LogManager::getSingleton().Write(LOG_CommandResponse, L"Listing ResourceManagers...", LOGLVL_Infomation);

					const ResourceManager::ManagerList& list = ResourceManager::getManagerInstances();
					for (int32 i=0;i<list.getCount();i++)
					{
						LogManager::getSingleton().Write(LOG_CommandResponse, L"  " + StringUtils::ToString(i+1) + L". " + list[i]->getName(), LOGLVL_Infomation);
					}
				}
			};

			class ReloadCommand : public Command
			{
			public:
				ReloadCommand()
				{
					m_desc.Name = L"Reload Resources";
					m_desc.CommandName = L"reload";
					m_desc.NumOfParameters = 1;
				}

				virtual void Execute(const Apoc3D::Collections::List<String>& args)
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
			} m_reloadCommand;

			ResourceManagementCommands()
			{
				m_desc.Name = L"Resource Management Command Set";
				m_desc.CommandName = L"res";
				m_desc.NumOfParameters = 1;
				m_desc.SubCommands.Add(new ListCommand());
				m_desc.SubCommands.Add(new ReloadCommand());
			}
			virtual void Execute(const Apoc3D::Collections::List<String>& args)
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
					msg.append(L"MB] [OP = ");
					msg.append(StringUtils::ToString(mgr->GetCurrentOperationCount()));
					msg.append(L"] ");
					if (mgr->usesAsync())
					{
						msg.append(L"[Async]");
					}

					LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);

					msg = L"Managing ";
					msg.append(StringUtils::ToString(mgr->getResourceCount()));
					msg.append(L" resources.");

					LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);
				}
				else LogManager::getSingleton().Write(LOG_CommandResponse, L"No such ordinal.", LOGLVL_Error);
			}
		};

		class ExecCommand : public Command
		{
		public:
			ExecCommand()
			{
				m_desc.Name = L"Execute Script";
				m_desc.CommandName = L"exec";
				m_desc.NumOfParameters = 1;
			}
			virtual void Execute(const Apoc3D::Collections::List<String>& args)
			{
				FileLocation* fl = FileSystem::getSingleton().TryLocate(args[0], FileLocateRule::Default);

				if (fl)
				{
					String msg = L"Executing ";
					msg.append(args[0]);
					msg.append(L" ...");

					LogManager::getSingleton().Write(LOG_CommandResponse, msg, LOGLVL_Infomation);

					Stream* inStrm = fl->GetReadStream();
					char* binaryBuffer = new char[static_cast<int32>(inStrm->getLength())];
					inStrm->Read(binaryBuffer, inStrm->getLength());
					inStrm->Close();
					delete inStrm;
					delete fl;

					istringstream readStrm(binaryBuffer, std::ios::in | std::ios::out | std::ios::binary);
					while (!readStrm.eof())
					{
						std::string line;
						getline(readStrm, line);

						CommandInterpreter::getSingleton().RunLine(StringUtils::toWString(line), false);
					}
					
					delete[] binaryBuffer;
				}
				else LogManager::getSingleton().Write(LOG_CommandResponse, String(L"Script '") + args[0] + String(L"'not found."), LOGLVL_Error);
			}
		};

		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		
		CommandInterpreter::CommandInterpreter()
		{
			RegisterCommand(new ResourceManagementCommands());
			RegisterCommand(new ExecCommand());
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
					if (!m_eCommandSubmited.empty())
					{
						String cmd = args[0];
						StringUtils::ToLowerCase(cmd);

						m_eCommandSubmited(cmd, &args);
					}
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
							if (cmdRec->Cmd->getDescription().NumOfParameters == remainingArgCount)
							{
								List<String> finalArgs(args.getCount() - startingIndex);

								for (int32 i=startingIndex+1;i<args.getCount();i++)
									finalArgs.Add(args[i]);

								cmdRec->Cmd->Execute(finalArgs);

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

		void CommandInterpreter::RegisterCommand(Command* cmd)
		{
			AddCommandTree(cmd, m_rootNode.SubCommands);
		}
		void CommandInterpreter::UnregisterCommand(Command* cmd)
		{
			DestoryCommandTree(cmd, m_rootNode.SubCommands);
		}

		void CommandInterpreter::AddCommandTree(Command* cmd, CommandRecord::SubCommandTable& table)
		{
			const CommandDescription& desc = cmd->getDescription();
			CommandRecord* rec = new CommandRecord();
			rec->Cmd = cmd;

			for (int32 i=0;i<desc.SubCommands.getCount();i++)
			{
				AddCommandTree(desc.SubCommands[i], rec->SubCommands);
			}

			String cmdName = desc.CommandName;
			StringUtils::ToLowerCase(cmdName);
			table.Add(cmdName, rec);
		}
		void CommandInterpreter::DestoryCommandTree(Command* cmd, CommandRecord::SubCommandTable& table)
		{
			const CommandDescription& desc = cmd->getDescription();

			String cmdName = desc.CommandName;
			StringUtils::ToLowerCase(cmdName);

			CommandRecord* rec;
			if (table.TryGetValue(cmdName, rec))
			{
				for (int32 i=0;i<desc.SubCommands.getCount();i++)
				{
					DestoryCommandTree(desc.SubCommands[i], rec->SubCommands);
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
		

	}
}