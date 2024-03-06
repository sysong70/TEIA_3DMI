#include "StdAfx.h"

#include "OPERATOR.Select.h"

#include "Impl/OperatorImpl.h"

#include "Kernel.DocView.h"
#include "Impl/Kernel.DocViewImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <3DF/Facility.AppOptions.h>

#include <Sprocket/3DF.View.h>
#include <3DF/Window.h>
#include <3DF/Selection.h>
#include <3DF/Highlight.h>
#include <3DF/Visibility.h>
#include <3DF/LineAttribute.h>
#include <3DF/AttributeLock.h>

#include <Json.h>

#include <HEventListener.h>

#include "Operator.HighlightObjectSnap.h"
#include "Impl/Operator.HighlightObjectSnapImpl.h"
#include "Operator.ModelPanel.h"

using namespace KERNEL;

//== SelectImpl 관련 함수 ============================================================================

namespace KERNEL
{
	namespace Operator
	{
		class SelectImpl : public OperatorImpl
		{
		public:
			SelectImpl(const DocView * pcInDocView);

			void Copy(SelectImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			DWORD m_nSelectPickCount = 200;
			ULONGLONG m_nMouseDownTickCount = 0;
			H3DF::Point2D m_cLButtonDownPosition;

			// HighlightObjectSnap Operator
			Operator::HighlightObjectSnap m_cHighlightOSnapOperator;

			float m_fLineWeight = 3;
			H3DF::HighlightControl m_cHighlightCtrl;
			H3DF::HighlightControl m_cLineHighlightCtrl;

			// 현재 선택된 요소들이 저장되는 변수
			H3DF::SelectionResults m_cSelectionResult;
			H3DF::SelectionResults m_cDynSelectionResult;

			DWORD m_nOSnapMode = 0;
			DWORD m_nSelFilter = 0;

			H3DF::HighlightControl & DynHighlightControl() { return *m_pcDynHighlightControl; }
			H3DF::HighlightControl & DynLineHighlightControl() { return *m_pcDynLineHighlightControl; }

			KERNEL::Operator::ModelPanel & ModelPanel();

		protected:
			H3DF::SelectionResults m_cNewHighlightSelection;
			H3DF::SelectionResults m_cOldHighlightSelection;
			H3DF::SelectionResults m_cHighlightSelection;

			HPoint m_cClickPoint;

			H3DF::HighlightControl * m_pcDynHighlightControl = nullptr;
			H3DF::HighlightControl * m_pcDynLineHighlightControl = nullptr;
		};
	}
}

KERNEL::Operator::SelectImpl::SelectImpl(const DocView * pcInDocView) :
	OperatorImpl(pcInDocView),
	m_cHighlightOSnapOperator(pcInDocView),
	m_cHighlightCtrl(Window()),
	m_cLineHighlightCtrl(Window())
{
	m_nOSnapMode += (DWORD)OSnap::Type::EndPoint;
	m_nOSnapMode += (DWORD)OSnap::Type::MidPoint;
	m_nOSnapMode += (DWORD)OSnap::Type::Center;
	m_nOSnapMode += (DWORD)OSnap::Type::Intersection;
	m_nOSnapMode += (DWORD)OSnap::Type::Perpendicular;
	m_nOSnapMode += (DWORD)OSnap::Type::Quadrant;
	m_nOSnapMode += (DWORD)OSnap::Type::OnSurface;
	m_nOSnapMode += (DWORD)OSnap::Type::Axis;

	m_nSelFilter += (DWORD)SelectionFilter::Type::Point;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Curve;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Edge;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Face;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Solid;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Axis;
	m_nSelFilter += (DWORD)SelectionFilter::Type::PMI;

	m_cHighlightOSnapOperator.SetObjectSnapMode(m_nOSnapMode);
	m_cHighlightOSnapOperator.SetSelectionFilter(m_nSelFilter);

	H3DF::MaterialMappingKit cHighlightMatMapping;

	cHighlightMatMapping.SetLineColor(H3DF::RGBAColor(RGB(200, 20, 10)));
	cHighlightMatMapping.SetFaceColor(H3DF::RGBAColor(RGB(250, 105, 95)));

	m_cHighlightCtrl.SetMaterialMapping(cHighlightMatMapping);
	// Shell 선택시에 Line Visibility를 설정한대로 적용하기 위해서 Lock을 걸도록 한다.
	m_cHighlightCtrl.GetAttributeLockControl().SetLock(H3DF::AttributeLock::Type::Visibility);
	m_cHighlightCtrl.GetVisibilityControl().SetLines(false);
	m_cHighlightCtrl.GetVisibilityControl().SetEdges(false);

	m_cLineHighlightCtrl.SetMaterialMapping(cHighlightMatMapping);
	m_cLineHighlightCtrl.GetLineAttributeControl().SetWeight(m_fLineWeight);

	Operator::HighlightObjectSnapImpl * pcOSnapImpl = dynamic_cast<Operator::HighlightObjectSnapImpl *>(m_cHighlightOSnapOperator.GetImpl());
	DEBUG_VALID(pcOSnapImpl);

	m_pcDynHighlightControl = &pcOSnapImpl->m_cDynHighlightControl;
	DEBUG_VALID(m_pcDynHighlightControl);

	m_pcDynLineHighlightControl = &pcOSnapImpl->m_cDynLineHighlightCtrl;
	DEBUG_VALID(m_pcDynLineHighlightControl);
}

KERNEL::Operator::ModelPanel & KERNEL::Operator::SelectImpl::ModelPanel()
{
	DocViewImpl * pcImpl = (DocViewImpl *)GetDocView().GetImpl();
	DEBUG_VALID(pcImpl);

	return pcImpl->ModelPanel();
}

//== Select 관련 함수 ================================================================================

KERNEL::Operator::Select::Select(const DocView * pcInDocView)
{
	SelectImpl * pcImpl = new SelectImpl(pcInDocView);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

//== Mouse Event 관련 함수 ===========================================================================
int KERNEL::Operator::Select::MouseMove(HEventInfo & cInEvent)
{
	auto * pcImpl = (SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cHighlightOSnapOperator.MouseMove(cInEvent);

	return HLISTENER_PASS_EVENT;
}

int KERNEL::Operator::Select::LButtonDown(HEventInfo & cInEvent)
{
	auto * pcImpl = (SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cLButtonDownPosition.Set(cInEvent.GetMousePixelPos().x, cInEvent.GetMousePixelPos().y);
	pcImpl->m_nMouseDownTickCount = GetTickCount64();

	return 0;
}

int KERNEL::Operator::Select::LButtonUp(HEventInfo & cInEvent)
{
	auto * pcImpl = (SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	H3DF::Point2D cMousePosition(cInEvent.GetMousePixelPos().x, cInEvent.GetMousePixelPos().y);
	DWORD nMouseUpTickCount = GetTickCount64();
	ULONGLONG nTickCount = nMouseUpTickCount - pcImpl->m_nMouseDownTickCount;

	// 1. 2 Pixel이하 200 Tick이하에서만 선택하는 것으로 판정한다.
	if (pcImpl->m_nSelectPickCount > nTickCount) {
		double dLength = pcImpl->m_cLButtonDownPosition.DistanceWith(cMousePosition);

		if (2.0 < dLength) {
			return HLISTENER_PASS_EVENT;
		}
	}
	else {
		return HLISTENER_PASS_EVENT;
	}

	// 2. Dynamic Highlight된 Item을 가져옴. 
	H3DF::SelectionItem & cSelItem = pcImpl->m_cHighlightOSnapOperator.DynamicHighlightSelectionItem();
	if (false == cSelItem.IsValid()) {
		return HLISTENER_PASS_EVENT;
	}

#ifdef _DEBUG
	CString strPath;
	cSelItem.ShowPathString(strPath);
#endif

	// 3. 기존에 선택되어 있는 Dynamic highlight를 모두 지움.
	pcImpl->m_cHighlightOSnapOperator.UnhighlightEverything();

	H3DF::HighlightOptionsKit cOptions;

	if (false == pcImpl->m_cSelectionResult.IsExist(cSelItem)) {
		if (H3DF::Type::LineKey == cSelItem.Type()) {
			pcImpl->m_cLineHighlightCtrl.Highlight(cSelItem, cOptions, false);
		}
		else {
			pcImpl->m_cHighlightCtrl.Highlight(cSelItem, cOptions, false);
		}

		// 선택된 객체를 SelectionResult에 추가
		pcImpl->m_cSelectionResult.PushFront(cSelItem);
	}
	else {
		if (H3DF::Type::LineKey == cSelItem.Type()) {
			pcImpl->m_cLineHighlightCtrl.Unhighlight(cSelItem, cOptions);
		}
		else {
			pcImpl->m_cHighlightCtrl.Unhighlight(cSelItem, cOptions);
		}

		pcImpl->m_cSelectionResult.Erase(cSelItem);
	}

	// 4. ModelPanel에 선택된 객체를 전달
	pcImpl->ModelPanel().SetSelectItem(cSelItem);

	pcImpl->View().Update();

	return HLISTENER_PASS_EVENT;
}

//== Object Snap 관련 함수 ===========================================================================

void KERNEL::Operator::Select::DrawSnapItems()
{
	auto * pcImpl = (SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cHighlightOSnapOperator.DrawSnapItems();
}

void KERNEL::Operator::Select::SetObjectSnapMode(OSnap::Type eInType)
{
	auto * pcImpl = (SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	// Osnap type이 없는 경우 추가
	if (0 == (pcImpl->m_nOSnapMode & (DWORD)eInType)) {
		pcImpl->m_nOSnapMode += (DWORD)eInType;
	}
	else { // Osnap type이 없는 경우 제거
		pcImpl->m_nOSnapMode -= (DWORD)eInType;
	}

	pcImpl->m_cHighlightOSnapOperator.SetObjectSnapMode(pcImpl->m_nOSnapMode);
}

//== Select 관련 함수 ================================================================================

bool KERNEL::Operator::Select::SelectByResult(H3DF::SelectionResults & cInResults)
{
	auto * pcImpl = (Operator::SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	H3DF::HighlightOptionsKit cOptions;

	H3DF::SelectionResultsIterator cIter = cInResults.GetIterator();

	while (true == cIter.IsValid()) {
		H3DF::SelectionItem cItem = cIter.GetItem();

		if (false == pcImpl->m_cSelectionResult.IsExist(cItem)) {
			if (H3DF::Type::LineKey == cItem.Type()) {
				pcImpl->m_cLineHighlightCtrl.Highlight(cItem, cOptions, false);
			}
			else {
				pcImpl->m_cHighlightCtrl.Highlight(cItem, cOptions, false);
			}

			// 선택된 객체를 SelectionResult에 추가
			pcImpl->m_cSelectionResult.PushFront(cItem);
		}

		cIter.Next();
	}

	pcImpl->DynHighlightControl().UnhighlightEverything();
	pcImpl->DynLineHighlightControl().UnhighlightEverything();

	pcImpl->View().Update();

	return true;
}

bool KERNEL::Operator::Select::DynamicSelectByResult(H3DF::SelectionResults & cInResults)
{
	auto * pcImpl = (Operator::SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	H3DF::HighlightOptionsKit cOptions;

	H3DF::SelectionResultsIterator cIter = cInResults.GetIterator();

	pcImpl->DynHighlightControl().UnhighlightEverything();
	pcImpl->DynLineHighlightControl().UnhighlightEverything();
	pcImpl->m_cDynSelectionResult.Reset();

	while (true == cIter.IsValid()) {
		H3DF::SelectionItem cItem = cIter.GetItem();

		if (false == pcImpl->m_cDynSelectionResult.IsExist(cItem)) {
			if (H3DF::Type::LineKey == cItem.Type()) {
				pcImpl->DynLineHighlightControl().Highlight(cItem, cOptions, false);
			}
			else {
				pcImpl->DynHighlightControl().Highlight(cItem, cOptions, false);
			}

			// 선택된 객체를 SelectionResult에 추가
			pcImpl->m_cDynSelectionResult.PushFront(cItem);
		}

		cIter.Next();
	}

	pcImpl->View().Update();

	return true;
}

void KERNEL::Operator::Select::SetSelectionFilter(SelectionFilter::Type eInType)
{
	auto * pcImpl = (Operator::SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	// Selection filter type이 없는 경우 추가
	if (0 == (pcImpl->m_nSelFilter & (DWORD)eInType)) {
		pcImpl->m_nSelFilter += (DWORD)eInType;
	}
	else { // Selection filter type이 없는 경우 제거
		pcImpl->m_nSelFilter -= (DWORD)eInType;
	}

	pcImpl->m_cHighlightOSnapOperator.SetSelectionFilter(pcImpl->m_nSelFilter);
}

H3DF::SelectionResults & KERNEL::Operator::Select::Results()
{
	auto * pcImpl = (Operator::SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cSelectionResult;
}

//== Highlight 관련 함수 =============================================================================
H3DF::HighlightControl & KERNEL::Operator::Select::DynHighlightControl()
{
	auto * pcImpl = (Operator::SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->DynHighlightControl();
}

void KERNEL::Operator::Select::Unhighlight(H3DF::SelectionResults const & cInItems)
{
	auto * pcImpl = (SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->View().SuppressUpdate(true);

	pcImpl->m_cHighlightCtrl.Unhighlight(cInItems);
	pcImpl->m_cLineHighlightCtrl.Unhighlight(cInItems);

	// 선택한 후에 Unhighlight 제거
	pcImpl->DynHighlightControl().UnhighlightEverything();
	pcImpl->DynLineHighlightControl().UnhighlightEverything();

	pcImpl->m_cSelectionResult.Erase(cInItems);

	pcImpl->View().SuppressUpdate(false);

	pcImpl->View().Update();
}

void KERNEL::Operator::Select::UnhighlightEverything()
{
	auto * pcImpl = (SelectImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->View().SuppressUpdate(true);

	pcImpl->m_cHighlightOSnapOperator.UnhighlightEverything();
	
	pcImpl->m_cHighlightCtrl.UnhighlightEverything();
	pcImpl->m_cLineHighlightCtrl.UnhighlightEverything();

	pcImpl->DynHighlightControl().UnhighlightEverything();
	pcImpl->DynLineHighlightControl().UnhighlightEverything();

	pcImpl->m_cSelectionResult.Reset();
	pcImpl->m_cDynSelectionResult.Reset();

	pcImpl->View().SuppressUpdate(false);

	pcImpl->View().Update();
}