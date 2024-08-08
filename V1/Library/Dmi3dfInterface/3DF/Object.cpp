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

bool Object::HasType(H3DF::Type eInMask) const
{
	return ((DWORD) staticType & (DWORD) eInMask) == (DWORD) eInMask;
}