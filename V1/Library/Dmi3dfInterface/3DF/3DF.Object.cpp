#include "StdAfx.h"

#include "3DF.Object.h"

USING_3DF_NAMESPACE

Object::Object()
{
}

Object::~Object()
{
	if (nullptr != m_pcImpl) {
		delete m_pcImpl;  
		m_pcImpl = nullptr;
	}
}

TDF::Type Object::Type() const
{
	if (nullptr == m_pcImpl) {
		return TDF::Type::None;
	}

	return m_pcImpl->Type();
}