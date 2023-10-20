#include "StdAfx.h"

#include "Object.h"

using namespace KERNEL;

KERNEL::Object::Object()
{
}

KERNEL::Object::~Object()
{
	if (nullptr != m_pcImpl) {
		delete m_pcImpl;
		m_pcImpl = nullptr;
	}
}

KERNEL::Type KERNEL::Object::Type() const
{
	if (nullptr == m_pcImpl) {
		return KERNEL::Type::None;
	}

	return m_pcImpl->Type();
}