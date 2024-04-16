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

Component * H3DF::CADModel::GetComponent(HC_KEY cInKey) const
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	
// 	Component * pcOutComponent = nullptr;
// 	if (true == pcImpl->m_pmComponentMap->Lookup(cInKey, pcOutComponent)) {
// 		return pcOutComponent;
// 	}

	return nullptr;
}

Component * H3DF::CADModel::GetComponent(H3DF::SelectionItem & cInItem) const
{
	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	H3DF::KeyPath cPath;
	cInItem.ShowPath(cPath);

#ifdef _DEBUG
	CString strText;
	cInItem.ShowPathString(strText);
#endif

	// 키값 배열을 가져온다.
	H3DF::KeyArray cKeys;
	cPath.ShowKeys(cKeys);

	std::reverse(cKeys.begin(), cKeys.end());

	Component * pcComponent = (Component *)this;
	Component * pcFindSubComponent = nullptr;

	// 가져온 키값을 이용해서 Component를 찾는다.
	for (auto & cKey : cKeys) {
		ComponentArray & cSubcomponentArray = pcComponent->GetSubComponents();

		if (true == cSubcomponentArray.empty()) {
			continue;
		}

		pcFindSubComponent = nullptr;

		for (auto pcSubComponent : cSubcomponentArray) {
			HC_KEY nSegmentKey = pcSubComponent->GetSegmentKey();
			HC_KEY nIncludeKey = pcSubComponent->GetIncludeKey();

#ifdef _DEBUG
			CString strName = pcSubComponent->GetName();
			H3DF::Component::Type eType = pcSubComponent->GetType();

			H3DF::SegmentKey cSegmentKey(nSegmentKey);
			CStringA strSegmentName = cSegmentKey.Name(false);
#endif // _DEBUG

			if (INVALID_KEY != nIncludeKey && cKey.KeyValue() == nIncludeKey) {
				pcFindSubComponent = pcSubComponent;
				break;
			}
			else if (INVALID_KEY == nIncludeKey && cKey.KeyValue() == nSegmentKey) {
				pcFindSubComponent = pcSubComponent;
				break;
			}
		}

		if (nullptr == pcFindSubComponent) {
			DEBUG_STOP;
			continue;
		}

		pcComponent = pcFindSubComponent;
	}

	return pcComponent;
}

// 1. 주어진 Component를 이용해서, End Item을 찾아서 Selection Item을 생성한다.
bool H3DF::CADModel::ShowSelectionResult(Component * pcInComponent, H3DF::SelectionResults & cOutResults)
{
	if (nullptr == pcInComponent) {
		return false;
	}

	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	// RepresentationItem이면 마지막 하부까지 탐색한걸로 간주한다.
	if (true == pcInComponent->IsRepresentationItem()) {

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

		SelectionItem cInItem;
		SelectionItemImpl * pcImpl = dynamic_cast<SelectionItemImpl *>(cInItem.GetImpl());
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
// 		cInItem.ShowPathString(strPath);
#endif

		cOutResults.PushFront(cInItem);

		return true;
	}

	for (auto pcSubcomponents : pcInComponent->GetSubComponents()) {
		ShowSelectionResult(pcSubcomponents, cOutResults);
	}

	return true;
}