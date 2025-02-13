#include <StdAfx.h>

#include "Kernel.Session.Impl.h"
#include "../Kernel.Session.h"

#include "../Command.Camera.h"
#include "../Command.VisualEffects.h"
#include "../Command.Attribute.h"
#include "../Command.PMI.Distance.h"
#include "../Command.Event.h"

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

using namespace H3DF;
using namespace KERNEL;


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

H3DF::WindowKey & KERNEL::SessionImpl::Window()
{
	DEBUG_VALID(m_pcCanvas);
	return m_pcCanvas->GetFrontView().GetWindowKey();
}

const H3DF::WindowKey & KERNEL::SessionImpl::Window() const
{
	DEBUG_VALID(m_pcCanvas);
	return m_pcCanvas->GetFrontView().GetWindowKey();
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
bool KERNEL::SessionImpl::MouseMove(int nFlag, int x, int y)
{
	Command::Event cEvent(Window());
	cEvent.SetPoint(H3DF::Operator::Event::Type::MouseMove, x, y, MouseMapFlags(nFlag));

	// View Control Mouse Event 처리 함수, Mouse L, R Button Down 상태로 마우스를 이동하면, View Control을 사용하고 있는 것으로
	// 간주하여 Cosume Type을 리턴한다.
	if (Command::Result::Type::Consume == CameraControlMouseMove(cEvent)) {
		return true;
	}

	// Select(Object Snap) 
	SelectControlMouseMove(cEvent);

	// Command가 설정되었다면, Command에 명령어를 처리할 수 있도록 좌표를 전달한다.
	if (false == IsCommandActive()) {
		return true;
	}

	// 현재 활성화되어 있는 Command에 Left button up 이벤트 전달.
	CommandMouseMove(cEvent);

	return true;
}

bool KERNEL::SessionImpl::LButtonDown(int nFlag, int x, int y)
{
	Command::Event cEvent(Window());
	cEvent.SetPoint(H3DF::Operator::Event::Type::LButtonDown, x, y, MouseMapFlags(nFlag));

	// Camera 및 Select Control에서는 L Button Click 위치를 저장해 놓는 단순한 역활만 수행함.
	// View Control Mouse Event 처리 함수
	CameraControlLButtonDown(cEvent);

	// Select(Object Snap) 처리 함수
	SelectControlLButtonDown(cEvent);

	return true;
}

bool KERNEL::SessionImpl::LButtonUp(int nFlag, int x, int y)
{
	Command::Event cEvent(Window());
	cEvent.SetPoint(H3DF::Operator::Event::Type::LButtonUp, x, y, MouseMapFlags(nFlag));

	// View Control Mouse Event 처리 함수
	if (Command::Result::Type::Consume == CameraControlLButtonUp(cEvent)) {
		return true;
	}

	// Select(Object Snap) 및 View Control Mouse Event 처리 함수
	SelectControlLButtonUp(cEvent);

	// Command가 설정되었다면, Command에 명령어를 처리할 수 있도록 좌표를 전달한다.
	if(false == IsCommandActive()) {
		return true;
	}

	// 현재 활성화되어 있는 Command에 Left button up 이벤트 전달.
	CommandLButtonUp(cEvent);

	// Drag 상태를 확인하도록 한다. 명령어는 Mouse Drag 상태에서는 사용하지 않도록 한다.
	// Current command에 Input 상태를 확인해야 함.

	// Current command가 Setting되어 있는 경우에, Request Value에 Coordinate가 있는 경우 Command에 Input Coordinate를 전달한다.
	// pcImpl->CommandRequestCoordinate(cEvent);

	return true;
}

DWORD KERNEL::SessionImpl::MouseMapFlags(DWORD nState) 
{
	DWORD nFlag = 0;

	// map the mfc events state to MVO
	if (nState & MK_LBUTTON) nFlag |= (DWORD) H3DF::Operator::Event::Flag::LeftButton;
	if (nState & MK_RBUTTON) nFlag |= (DWORD) H3DF::Operator::Event::Flag::RightButton;
	if (nState & MK_MBUTTON) nFlag |= (DWORD) H3DF::Operator::Event::Flag::MiddleButton;
	if (nState & MK_SHIFT) nFlag |= (DWORD) H3DF::Operator::Event::Flag::Shift;
	if (nState & MK_CONTROL) nFlag |= (DWORD) H3DF::Operator::Event::Flag::Control;

	return nFlag;
}
//== View Control 관련 함수 ==========================================================================
Command::Result::Type KERNEL::SessionImpl::SelectControlMouseMove(Command::Event & cInEvent)
{
	if (200 > GetTickCount() - m_nMouseWhellStartTick) {
		return Command::Result::Type::Pass;
	}

	Select().MouseMove(cInEvent);

	return Command::Result::Type::Pass;
}

bool KERNEL::SessionImpl::SelectControlLButtonDown(Command::Event & cInEvent)
{
	// Camera 및 Select 처리
	m_cLButtonDownPixelPoint = cInEvent.GetMousePixelPoint();
	Select().SetMouseDownTickCount(GetTickCount64());

	// 카메라에서 LButtonDown 처리는 First Point등을 설정하는 것임.
	// Camera().LButtonDown(cInEvent);

	// Select에서 LButtonDown 처리는 버튼 위치를 저장하는 것임.
	Select().LButtonDown(cInEvent);

	return true;
}

// 3. Select 및 View Control Mouse Event 처리 함수 #Selection
Command::Result::Type KERNEL::SessionImpl::SelectControlLButtonUp(Command::Event & cInEvent)
{
	Select().SetMouseUpTickCount(GetTickCount64());
/*

	// Camera 관련 처리
	H3DF::Camera::Mode eMode = Camera().CameraMode();

	if (H3DF::Camera::Mode::ZoomBox == eMode) {
		GetCanvas().GetFrontView().SuppressUpdate(true);
	}

	// NavigationCube가 선택된 경우를 처리한다. NavigationCube가 선택되어 View를 변경한 경우에는 
	// HLISTENER_CONSUME_EVENT값을 리턴한다.
	if (Command::Result::Type::Consume == Camera().LButtonUp(cInEvent)) {
		return Command::Result::Type::Consume;
	}

	if (H3DF::Camera::Mode::ZoomBox == eMode) {
		Select().DrawSnapItems();
		GetCanvas().GetFrontView().SuppressUpdate(false);
		GetCanvas().GetFrontView().Update();

		return Command::Result::Type::Consume;
	}
*/

	// 카메라 처리가 끝나면 Select 처리를 한다.
	// 앞단에서, NavigationCube가 선택되어 View가 변경된 경우에는 Select 처리를 하지 않는다.
	Select().LButtonUp(cInEvent);

	// 선택된 값을 판단해서 InputType을 리턴한다.

	return Command::Result::Type::Pass;
}

//== Camera Control 관련 함수 ========================================================================
Command::Result::Type KERNEL::SessionImpl::CameraControlMouseMove(Command::Event & cInEvent)
{
	if (200 > GetTickCount() - m_nMouseWhellStartTick) {
		return Command::Result::Type::Pass;
	}

	return Camera().MouseMove(cInEvent);
}

Command::Result::Type KERNEL::SessionImpl::CameraControlLButtonDown(Command::Event & cInEvent)
{
	// Camera 및 Select 처리
	m_cLButtonDownPixelPoint = cInEvent.GetMousePixelPoint();
	Select().SetMouseDownTickCount(GetTickCount64());

	// 카메라에서 LButtonDown 처리는 First Point등을 설정하는 것임.
	return Camera().LButtonDown(cInEvent);

	// Select에서 LButtonDown 처리는 버튼 위치를 저장하는 것임.
	// Select().LButtonDown(cInEvent);
}

// 3. Select 및 View Control Mouse Event 처리 함수 
Command::Result::Type KERNEL::SessionImpl::CameraControlLButtonUp(Command::Event & cInEvent)
{
	// Camera 관련 처리
	H3DF::Camera::Mode eMode = Camera().CameraMode();

	if (H3DF::Camera::Mode::ZoomBox == eMode) {
		GetCanvas().GetFrontView().SuppressUpdate(true);
	}

	// NavigationCube가 선택된 경우를 처리한다. NavigationCube가 선택되어 View를 변경한 경우에는 
	// HLISTENER_CONSUME_EVENT값을 리턴한다.
	if (Command::Result::Type::Consume == Camera().LButtonUp(cInEvent)) {
		return Command::Result::Type::Consume;
	}

	if (H3DF::Camera::Mode::ZoomBox == eMode) {
		Select().DrawSnapItems();
		GetCanvas().GetFrontView().SuppressUpdate(false);
		GetCanvas().GetFrontView().Update();

		return Command::Result::Type::Consume;
	}

	// 카메라 처리가 끝나면 Select 처리를 한다.
	// 앞단에서, NavigationCube가 선택되어 View가 변경된 경우에는 Select 처리를 하지 않는다.
	// Select().LButtonUp(cInEvent);

	// 선택된 값을 판단해서 InputType을 리턴한다.

	return Command::Result::Type::Pass;
}

//== Attribute 관련 함수 =============================================================================

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
void KERNEL::SessionImpl::SetCommand(int nInCommandId)
{
	// Command를 설정하기 전에 기존 Command를 초기화한다.
	m_vpcCommandSets.clear();

	switch (nInCommandId)
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

//== Command 관련 함수 ===============================================================================
bool KERNEL::SessionImpl::IsCommandActive()
{
	return !m_vpcCommandSets.empty();
}

bool KERNEL::SessionImpl::CommandLButtonUp(Command::Event & cInEvent)
{
	m_vpcCommandSets.front()->EventExecution(cInEvent);
/*
	for (auto & pcCommand : m_vpcCommandSets) {
		pcCommand->LButtonUp(cEvent);
	}
*/

	return true;
}

bool KERNEL::SessionImpl::CommandMouseMove(Command::Event & cInEvent)
{
	// 첫번째 Command에 MouseMove 이벤트를 전달한다.
	m_vpcCommandSets.front()->EventExecution(cInEvent);

	return true;
}