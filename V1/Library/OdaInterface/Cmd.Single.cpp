#include "stdafx.h"

#include "Commands.h"

//**************************************************************************************************

void CmdUndo::Run(Renderer* pRenderer)
{
	if (pRenderer->GetDatabase()->hasUndoMark()) {
		pRenderer->GetDatabase()->undoBack();
	}

	TheCommandStack.Completed(Name(), true);
}

//**************************************************************************************************

void CmdRedo::Run(Renderer* pRenderer)
{
	if (pRenderer->GetDatabase()->hasRedo()) {
		pRenderer->GetDatabase()->redo();
	}

	TheCommandStack.Completed(Name(), true);
}
