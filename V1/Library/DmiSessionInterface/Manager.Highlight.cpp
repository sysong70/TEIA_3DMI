#include <StdAfx.h>

#include "Manager.Highlight.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

SESSION::Manager::Highlight theHighlightManager;

using namespace SESSION;

Manager::Highlight::Highlight()
{

}

Manager::Highlight::~Highlight()
{

}

Manager::Type SESSION::Manager::Highlight::Type()
{
	return SESSION::Manager::Type::Highlight;
}
