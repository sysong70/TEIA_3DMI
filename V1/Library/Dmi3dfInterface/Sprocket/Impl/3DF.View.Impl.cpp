#include "StdAfx.h"

#include "3DF.View.Impl.h"

#include "CanvasImpl.h"

#include "../3DF.Model.h"
#include "ModelImpl.h"

#include <hc.h>
#include <HTools.h>
#include <HBaseModel.h>
#include <HEventManager.h>
#include <HBhvBehaviorManager.h>
#include <HMarkupManager.h>
#include <HSharedKey.h>
#include <HUtilityGeomHandle.h>
#include <HEventListener.h>
#include <HOpCameraManipulate.h>
#include <HOpCameraOrbit.h>
#include <HOpCameraPan.h>
#include <HOpCameraZoom.h>
#include <HOpMoveHandle.h>
#include <HUndoManager.h>
#include <hic.h>
#include <HConstantFrameRate.h>

#include "Common_Define.h"

#include "../../3DF/Selection.h"
#include "../../3DF/Highlight.h"
#include "../../3DF/SelectionSet.h"

#include "../../3DF/NavigationCube.h"
#include "../../3DF/3DF.Utility.h"
#include "../../3DF/Facility.AppOptions.h"

#include "../../3DF/Operator.CameraControl.h"
#include "../../3DF/Operator.SelectArea.h"

#include "../../3DF/Window.h"
#include "../../3DF/Impl/WindowImpl.h"
#include "../../3DF/Visibility.h"
#include "../../3DF/Material.h"
#include "../../3DF/LineAttribute.h"
#include "../../3DF/DrawingAttribute.h"
#include "../../3DF/Impl/SegmentImpl.h"

#include "../../3DF/Database.h"
#include "../../3DF/Portfolio.h"

#include "../../Signal/Signal.h"

#define SEGMENT_TYPE						1
#define ENTITY_TYPE							2
#define SUBENTITY_TYPE						3
#define REGION_TYPE							4

#define	DEBUG_NO_WINDOWS_HOOK				0x00000040
#define DEBUG_STARTUP_CLEAR_BLACK			0x00004000
#define DEBUG_FORCE_SOFTWARE				0x01000000

using namespace H3DF;

#define TheKenel TheAppOptions.Kernel
#define ThePreset TheAppOptions.Preset

#define ColorValue(x) GetRValue(x) / 255.0f, GetGValue(x) / 255.0f, GetBValue(x) / 255.0f
#define ColorRGBA(x, alpha) GetRValue(x), GetGValue(x), GetBValue(x), (unsigned char)alpha

#ifndef PI
#	define PI 3.1415926535897932384626433832795028841971693993751
#endif

//== ViewPrivate Class =============================================================================

H3DF::ViewImpl::ViewImpl()
{
}

H3DF::ViewImpl::~ViewImpl()
{
}

void H3DF::ViewImpl::Copy(const ViewImpl * pcInThat)
{
	m_cWindow = pcInThat->m_cWindow;

	m_strName = pcInThat->m_strName;
	m_eRenderingMode = pcInThat->m_eRenderingMode;

	m_cKey = pcInThat->m_cKey;

	m_pcModel = pcInThat->m_pcModel;
	m_cModelKey = pcInThat->m_cModelKey;

	m_bSimpleShadowFlag = pcInThat->m_bSimpleShadowFlag;
	m_bSimpleReflection = pcInThat->m_bSimpleReflection;
}

SegmentKey H3DF::ViewImpl::GetSegmentKey()
{ 
	return m_cKey; 
}

H3DF::WindowKey & H3DF::ViewImpl::GetWindowKey()
{
	return m_cWindow;
}

H3DF::BaseView * H3DF::ViewImpl::GetBaseView()
{
	if (H3DF::Type::None == m_cWindow.Type()) {
		DEBUG_STOP;
		return nullptr;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) m_cWindow.GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	return pcBaseView;
}

bool H3DF::ViewImpl::GetKeyState(unsigned int key, int & flags)
{
	unsigned char state[256];
	flags = 0;
	GetKeyboardState(state);
	if (state[VK_LSHIFT] >= 128)
		flags |= MVO_LEFT_SHIFT;
	if (state[VK_SHIFT] >= 128)
		flags |= MVO_SHIFT;
	if (state[VK_RSHIFT] >= 128)
		flags |= MVO_RIGHT_SHIFT;
	if (state[VK_CONTROL] >= 128)
		flags |= MVO_CONTROL;
	if (state[VK_LMENU] >= 128 || state[VK_RMENU] >= 128)
		flags |= MVO_ALT;
	if (state[key] >= 128)
		return true;
	else
		return false;
}

bool H3DF::ViewImpl::GetSimpleShadow()
{
	return m_bSimpleShadowFlag;
}

void H3DF::ViewImpl::SetSimpleShadow(bool bFlag)
{
	m_bSimpleShadowFlag = bFlag;
}

bool H3DF::ViewImpl::GetSimpleReflection()
{
	return m_bSimpleReflection;
}

void H3DF::ViewImpl::SetSimpleReflection(bool bFlag)
{
	m_bSimpleReflection = bFlag;
}

//== Keyboard 관련 함수 ==============================================================================
bool H3DF::ViewImpl::Char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return true;
}

#include "../../3DF/Operator.KeyboardTest.h"
// TEMP
Operator::KeyboardTest * g_pOperator = nullptr;

bool H3DF::ViewImpl::KeyboardInput(Json::Object & cInObject)
{
	return false;
	using namespace Signal;

// 	if (g_pOperator == nullptr) {
// 		g_pOperator = new Operator::KeyboardTest(this->m_pcWindow);
// 	}

	Signal::View::Action action = (Signal::View::Action)cInObject.GetInteger(SKW_ACTION);

	switch (action) {
		case Signal::View::Action::OnChar:
		case Signal::View::Action::OnKeyDown:
		case Signal::View::Action::OnKeyUp:
		case Signal::View::Action::OnInput:
			g_pOperator->OnKeyboard(cInObject);
			break;

		default:
			return false;
	}

	return true;
}

DWORD H3DF::ViewImpl::MouseMapFlags(DWORD nState)
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