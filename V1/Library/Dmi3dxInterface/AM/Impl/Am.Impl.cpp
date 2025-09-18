#include "StdAfx.h"

#include "AM.Impl.h"

void AM::Impl::Copy(const Impl * pcInThat)
{
	m_type = pcInThat->m_type;
}

bool AM::Impl::Equal(const Impl * pcInThat) const
{
	if (m_type != pcInThat->m_type) {
		return false;
	}

	return true;
}

Result AM::Impl::Bind(std::ostream * os) noexcept
{
	if (nullptr == os) {
		return Result::Fail(Error::InvalidArgument, "DatalWriter::Bind: os is null");
	}

	m_os = os;

	return Result::Ok();
}

//== Impl Factory ==================================================================================

template <typename ImplT>
std::unique_ptr<ImplT> InitImpl()
{
	auto impl = std::make_unique<ImplT>();
	DEBUG_VALID(impl.get());
	return impl;
}