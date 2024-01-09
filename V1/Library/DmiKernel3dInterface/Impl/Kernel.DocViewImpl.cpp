#include <StdAfx.h>

#include "Kernel.DocViewImpl.h"

#include "../Operator.Camera.h"
#include "../Operator.VisualEffects.h"
#include "../Operator.Visibility.h"

#include "../Signal.Connector.h"
#include "../../Signal/Signal.h"
#include "../../Common/Common_Define.h"

#include "../../../UiMain/Command.Resource.h"

#include <3DF/Segment.h>
#include <3DF/VisualEffects.h>

using namespace KERNEL;
using namespace H3DF;

#define TheKenel TheAppOptions.Kernel

//== Visual Effects 관련 함수 ========================================================================

KERNEL::DocViewImpl::DocViewImpl()
{
	m_nOSnapMode += (DWORD) OSnap::Type::EndPoint;
	m_nOSnapMode += (DWORD) OSnap::Type::MidPoint;
	m_nOSnapMode += (DWORD) OSnap::Type::Center;
	m_nOSnapMode += (DWORD) OSnap::Type::Intersection;
	m_nOSnapMode += (DWORD) OSnap::Type::Perpendicular;
	m_nOSnapMode += (DWORD) OSnap::Type::Quadrant;
	m_nOSnapMode += (DWORD) OSnap::Type::OnSurface;
	m_nOSnapMode += (DWORD) OSnap::Type::Axis;

	m_nSelFilter += (DWORD)SelectionFilter::Type::Point;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Curve;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Edge;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Face;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Solid;
	m_nSelFilter += (DWORD)SelectionFilter::Type::Axis;
	m_nSelFilter += (DWORD)SelectionFilter::Type::PMI;

	for (auto & pcOperator : m_apcOperator) {
		pcOperator = nullptr;
	}
}

H3DF::BaseView * KERNEL::DocViewImpl::GetBaseView()
{
	return m_cCanvas.GetFrontView().GetWindowKey().GetBaseView();
}

Signal::Delivery & KERNEL::DocViewImpl::Delivery() 
{ 
	return *(Signal::Delivery *)m_pcDelivery;
}

const Signal::Delivery & KERNEL::DocViewImpl::Delivery() const 
{ 
	return *m_pcDelivery; 
}

void KERNEL::DocViewImpl::SetDelivery(const Signal::Delivery * pcInDelivery)
{
	m_pcDelivery = pcInDelivery;
}

//== Operator 관련 함수 ==============================================================================

void KERNEL::DocViewImpl::CancelCommands()
{
	m_cSelectionResult.Reset();
	m_pcHighlightControl->UnhighlightEverything();
	m_cCanvas.GetFrontView().GetWindowKey().Update();
}

void KERNEL::DocViewImpl::AllocationOperator(H3DF::View * pcInView, Signal::Delivery & cDelivery)
{
	// Highlight Object Snap Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::HighlightObjectSnap] = new KERNEL::Operator::HighlightObjectSnap(pcInView, &cDelivery);
	((Operator::HighlightObjectSnap *)m_apcOperator[(int)KERNEL::Operator::Type::HighlightObjectSnap])->SetObjectSnapMode(m_nOSnapMode);
	((Operator::HighlightObjectSnap *)m_apcOperator[(int)KERNEL::Operator::Type::HighlightObjectSnap])->SetSelectionFilter(m_nSelFilter);

	// VisualEffects Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::VisualEffects] = new KERNEL::Operator::VisualEffects(pcInView, &cDelivery);

	// Visibility Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::Visibility] = new KERNEL::Operator::Visibility(pcInView, &cDelivery);

	// Camera Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::Camera] = new KERNEL::Operator::Camera(pcInView, &cDelivery);

	// Camera Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::Select] = new KERNEL::Operator::Select(pcInView, &cDelivery);

	m_pcHighlightControl = new H3DF::HighlightControl(pcInView->GetWindowKey(), false);

	H3DF::MaterialMappingKit cHighlightMaterialMapping;
	// 	cHighlightMaterialMapping.SetLineColor(RGBAColor(RGB(0, 0, 128)));
	// 	cHighlightMaterialMapping.SetEdgeColor(RGBAColor(0, 0, 0));
	// 	cHighlightMaterialMapping.SetFaceColor(RGBAColor(RGB(0, 162, 232)));

	cHighlightMaterialMapping.SetLineColor(RGBAColor(RGB(255, 131, 145)));
	cHighlightMaterialMapping.SetEdgeColor(RGBAColor(0, 0, 0));
	cHighlightMaterialMapping.SetFaceColor(RGBAColor(RGB(255, 131, 145)));

	cHighlightMaterialMapping.SetFaceColor(RGBAColor(RGB(255, 131, 145)));
}

KERNEL::Operator::OperatorBase * KERNEL::DocViewImpl::GetOperator(Operator::Type eInType)
{ 
	return m_apcOperator[(int)eInType]; 
}

KERNEL::Operator::HighlightObjectSnap & KERNEL::DocViewImpl::HighlightOSnapOperator()
{ 
	return *(Operator::HighlightObjectSnap *)m_apcOperator[(int)Operator::Type::HighlightObjectSnap]; 
}

