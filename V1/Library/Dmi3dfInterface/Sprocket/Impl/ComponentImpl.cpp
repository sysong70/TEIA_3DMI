#include "StdAfx.h"

#include "ComponentImpl.h"


#include "../../3DF/Segment.h"
#include "../../3DF/3DF.Utility.h"
#include "../../3DF/KeyPath.h"
#include "../../3DF/Selection.h"
#include "../../3DF/Impl/SelectionImpl.h"

#include <Common_Define.h>

#include <ranges>

using namespace H3DF;

H3DF::ComponentImpl::ComponentImpl()
{
	m_pvSubcomponents = new ComponentArray();
	m_pstrName = new CString();
}

H3DF::ComponentImpl::~ComponentImpl()
{
	//Cleart the subcomponents
	if (nullptr != m_pvSubcomponents) {
		for (auto & pcSubcomponent : *m_pvSubcomponents)
		{
			delete pcSubcomponent;
		}

		m_pvSubcomponents->clear();
		delete m_pvSubcomponents;
	}

	if (nullptr == m_pstrName) {
		delete m_pstrName;
	}
}


void H3DF::ComponentImpl::Copy(ComponentImpl * pcInThat)
{
	m_nKey = pcInThat->m_nKey;
	m_nStatus = pcInThat->m_nStatus;
	m_pcOwner = pcInThat->m_pcOwner;
	m_pvSubcomponents = pcInThat->m_pvSubcomponents;
}

