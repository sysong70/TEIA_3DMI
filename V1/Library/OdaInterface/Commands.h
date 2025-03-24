#pragma once

#include "Renderer.h"
#include "Trackers.h"
#include "UserIO.h"

#include "DbBlockTableRecord.h"
#include "DbDatabase.h"
#include "Ed/EdUserIO.h"
#include "Ge/GePoint3d.h"
#include "Gs/Gs.h"

#include <vector>

//--------------------------------------------------------------------------------------------------

class CommandParams
{
public:

	CString Name;
	Renderer* RendererPtr = nullptr;
	OdGsView* GsViewPtr = nullptr;
	OdDbDatabase* DbPtr = nullptr;
	OdDbBlockTableRecordPtr SpacePtr;
	UserIO* IoPtr = nullptr;
	bool Completed = false;

	CommandParams(const CString& name, Renderer* pRenderer);

	~CommandParams();
};

//--------------------------------------------------------------------------------------------------

class CommandBase
{
public:

	static OdDbObjectId GetActiveBlockTableRecord(const OdDbDatabase* pDb);

	static OdDbBlockTableRecordPtr OpenActiveSpace(const OdDbDatabase* pDb);

public:

	CommandBase() {}

	virtual CString Name() { return L"Base"; }

	virtual void Run(Renderer* pRenderer) { DEBUG_STOP; }
};

//--------------------------------------------------------------------------------------------------

#define DECLARE_COMMAND_CLASS(x) class Cmd##x : public CommandBase \
{ \
public: \
	Cmd##x() {}; \
	CString Name() override { return L#x; } \
	void Run(Renderer* pRenderer) override; \
}

DECLARE_COMMAND_CLASS(Undo);
DECLARE_COMMAND_CLASS(Redo);
DECLARE_COMMAND_CLASS(Line);
DECLARE_COMMAND_CLASS(Circle);

#undef DECLARE_COMMAND_CLASS

//--------------------------------------------------------------------------------------------------

struct CommandInfo
{
	CommandBase* CommandPtr = nullptr;
	UINT Id = 0;
	CString Name;
	CString Step;

	CString GetName();
};

//--------------------------------------------------------------------------------------------------

class CommandStack
{
	std::vector<CommandInfo> Commands;
	WStringArray ActiveCommands;
	Renderer* RendererPtr = nullptr;

public:

	CommandStack();

public: // From IO

	bool IsActivated();

	CommandInfo* Find(UINT id);

	CommandInfo* Find(const CString& name);

	bool Execute(UINT id, Renderer* pRenderer);

	bool Execute(const CString& name, Renderer* pRenderer);

	bool Execute(CommandInfo* pInfo, Renderer* pRenderer);

public: // From Command

	void Completed(const CString& name, bool complete);
};



extern CommandStack TheCommandStack;
