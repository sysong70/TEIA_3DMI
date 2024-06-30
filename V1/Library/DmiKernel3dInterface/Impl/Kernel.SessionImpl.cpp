#include <StdAfx.h>

#include "Kernel.SessionImpl.h"
#include "../Kernel.Session.h"

#include "../Command.Camera.h"
#include "../Command.VisualEffects.h"
#include "../Command.Attribute.h"
#include "../Command.PMI.Distance.h"

#include "../Signal.Connector.h"
#include "../../Signal/Signal.h"
#include "../../Common/Common_Define.h"

#include "../../../UiMain/Command.Resource.h"

#include <3DF/Segment.h>
#include <3DF/VisualEffects.h>
#include <3DF/Visibility.h>
#include <3DF/LineAttribute.h>
#include <3DF/AttributeLock.h>
#include <3DF/NavigationCube.h>

using namespace KERNEL;
using namespace H3DF;

#define TheKenel TheAppOptions.Kernel

//== Visual Effects 관련 함수 ========================================================================

KERNEL::SessionImpl::SessionImpl()
{
	m_eType = KERNEL::Type::View;
}

KERNEL::SessionImpl::~SessionImpl()
{
	for (auto & cIterator : m_mpcCommandMap) {
		Command::Set * pcOperator = cIterator.second;
		if (nullptr != pcOperator) {
			delete pcOperator;
		}
	}

	m_mpcCommandMap.clear();

	if (nullptr != m_pcCanvas) {
		delete m_pcCanvas;
	}
}

H3DF::Model & KERNEL::SessionImpl::GetModel()
{
	DEBUG_VALID(m_pcCanvas);

	return m_pcCanvas->GetModel();
}

H3DF::CADModel & KERNEL::SessionImpl::CADModel()
{
	return m_cCADModel;
}

H3DF::BaseView * KERNEL::SessionImpl::GetBaseView()
{
	return GetCanvas().GetFrontView().GetWindowKey().GetBaseView();
}

Signal::Delivery & KERNEL::SessionImpl::Delivery() 
{ 
	((Signal::Delivery *)m_pcDelivery)->ViewId = m_nViewId;
	return *(Signal::Delivery *)m_pcDelivery;
}

const Signal::Delivery & KERNEL::SessionImpl::Delivery() const 
{ 
	((Signal::Delivery *)m_pcDelivery)->ViewId = m_nViewId;
	return *m_pcDelivery; 
}

void KERNEL::SessionImpl::SetDelivery(const Signal::Delivery * pcInDelivery)
{
	m_pcDelivery = pcInDelivery;
}

void KERNEL::SessionImpl::CancelCommands()
{
	m_cSelectionResult.Reset();
	Select().UnhighlightEverything();
	GetCanvas().GetFrontView().GetWindowKey().Update();
}

//== Operator 관련 함수 ==============================================================================

void KERNEL::SessionImpl::AllocationOperator(const Session * pcInSession)
{
	m_mpcCommandMap.insert(std::make_pair(KERNEL::Command::Type::VisualEffects, new KERNEL::Command::VisualEffects(pcInSession)));
	// )[(int)KERNEL::Command::Type::VisualEffects] = new KERNEL::Command::VisualEffects(pcInSession);

	m_mpcCommandMap.insert(std::make_pair(KERNEL::Command::Type::Attribute, new KERNEL::Command::Attribute(pcInSession)));

	m_mpcCommandMap.insert(std::make_pair(KERNEL::Command::Type::Camera, new KERNEL::Command::Camera(pcInSession)));

	m_mpcCommandMap.insert(std::make_pair(KERNEL::Command::Type::Select, new KERNEL::Command::Select(pcInSession)));

	m_mpcCommandMap.insert(std::make_pair(KERNEL::Command::Type::ModelPanel, new KERNEL::Command::ModelPanel(pcInSession)));

	m_mpcCommandMap.insert(std::make_pair(KERNEL::Command::Type::PMI_Distance, new KERNEL::Command::PMI::Distance(pcInSession)));

	// Navigation Cube에서 사용하는 DynHighlightControl을 설정한다. Cube에서 선택된 부분을 Unhighlight하기 위함.
	KERNEL::Command::Select * pcSelect = (KERNEL::Command::Select *)m_mpcCommandMap[KERNEL::Command::Type::Select];
	GetCanvas().GetFrontView().GetNavigationCube().SetHighlightControl(pcSelect->DynHighlightControl());
}

