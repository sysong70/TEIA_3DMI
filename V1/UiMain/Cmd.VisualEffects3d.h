#pragma once

#include "Cmd.Base.h"

//--------------------------------------------------------------------------------------------------

class CmdVisualEffects3d : public CmdBase
{
public:

	CmdVisualEffects3d() {}

public:

	bool ReceiveSignal(Json::Object* pData) override;

	void Run(WndView* pView) override;
};
