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