#include <StdAfx.h>

#include "Manager.ObjectSnap.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

SESSION::Manager::ObjectSnap theObjectSnapManager;

using namespace SESSION;

Manager::ObjectSnap::ObjectSnap()
{

}

Manager::ObjectSnap::~ObjectSnap()
{

}

Manager::Type SESSION::Manager::ObjectSnap::Type()
{
	return SESSION::Manager::Type::ObjectSnap;
}
