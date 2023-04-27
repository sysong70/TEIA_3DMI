#include "StdAfx.h"

#include "3DF.h"
#include "3DF.Object.h"

#include <iostream>
#include <string>
#include <vector>

USING_3DF_NAMESPACE

//== 메모리 관리 =====================================================================================

void * Memory::Allocate(size_t nInBytes, bool bInClearMemory)
{
	void * pByte = new BYTE[nInBytes];
	if (true == bInClearMemory) {
		memset(pByte, 0, nInBytes);
	}

	return pByte;
}

void Memory::Free(void * pInPointer)
{
	delete[] pInPointer;
	pInPointer = nullptr;
}

Memory::Memory() {}


TDF::Type PrivateImpl::Type() const {
	return m_eType;
}

void PrivateImpl::SetType(TDF::Type eType) {
	m_eType = eType;
}

void PrivateImpl::SetImpl(Object * pcObject, PrivateImpl * pcImpl)
{
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if(nullptr != pcObject->m_pcImpl) {
		delete pcObject->m_pcImpl;
	}

	pcObject->m_pcImpl = pcImpl;
}