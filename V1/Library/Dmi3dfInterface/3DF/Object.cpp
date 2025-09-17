#include "StdAfx.h"

#include "Object.h"

USING_3DF_NAMESPACE

Object::Object()
	: m_pcImpl(nullptr)
{
}

Object::Object(Object const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
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
	if(nullptr == m_pcImpl) {
		return H3DF::Type::None;
	}

	return m_pcImpl->Type();
}

bool Object::HasType(H3DF::Type eInMask) const
{
	return ((DWORD) ObjectType() & (DWORD) eInMask) == (DWORD) eInMask;
}

Impl * H3DF::Object::SetImpl(H3DF::Type eInType, std::unique_ptr<Impl> pcInImpl)
{
	m_pcImpl = std::move(pcInImpl);
	DEBUG_VALID(m_pcImpl);

	m_pcImpl->SetType(eInType);

	return m_pcImpl.get();
}

/*
void Object::SetImpl(std::unique_ptr<Impl> && pcInImpl)
{
	if (pcInImpl) {
		m_pcImpl = std::move(pcInImpl);
	} else {
		m_pcImpl.reset();
	}
}

*/
