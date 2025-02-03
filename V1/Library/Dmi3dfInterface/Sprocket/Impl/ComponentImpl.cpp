#include "StdAfx.h"

#include "ComponentImpl.h"

#include "../3DF.MetaData.h"

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
}

H3DF::ComponentImpl::~ComponentImpl()
{
	if (nullptr != m_pvSubComponents) {
		for (auto * pcSubComponent : *m_pvSubComponents) {
			delete pcSubComponent;
		}

		delete m_pvSubComponents;
		m_pvSubComponents = nullptr;
	}

	if (nullptr != m_pvMetaDatas) {
		for (auto * pcMetaData : *m_pvMetaDatas) {
			delete pcMetaData;
		}

		delete m_pvMetaDatas;
		m_pvMetaDatas = nullptr;
	}
}

void H3DF::ComponentImpl::Copy(ComponentImpl * pcInThat)
{
	m_nSegmentKey = pcInThat->m_nSegmentKey;
	m_nIncludeKey = pcInThat->m_nIncludeKey;
	m_eType = pcInThat->m_eType;
	m_nStatus = pcInThat->m_nStatus;
	m_pcOwner = pcInThat->m_pcOwner;
	m_strName = pcInThat->m_strName;

	if (nullptr != pcInThat->m_pvSubComponents) {
		m_pvSubComponents = new ComponentArray();

		for (auto * pcSubComponent : *pcInThat->m_pvSubComponents) {
			Component * pcComponent = new Component(*pcSubComponent);
			m_pvSubComponents->push_back(pcComponent);
		}
	}
}

void H3DF::ComponentImpl::SetName(CString strInName)
{
	m_strName = strInName;
}

CString H3DF::ComponentImpl::TypeName()
{
	CString strTypeName;

	switch (m_eType)
	{
		case H3DF::Component::Type::ExchangeProductOccurrence:
			strTypeName = L"Product";
			break;

		case H3DF::Component::Type::ExchangePartDefinition:
			strTypeName = L"Part";
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

		case H3DF::Component::Type::PMIGroupComponent:
			strTypeName = L"PMI Group";
			break;

		case H3DF::Component::Type::ViewGroupComponent:
			strTypeName = L"View Group";
			break;

		case H3DF::Component::Type::ExchangeMkpView:
			strTypeName = L"Markup View";
			break;

		case H3DF::Component::Type::ExchangePMI:
			strTypeName = L"PMI";
			break;

		default:
			strTypeName.Format(L"Type: 0x%x", (int)m_eType);
			break;
	};

	return strTypeName;
}

void H3DF::ComponentImpl::AddSubComponent(Component & cInSubComponent)
{
	if (nullptr == m_pvSubComponents) {
		m_pvSubComponents = new ComponentArray();
	}
	
	DEBUG_VALID(m_pvSubComponents);
	m_pvSubComponents->push_back(&cInSubComponent);
}

ComponentArray * H3DF::ComponentImpl::GetAllSubcomponents(Component::Type eInType) const
{
	ComponentArray * pcSubComponents = new ComponentArray();

	GetSubcomponents(eInType, pcSubComponents, true);

	return pcSubComponents;
}

void H3DF::ComponentImpl::GetSubcomponents(Component::Type eInType, ComponentArray * pcOutSubComponents, bool bRecursive) const
{
	if (nullptr == pcOutSubComponents) {
		DEBUG_STOP;
		return;
	}

	if (nullptr != m_pvSubComponents) {
		for (auto * pcSubComponent : *m_pvSubComponents) {
			if (eInType == pcSubComponent->GetType()) {
				pcOutSubComponents->push_back(pcSubComponent);
			}
		}

		if (true == bRecursive) {
			for (auto * pcSubComponent : *m_pvSubComponents) {
				ComponentImpl * pcSubImpl = dynamic_cast<ComponentImpl *>(pcSubComponent->GetImpl());
				DEBUG_VALID(pcSubImpl);

				pcSubImpl->GetSubcomponents(eInType, pcOutSubComponents, bRecursive);
			}
		}
	}
}

Component * H3DF::ComponentImpl::FindSubComponentBySegmentKey(HC_KEY nInSegmentKey, bool bRecursive)
{
	if (INVALID_KEY == nInSegmentKey) {
		DEBUG_STOP;
		return nullptr;
	}

	if (nullptr != m_pvSubComponents) {
		for (auto * pcSubComponent : *m_pvSubComponents) {
			if (nInSegmentKey == pcSubComponent->GetSegmentKey()) {
				return pcSubComponent;
			}
		}

		if (true == bRecursive) {
			for (auto * pcSubComponent : *m_pvSubComponents) {
				ComponentImpl * pcSubImpl = dynamic_cast<ComponentImpl *>(pcSubComponent->GetImpl());
				DEBUG_VALID(pcSubImpl);
				Component * pcComponent = pcSubImpl->FindSubComponentBySegmentKey(nInSegmentKey, bRecursive);
				if (nullptr != pcComponent) {
					return pcComponent;
				}
			}
		}
	}

	return nullptr;
}

//== Utility Functions =============================================================================

bool H3DF::ComponentUtility::SetData(Component & cInComponent, CString strInName, HC_KEY nKey, HC_KEY nIncludeKey, Component::Type eInType)
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

bool H3DF::ComponentUtility::SetName(Component & cInComponent, CString strInName)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return false;
	}

	pcImpl->SetName(strInName);

	return true;
}

bool H3DF::ComponentUtility::AddSubComponent(Component & cInParentComponent, Component & cInComponent)
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

CString H3DF::ComponentUtility::TypeName(Component & cInComponent)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	if (nullptr == pcImpl) {
		DEBUG_STOP;
		return L"";
	}

	return pcImpl->TypeName();
}

