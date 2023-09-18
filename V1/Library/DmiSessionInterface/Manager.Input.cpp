#include <StdAfx.h>

#include "Manager.Root.h"
#include "Manager.Input.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

SESSION::Manager::Input theInputManager;

using namespace SESSION;

Manager::Input::Input()
{

}

Manager::Input::~Input()
{

}

Manager::Type SESSION::Manager::Input::Type()
{
	return SESSION::Manager::Type::Input;
}
