#include "StdAfx.h"

#include "MetaDataImpl.h"

#include "../../3DF/Segment.h"
#include "../../3DF/Reference.h"
#include "../../3DF/3DF.Utility.h"
#include "../../3DF/KeyPath.h"
#include "../../3DF/Selection.h"
#include "../../3DF/Impl/SelectionImpl.h"

#include <Common_Define.h>

#include <ranges>

using namespace H3DF;

//== MetaDataImpl Class ============================================================================
H3DF::MetaDataImpl::MetaDataImpl()
{
	m_eType = H3DF::Type::MetaData;
}

void H3DF::MetaDataImpl::Copy(MetaDataImpl * pcInThat)
{
	if (nullptr == pcInThat) {
		DEBUG_STOP;
		return;
	}

	m_eIndex = pcInThat->m_eIndex;
}

//== StringMetaDataImp Class =======================================================================
H3DF::StringMetaDataImpl::StringMetaDataImpl()
{
	m_eType = H3DF::Type::StringMetaData;
}

void H3DF::StringMetaDataImpl::Copy(StringMetaDataImpl * pcInThat)
{
	if (nullptr == pcInThat) {
		DEBUG_STOP;
		return;
	}

	MetaDataImpl::Copy(pcInThat);

	m_strValue = pcInThat->m_strValue;
}

//== DwordPtrMetaDataImpl Class ====================================================================
H3DF::DwordPtrMetaDataImpl::DwordPtrMetaDataImpl()
{
	m_eType = H3DF::Type::DwordPtrMetaData;
}

void H3DF::DwordPtrMetaDataImpl::Copy(DwordPtrMetaDataImpl * pcInThat)
{
	if (nullptr == pcInThat) {
		DEBUG_STOP;
		return;
	}

	MetaDataImpl::Copy(pcInThat);

	m_nValue = pcInThat->m_nValue;
}