#include "stdafx.h"

#include "DmiSelectOperator.h"

#include "../DmiHpsView.h"
#include "../DmiHpsUtility.h"

using namespace HPS;

DmiSelectOperator::DmiSelectOperator(MouseButtons cInMouseTrigger, ModifierKeys cInModifierTrigger)
	: Operator(cInMouseTrigger, cInModifierTrigger)
{
	m_cSelectionOptions = HPS::SelectionOptionsKit::GetDefault();

	m_cSelectionOptions.SetRelatedLimit(0);
	//m_cSelectionOptions.SetInternalLimit(0);
	m_cSelectionOptions.SetLevel(HPS::Selection::Level::Entity);
 	m_cSelectionOptions.SetAlgorithm(Selection::Algorithm::Visual);
	m_cSelectionOptions.SetSorting(Selection::Sorting::Default);
	//m_cSelectionOptions.SetGranularity(Selection::Granularity::General);
	m_cSelectionOptions.SetProximity(0.0f);
}

//== Mouse Event 처리 ===============================================================================

bool DmiSelectOperator::OnMouseDown(MouseState const & cInState)
{
	if(true == IsMouseTriggered(cInState))
	{
		HPS::WindowKey window = cInState.GetEventSource();
		return SelectCommon(m_cActiveSelectionResult, cInState.GetLocation(), window, cInState.GetModifierKeys());
	}
	return false;
}

bool DmiSelectOperator::OnMouseUp(MouseState const & cInState)
{
	if(false == IsMouseTriggered(cInState))
	{
		HPS::WindowKey window = cInState.GetEventSource();
		return SelectCommon(m_cActiveSelectionResult, cInState.GetLocation(), window, cInState.GetModifierKeys());
	}
	return false;
}

bool DmiSelectOperator::OnMouseMove(MouseState const & cInState)
{
	//if(IsMouseTriggered(cInState))
	{
		HPS::WindowKey window = cInState.GetEventSource();
		return SelectCommon(m_cActiveSelectionResult, cInState.GetLocation(), window, cInState.GetModifierKeys());
	}

	return false;
}

bool DmiSelectOperator::OnDaynamicHighlightMouseMove(HPS::MouseState const & cInState)
{
	ULONG64 nStartTick = GetTickCount64();

	HPS::WindowKey cInWindow = cInState.GetEventSource();
	m_cOldDynamicActiveSelection = m_cNewDynamicActiveSelection;
 	size_t nSelectedCount = cInWindow.GetSelectionControl().SelectByPoint(cInState.GetLocation(), 
 		m_cSelectionOptions, m_cNewDynamicActiveSelection);
	//size_t nSelectedCount = cInWindow.GetSelectionControl().SelectByPoint(cInState.GetLocation(), m_cDynamicActiveSelection);

	ULONG64 nEndTick = GetTickCount64();

	TRACE(L"SelectByPoint: %d\r\n", nEndTick - nStartTick);

	return true;
}

bool DmiSelectOperator::OnTouchDown(TouchState const & cInState)
{
	HPS::WindowKey window = cInState.GetEventSource();
	return SelectCommon(m_cActiveSelectionResult, cInState.GetTouches()[0].Location, window, cInState.GetModifierKeys());
}

//== Select 관련 함수 ================================================================================

// 1. 주어진 위치에서 Select를 한다. (Ctrl 버튼이나, 기타 기능은 아직 추가하지 않음)
bool DmiSelectOperator::SelectCommon(HPS::SelectionResults & cInSelection, HPS::Point const & cInLocation, 
	HPS::WindowKey & cInWindow, HPS::ModifierKeys cInModifiers)
{
	try
	{
		m_bSameSelectSourceFlag = false;

		HPS::SelectionResults cNewSelection;
		size_t nSelectedCount = cInWindow.GetSelectionControl().SelectByPoint(cInLocation, m_cSelectionOptions, cNewSelection);
		if(0 == nSelectedCount) {
			return false;
		}

		// 선택된 Active Selection이 있고 선택된 Item이 있다면 같은 Item인지 여부를 확인해서 Falg을 변동시킨다.
		//if(0 < m_cActiveSeleRctionResult.GetCount() && true == cInModifiers.Control() && 0 < nSelectedCount)
		if(0 < cInSelection.GetCount() && 0 < nSelectedCount)
		{
			// 이전 선택에서 같은게 있으면 제거되고, 없으면 추가됨.
			cInSelection.SymmetricDifference(cNewSelection);

// 			if(true == cInSelection.SymmetricDifference(cNewSelection)) {
// 				cInSelection = cNewSelection;
// 			}

/*
			//if(false == m_cActiveSeleRctionResult.Equals(cNewSelection)) {
			//if(true == m_cActiveSeleRctionResult.SymmetricDifference(cNewSelection)) {
			if(!m_cActiveSeleRctionResult.SymmetricDifference(cNewSelection)) {
				m_cActiveSeleRctionResult = cNewSelection;
			}

			if(true == m_cActiveSeleRctionResult.Equals(cNewSelection)) {
				int i = 0;
			}
*/

		}
		else {
			cInSelection = cNewSelection;
		}
	}
	catch(HPS::InvalidObjectException const &)
	{
		//do nothing
	}

	return true;
}

bool DmiSelectOperator::IsSameSelectSource(HPS::SelectionResults & cSelOne, HPS::SelectionResults & cSelTwo)
{
	if(cSelOne.GetCount() != cSelTwo.GetCount()) {
		return false;
	}

	SelectionResultsIterator cIteratorOne = cSelOne.GetIterator();

	while(true == cIteratorOne.IsValid()) {

		SelectionItem cSelOneItem = cIteratorOne.GetItem();

		Key cSelOneItemkey;
		cSelOneItem.ShowSelectedItem(cSelOneItemkey);

// 		HPS::KeyPath cSelItemOnePath;
// 		if(false == cIteratorOne.GetItem().ShowPath(cSelItemOnePath)) {
// 			continue;
// 		}

		SelectionResultsIterator cIteratorTwo = cSelTwo.GetIterator();

		while(true == cIteratorTwo.IsValid()) {

			SelectionItem cSelTwoItem = cIteratorTwo.GetItem();

			Key cSelTwoItemkey;
			cSelTwoItem.ShowSelectedItem(cSelTwoItemkey);

			if(cSelOneItemkey == cSelTwoItemkey) {
				return true;
			}

/*
			HPS::KeyPath cSelItemTwoPath;
			if(false == cIteratorTwo.GetItem().ShowPath(cSelItemTwoPath)) {
				continue;
			}

			if(cSelItemOnePath == cSelItemTwoPath) {
				return true;
			}
*/

			cIteratorTwo.Next();
		}

		cIteratorOne.Next();
	}

	return false;
}