KERNEL::Command::Set * KERNEL::SessionImpl::GetOperator(Command::Type eInType)
{ 
	return m_mpcCommandMap[eInType]; 
}

KERNEL::Command::Attribute & KERNEL::SessionImpl::Attribute()
{
	return *(Command::Attribute *)m_mpcCommandMap[Command::Type::Attribute];
}

KERNEL::Command::Camera & KERNEL::SessionImpl::Camera()
{
	return *(Command::Camera *)m_mpcCommandMap[Command::Type::Camera];
}

KERNEL::Command::Select & KERNEL::SessionImpl::Select()
{
	return *(Command::Select *)m_mpcCommandMap[Command::Type::Select];
}

KERNEL::Command::ModelPanel & KERNEL::SessionImpl::ModelPanel()
{
	return *(Command::ModelPanel *)m_mpcCommandMap[Command::Type::ModelPanel];
}

//== Mouse 관련 함수 =================================================================================

// 1. Mouse Move 함수 처리

bool KERNEL::SessionImpl::SelectViewControlMouseMove(int nFlag, int x, int y)
{
	if (200 > GetTickCount() - m_nMouseWhellStartTick) {
		return false; 
	}

	DWORD nNewFlags = MouseMapFlags(nFlag);
	HEventInfo cEvent((HBaseView *) GetBaseView());
	cEvent.SetPoint(HE_MouseMove, x, y, nNewFlags);

	Camera().MouseMove(cEvent);

	Select().MouseMove(cEvent);

	return true;
}

bool KERNEL::SessionImpl::MouseMove(int nFlag, int x, int y)
{
	if (true == m_vpcCommandSets.empty()) {
		return false;
	}

	return true;
}

// 2. Left Button Down 함수 처리
bool KERNEL::SessionImpl::SelectViewControlLButtonDown(int nFlag, int x, int y)
{
	// Camera 및 Select 처리
	m_cLButtonDownPosition.Set(x, y);
	Select().SetMouseDownTickCount(GetTickCount64());

	HEventInfo cEvent((HBaseView *) GetBaseView());
	cEvent.SetPoint(HE_LButtonDown, x, y, MouseMapFlags(nFlag));

	// 카메라에서 LButtonDown 처리는 First Point등을 설정하는 것임.
	Camera().LButtonDown(cEvent);

	// Select에서 LButtonDown 처리는 버튼 위치를 저장하는 것임.
	Select().LButtonDown(cEvent);

	return true;
}

bool KERNEL::SessionImpl::LButtonDown(int nFlag, int x, int y)
{
	if (true == m_vpcCommandSets.empty()) {
		return false;
	}

	return true;
}

// 3. Left Button Up 함수 처리

