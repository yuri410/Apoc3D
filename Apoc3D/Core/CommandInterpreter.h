#ifndef APOC3D_COMMANDINTERPRETER_H
#define APOC3D_COMMANDINTERPRETER_H

#include "apoc3d/Common.h"
#include "apoc3d/Collections/FastList.h"
#include "apoc3d/Collections/FastMap.h"

#include "Singleton.h"

namespace Apoc3D
{
	namespace Core
	{
		struct CommandDescription
		{
			String Name;
			String Description;
			
			String CommandName;

			int32 NumOfParameters;

			Apoc3D::Collections::FastList<Command*> SubCommands;
		};

		class Command
		{
		public:
			virtual void Execute(const Apoc3D::Collections::List<String>& args) = 0;

			const CommandDescription& getDescription() const { return m_desc; }
		protected:
			CommandDescription m_desc;
		};

		typedef fastdelegate::FastDelegate2<String, Apoc3D::Collections::List<String>*> CommandHandler;

		class CommandInterpreter : public Singleton<CommandInterpreter>
		{
		public:
			CommandInterpreter();
			~CommandInterpreter();

			bool RunLine(const String& line, bool triggersEvent = false);

			void RegisterCommand(Command* cmd);
			void UnregisterCommand(Command* cmd);

			CommandHandler& eventCommandSubmited() { return m_eCommandSubmited; }

			SINGLETON_DECL_HEARDER(CommandInterpreter);
		private:
			struct CommandRecord
			{
				typedef Apoc3D::Collections::FastMap<String, CommandRecord*> SubCommandTable;
				Command* Cmd;
				SubCommandTable SubCommands;

				CommandRecord() : Cmd(nullptr) { }
			};

			enum CommandMatchingResult
			{
				CMR_NoSuchCommand,
				CMR_NoSuchArgument,
				CMR_OK
			};

			CommandMatchingResult RunCommand(int32 startingIndex, const CommandRecord::SubCommandTable& table, const Apoc3D::Collections::List<String>& args);

			void AddCommandTree(Command* cmd, CommandRecord::SubCommandTable& table);
			void DestoryCommandTree(Command* cmd, CommandRecord::SubCommandTable& table);

			void DesturctCommandTree(CommandRecord::SubCommandTable& table);


			CommandRecord m_rootNode;
			CommandHandler m_eCommandSubmited;

		};
	}
}

#endif