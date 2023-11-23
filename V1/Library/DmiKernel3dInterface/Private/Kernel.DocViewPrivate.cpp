#include <StdAfx.h>

#include "Kernel.DocViewPrivate.h"

#include "../Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../../UiMain/Command.Resource.h"

#include "../../Dmi3dfInterface/3DF/Segment.h"
#include "../../Dmi3dfInterface/3DF/VisualEffects.h"

using namespace KERNEL;

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
