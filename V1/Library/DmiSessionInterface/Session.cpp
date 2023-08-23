#include <StdAfx.h>

#include "Session.h"

using namespace SESSION;

Session::Session()
{

}

void Session::ExecuteSignal(Json::Object & cInObject)
{
	int nTarget = cInObject.GetInteger(SKW_TARGET);

	switch ((Signal::Target)nTarget)
	{
		case Signal::Target::Application:
			m_cApplication.ExecuteSignal(cInObject);
			break;

		case Signal::Target::View:
			m_cView.ExecuteSignal(cInObject);
			break;

		default:
			assert(false);
			break;
	}
}