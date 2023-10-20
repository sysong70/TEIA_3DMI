#include <StdAfx.h>

#include "Kernel.Application.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace KERNEL;

KERNEL::Application::Application()
{
}

void KERNEL::Application::InitInstance()
{
	m_cApplication.InitInstance();
}

void KERNEL::Application::ExitInstance()
{
	m_cApplication.ExitInstance();
}
