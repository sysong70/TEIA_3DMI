#pragma once

class Renderer;
class CommandBase;

//--------------------------------------------------------------------------------------------------

class CommandStack
{
	std::map<CString, CommandBase*> m_commands;
	std::stack<CommandBase*> m_stack;
	bool m_bActivated = false;

public:

	CommandStack();

	CommandBase* Find(const CString& name);

	bool Execute(const CString& name, Renderer* pRenderer);

	bool Execute(CommandBase* pCommand, Renderer* pRenderer);
};

extern CommandStack TheCommandStack;
