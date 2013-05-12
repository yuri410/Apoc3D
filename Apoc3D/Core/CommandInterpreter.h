#pragma once

#ifndef APOC3D_COMMANDINTERPRETER_H
#define APOC3D_COMMANDINTERPRETER_H

/**
 * -----------------------------------------------------------------------------
 * This source file is part of Apoc3D Engine
 * 
 * Copyright (c) 2009+ Tao Xin
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
 * http://www.gnu.org/copyleft/gpl.txt.
 * 
 * -----------------------------------------------------------------------------
 */

#include "apoc3d/Collections/LinkedList.h"
#include "apoc3d/Collections/List.h"
#include "apoc3d/Collections/HashMap.h"

#include "Singleton.h"

namespace Apoc3D
{
	namespace Core
	{
		typedef const Apoc3D::Collections::List<String>& CommandArgsConstRef;
		typedef fastdelegate::FastDelegate1<CommandArgsConstRef> CommandHandler;


		struct CommandDescription
		{
			String Name;
			String Description;

			String CommandName;

			int32 NumOfParameters;
			CommandHandler Handler;

			Apoc3D::Collections::LinkedList<CommandDescription> SubCommands;

			CommandDescription()
				: NumOfParameters(0)
			{ }
			CommandDescription(const String& cmdName, int paramCount, const CommandHandler& handler)
				: Name(cmdName), CommandName(cmdName), NumOfParameters(paramCount), Handler(handler)
			{ }
			CommandDescription(const String& command, int paramCount, const CommandHandler& handler, const String& name, const String& description)
				: Name(name), Description(description), CommandName(command), NumOfParameters(paramCount), Handler(handler)
			{ }
		};

		typedef fastdelegate::FastDelegate2<String, Apoc3D::Collections::List<String>*> RawCommandHandler;
		class APAPI CommandInterpreter : public Singleton<CommandInterpreter>
		{
		public:
			CommandInterpreter();
			~CommandInterpreter();

			bool RunLine(const String& line, bool triggersEvent = false);

			void RegisterCommand(const CommandDescription& cmd);
			void UnregisterCommand(const CommandDescription& cmd);

			RawCommandHandler& eventCommandSubmited() { return m_eCommandSubmited; }

			SINGLETON_DECL_HEARDER(CommandInterpreter);
		private:


			struct CommandRecord
			{
				typedef Apoc3D::Collections::HashMap<String, CommandRecord*> SubCommandTable;
				CommandDescription* Cmd;
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

			void AddCommandTree(const CommandDescription& cmd, CommandRecord::SubCommandTable& table);
			void DestoryCommandTree(const CommandDescription& cmd, CommandRecord::SubCommandTable& table);

			void DesturctCommandTree(CommandRecord::SubCommandTable& table);


			CommandRecord m_rootNode;
			RawCommandHandler m_eCommandSubmited;
		};
	}
}

#endif