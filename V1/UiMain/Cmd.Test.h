#pragma once

#include "Cmd.Base.h"

//--------------------------------------------------------------------------------------------------

class CmdTest8 : public CmdBase
{
public:

	CmdTest8() {}

public:

	void Run(WndView* pView) override {}

	bool IsRunOnlyOnce() override { return true; }
};

//--------------------------------------------------------------------------------------------------

class CmdTest9 : public CmdBase
{
public:

	Json::Object Data;

public:

	CmdTest9() {}

public:

	bool ReceiveSignal(Json::Object* pData) override;

	void Run(WndView* pView) override;
};
