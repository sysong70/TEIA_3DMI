#include "StdAfx.h"

#include "3DF.h"
#include "3DF.Object.h"

#include <iostream>
#include <string>
#include <vector>

//#undef max
//#define NOMINMAX

// #include <boost/pool/pool.hpp>
// #include <boost/pool/object_pool.hpp>
// #include <boost/pool/singleton_pool.hpp>
// #include <boost/pool/pool_alloc.hpp>

USING_3DF_NAMESPACE

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

void PrivateImpl::SetObject(Object * pcObject)
{
	if (nullptr != pcObject->m_pcImpl) {
		delete pcObject->m_pcImpl;
	}

	pcObject->m_pcImpl = this;
}