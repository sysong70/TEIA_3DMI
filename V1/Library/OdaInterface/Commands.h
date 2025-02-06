#pragma once

#include "Renderer.h"
#include "Trackers.h"
#include "UserIO.h"

#include "DbBlockTableRecord.h"
#include "DbDatabase.h"
#include "Ed/EdUserIO.h"
#include "Ge/GePoint3d.h"
#include "Gs/Gs.h"

//--------------------------------------------------------------------------------------------------

class CommandParams
{
public:

	Renderer* pRenderer = nullptr;
	OdGsView* pGsView = nullptr;
	OdDbDatabase* pDb = nullptr;
	OdDbBlockTableRecordPtr pSpace;
	UserIO* pIo = nullptr;
	bool Completed = false;

	CommandParams(Renderer* pRenderer);

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

#define DECLARE_COMMAND_CLASS(x) class x##Command : public CommandBase \
{ \
public: \
	x##Command() {}; \
	CString Name() override { return L#x; } \
	void Run(Renderer* pRenderer) override; \
}

DECLARE_COMMAND_CLASS(Undo);
DECLARE_COMMAND_CLASS(Redo);
DECLARE_COMMAND_CLASS(Line);
DECLARE_COMMAND_CLASS(Circle);

#undef DECLARE_COMMAND_CLASS
