#include <StdAfx.h>

#include "Manager.Tracker.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

SESSION::Manager::Tracker theTreeManager;

using namespace SESSION;

Manager::Tracker::Tracker()
{

}

Manager::Tracker::~Tracker()
{

}

Manager::Type SESSION::Manager::Tracker::Type()
{
	return SESSION::Manager::Type::Tracker;
}
