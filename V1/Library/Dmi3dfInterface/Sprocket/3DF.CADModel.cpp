#include "StdAfx.h"

#include "3DF.CADModel.h"
#include "Impl/CADModelImpl.h"

#include "3DF.Component.h"
#include "Impl/ComponentImpl.h"

#include <Common_Define.h>

#include "../3DF/Object.h"
#include "../3DF/Selection.h"
#include "../3DF/Impl/SelectionImpl.h"
#include "../3DF/3DF.Utility.h"

using namespace H3DF;

//== CADModel Class =================================================================================
H3DF::CADModel::CADModel()
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	m_pcImpl = new CADModelImpl();
	DEBUG_VALID(m_pcImpl);
}
/*
H3DF::CADModel::CADModel(Component const & cInThat)
{
	if (nullptr != m_pcImpl) {
		REMOVE_POINTER(m_pcImpl);
	}

	m_pcImpl = new CADModelImpl();
	DEBUG_VALID(m_pcImpl);

	Set(cInThat);
}
*/

void H3DF::CADModel::Set(CADModel const & cInThat)
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	CADModelImpl * pcInThatImpl = (CADModelImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

CADModel & H3DF::CADModel::operator = (CADModel const & cInThat)
{
	Set(cInThat);
	return *this;
}

Component * H3DF::CADModel::GetComponentFromKey(HC_KEY cInKey) const
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

// 	Component * pcOutComponent = nullptr;
// 	if (true == pcImpl->m_pmComponentMap->Lookup(cInKey, pcOutComponent)) {
// 		return pcOutComponent;
// 	}

	return nullptr;
}

// 1. 주어진 Component를 이용해서, End Item을 찾아서 Selection Item을 생성한다.
bool H3DF::CADModel::ShowSelectionResult(Component * pcInComponent, H3DF::SelectionResults & cOutResults)
{
	if (nullptr == pcInComponent) {
		return false;
	}

	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	// Component 구조상 맨 마지막 Item은 Segment로 Include Key값이 INVALID_KEY이다.
	if (INVALID_KEY == pcInComponent->GetIncludeKey()) {

		// 선택된 Item을 상위 탐색을 통해서, Models Group Item까지 값을 저장한다.
		std::vector<HC_KEY> vnKeys;
		while (nullptr != pcInComponent) {
			vnKeys.push_back(pcInComponent->GetIncludeKey());
			if (pcInComponent == pcImpl->m_pcModels) {
				break;
			}
			pcInComponent = &pcInComponent->GetOwner();
		}

		// 적어도 2개 이상의 Key가 있어야 한다.
		if (2 > vnKeys.size()) {
			return false;
		}

		SelectionItem cSelItem;
		SelectionItemImpl * pcImpl = dynamic_cast<SelectionItemImpl *>(cSelItem.GetImpl());
		DEBUG_VALID(pcImpl);

		H3DF::Type eType = H3DF::Utility::GetType(vnKeys[0]);

		int nIncludeCount = 0;
		int nStartIndex = 0;

		if (H3DF::Type::IncludeKey == eType) {
			IncludeKey cInclude(vnKeys[0]);
			pcImpl->m_cKey = cInclude.GetTarget();
			nIncludeCount = (int)vnKeys.size();
			nStartIndex = 0;
		}
		else {
			pcImpl->m_cKey = SegmentKey(vnKeys[0]);
			nIncludeCount = (int)vnKeys.size() - 1;
			nStartIndex = 1;
		}

		pcImpl->m_nIncludeCount = nIncludeCount;
		pcImpl->m_pnIncludeKeys = new HC_KEY[nIncludeCount];

		// 역방향으로 넣어야 순서가 Selection이 됨.
		int nIncludeIndex = 0;
		for (int nIndex = nStartIndex; nIndex < nIncludeCount; nIndex++) {
			pcImpl->m_pnIncludeKeys[nIncludeIndex++] = vnKeys[nIncludeCount - nIndex - 1];
		}

#ifdef _DEBUG
// 		CString strPath;
// 		cSelItem.ShowPathString(strPath);
#endif

		cOutResults.PushFront(cSelItem);

		return true;
	}

	for (auto pcSubcomponents : pcInComponent->GetSubcomponents()) {
		ShowSelectionResult(pcSubcomponents, cOutResults);
	}

	return true;
}