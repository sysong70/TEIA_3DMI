#include <StdAfx.h>

#include "Kernel.Application.h"
#include "Impl/Kernel.ApplicationImpl.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../Dmi3dxInterface/3DX.FileOptions.h"
//#include "../Dmi3dxInterface/3DX.FileOptions.cpp"

using namespace KERNEL;

KERNEL::Application::Application()
{
	m_pcImpl = new ApplicationImpl();
}

void KERNEL::Application::InitInstance()
{
	ApplicationImpl * pcImpl = (ApplicationImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cApplication.InitInstance();
}

void KERNEL::Application::ExitInstance()
{
	ApplicationImpl * pcImpl = (ApplicationImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cApplication.ExitInstance();
}

void KERNEL::Application::OnUpdateFileOption(Json::Object& data)
{
	TheFileOptions.Import.Set(data);
}

void KERNEL::Application::OnFileOptionReference(Json::Object& data)
{
	TheFileOptions.SetReference(data);
}

Session * KERNEL::Application::GetDocView(int nId)
{
	ApplicationImpl * pcImpl = (ApplicationImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	Session * pcDocView = pcImpl->m_mapDocView[nId];

	if (nullptr == pcDocView) {
		pcDocView = new Session();
		if (nullptr == pcDocView) {
			DEBUG_STOP;
			return nullptr;
		}
		else {
			pcDocView->ViewId(nId);
			pcImpl->m_mapDocView[nId] = pcDocView;
		}
	}

	return pcDocView;
}

void KERNEL::Application::RemoveDocView(int nId)
{
	ApplicationImpl * pcImpl = (ApplicationImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	Session * pcDocView = pcImpl->m_mapDocView[nId];

	if (nullptr != pcDocView) {
		delete pcDocView;
		pcImpl->m_mapDocView.erase(nId);
	}
}