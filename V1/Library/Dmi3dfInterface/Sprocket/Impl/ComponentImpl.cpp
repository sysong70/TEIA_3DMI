#include "StdAfx.h"

#include "ComponentImpl.h"


#include "../../3DF/Segment.h"
#include "../../3DF/Reference.h"
#include "../../3DF/3DF.Utility.h"
#include "../../3DF/KeyPath.h"
#include "../../3DF/Selection.h"
#include "../../3DF/Impl/SelectionImpl.h"

#include <Common_Define.h>

#include <ranges>

using namespace H3DF;

H3DF::ComponentImpl::ComponentImpl()
{
	m_pvSubComponents = new ComponentArray();
	m_pstrName = new CString();
}

H3DF::ComponentImpl::~ComponentImpl()
{
	if (nullptr != m_pvSubComponents) {
		for (auto * pcSubComponent : *m_pvSubComponents) {
			delete pcSubComponent;
		}

		delete m_pvSubComponents;
	}

	if (nullptr != m_pstrName) {
		delete m_pstrName;
	}
}

void H3DF::ComponentImpl::Copy(ComponentImpl * pcInThat)
{
	m_nSegmentKey = pcInThat->m_nSegmentKey;
	m_nIncludeKey = pcInThat->m_nIncludeKey;
	m_eType = pcInThat->m_eType;
	m_nStatus = pcInThat->m_nStatus;
	m_pcOwner = pcInThat->m_pcOwner;
	*m_pstrName = *pcInThat->m_pstrName;

	for (auto * pcSubComponent : *pcInThat->m_pvSubComponents) {
		Component * pcComponent = new Component(*pcSubComponent);
		m_pvSubComponents->push_back(pcComponent);
	}
}

void H3DF::ComponentImpl::SetName(CString strInName)
{
	*m_pstrName = strInName;
}

CString H3DF::ComponentImpl::TypeName()
{
	CString strTypeName;

	switch (m_eType)
	{
		case H3DF::Component::Type::ExchangeProductOccurrence:
			strTypeName = L"ProductOccurrence";
			break;

		case H3DF::Component::Type::ExchangePartDefinition:
			strTypeName = L"PartDefinition";
			break;

		case H3DF::Component::Type::ExchangeRIBRepModelSolid:
		case H3DF::Component::Type::ExchangeRIPolyBRepModelSolid:
			strTypeName = L"Solid";
			break;

		case H3DF::Component::Type::ExchangeRIBRepModelSurface:
		case H3DF::Component::Type::ExchangeRIPolyBRepModelSurface:
			strTypeName = L"Surface";
			break;

		case H3DF::Component::Type::ExchangeRICurve:
		case H3DF::Component::Type::ExchangeRIPolyWire:
			strTypeName = L"Curve";
			break;

		case H3DF::Component::Type::ExchangeRISet:
			strTypeName = L"Group";
			break;
		case H3DF::Component::Type::ExchangeRIPointSet:
			strTypeName = L"Point Set";
			break;

		default:
			strTypeName.Format(L"Type: 0x%x", (int)m_eType);
			break;
	};

	return strTypeName;
}

void H3DF::ComponentImpl::AddSubComponent(Component & cInSubComponent)
{
	m_pvSubComponents->push_back(&cInSubComponent);
}

//== Utility Functions =============================================================================

bool H3DF::ComponentImpl::SetData(Component & cInComponent, CString strInName, HC_KEY nKey, HC_KEY nIncludeKey, Component::Type eInType)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return false;
	}

	pcImpl->SetName(strInName);
	pcImpl->m_eType = eInType;
	pcImpl->m_nSegmentKey = nKey;
	pcImpl->m_nIncludeKey = nIncludeKey;

	return true;
}

bool H3DF::ComponentImpl::SetName(Component & cInComponent, CString strInName)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return false;
	}

	pcImpl->SetName(strInName);

	return true;
}

bool H3DF::ComponentImpl::AddSubComponent(Component & cInParentComponent, Component & cInComponent)
{
	ComponentImpl * pcParentImpl = dynamic_cast<ComponentImpl *>(cInParentComponent.GetImpl());
	if (nullptr == pcParentImpl) {
		DEBUG_STOP;
		return false;
	}

	pcParentImpl->AddSubComponent(cInComponent);

 	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
 	DEBUG_VALID(pcImpl);
 
 	pcImpl->m_pcOwner = &cInParentComponent;

	return true;
}

CString H3DF::ComponentImpl::TypeName(Component & cInComponent)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return L"";
	}

	return pcImpl->TypeName();
}

// 주어진 Component를 기준으로 상위에 있는 PartDefinition를 찾는다.
bool H3DF::ComponentImpl::FindParentPartDefinition(Component & cInComponent, Component *& pcOutComponent)
{
	// In Component가 NULL이면 Parent에 PartDefinition이 없는 것으로 한다.
	if (nullptr == &cInComponent) {
		return false;
	}

	if (H3DF::Component::Type::ExchangePartDefinition == cInComponent.GetType()) {
		pcOutComponent = &cInComponent;
		return true;
	}

	return FindParentPartDefinition(cInComponent.GetOwner(), pcOutComponent);
}

