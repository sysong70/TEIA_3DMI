#include <StdAfx.h>

#include "Kernel.Application.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../Dmi3dxInterface/3DX.FileOptions.h"

#include "../Dmi3dxInterface/3DX.FileOptions.h"

using namespace KERNEL;

KERNEL::Application::Application()
{
}

void KERNEL::Application::InitInstance()
{
	m_cApplication.InitInstance();

// 	A3DRWParamsLoadData param;
// 	TheFileOptions.GetImport("CATIA5", param);
}

void KERNEL::Application::ExitInstance()
{
	m_cApplication.ExitInstance();
}
