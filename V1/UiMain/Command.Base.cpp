#include "stdafx.h"
#include "Command.Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

Command::Base::Base()
{
}



Command::Base::~Base()
{
}



bool Command::Base::ReceiveSignal(Json::Object* pData)
{
	REMOVE_POINTER(pData);
	RETURN_FALSE;
}



void Command::Base::Run(Window::View* pView)
{
	m_pView = pView;
}



void Command::Base::Cancel()
{
	m_pView = nullptr;
}
