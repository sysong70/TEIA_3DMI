#include "stdafx.h"
#include "Commands.h"
#include "CommandStack.h"

//**************************************************************************************************

void UndoCommand::Run(Renderer* pRenderer)
{
	if (pRenderer->GetDatabase()->hasUndoMark()) {
		pRenderer->GetDatabase()->undoBack();
		TheCommandStack.Completed();
	}
	else {
		TheCommandStack.Canceled();
	}

}

//**************************************************************************************************

void RedoCommand::Run(Renderer* pRenderer)
{
	if (pRenderer->GetDatabase()->hasRedo()) {
		pRenderer->GetDatabase()->redo();
		TheCommandStack.Completed();
	}
	else {
		TheCommandStack.Canceled();
	}
}
