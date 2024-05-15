#include <StdAfx.h>

#include "Kernel.DocViewImpl.h"
#include "../Kernel.DocView.h"

#include "../Command.Camera.h"
#include "../Command.VisualEffects.h"
#include "../Command.Attribute.h"

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

KERNEL::DocViewImpl::DocViewImpl()
{
	m_eType = KERNEL::Type::View;

	for (auto & pcOperator : m_apcOperator) {
		pcOperator = nullptr;
	}
}

KERNEL::DocViewImpl::~DocViewImpl()
{
	// Operator를 먼저 삭제해야 함.
	for (auto & pcOperator : m_apcOperator) {
		if (nullptr != pcOperator) {
			delete pcOperator;
		}
	}

	if (nullptr != m_pcCanvas) {
		delete m_pcCanvas;
	}
}

H3DF::Model & KERNEL::DocViewImpl::GetModel()
{
	DEBUG_VALID(m_pcCanvas);

	return m_pcCanvas->GetModel();
}

H3DF::CADModel & KERNEL::DocViewImpl::CADModel()
{
	return m_cCADModel;
}

H3DF::BaseView * KERNEL::DocViewImpl::GetBaseView()
{
	return GetCanvas().GetFrontView().GetWindowKey().GetBaseView();
}

Signal::Delivery & KERNEL::DocViewImpl::Delivery() 
{ 
	((Signal::Delivery *)m_pcDelivery)->ViewId = m_nViewId;
	return *(Signal::Delivery *)m_pcDelivery;
}

const Signal::Delivery & KERNEL::DocViewImpl::Delivery() const 
{ 
	((Signal::Delivery *)m_pcDelivery)->ViewId = m_nViewId;
	return *m_pcDelivery; 
}

void KERNEL::DocViewImpl::SetDelivery(const Signal::Delivery * pcInDelivery)
{
	m_pcDelivery = pcInDelivery;
}

void KERNEL::DocViewImpl::CancelCommands()
{
	m_cSelectionResult.Reset();
	Select().UnhighlightEverything();
	GetCanvas().GetFrontView().GetWindowKey().Update();
}

//== Operator 관련 함수 ==============================================================================

void KERNEL::DocViewImpl::AllocationOperator(const DocView * pcInDocView)
{
	m_apcOperator[(int)KERNEL::Command::Type::VisualEffects] = new KERNEL::Command::VisualEffects(pcInDocView);

	m_apcOperator[(int)KERNEL::Command::Type::Attribute] = new KERNEL::Command::Attribute(pcInDocView);

	m_apcOperator[(int)KERNEL::Command::Type::Camera] = new KERNEL::Command::Camera(pcInDocView);

	m_apcOperator[(int)KERNEL::Command::Type::Select] = new KERNEL::Command::Select(pcInDocView);
	
	m_apcOperator[(int)KERNEL::Command::Type::ModelPanel] = new KERNEL::Command::ModelPanel(pcInDocView);

	// Navigation Cube에서 사용하는 DynHighlightControl을 설정한다. Cube에서 선택된 부분을 Unhighlight하기 위함.
	KERNEL::Command::Select * pcSelect = (KERNEL::Command::Select *)m_apcOperator[(int)KERNEL::Command::Type::Select];
	GetCanvas().GetFrontView().GetNavigationCube().SetHighlightControl(pcSelect->DynHighlightControl());
}

KERNEL::Command::CommandBase * KERNEL::DocViewImpl::GetOperator(Command::Type eInType)
{ 
	return m_apcOperator[(int)eInType]; 
}

KERNEL::Command::Attribute & KERNEL::DocViewImpl::Attribute()
{
	return *(Command::Attribute *)m_apcOperator[(int)Command::Type::Attribute];
}

KERNEL::Command::Camera & KERNEL::DocViewImpl::Camera()
{
	return *(Command::Camera *)m_apcOperator[(int)Command::Type::Camera];
}

KERNEL::Command::Select & KERNEL::DocViewImpl::Select()
{
	return *(Command::Select *)m_apcOperator[(int)Command::Type::Select];
}

KERNEL::Command::ModelPanel & KERNEL::DocViewImpl::ModelPanel()
{
	return *(Command::ModelPanel *)m_apcOperator[(int)Command::Type::ModelPanel];
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

//== Attribute 관련 함수 ============================================================================

// 1. 전달받은 Attribute 명령어를 분기 처리하는 함수.
void KERNEL::DocViewImpl::SetVisibility(int nId)
{
	Command::Attribute * pcOperator = (Command::Attribute *)m_apcOperator[(int)KERNEL::Command::Type::Attribute];
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
	Command::VisualEffects * pcOperator = (Command::VisualEffects *) m_apcOperator[(int)KERNEL::Command::Type::VisualEffects];
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
	Command::VisualEffects * pcOperator = (Command::VisualEffects *)m_apcOperator[(int)KERNEL::Command::Type::VisualEffects];
	DEBUG_VALID(pcOperator);
	pcOperator->Change(cInObject);
}
