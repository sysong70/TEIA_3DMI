#include "stdafx.h"
#include "Commands.h"

#include "DbViewport.h"

//**************************************************************************************************

CommandParams::CommandParams(Renderer* pRenderer)
{
    this->pRenderer = pRenderer;

    pGsView = pRenderer->GetGsView();
    pDb = pRenderer->GetDatabase();
    pSpace = CommandBase::OpenActiveSpace(pDb);
    pio = &pRenderer->GetUserIO();

    pDb->setUndoMark();
}



void CommandParams::Cancel()
{
    pDb->undoBack();
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
