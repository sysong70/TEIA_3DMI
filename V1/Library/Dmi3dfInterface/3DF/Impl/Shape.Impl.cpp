#include "StdAfx.h"

#include "Shape.Impl.h"

using namespace H3DF;


bool H3DF::ShapeElementImpl::Equals(ShapeElementImpl const * pcInThat) const
{
	if (m_bFillFlag != pcInThat->m_bFillFlag) {
		return false;
	}

	if (m_bDisjointedFlag != pcInThat->m_bDisjointedFlag) {
		return false;
	}

	return true;
}