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
KERNEL::Type KERNEL::Impl::Type() const {
	return m_eType;
}

void KERNEL::Impl::SetType(KERNEL::Type eType) {
	m_eType = eType;
}

void KERNEL::Impl::SetImpl(Object * pcObject, Impl * pcImpl)
{
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if (nullptr != pcObject->m_pcImpl) {
		delete pcObject->m_pcImpl;
	}

	pcObject->m_pcImpl = pcImpl;
}