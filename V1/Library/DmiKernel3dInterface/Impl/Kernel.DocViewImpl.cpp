#include <StdAfx.h>

#include "Kernel.DocViewImpl.h"
#include "../Kernel.DocView.h"

#include "../Operator.Camera.h"
#include "../Operator.VisualEffects.h"
#include "../Operator.Attribute.h"

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

H3DF::BaseView * KERNEL::DocViewImpl::GetBaseView()
{
	return GetCanvas().GetFrontView().GetWindowKey().GetBaseView();
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
	Select().UnhighlightEverything();
	GetCanvas().GetFrontView().GetWindowKey().Update();
}

void KERNEL::DocViewImpl::AllocationOperator(const DocView * pcInDocView)
{
	// VisualEffects Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::VisualEffects] = new KERNEL::Operator::VisualEffects(pcInDocView);

	// Attribute Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::Attribute] = new KERNEL::Operator::Attribute(pcInDocView);

	// Camera Operator 생성 및 설정
	m_apcOperator[(int)KERNEL::Operator::Type::Camera] = new KERNEL::Operator::Camera(pcInDocView);

	// Camera Operator 생성 및 설정
	KERNEL::Operator::Select * pcSelect = new KERNEL::Operator::Select(pcInDocView);
	m_apcOperator[(int)KERNEL::Operator::Type::Select] = pcSelect;
	
	// Model Panel Operator 생성 및 설정
	KERNEL::Operator::ModelPanel * pcModelPanel = new KERNEL::Operator::ModelPanel(pcInDocView);
	pcModelPanel->SetSelect((KERNEL::Operator::Select *)m_apcOperator[(int)KERNEL::Operator::Type::Select]);
	m_apcOperator[(int)KERNEL::Operator::Type::ModelPanel] = pcModelPanel;

	pcSelect->SetModelPanel(pcModelPanel);

	// Navigation Cube에서 사용하는 DynHighlightControl을 설정한다. Cube에서 선택된 부분을 Unhighlight하기 위함.
	GetCanvas().GetFrontView().GetNavigationCube().SetHighlightControl(pcSelect->DynHighlightControl());
}

KERNEL::Operator::OperatorBase * KERNEL::DocViewImpl::GetOperator(Operator::Type eInType)
{ 
	return m_apcOperator[(int)eInType]; 
}

KERNEL::Operator::Camera & KERNEL::DocViewImpl::Camera()
{
	return *(Operator::Camera *)m_apcOperator[(int)Operator::Type::Camera];
}

KERNEL::Operator::Select & KERNEL::DocViewImpl::Select()
{
	return *(Operator::Select *)m_apcOperator[(int)Operator::Type::Select];
}

KERNEL::Operator::ModelPanel & KERNEL::DocViewImpl::ModelPanel()
{
	return *(Operator::ModelPanel *)m_apcOperator[(int)Operator::Type::ModelPanel];
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
	Operator::Attribute * pcOperator = (Operator::Attribute *)m_apcOperator[(int)KERNEL::Operator::Type::Attribute];
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
			pcOperator->ShowToggle();
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
