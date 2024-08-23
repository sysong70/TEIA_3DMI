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

void Object::Set(Object const & cInThat)
{

}

Object const & Object::operator = (Object const & cInThat)
{
	Set(cInThat);
	return *this;
}

Object::Object(Object && cInThat) noexcept 
{
	m_pcImpl = cInThat.m_pcImpl;
	cInThat.m_pcImpl = nullptr;
}

Object & Object::operator = (Object && cInThat) noexcept 
{
	m_pcImpl = cInThat.m_pcImpl;
	cInThat.m_pcImpl = nullptr;

	return *this;
}

H3DF::Type Object::Type() const
{
	return m_pcImpl->Type();
}

bool Object::HasType(H3DF::Type eInMask) const
{
	return ((DWORD) ObjectType() & (DWORD) eInMask) == (DWORD) eInMask;
}