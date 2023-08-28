#include <StdAfx.h>

#include "Manager.Base.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace SESSION;

Manager::Base::Base()
{

}

Manager::Base::~Base()
{

}

void Manager::Base::SetSessionManager(SESSION::Manager::Session * pcSessionManager)
{
	m_pcSessionManager = pcSessionManager;
}