// 3-1. Select 및 View Control Mouse Event 처리 함수
bool KERNEL::SessionImpl::SelectViewControlLButtonUp(int nFlag, int x, int y)
{
	H3DF::Point2D cLButtonUpPosition(x, y);
	Select().SetMouseUpTickCount(GetTickCount64());

	GetCanvas().GetFrontView().GetWindowKey().GetBaseView();

	// Camera 관련 처리
	H3DF::Camera::Mode eMode = Camera().CameraMode();

	if (H3DF::Camera::Mode::ZoomBox == eMode) {
		GetCanvas().GetFrontView().SetSuppressUpdate(true);
	}

	HEventInfo cEvent((HBaseView *) GetBaseView());
	cEvent.SetPoint(HE_LButtonUp, x, y, MouseMapFlags(nFlag));

	// NavigationCube가 선택된 경우를 처리한다. NavigationCube가 선택되어 View를 변경한 경우에는 
	// HLISTENER_CONSUME_EVENT값을 리턴한다.
	if (HLISTENER_CONSUME_EVENT == Camera().LButtonUp(cEvent)) {
		return false;
	}

	if (H3DF::Camera::Mode::ZoomBox == eMode) {
		Select().DrawSnapItems();
		GetCanvas().GetFrontView().SetSuppressUpdate(false);
		GetCanvas().GetFrontView().Update();
	}

	// 카메라 처리가 끝나면 Select 처리를 한다.
	// 앞단에서, NavigationCube가 선택되어 View가 변경된 경우에는 Select 처리를 하지 않는다.
	Select().LButtonUp(cEvent);

	return true;
}

bool KERNEL::SessionImpl::LButtonUp(int nFlag, int x, int y)
{
	if (true == m_vpcCommandSets.empty()) {
		return false;
	}

	return true;
}

DWORD KERNEL::SessionImpl::MouseMapFlags(DWORD nState)
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

//== Attribute 관련 함수 ============================================================================

// 1. 전달받은 Attribute 명령어를 분기 처리하는 함수.
void KERNEL::SessionImpl::SetVisibility(int nId)
{
	Command::Attribute * pcOperator = (Command::Attribute *)m_mpcCommandMap[KERNEL::Command::Type::Attribute];
	DEBUG_VALID(pcOperator);

	switch (nId)
	{
		case HOME_3D_CMD_Visualize_ShowAll:
			pcOperator->ShowAll();
			break;

		case HOME_3D_CMD_Visualize_Hide:
			pcOperator->HideOnly();
			break;

		case HOME_3D_CMD_Visualize_ShowOnly:
			pcOperator->ShowOnly();
			break;

		case HOME_3D_CMD_Visualize_Toggle:
			pcOperator->ShowToggle();
			break;

		case HOME_3D_CMD_Visualize_Reset:
			pcOperator->ShowReset();
			break;

		default:
			assert(false);
			break;
	}
}

//== Command 관련 함수 ===============================================================================

// 1. Command Request 함수 처리
void KERNEL::SessionImpl::CommandRequest(Json::Object & cInObject)
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
void KERNEL::SessionImpl::RequestVisualEffects(Json::Object & cInObject)
{
	Command::VisualEffects * pcOperator = (Command::VisualEffects *) m_mpcCommandMap[KERNEL::Command::Type::VisualEffects];
	DEBUG_VALID(pcOperator);
	pcOperator->Request(cInObject);
}

// 2. Command Change 함수 처리
void KERNEL::SessionImpl::CommandChange(Json::Object & cInObject)
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
void KERNEL::SessionImpl::ChangeVisualEffects(Json::Object & cInObject)
{
	Command::VisualEffects * pcOperator = (Command::VisualEffects *)m_mpcCommandMap[KERNEL::Command::Type::VisualEffects];
	DEBUG_VALID(pcOperator);
	pcOperator->Change(cInObject);
}

// 3. 사용할 Command를 설정하는 함수 #Command
void KERNEL::SessionImpl::SetCommand(int nId)
{
	// Command를 설정하기 전에 기존 Command를 초기화한다.
	m_vpcCommandSets.clear();

	switch (nId)
	{
		case MEASURE_3D_CMD_Basic_Distance: {
			Command::PMI::Distance * pcCommand = (Command::PMI::Distance *)m_mpcCommandMap[KERNEL::Command::Type::PMI_Distance];
			DEBUG_VALID(pcCommand);

			// Command를 설정
			m_vpcCommandSets.push_back(pcCommand);
//			pcOperator->Set();
		} break;

		default:
			break;
	}
}