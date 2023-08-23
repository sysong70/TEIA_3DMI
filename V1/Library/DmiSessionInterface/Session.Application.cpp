#include <StdAfx.h>

#include "Session.Application.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace SESSION;

Application::Application()
{

}

void Application::ExecuteSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);

	switch ((Signal::Application::Action)nAction)
	{
		case Signal::Application::Action::OnInitInstance:
			m_cApplication.InitInstance();
			break;

		case Signal::Application::Action::OnExitInstance:
			m_cApplication.ExitInstance();
			break;

		//:Ken
		case Signal::Application::Action::OnDpiAware:
			// #처리필요
			//TheAppOptions.Preference.Session.DpiScale = cInObject.GetReal(SKW_DPISCALE);
			break;

		case Signal::Application::Action::OnUpdatePreference:
			// #처리필요
			//TheAppOptions.Preference.Set(&cInObject);
			break;

		case Signal::Application::Action::OnUpdateFileOption:
			break;

		default:
			assert(false);
			break;

/*
		case Signal::Target::Application:
			m_cApplication.ExecuteSignal(cInObject);
			break;

		case Signal::Target::View:
			m_cViewManager.ExecuteSignal(cInObject);
			break;

		default:
			assert(false);
			break;
*/
	}
}