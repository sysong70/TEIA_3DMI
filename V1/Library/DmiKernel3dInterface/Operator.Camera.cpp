#include "StdAfx.h"

#include "OPERATOR.Camera.h"

#include "Impl/OperatorImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <3DF.View.h>
#include <3DF/Window.h>
#include <3DF/VisualEffects.h>
#include <3DF/Facility.AppOptions.h>
#include <3DF/3DF.Operator.CameraControl.h>

#include <Json.h>

using namespace KERNEL;

namespace KERNEL
{
	namespace Operator
	{
		class CameraImpl : public OperatorImpl
		{
		public:
			CameraImpl(const DocView * pcInDocView);
			~CameraImpl();

			void Copy(CameraImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			H3DF::Operator::CameraControl & CameraControl() { return *m_pcCameraControl; }
			H3DF::Camera::Mode CameraMode();

		protected:
			H3DF::Operator::CameraControl * m_pcCameraControl = nullptr;
		};
	}
}

KERNEL::Operator::CameraImpl::CameraImpl(const DocView * pcInDocView)
	: OperatorImpl(pcInDocView)
{
	m_pcCameraControl = new H3DF::Operator::CameraControl(Window(), View().GetNavigationCube());
}

KERNEL::Operator::CameraImpl::~CameraImpl()
{
	if (nullptr != m_pcCameraControl) {
		delete m_pcCameraControl;
		m_pcCameraControl = nullptr;
	}
}

//== View Control 관련 함수 ==========================================================================

H3DF::Camera::Mode KERNEL::Operator::CameraImpl::CameraMode()
{
	return CameraControl().CameraMode();
}

//== Camera class ==================================================================================

KERNEL::Operator::Camera::Camera(const DocView * pcInDocView)
{
	CameraImpl * pcImpl = new CameraImpl(pcInDocView);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	pcImpl->CameraControl();
}

int KERNEL::Operator::Camera::MouseMove(HEventInfo & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (MVO_LBUTTON & cInEvent.GetFlags()) {
		return pcImpl->CameraControl().LButtonDownAndMove(cInEvent);
	}
	else if (MVO_RBUTTON & cInEvent.GetFlags()) {
		return pcImpl->CameraControl().RButtonDownAndMove(cInEvent);
	}

	return 0;
}

int KERNEL::Operator::Camera::LButtonDown(HEventInfo & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->CameraControl().LButtonDown(cInEvent);
}

int KERNEL::Operator::Camera::LButtonUp(HEventInfo & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->CameraControl().LButtonUp(cInEvent);
}

int KERNEL::Operator::Camera::RButtonDown(HEventInfo & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->CameraControl().RButtonDown(cInEvent);
}

int KERNEL::Operator::Camera::RButtonUp(HEventInfo & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->CameraControl().RButtonUp(cInEvent);
}

int KERNEL::Operator::Camera::MouseWheel(HEventInfo & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->CameraControl().MouseWheel(cInEvent);
}

//== View Control 관련 함수 ==========================================================================

H3DF::Camera::Mode KERNEL::Operator::Camera::CameraMode()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->CameraMode();
}

void KERNEL::Operator::Camera::SetPanViewControl()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (H3DF::Camera::Mode::Pan != pcImpl->CameraMode()) {
		pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::Pan);
	}
	else {
		pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::Multi);
	}
}

void KERNEL::Operator::Camera::SetOrbitViewControl()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (H3DF::Camera::Mode::Orbit != pcImpl->CameraMode()) {
		pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::Orbit);
	}
	else {
		pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::Multi);
	}
}

void KERNEL::Operator::Camera::SetOrbitTurntableViewControl()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);
	
	if (H3DF::Camera::Mode::OrbitTurntable != pcImpl->CameraMode()) {
		pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::OrbitTurntable);
	}
	else {
		pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::Multi);
	}
}

void KERNEL::Operator::Camera::SetZoomArea()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (H3DF::Camera::Mode::ZoomBox != pcImpl->CameraMode()) {
		pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::ZoomBox);
	}
	else {
		pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::Multi);
	}
}

void KERNEL::Operator::Camera::FitWorld()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::Multi);
	pcImpl->CameraControl().FitWorld();
}
