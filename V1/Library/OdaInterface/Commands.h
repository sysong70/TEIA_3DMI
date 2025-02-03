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

struct CommandParams
{
	Renderer* pRenderer = nullptr;
	OdGsView* pGsView = nullptr;
	OdDbDatabase* pDb = nullptr;
	OdDbBlockTableRecordPtr pSpace;
	UserIO* pio = nullptr;

	CommandParams(Renderer* pRenderer);

	void Cancel();
};

//--------------------------------------------------------------------------------------------------

class CommandBase
{
public:

	static OdDbObjectId GetActiveBlockTableRecord(const OdDbDatabase* pDb);

	static OdDbBlockTableRecordPtr OpenActiveSpace(const OdDbDatabase* pDb);

public:

	CommandBase() {}

	virtual CString Name() { return "Base"; }

	virtual void Run(Renderer* pRenderer) { DEBUG_STOP; }
};

//--------------------------------------------------------------------------------------------------

#define DECLARE_CLASS(x) class x##Command : public CommandBase \
{ \
public: \
	x##Command() {}; \
	CString Name() override { return L#x; } \
	void Run(Renderer* pRenderer) override; \
}

DECLARE_CLASS(Line);
DECLARE_CLASS(Circle);

#undef DECLARE_CLASS
