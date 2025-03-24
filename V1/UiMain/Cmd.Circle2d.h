#pragma once

#include "Cmd.Base.h"

//--------------------------------------------------------------------------------------------------

class CmdCircle2dTwoPoints : public CmdBase
{
public:

	CmdCircle2dTwoPoints() {}

public:

	bool ReceiveSignal(Json::Object* pData) override { RETURN_FALSE; }

	void Run(WndView* pView) override;
};

//--------------------------------------------------------------------------------------------------

class CmdCircle2dThreePoints : public CmdBase
{
public:

	CmdCircle2dThreePoints() {}

public:

	bool ReceiveSignal(Json::Object* pData) override { RETURN_FALSE; }

	void Run(WndView* pView) override;
};

//--------------------------------------------------------------------------------------------------

class CmdCircle2dCenterRadius : public CmdBase
{
public:

	CmdCircle2dCenterRadius() {}

public:

	bool ReceiveSignal(Json::Object* pData) override { RETURN_FALSE; }

	void Run(WndView* pView) override;
};

//--------------------------------------------------------------------------------------------------

class CmdCircle2dTwoTangentsRadius : public CmdBase
{
public:

	CmdCircle2dTwoTangentsRadius() {}

public:

	bool ReceiveSignal(Json::Object* pData) override { RETURN_FALSE; }

	void Run(WndView* pView) override;
};