// 주어진 Component를 기준으로 상위에 있는 PartDefinition를 찾는다.
bool H3DF::ComponentUtility::FindParentPartDefinition(Component & cInComponent, Component *& pcOutComponent)
{
	// In Component가 NULL이면 Parent에 PartDefinition이 없는 것으로 한다.
	if (nullptr == &cInComponent) {
		return false;
	}

	if (H3DF::Component::Type::ExchangePartDefinition == cInComponent.GetType()) {
		pcOutComponent = &cInComponent;
		return true;
	}

	return FindParentPartDefinition(*cInComponent.GetOwner(), pcOutComponent);
}

// 주어진 Component를 기준으로 하위에 있는 Segment를 찾는다.
Component * H3DF::ComponentUtility::FindSubComponentBySegmentKey(Component & cInComponent, HC_KEY nInSegmentKey, bool bRecursive)
{
	ComponentImpl * pcImpl = dynamic_cast<ComponentImpl *>(cInComponent.GetImpl());
	DEBUG_VALID(pcImpl);

	return pcImpl->FindSubComponentBySegmentKey(nInSegmentKey, bRecursive);
}

// Part Definition을 복제한다.
bool H3DF::ComponentUtility::ClonedParentPartDefinition(Component & cInComponent)
{
	if (H3DF::Component::Type::ExchangePartDefinition != cInComponent.GetType()) {
		DEBUG_STOP;
		return false;
	}

	ClonedComponent(cInComponent, *cInComponent.GetOwner(), true);

	return true;
}

// 주어진 Component를 복제
bool H3DF::ComponentUtility::ClonedComponent(Component & cInComponent, Component & cInOwnerComponent, bool bDeleteInclude)
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
		ComponentArray * pcOwnerSubComponents = cInOwnerComponent.GetSubComponents();
		if(nullptr != pcOwnerSubComponents)
		for (auto * pcSubComponent : *pcOwnerSubComponents) {
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
		H3DF::Type eType = cKey.ObjectType();

		cSegment.ReferenceGeometry(cKey);

		cIterator.Next();
	}

	// 7. 하부 Include 정보 복사
	ComponentArray * pcSubComponents = cInComponent.GetSubComponents();
	if (nullptr != pcSubComponents) {
		for (auto * pcSubComponent : *pcSubComponents) {
			ClonedComponent(*pcSubComponent, cInComponent, false);
		}
	}

	return true;
}

Component * H3DF::ComponentUtility::GetViewGroupComponent(Component & cInParentComp)
{
	// 입력된 값이 ViewGroupComponent이면 그냥 리턴한다.
	if (H3DF::Component::Type::ViewGroupComponent == cInParentComp.GetType()) {
		return &cInParentComp;
	}

	// 상위 Component에서 재귀하게 View Group Component를 찾는다.
	H3DF::Component * pcViewGroupComponent = cInParentComp.FindUpComponent(H3DF::Component::Type::ViewGroupComponent);
	if (nullptr != pcViewGroupComponent) {
		return pcViewGroupComponent;
	}

	// 하부 Component에서 View Group Component를 찾는다.
	if (nullptr == pcViewGroupComponent) {
		ComponentArray * pcSubComponents = cInParentComp.GetSubComponents(H3DF::Component::Type::ViewGroupComponent);
		if (nullptr != pcSubComponents) {
			if (false == pcSubComponents->empty()) {
				pcViewGroupComponent = pcSubComponents->front();
				return pcViewGroupComponent;
			}
		}
	}

	return nullptr;
}

Component * H3DF::ComponentUtility::GetAnnotationViewGroupComponent(Component & cInParentComp)
{
	if (H3DF::Component::Type::AnnotationViewGroupComponent == cInParentComp.GetType()) {
		return &cInParentComp;
	}

	H3DF::Component * pcAnnotationViewGroupComponent = cInParentComp.FindUpComponent(H3DF::Component::Type::AnnotationViewGroupComponent);
	if (nullptr != pcAnnotationViewGroupComponent) {
		return pcAnnotationViewGroupComponent;
	}

	if (nullptr == pcAnnotationViewGroupComponent) {
		ComponentArray * pcSubComponents = cInParentComp.GetAllSubcomponents(H3DF::Component::Type::AnnotationViewGroupComponent);
		if (nullptr != pcSubComponents) {
			if (false == pcSubComponents->empty()) {
				pcAnnotationViewGroupComponent = pcSubComponents->front();
				return pcAnnotationViewGroupComponent;
			}
		}
	}

	return nullptr;	
}

// PMI를 Group으로 처리하기 위해서, Parent Component에서 PMI Group을 검색해서 찾아온다.
Component * H3DF::ComponentUtility::GetPmiGroupComponent(Component & cInParentComp)
{
	if (H3DF::Component::Type::PMIGroupComponent == cInParentComp.GetType()) {
		return &cInParentComp;
	}

	H3DF::Component * pcPMIGroupComponent = cInParentComp.FindUpComponent(H3DF::Component::Type::PMIGroupComponent);
	if (nullptr != pcPMIGroupComponent) {
		return pcPMIGroupComponent;
	}

	if (nullptr == pcPMIGroupComponent) {
		ComponentArray * pcSubComponents = cInParentComp.GetAllSubcomponents(H3DF::Component::Type::PMIGroupComponent);
		//ComponentArray * pcSubComponents = cInParentComp.GetSubComponents(H3DF::Component::Type::PMIGroupComponent);
		if (nullptr != pcSubComponents) {
			if (false == pcSubComponents->empty()) {
				pcPMIGroupComponent = pcSubComponents->front();
				return pcPMIGroupComponent;
			}
		}
	}

	return nullptr;
}