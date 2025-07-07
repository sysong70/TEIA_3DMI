#include "StdAfx.h"

#include "3DF.Impl.h"

using namespace H3DF;

void H3DF::Impl::Copy(const Impl * pcInThat)
{
	m_eType = pcInThat->m_eType; 
}

bool H3DF::Impl::Equal(const Impl * pcInThat) const
{ 
	if (m_eType != pcInThat->m_eType) {
		return false;
	}

	return true; 
}

/*

//== 객체 관리 ======================================================================================
void H3DF::Impl::SetImpl(Object * pcObject, Impl * pcImpl)
{
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if(nullptr != pcObject->m_pcImpl) {
		delete pcObject->m_pcImpl;
	}

	pcObject->m_pcImpl = pcImpl;
}*/