KERNEL::Operator::Camera & KERNEL::DocViewImpl::Camera()
{
	return *(Operator::Camera *)m_apcOperator[(int)Operator::Type::Camera];
}

KERNEL::Operator::Select & KERNEL::DocViewImpl::Select()
{
	return *(Operator::Select *)m_apcOperator[(int)Operator::Type::Select];
}

DWORD KERNEL::DocViewImpl::MouseMapFlags(DWORD nState)
{
	DWORD nFlag = 0;

	// map the mfc events state to MVO
	if (nState & MK_LBUTTON) nFlag |= MVO_LBUTTON;
	if (nState & MK_RBUTTON) nFlag |= MVO_RBUTTON;
	if (nState & MK_MBUTTON) nFlag |= MVO_MBUTTON;
	if (nState & MK_SHIFT) nFlag |= MVO_SHIFT;
	if (nState & MK_CONTROL) nFlag |= MVO_CONTROL;

	return nFlag;
}

//== Object Snap 관련 함수 ===========================================================================
void KERNEL::DocViewImpl::SetObjectSnap(OSnap::Type eInType)
{
	// Osnap type이 없는 경우 추가
	if (0 == (m_nOSnapMode & (DWORD)eInType)) {
		m_nOSnapMode += (DWORD)eInType;
	}
	else { // Osnap type이 없는 경우 제거
		m_nOSnapMode -= (DWORD)eInType;
	}

	HighlightOSnapOperator().SetObjectSnapMode(m_nOSnapMode);
}


//== Selection Filter 관련 함수 ======================================================================
void KERNEL::DocViewImpl::SetSelectionFilter(SelectionFilter::Type eInType)
{
	// Selection filter type이 없는 경우 추가
	if (0 == (m_nSelFilter & (DWORD) eInType)) {
		m_nSelFilter += (DWORD) eInType;
	}
	else { // Selection filter type이 없는 경우 제거
		m_nSelFilter -= (DWORD) eInType;
	}

	HighlightOSnapOperator().SetSelectionFilter(m_nSelFilter);



/*
	// Selectability는 Selection option에서 attribute lock을 설정해야 해서 속도가 많이 느림.
	// 삭제해버림.
	SelectabilityKit cSelectability;
	cSelectability.SetLines(false);

	SelectionOptionsKit cSelectionOptionsKit;
	cSelectionOptionsKit.SetSelectability(cSelectability);

	m_cCanvas.GetFrontView().GetWindowKey().SetSelectionOptions(cSelectionOptionsKit);
*/
}

//== Visibility 관련 함수 ============================================================================

// 1. 전달받은 Visibility 명령어를 분기 처리하는 함수.
void KERNEL::DocViewImpl::SetVisibility(int nId)
{
	Operator::Visibility * pcOperator = (Operator::Visibility *)m_apcOperator[(int)KERNEL::Operator::Type::Visibility];
	DEBUG_VALID(pcOperator);

	switch (nId)
	{
		case HOME_3D_CMD_Visualize_ShowAll:
			pcOperator->ShowAll();
			break;

		case HOME_3D_CMD_Visualize_Hide:
			pcOperator->Hide();
			break;

		case HOME_3D_CMD_Visualize_ShowOnly:
			pcOperator->ShowOnly();
			break;

		case HOME_3D_CMD_Visualize_Toggle:
			pcOperator->Toggle();
			break;

		default:
			assert(false);
			break;
	}
}

//== Command 관련 함수 ===============================================================================

// 1. Command Request 함수 처리
void KERNEL::DocViewImpl::CommandRequest(Json::Object & cInObject)
{
	int nId = cInObject.GetInteger(SKW_ID);

	switch (nId) 
	{
		case HOME_3D_LST_VisualEffects:
			RequestVisualEffects(cInObject);
		break;

		default:
			assert(false);
			break;
	}
}

// 1-1. Visual Effects Request 요청 함수 처리
void KERNEL::DocViewImpl::RequestVisualEffects(Json::Object & cInObject)
{
	Operator::VisualEffects * pcOperator = (Operator::VisualEffects *) m_apcOperator[(int)KERNEL::Operator::Type::VisualEffects];
	DEBUG_VALID(pcOperator);
	pcOperator->Request(cInObject);
}

// 2. Command Change 함수 처리
void KERNEL::DocViewImpl::CommandChange(Json::Object & cInObject)
{
	int nId = cInObject.GetInteger(SKW_ID);

	switch (nId)
	{
		case HOME_3D_LST_VisualEffects:
			ChangeVisualEffects(cInObject);
			break;

		default:
			assert(false);
			break;
	}
}

// 2-1. Visual Effects Change 요청 함수 처리
void KERNEL::DocViewImpl::ChangeVisualEffects(Json::Object & cInObject)
{
	Operator::VisualEffects * pcOperator = (Operator::VisualEffects *)m_apcOperator[(int)KERNEL::Operator::Type::VisualEffects];
	DEBUG_VALID(pcOperator);
	pcOperator->Change(cInObject);
}
