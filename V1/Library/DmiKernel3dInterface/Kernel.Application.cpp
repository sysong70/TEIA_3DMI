#include <StdAfx.h>

#include "Kernel.Application.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../Dmi3dxInterface/3DX.FileOptions.h"
//#include "../Dmi3dxInterface/3DX.FileOptions.cpp"

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

void KERNEL::Application::OnUpdateFileOption(Json::Object& data)
{
	TheFileOptions.Import.Set(data);
}

void KERNEL::Application::OnFileOptionReference(Json::Object& data)
{
	TheFileOptions.SetReference(data);
}
