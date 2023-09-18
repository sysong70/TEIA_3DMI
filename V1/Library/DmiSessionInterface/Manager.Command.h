#pragma once

#include "Manager.Root.h"

#include <Json.h>
#include <vector>

namespace SESSION
{
	namespace Command {
		class Root;
	}

	namespace Manager
	{
		class Command : public Root
		{
		public:
			Command();
			~Command();

			Manager::Type Type() override;

			//== Command 관리 함수 ===================================================================
			void AddCommand(SESSION::Command::Root * pCommand);
			int GetCommandSize();
			void ClearCommand();

		private:
			// Vector를 이용해서 Command를 관리하자.
			// 순회를 자유롭게 할 수 있어서 좋다.
			std::vector<SESSION::Command::Root *> m_vCommand;
		};
	}
}

extern SESSION::Manager::Command theCommandManager;