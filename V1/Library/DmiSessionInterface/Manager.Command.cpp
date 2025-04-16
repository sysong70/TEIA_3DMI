#include <StdAfx.h>

#include "Manager.Command.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

SESSION::Manager::Command theCommandManager;

using namespace SESSION;

SESSION::Manager::Command::Command()
{

}

SESSION::Manager::Command::~Command()
{

}

Manager::Type SESSION::Manager::Command::Type()
{
	return SESSION::Manager::Type::Command;
}

//== Command 관리 함수 ===============================================================================

// 1. Command를 추가한다.
void SESSION::Manager::Command::AddCommand(SESSION::Command::Root * pcCommand)
{
	m_vCommand.push_back(pcCommand);
}

// 2. Command Vector 크기를 가져온다.
size_t SESSION::Manager::Command::GetCommandSize()
{
	return m_vCommand.size();
}

// 3. Command를 Clear한다.
void SESSION::Manager::Command::ClearCommand()
{
	m_vCommand.clear();
}
