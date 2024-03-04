#include "StdAfx.h"

#include "ComponentImpl.h"

#include "../Component.h"

#include "Common_Define.h"

#include "../3DF/Segment.h"
#include "../3DF/3DF.Utility.h"
#include "../3DF/KeyPath.h"
#include "../3DF/Selection.h"
#include "../3DF/Impl/SelectionImpl.h"

#include <ranges>

using namespace H3DF;

H3DF::ComponentImpl::ComponentImpl()
{
	m_vpnSubcomponents = new ComponentArray();
}

H3DF::ComponentImpl::~ComponentImpl()
{
	if (nullptr != m_vpnSubcomponents) {
		delete m_vpnSubcomponents;
	}
}


void H3DF::ComponentImpl::Copy(ComponentImpl * pcInThat)
{
	m_nKey = pcInThat->m_nKey;
	m_nStatus = pcInThat->m_nStatus;
	m_pcOwner = pcInThat->m_pcOwner;
	m_vpnSubcomponents = pcInThat->m_vpnSubcomponents;
}

