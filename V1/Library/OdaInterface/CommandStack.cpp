#include "stdafx.h"
#include "CommandStack.h"
#include "Commands.h"

//**************************************************************************************************

CommandStack TheCommandStack;

LineCommand theLine;
CircleCommand theCircle;

//**************************************************************************************************

CommandStack::CommandStack()
{
#define AddCommand(x) m_commands[the##x.Name().MakeLower()] = &the##x

    AddCommand(Line);
    AddCommand(Circle);

#undef Register
}



CommandBase* CommandStack::Find(const CString& name)
{
    if (m_commands.find(name) != m_commands.end()) {
        return m_commands[name];
    }
    else {
        RETURN_NULL;
    }
}



bool CommandStack::Execute(const CString& name, Renderer* pRenderer)
{
    return Execute(Find(name), pRenderer);
}



bool CommandStack::Execute(CommandBase* pCommand, Renderer* pRenderer)
{
    if (pCommand != nullptr) {
        std::thread([pCommand, pRenderer]() {
            pCommand->Run(pRenderer);
        }).detach();

        m_bActivated = true;
        return true;
    }

    return false;
}
