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
#include <3DF/Operator.CameraControl.h>

#include <Json.h>

using namespace KERNEL;

namespace KERNEL
{
	namespace Operator
	{
		class CameraImpl : public OperatorImpl
		{
		public:
			CameraImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			void Copy(CameraImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
			}

			int MouseMove(int nFlags, int x, int y);
			int LButtonDown(int nFlags, int x, int y);
			int LButtonUp(int nFlags, int x, int y);
			int RButtonDown(int nFlags, int x, int y);
			int RButtonUp(int nFlags, int x, int y);
			int MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop);

			H3DF::Operator::CameraControl & CameraControl() { return m_cCameraControl; }
			H3DF::Camera::Mode CameraMode();

		protected:
			H3DF::Operator::CameraControl m_cCameraControl;
		};
	}
}

KERNEL::Operator::CameraImpl::CameraImpl(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
	: OperatorImpl(pcInView, pcInDelivery),
	m_cCameraControl(pcInView->GetWindowKey(), pcInView->GetNavigationCube())
{
}

int KERNEL::Operator::CameraImpl::MouseMove(int nFlags, int x, int y)
{
	if (MK_LBUTTON & nFlags) {
		return m_cCameraControl.LButtonDownAndMove(nFlags, x, y);
	}
	else if (MK_RBUTTON & nFlags) {
		return m_cCameraControl.RButtonDownAndMove(nFlags, x, y);
	}

	return 0;
}

int KERNEL::Operator::CameraImpl::LButtonDown(int nFlags, int x, int y)
{
	return m_cCameraControl.LButtonDown(nFlags, x, y);
}

int KERNEL::Operator::CameraImpl::LButtonUp(int nFlags, int x, int y)
{
	return m_cCameraControl.LButtonUp(nFlags, x, y);
}

int KERNEL::Operator::CameraImpl::RButtonDown(int nFlags, int x, int y)
{
	return m_cCameraControl.RButtonDown(nFlags, x, y);
}

int KERNEL::Operator::CameraImpl::RButtonUp(int nFlags, int x, int y)
{
	return m_cCameraControl.RButtonUp(nFlags, x, y);
}

int KERNEL::Operator::CameraImpl::MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop)
{
	return m_cCameraControl.MouseWheel(nFlags, zDelta, x, y, nLeft, nTop);
}

//== View Control 관련 함수 ==========================================================================

H3DF::Camera::Mode KERNEL::Operator::CameraImpl::CameraMode()
{
	return m_cCameraControl.CameraMode();
}

//== Camera class ==================================================================================

KERNEL::Operator::Camera::Camera(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery)
{
	CameraImpl * pcImpl = new CameraImpl(pcInView, pcInDelivery);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;
}

int KERNEL::Operator::Camera::MouseMove(int nFlags, int x, int y)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (MK_LBUTTON & nFlags) {
		return pcImpl->CameraControl().LButtonDownAndMove(nFlags, x, y);
	}
	else if (MK_RBUTTON & nFlags) {
		return pcImpl->CameraControl().RButtonDownAndMove(nFlags, x, y);
	}

	return 0;
}

int KERNEL::Operator::Camera::LButtonDown(int nFlags, int x, int y)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->LButtonDown(nFlags, x, y);
}

int KERNEL::Operator::Camera::LButtonUp(int nFlags, int x, int y)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->LButtonUp(nFlags, x, y);
}

int KERNEL::Operator::Camera::RButtonDown(int nFlags, int x, int y)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->RButtonDown(nFlags, x, y);
}

int KERNEL::Operator::Camera::RButtonUp(int nFlags, int x, int y)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->RButtonUp(nFlags, x, y);
}

int KERNEL::Operator::Camera::MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->MouseWheel(nFlags, zDelta, x, y, nLeft, nTop);
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
