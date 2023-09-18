#include <StdAfx.h>

#include "Manager.Root.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace SESSION;

Manager::Root::Root()
{

}

Manager::Root::~Root()
{

}

void Manager::Root::SetSessionManager(SESSION::Manager::Session * pcSessionManager)
{
	m_pcSessionManager = pcSessionManager;
}
