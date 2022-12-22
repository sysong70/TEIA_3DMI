#include "stdafx.h"

#include "3DFSignal.Manager.h"

#include "3DFSignal.ApplicationManager.h"

#include "../Signal/Signal.h"
#include <Common_Define.h>

using namespace std;
using namespace std::chrono;

USING_3DF_SIGNAL_NAMESPACE

Manager::Manager(Interface * pc3dfInterface) :
	m_pc3dfInterface(pc3dfInterface)
{
// Wrapper를 통해서, Child Class에서 Manager 변수를 사용할 수 있도록 설정함.
#define SetWrapper(className) className.m_pcWrapper = this
	SetWrapper(m_cApplicationManager);
	SetWrapper(m_cViewManager);
#undef SetWrapper
}

Manager::~Manager()
{
}

//== 전달 받은 명령어 분기 =============================================================================

void Manager::ExecuteSignal(Json::Object & cInObject)
{
	int nTarget = cInObject.GetInteger(SKW_TARGET);

	switch((Signal::Target) nTarget)
	{
		case Signal::Target::Application:
			m_cApplicationManager.ExecuteSignal(cInObject);
			break;

		case Signal::Target::View:
			m_cViewManager.ExecuteSignal(cInObject);
			break;

		default:
			assert(false);
			break;
	}
}