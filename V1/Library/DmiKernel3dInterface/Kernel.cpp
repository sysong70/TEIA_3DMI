#include <StdAfx.h>

#include "Kernel.h"

#include "Object.h"

using namespace KERNEL;

Kernel::Kernel()
{

}

void Kernel::ExecuteSignal(Json::Object & cInObject)
{
}

//== 객체 관리 ======================================================================================
KERNEL::Type KERNEL::PrivateImpl::Type() const {
	return m_eType;
}

void KERNEL::PrivateImpl::SetType(KERNEL::Type eType) {
	m_eType = eType;
}

void KERNEL::PrivateImpl::SetImpl(Object * pcObject, PrivateImpl * pcImpl)
{
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if (nullptr != pcObject->m_pcImpl) {
		delete pcObject->m_pcImpl;
	}

	pcObject->m_pcImpl = pcImpl;
}