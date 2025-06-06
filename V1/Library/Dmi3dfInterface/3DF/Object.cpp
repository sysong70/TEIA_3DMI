#include "StdAfx.h"

#include "Object.h"

USING_3DF_NAMESPACE

Object::Object()
	: m_pcImpl(nullptr)
{
}

Object::Object(Object const & cInThat)
{
	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

Object::~Object() = default;

Object const & Object::operator = (Object const & cInThat)
{
	if (this != &cInThat) {
		m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
	}

	return *this;
}

Object::Object(Object && cInThat) noexcept = default;

Object & Object::operator=(Object && cInThat) noexcept = default;


// this는 선언된 class를 가르치므로 SegmentKey라면 Type을 staticType이 아닌 Type::SegmentKey를 반환함.
H3DF::Type Object::Type() const
{
	return this->ObjectType();
}

bool Object::HasType(H3DF::Type eInMask) const
{
	return ((DWORD) ObjectType() & (DWORD) eInMask) == (DWORD) eInMask;
}