// Part Definition을 복제한다.
bool H3DF::ComponentImpl::ClonedParentPartDefinition(Component & cInComponent)
{
	if (H3DF::Component::Type::ExchangePartDefinition != cInComponent.GetType()) {
		DEBUG_STOP;
		return false;
	}

	ClonedComponent(cInComponent, cInComponent.GetOwner(), true);

	return true;

	// Parent Segment에서 Include를 생성하고, Segment를 만들어서 원본의 정보를 복사한다.
	
	// 1. Parent Segment 설정
	HC_KEY nOwnerKey = cInComponent.GetOwner().GetSegmentKey();
	SegmentKey cParentSegment(cInComponent.GetOwner().GetSegmentKey());

	// 2. 원본 Segment의 정보를 수집하고 Include키는 삭제.
	IncludeKey cOriginInclude(cInComponent.GetIncludeKey());
	SegmentKey cOirignSegment(cInComponent.GetSegmentKey());
	CStringA strOirignSegmentName = cOirignSegment.Name();

	cOriginInclude.Delete(); // Include를 삭제해서 기본에 있던 PartDefinition의 연결을 제거한다.

	// 3. 새로운 Segment를 생성하고, 원본 Segment의 정보를 복사한다.
	strOirignSegmentName.Format("%s_c%d", strOirignSegmentName, HDB::GetUniqueID());

	H3DF::SegmentKey cSegment(strOirignSegmentName);
	H3DF::IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

	// 4. User Data 및 Style을 복사.

	// 5. 하부 Include 정보 복사

	return true;
}

// 주어진 Component를 복제
bool H3DF::ComponentImpl::ClonedComponent(Component & cInComponent, Component & cInOwnerComponent, bool bDeleteInclude)
{
	// 1. Parent Segment 설정
	SegmentKey cParentSegment(cInOwnerComponent.GetSegmentKey());
	CStringA strParentName = cParentSegment.Name();

	// 2. 원본 Segment의 정보를 수집한다.
	IncludeKey cOriginInclude(cInComponent.GetIncludeKey());
	SegmentKey cOirignSegment(cInComponent.GetSegmentKey());

	// 3. Include를 삭제한다.
	if (true == bDeleteInclude) {
		cParentSegment.Flush(Search::Type::Include, Search::Space::SegmentOnly);

		// 3-1. 입력받은 Component가 아닌 다른 Component는 Include 관계를 재설정한다.
		ComponentArray & aOwnerSubComponents = cInOwnerComponent.GetSubComponents();
		for (auto * pcSubComponent : aOwnerSubComponents) {
			// 입력 받은 Component가 아닌 SubComponent를 찾는다.
			if (pcSubComponent != &cInComponent) {
				SegmentKey cSubSegment(pcSubComponent->GetSegmentKey());
				// 새롭게 Include를 생성한다.
				H3DF::IncludeKey cInclude = cParentSegment.IncludeSegment(cSubSegment);

				ComponentImpl * pcSubImpl = dynamic_cast<ComponentImpl *>(pcSubComponent->GetImpl());
				DEBUG_VALID(pcSubImpl);

				pcSubImpl->m_nIncludeKey = cInclude.KeyValue();
			}
		}
	}

	// 4. 새로운 Segment와 Parent Segment에서 새로운 Include를 생성.
	CStringA strName;
	strName.Format("%s_c%d", cOirignSegment.Name(), HDB::GetUniqueID());

	H3DF::SegmentKey cSegment(strName);
	H3DF::IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

	// 5. Component의 Segment 및 Include 정보를 교체.
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	DEBUG_VALID(pcImpl);

	pcImpl->m_nSegmentKey = cSegment.KeyValue();
	pcImpl->m_nIncludeKey = cInclude.KeyValue();

	// 6. User Data 및 Style을 복사.
	// 6-1. Style 복사
	StyleKeyArray aOirignStyles;
	cOirignSegment.GetStyleControl().Show(aOirignStyles);

	for (auto & cStyle : aOirignStyles) {
		SegmentKey cStyleSegment;
		if (true == cStyle.ShowSource(cStyleSegment)) {
			cSegment.GetStyleControl().PushSegment(cStyleSegment);
		}
	}

	// 6-2. User Data 복사
	H3DF::UserData::Copy(cOirignSegment, cSegment);

	// 7. Goeometry 정보 Reference로 저장.
	// 원본 Segment의 Geometry 정보를 찾는다.
	SearchResults cResults;
	cOirignSegment.Find(Search::Type::Geometry, Search::Space::SegmentOnly, cResults);

	SearchResultsIterator cIterator = cResults.GetIterator();

	while (true == cIterator.IsValid()) {
		Key cKey = cIterator.GetItem();
		H3DF::Type eType = cKey.Type();

		cSegment.ReferenceGeometry(cKey);

		cIterator.Next();
	}

	// 7. 하부 Include 정보 복사
	ComponentArray & aSubComponents = cInComponent.GetSubComponents();
	for (auto * pcSubComponent : aSubComponents) {
		ClonedComponent(*pcSubComponent, cInComponent, false);
	}

	return true;
}