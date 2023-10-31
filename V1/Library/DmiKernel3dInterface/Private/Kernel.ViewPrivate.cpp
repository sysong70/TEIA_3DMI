#include <StdAfx.h>

#include "Kernel.ViewPrivate.h"

#include "../Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../../UiMain/Command.Resource.h"

#include "../../Dmi3dfInterface/3DF/Segment.h"
#include "../../Dmi3dfInterface/3DF/VisualEffects.h"

using namespace KERNEL;

//== Visual Effects 관련 함수 ========================================================================

// 1. 그림자 효과 설정 함수
void KERNEL::ViewPrivate::SetVisualEffectsShadow()
{
	bool bShadowFlag = !m_cCanvas.GetFrontView().GetSimpleShadow();

	m_cCanvas.GetFrontView().SetSimpleShadow(bShadowFlag);

	m_cCanvas.Update();
}

void KERNEL::ViewPrivate::SetVisualEffectsReflection()
{
	bool bReflectionFlag = !m_cCanvas.GetFrontView().GetSimpleReflection();

	m_cCanvas.GetFrontView().SetSimpleReflection(bReflectionFlag);

	m_cCanvas.Update();
}

void KERNEL::ViewPrivate::SetVisualEffectsAmbientOcclusion()
{
	H3DF::SegmentKey cViewSegment = m_cCanvas.GetFrontView().GetSegmentKey();

	bool bState = false;
	cViewSegment.GetVisualEffectsControl().ShowAmbientOcclusionEnabled(bState);

	cViewSegment.GetVisualEffectsControl().SetAmbientOcclusionEnabled(!bState);

	m_cCanvas.Update();
}