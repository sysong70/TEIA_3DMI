#pragma once

class Renderer;
class CommandBase;

//--------------------------------------------------------------------------------------------------

struct CommandInfo
{
	CommandBase* pCommand = nullptr;
	UINT Id = 0;
	CString Name;
	CString Step;
};



class CommandStack
{
	std::vector<CommandInfo> m_commands;
	Renderer* m_pRenderer = nullptr;
	bool m_bActivated = false;

public:

	CommandStack();

public: // From IO

	CommandInfo* Find(UINT id);

	CommandInfo* Find(const CString& name);

	bool Execute(UINT id, Renderer* pRenderer);

	bool Execute(const CString& name, Renderer* pRenderer);

	bool Execute(CommandBase* pCommand, Renderer* pRenderer);

public: // From Command

	void Completed();

	void Canceled();
};

extern CommandStack TheCommandStack;
