#include "stdafx.h"

#include "Commands.h"

#include "DbViewport.h"

#include <thread>

//**************************************************************************************************

CommandParams::CommandParams(const CString& name, Renderer* pRenderer)
{
    Name = name;
    RendererPtr = pRenderer;

    GsViewPtr = pRenderer->GetGsView();
    DbPtr = pRenderer->GetDatabase();
    SpacePtr = CommandBase::OpenActiveSpace(DbPtr);
    IoPtr = &pRenderer->UserIo;
    Completed = false;

    //:WARNING - not pDb->disableUndoRecording
    DbPtr->setUndoMark();
}



CommandParams::~CommandParams()
{
    if (Completed) {
        TheCommandStack.Completed(Name, true);
    }
    else {
        //:WARNING - not pDb->undo()
        DbPtr->undoBack();
        TheCommandStack.Completed(Name, false);
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

//**************************************************************************************************

CommandStack TheCommandStack;

static CmdUndo      theUndo;
static CmdRedo      theRedo;
static CmdLine      theLine;
static CmdCircle    theCircle;

//**************************************************************************************************

CString CommandInfo::GetName()
{
    if (Name.IsEmpty()) {
        DEBUG_VALID(CommandPtr);
        return CommandPtr->Name().MakeUpper();
    }
    else {
        return Name;
    }
}

//**************************************************************************************************

CommandStack::CommandStack()
{
#define Keyword(x)  L"&" L#x
#define NameOf(x)   x.Name().MakeUpper()

#ifdef _DEBUG
    CString v1(Keyword(SAMPLE));
    CString v2(CString(PRE_KEYWORD) + L"SAMPLE");
    ASSERT(v1 == v2);
#endif

    Commands.push_back({ &theUndo, EDIT_2D_CMD_Undo, NameOf(theUndo) });
    Commands.push_back({ &theRedo, EDIT_2D_CMD_Redo, NameOf(theRedo) });

    Commands.push_back({ &theLine, DRAW_2D_CMD_Line, NameOf(theLine) });
    Commands.push_back({ &theCircle, DRAW_2D_CMD_Circle_CenterRadius, NameOf(theCircle) });
    Commands.push_back({ &theCircle, DRAW_2D_CMD_Circle_2Points, L"", Keyword(2P) });
    Commands.push_back({ &theCircle, DRAW_2D_CMD_Circle_3Points, L"", Keyword(3P) });
    Commands.push_back({ &theCircle, DRAW_2D_CMD_Circle_2TangentsRadius, L"", Keyword(TTR) });

#undef NameOf
#undef Keyword
}



bool CommandStack::IsActivated()
{
    return ActiveCommands.size() > 0;
}



CommandInfo* CommandStack::Find(UINT id)
{
    for (auto& item : Commands) {
        if (item.Id == id) {
            return &item;
        }
    }

    return nullptr;
}



CommandInfo* CommandStack::Find(const CString& name)
{
    for (auto& item : Commands) {
        if (item.Name == name) {
            return &item;
        }
    }

    return nullptr;
}



bool CommandStack::Execute(UINT id, Renderer* pRenderer)
{
    CommandInfo* pInfo = Find(id);
    if (pInfo != nullptr) {
        return Execute(pInfo, pRenderer);
    }

    RETURN_FALSE;
}

bool CommandStack::Execute(const CString& name, Renderer* pRenderer)
{
    CommandInfo* pInfo = Find(name);
    if (pInfo != nullptr) {
        return Execute(pInfo, pRenderer);
    }

    RETURN_FALSE;
}

bool CommandStack::Execute(CommandInfo* pInfo, Renderer* pRenderer)
{
    DEBUG_VALID(pInfo);

    CommandBase* pCommand = pInfo->CommandPtr;

    if (pCommand != nullptr) {
        std::thread([pCommand, pRenderer]() {
            pCommand->Run(pRenderer);
        }).detach();

        RendererPtr = pRenderer;
        ActiveCommands.push_back(pCommand->Name());

        return true;
    }
    else {
        RendererPtr = nullptr;
        return false;
    }
}



void CommandStack::Completed(const CString& name, bool complete)
{
    if (name == ActiveCommands.front()) {
        ActiveCommands.erase(ActiveCommands.cbegin());
    }
    else {
        DEBUG_STOP;
    }

    if (RendererPtr != nullptr) {
        RendererPtr->UserIo.CommandCompleted();
    }
}
