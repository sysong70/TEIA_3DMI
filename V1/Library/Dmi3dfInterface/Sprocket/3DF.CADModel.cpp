#include "StdAfx.h"

#include "3DF.CADModel.h"
#include "Impl/CADModelImpl.h"

#include "3DF.Component.h"
#include "Impl/ComponentImpl.h"

#include <Common_Define.h>

#include "../3DF/Object.h"
#include "../3DF/Selection.h"
#include "../3DF/Impl/Selection.Impl.h"
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
	cPath.ShowString(strText);
#endif

	// 키값 배열을 가져온다.
	H3DF::KeyArray cKeys;
	cPath.ShowKeys(cKeys);

	std::reverse(cKeys.begin(), cKeys.end());

	Component * pcComponent = (Component *)this;
	Component * pcFindSubComponent = nullptr;

	// 가져온 키값을 이용해서 Component를 찾는다.
	for (auto & cKey : cKeys) {
		ComponentArray * pcSubcomponentArray = pcComponent->GetSubComponents();

		if (nullptr == pcSubcomponentArray) {
			continue;
		}

		pcFindSubComponent = nullptr;

		for (auto pcSubComponent : *pcSubcomponentArray) {
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

		// 찾는 Component가 없으면 다음 Key를 이용해서 탐색을 계속한다.
		if (nullptr == pcFindSubComponent) {
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

	// Show/No Show 검사
	if (false == pcInComponent->IsShow()) {
		return true;
	}

	CADModelImpl * pcImpl = (CADModelImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	// RepresentationItem이면 마지막 하부까지 탐색한걸로 간주한다.
	// RepresentationItem이 아니면 하부까지 탐색한다.
	// RepresentationItem이면 Component를 중심으로 상위로 탐색을 시작하면서 Key 값을 저장한다.
	// Shell값등의 Geometry를 최상단으로 넣어야 하는지는 결정되지 않았음.
	// Component class에서 ShowSelectionResult를 처리하는 것이 더 자	연스러울 수 있음. (아님.. 아래에서 위로 선택하니까. 
	if (true == pcInComponent->IsRepresentationItem()) {

		SelectionItem cInItem;
		SelectionItemImpl * pcInItemImpl = (SelectionItemImpl *) cInItem.GetImpl();
		DEBUG_VALID(pcInItemImpl);

		// Shell을 찾아서 넣는다. 그렇게 해야 Selection에서 화면에 보이는 것이 제대로 나옴.
		SearchResults cResults;
		SegmentKey cSegment(pcInComponent->GetSegmentKey());
		cSegment.Find(Search::Type::Geometry, Search::Space::SegmentOnly, cResults);
		
		SearchResultsIterator cIter = cResults.GetIterator();
		while (true == cIter.IsValid()) {
			Key cKey = cIter.GetItem();
			H3DF::Type cType = cKey.Type();

			if (H3DF::Type::ShellKey == cType) {
				cInItem.KeyPushBack(cKey, H3DF::Type::ShellKey);
			}

			cIter.Next();
		}

		while (nullptr != pcInComponent) {
			if (INVALID_KEY != pcInComponent->GetSegmentKey()) {
				cInItem.KeyPushBack(pcInComponent->GetSegmentKey(), H3DF::Type::SegmentKey);
			}

			if (INVALID_KEY != pcInComponent->GetIncludeKey()) {
				cInItem.KeyPushBack(pcInComponent->GetIncludeKey(), H3DF::Type::IncludeKey);
			}

			if (pcInComponent == pcImpl->m_pcModels) {
				break;
			}

			pcInComponent = pcInComponent->GetOwner();
		}

/*
		pcImpl->m_nIncludeCount = nIncludeCount;
		pcImpl->m_pnIncludeKeys = new HC_KEY[nIncludeCount];

		// 역방향으로 넣어야 순서가 Selection이 됨.
		int nIncludeIndex = 0;
		for (int nIndex = nStartIndex; nIndex < nIncludeCount; nIndex++) {
			pcImpl->m_pnIncludeKeys[nIncludeIndex++] = vnKeys[nIncludeCount - nIndex - 1];
		}
*/

#ifdef _DEBUG
// 		CString strPath;
// 		cInItem.ShowPathString(strPath);
#endif

		cOutResults.PushFront(cInItem);

		return true;
	}

	if (nullptr != pcInComponent->GetSubComponents()) {
		for (auto pcSubcomponents : *pcInComponent->GetSubComponents()) {
			ShowSelectionResult(pcSubcomponents, cOutResults);
		}
	}

	return true;
}