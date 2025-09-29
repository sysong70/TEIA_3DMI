#include "StdAfx.h"

#include "Am.Object.h"

#include "Impl/AM.Impl.h"
#include "Impl/AM.Object.Impl.h"

using namespace AM;

AM::Object::Object()
{
}

AM::Object::Object(Object const & that)
{
	if (that.m_impl) {
		m_impl = that.m_impl->Clone();
	}
}

AM::Object::~Object() = default;

AM::Object const & AM::Object::operator = (Object const & that)
{
	if (this != &that) {
		m_impl = (nullptr != that.m_impl) ? that.m_impl->Clone() : nullptr;
	}

	return *this;
}

AM::Object::Object(Object && that) noexcept = default;

Object & AM::Object::operator=(Object && that) noexcept = default;


AM::Type AM::Object::Type() const
{
	if (nullptr == m_impl) {
		return AM::Type::None;
	}

	return m_impl->Type();
}


Result AM::Object::writeDatal(std::ostream * os) noexcept
{
	if (nullptr == os) {
		return Result::Fail(Error::InvalidArgument, "ostream is null.");
	}

	auto * impl = ENSURE_IMPL(Object);
	if (nullptr == impl) {
		return Result::Fail(Error::NotInitialized, "Object implementation is not valid.");
	}

	return impl->writeDatal(os);
}