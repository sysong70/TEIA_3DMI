#include "stdafx.h"
#include "Commands.h"
#include "CommandStack.h"

#include "DbViewport.h"

//**************************************************************************************************

CommandParams::CommandParams(Renderer* pRenderer)
{
    this->pRenderer = pRenderer;

    pGsView = pRenderer->GetGsView();
    pDb = pRenderer->GetDatabase();
    pSpace = CommandBase::OpenActiveSpace(pDb);
    pIo = &pRenderer->GetUserIO();
    Completed = false;
    //:WARNING - not pDb->disableUndoRecording
    pDb->setUndoMark();
}



CommandParams::~CommandParams()
{
    if (Completed) {
        TheCommandStack.Completed();
    }
    else {
        //:WARNING - not pDb->undo()
        pDb->undoBack();
        TheCommandStack.Canceled();
    }
}

//**************************************************************************************************

OdDbObjectId CommandBase::GetActiveBlockTableRecord(const OdDbDatabase* pDb)
{
    OdDbObjectId spaceId = pDb->getActiveLayoutBTRId();
    if (spaceId == pDb->getPaperSpaceId()) {
        OdDbViewportPtr pVp = pDb->activeViewportId().safeOpenObject();
        if (pVp->number() > 1) {
            spaceId = pDb->getModelSpaceId();
        }
    }

    return spaceId;
}



OdDbBlockTableRecordPtr CommandBase::OpenActiveSpace(const OdDbDatabase* pDb)
{
    return GetActiveBlockTableRecord(pDb).safeOpenObject(OdDb::kForWrite);
}
