#include "stdafx.h"
#include "CommandStack.h"
#include "Commands.h"

//**************************************************************************************************

CommandStack TheCommandStack;

static UndoCommand theUndo;
static RedoCommand theRedo;
static LineCommand theLine;
static CircleCommand theCircle;

//**************************************************************************************************

CommandStack::CommandStack()
{
#define NameOf(x) x.Name().MakeUpper()

    m_commands.push_back({ &theUndo, EDIT_2D_CMD_Undo, NameOf(theUndo) });
    m_commands.push_back({ &theRedo, EDIT_2D_CMD_Redo, NameOf(theRedo)});

    m_commands.push_back({ &theLine, DRAW_2D_CMD_Line, NameOf(theLine) });
    m_commands.push_back({ &theCircle, DRAW_2D_CMD_Circle_CenterRadius, NameOf(theCircle) });
    m_commands.push_back({ &theCircle, DRAW_2D_CMD_Circle_2Points, L"", L".2P" });
    m_commands.push_back({ &theCircle, DRAW_2D_CMD_Circle_3Points, L"", L".3P" });
    m_commands.push_back({ &theCircle, DRAW_2D_CMD_Circle_2TangentsRadius, L"", L".TTR" });

#undef NameOf
}



CommandInfo* CommandStack::Find(UINT id)
{
    for (auto& item : m_commands) {
        if (item.Id == id) {
            return &item;
        }
    }

    return nullptr;
}



CommandInfo* CommandStack::Find(const CString& name)
{
    for (auto& item : m_commands) {
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
        return Execute(pInfo->pCommand, pRenderer);
    }

    RETURN_FALSE;
}

bool CommandStack::Execute(const CString& name, Renderer* pRenderer)
{
    CommandInfo* pInfo = Find(name);
    if (pInfo != nullptr) {
        return Execute(pInfo->pCommand, pRenderer);
    }

    RETURN_FALSE;
}

bool CommandStack::Execute(CommandBase* pCommand, Renderer* pRenderer)
{
    ASSERT(m_bActivated == false);

    if (pCommand != nullptr) {
        std::thread([pCommand, pRenderer]() {
            pCommand->Run(pRenderer);
        }).detach();

        m_pRenderer = pRenderer;
        return m_bActivated = true;
    }
    else {
        m_pRenderer = nullptr;
        return m_bActivated = false;
    }
}



void CommandStack::Completed()
{
    m_bActivated = false;
    if (m_pRenderer != nullptr) {
        m_pRenderer->PostPaintSignal();
        m_pRenderer->GetUserIO().StandbyCommand();

        //:TODO - Add history
    }
}



void CommandStack::Canceled()
{
    m_bActivated = false;
    if (m_pRenderer != nullptr) {
        m_pRenderer->PostPaintSignal();
        m_pRenderer->GetUserIO().StandbyCommand();

        //:TODO - Add history
    }
}
