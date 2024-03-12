#include "StdAfx.h"

#include "Object.h"

USING_3DF_NAMESPACE

Object::Object()
{
}

Object::Object(Object const & cInThat)
{
	m_pcImpl = cInThat.m_pcImpl;
}

Object::~Object()
{
	if (nullptr != m_pcImpl) {
		
		delete m_pcImpl;  
		m_pcImpl = nullptr;
	}
}

H3DF::Type Object::Type() const
{
	if (nullptr == m_pcImpl) {
		return H3DF::Type::None;
	}

	return m_pcImpl->Type();
}