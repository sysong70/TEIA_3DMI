#include <StdAfx.h>

#include "Kernel.DocViewPrivate.h"

#include "../Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

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

	m_pcVisualEffectsSetting = TheKenel.VisualEffects.Get();
}

//== Visual Effects 관련 함수 ========================================================================

// 1. 그림자 효과 설정 함수
void KERNEL::DocViewPrivate::SetVisualEffectsShadow()
{
	bool bShadowFlag = !m_cCanvas.GetFrontView().GetSimpleShadow();

	m_cCanvas.GetFrontView().SetSimpleShadow(bShadowFlag);

	m_cCanvas.Update();
}

void KERNEL::DocViewPrivate::SetVisualEffectsReflection()
{
	bool bReflectionFlag = !m_cCanvas.GetFrontView().GetSimpleReflection();

	m_cCanvas.GetFrontView().SetSimpleReflection(bReflectionFlag);

	m_cCanvas.Update();
}

void KERNEL::DocViewPrivate::SetVisualEffectsAmbientOcclusion()
{
	H3DF::SegmentKey cViewSegment = m_cCanvas.GetFrontView().GetSegmentKey();

	bool bState = false;
	if (false == cViewSegment.GetVisualEffectsControl().ShowAmbientOcclusionEnabled(bState)) {
		DEBUG_RETURN;
	}

	cViewSegment.GetVisualEffectsControl().SetAmbientOcclusionEnabled(!bState);

	m_cCanvas.Update();
}

void KERNEL::DocViewPrivate::SetVisualEffectsSilhouetteEdges()
{
	H3DF::SegmentKey cViewSegment = m_cCanvas.GetFrontView().GetSegmentKey();

	bool bState = false;
	if (false == cViewSegment.GetVisualEffectsControl().ShowSilhouetteEdgesEnabled(bState)) {
		DEBUG_RETURN;
	}

	cViewSegment.GetVisualEffectsControl().SetSilhouetteEdgesEnabled(!bState);

	m_cCanvas.Update();
}

void KERNEL::DocViewPrivate::SetVisualEffectsBloom()
{
	H3DF::SegmentKey cViewSegment = m_cCanvas.GetFrontView().GetSegmentKey();

	bool bState = false;
	if (false == cViewSegment.GetVisualEffectsControl().ShowBloomEnabled(bState)) {
		DEBUG_RETURN;
	}

	cViewSegment.GetVisualEffectsControl().SetBloomEnabled(!bState);

	m_cCanvas.Update();
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

	m_pcObjectSnapOperator->SetObjectSnapMode(m_nOSnapMode);
}


//== Selection Filter 관련 함수 ======================================================================
void KERNEL::DocViewPrivate::SetSelectionFilter(SelectionFilter::Type eInType)
{
	// Selection filter type이 없는 경우 추가
	if (0 == (m_nSelectionFilter & (DWORD) eInType)) {
		m_nSelectionFilter += (DWORD) eInType;
	}
	else { // Selection filter type이 없는 경우 제거
		m_nSelectionFilter -= (DWORD) eInType;
	}

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

// 1. Visual Effects setting 요청 함수 처리
void KERNEL::DocViewPrivate::RequestVisualEffectsSetting(Json::Object & cInObject)
{
	Json::Object cData;
	
	cData.SetInteger(SKW_TARGET, (int) Signal::Target::TaskBar);
	cData.SetInteger(SKW_ACTION, (int) Signal::TaskBar::Action::ResponseValue);
	cData.SetInteger(SKW_ID, HOME_3D_LST_VisualEffects);

	// cInObject에 Value값이 있는 경우 처리 (UI에서 변경된 값을 전달한 경우 처리)
	Json::Object & cInValue = cInObject.GetObject(SKW_VALUE);
	if (false == cInValue.IsEmpty()) {
		CString strText1;
		cInValue.Stringify(strText1);
	}

	Json::Object * pcSetting = new Json::Object(*m_pcVisualEffectsSetting);
	Json::Object * pcDefault = TheKenel.VisualEffects.Get();

	cData.SetObject(SKW_VALUE, pcSetting);
	cData.SetObject(SKW_DEFAULTDATA, pcDefault);

	CString strText;
	cInObject.Stringify(strText);

	if(nullptr != m_pcDelivery) { 
		m_pcDelivery->SendData(cData);
	}
	else {
		DEBUG_RETURN;
	}
}

