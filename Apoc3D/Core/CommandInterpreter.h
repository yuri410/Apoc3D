#ifndef APOC3D_COMMANDINTERPRETER_H
#define APOC3D_COMMANDINTERPRETER_H

#include "apoc3d/Common.h"
#include "apoc3d/Collections/FastList.h"

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

			uint64 NumOfParameterSet;

			Apoc3D::Collections::FastList<CommandDescription*> SubCommands;
		};

		class Command
		{
		public:

			virtual void Execute(const String& cmd, const Apoc3D::Collections::List<String>& args);
		private:
		};

		class CommandInterpreter : public Singleton<CommandInterpreter>
		{
		public:
			CommandInterpreter();
			~CommandInterpreter();

			void RunLine(const String& line);

			SINGLETON_DECL_HEARDER(CommandInterpreter);
		private:

			
		};
	}
}

#endif