#include <StdAfx.h>

#include "Command.Line.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace SESSION;

SESSION::Command::Line::Line()
{

}

SESSION::Command::Line::~Line()
{

}

int SESSION::Command::Line::InputCount()
{
	return 2;
}

Manager::Input::InputType * SESSION::Command::Line::InputTypes()
{
	Manager::Input::InputType eType[2];

	eType[0] = Manager::Input::InputType::Cordinate;
	eType[1] = Manager::Input::InputType::Cordinate;

	return eType;
}