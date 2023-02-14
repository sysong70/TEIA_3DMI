#include "stdafx.h"
#include "3DFSignal.Interface.h"

#include "3DFSignal.Manager.h"

#include "../Signal/Signal.h"
#include "Common_Define.h"

#include <Json.h>

USING_3DF_SIGNAL_NAMESPACE

Interface::Interface()
{
	m_pcSignalManager = (DWORD_PTR *) new Manager(this);
	if(nullptr == m_pcSignalManager) {
		assert(false);
	}
}

Interface::~Interface()
{
	//:Ken
	if(nullptr != m_pcSignalManager) {
		delete (Manager*)m_pcSignalManager;
		m_pcSignalManager = nullptr;
	}
}

void Interface::ExecuteSignal(Json::Object & cInObject)
{
	assert(m_pcSignalManager);
	((Manager *)m_pcSignalManager)->ExecuteSignal(cInObject);
}