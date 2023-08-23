#include <StdAfx.h>

#include "Kernel.h"

using namespace KERNEL;

Kernel::Kernel()
{

}

void Kernel::ExecuteSignal(Json::Object & cInObject)
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