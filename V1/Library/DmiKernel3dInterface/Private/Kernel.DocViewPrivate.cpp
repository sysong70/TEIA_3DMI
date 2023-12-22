#include <StdAfx.h>

#include "Kernel.DocViewPrivate.h"

#include "../Operator.VisualEffects.h"

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

KERNEL::DocViewPrivate::DocViewPrivate()
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

Signal::Delivery & KERNEL::DocViewPrivate::Delivery() 
{ 
	return *(Signal::Delivery *)m_pcDelivery;
}

const Signal::Delivery & KERNEL::DocViewPrivate::Delivery() const 
{ 
	return *m_pcDelivery; 
}


void KERNEL::DocViewPrivate::SetDelivery(const Signal::Delivery * pcInDelivery)
{
	m_pcDelivery = pcInDelivery;
}

//== Operator 관련 함수 ==============================================================================

void KERNEL::DocViewPrivate::AllocationOperator(H3DF::View * pcInView, Signal::Delivery & cDelivery)
{
	// Highlihgt Object Snap Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::HighlightObjectSnap] = new KERNEL::Operator::HighlightObjectSnap(pcInView, &cDelivery);
	((Operator::HighlightObjectSnap *)m_apcOperator[(int)KERNEL::Operator::Type::HighlightObjectSnap])->SetObjectSnapMode(m_nOSnapMode);
	((Operator::HighlightObjectSnap *)m_apcOperator[(int)KERNEL::Operator::Type::HighlightObjectSnap])->SetSelectionFilter(m_nSelFilter);

	// VisualEffects Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::VisualEffects] = new KERNEL::Operator::VisualEffects(pcInView, &cDelivery);
}

KERNEL::Operator::OperatorBase * KERNEL::DocViewPrivate::GetOperator(Operator::Type eInType)
{ 
	return m_apcOperator[(int)eInType]; 
}

KERNEL::Operator::HighlightObjectSnap & KERNEL::DocViewPrivate::HighlightOSnapOperator()
{ 
	return *(Operator::HighlightObjectSnap *)m_apcOperator[(int)Operator::Type::HighlightObjectSnap]; 
}

//== Object Snap 관련 함수 ===========================================================================
void KERNEL::DocViewPrivate::SetObjectSnap(OSnap::Type eInType)
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
void KERNEL::DocViewPrivate::SetSelectionFilter(SelectionFilter::Type eInType)
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

//== Command 관련 함수 ===============================================================================

// 1. Command Request 함수 처리
void KERNEL::DocViewPrivate::CommandRequest(Json::Object & cInObject)
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
void KERNEL::DocViewPrivate::RequestVisualEffects(Json::Object & cInObject)
{
	Operator::VisualEffects * pcOperator = (Operator::VisualEffects *) m_apcOperator[(int)KERNEL::Operator::Type::VisualEffects];
	DEBUG_VALID(pcOperator);
	pcOperator->Request(cInObject);
}

// 2. Command Change 함수 처리
void KERNEL::DocViewPrivate::CommandChange(Json::Object & cInObject)
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
void KERNEL::DocViewPrivate::ChangeVisualEffects(Json::Object & cInObject)
{
	Operator::VisualEffects * pcOperator = (Operator::VisualEffects *)m_apcOperator[(int)KERNEL::Operator::Type::VisualEffects];
	DEBUG_VALID(pcOperator);
	pcOperator->Change(cInObject);
}
