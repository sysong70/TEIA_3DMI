#include "stdafx.h"
#include "Dialog.h"
#include "Dialog.Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#pragma region Instances Class

Dialog::Instances::Instances()
{
}



Dialog::Instances::~Instances()
{
	for (auto dlg : m_buffer) {
		REMOVE_POINTER(dlg);
	}

	m_buffer.clear();
}



void Dialog::Instances::Add(Base* pValue)
{
	DEBUG_VALID(pValue);

	m_buffer.push_back(pValue);
}



Dialog::Base* Dialog::Instances::Get(int id)
{
	for (auto dlg : m_buffer) {
		if (dlg->GetSignalTargetId() == (Signal::Target)id) {
			return dlg;
		}
	}

	return nullptr;
}



void Dialog::Instances::Remove(int id)
{
	Base* pValue = nullptr;
	for (auto dlg : m_buffer) {
		if (dlg->GetSignalTargetId() == (Signal::Target)id) {
			pValue = dlg;
			break;
		}
	}

	if (pValue != nullptr) {
		m_buffer.remove(pValue);

		pValue->DestroyWindow();
		REMOVE_POINTER(pValue);
	}
}

#pragma endregion //:REGION

//**************************************************************************************************

CSize Dialog::FramePadding()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}



CSize Dialog::WindowPadding()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}
