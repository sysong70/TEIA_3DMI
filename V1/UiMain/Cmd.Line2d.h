#pragma once

#include "Cmd.Base.h"

//--------------------------------------------------------------------------------------------------

class CmdLine2dTwoPoints : public CmdBase
{
public:

	CmdLine2dTwoPoints() {}

public:

	bool ReceiveSignal(Json::Object* pData) override { RETURN_FALSE; }

	void Run(WndView* pView) override;
};

//--------------------------------------------------------------------------------------------------

class CmdLine2dPolyline : public CmdBase
{
public:

	CmdLine2dPolyline() {}

public:

	bool ReceiveSignal(Json::Object* pData) override { RETURN_FALSE; }

	void Run(WndView* pView) override;
};

//--------------------------------------------------------------------------------------------------

class CmdLine2dPolygon : public CmdBase
{
public:

	CmdLine2dPolygon() {}

public:

	bool ReceiveSignal(Json::Object* pData) override { RETURN_FALSE; }

	void Run(WndView* pView) override;
};

//--------------------------------------------------------------------------------------------------

class CmdLine2dRectangle : public CmdBase
{
public:

	CmdLine2dRectangle() {}

public:

	bool ReceiveSignal(Json::Object* pData) override { RETURN_FALSE; }

	void Run(WndView* pView) override;
};
