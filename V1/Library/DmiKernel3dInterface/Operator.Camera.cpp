#include "StdAfx.h"

#include "OPERATOR.Camera.h"

#include "Impl/OperatorImpl.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <Sprocket/3DF.View.h>
#include <3DF/Window.h>
#include <3DF/Bounding.h>
#include <3DF/Camera.h>
#include <3DF/VisualEffects.h>
#include <3DF/Facility.AppOptions.h>
#include <3DF/3DF.Operator.CameraControl.h>

#include <Json.h>

using namespace H3DF;
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

void KERNEL::Operator::Camera::FitWorldOnly()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->CameraControl().FitWorld();
}

void KERNEL::Operator::Camera::SetCamera(H3DF::CameraKit & cInCamera)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->CameraControl().SetCamera(cInCamera);
}

void KERNEL::Operator::Camera::SetCameraFitSelection(H3DF::MatrixKit & cInMatrix, H3DF::SegmentKey & cInSegment)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	// pcImpl->CameraControl().SetCameraFitSelection(cInMatrix, cInSegment);

	H3DF::BoundingKit cBounding;
	if (false == cInSegment.ShowBounding(cBounding)) {
		DEBUG_STOP;
		return;
	}

	H3DF::SimpleSphere cSphere;
	H3DF::SimpleCuboid cCuboid;
	if (false == cBounding.ShowVolume(cSphere, cCuboid)) {
		DEBUG_STOP;
		return;
	}

	H3DF::Point cCenter = (cCuboid.cMax + cCuboid.cMin) / 2.0;
	H3DF::Vector cVector = cCuboid.cMax - cCuboid.cMin;
	double dLength = cVector.Length();

	//===== 카메라 위치 설정 =====
	// 카메라가 놓이는 위치 설정
	H3DF::Point cCameraPosition = cCenter + cInMatrix.ZAxis() * (dLength * 2.5);

	// 카메라가 바라보는 방향 설정. Matrix Z축의 반대 방향으로 설정한다.
	H3DF::Point cCameraTarget = cCenter;

	H3DF::CameraKit cCamera;
	cCamera.SetTarget(cCameraTarget);
	cCamera.SetPosition(cCameraPosition);
	cCamera.SetUpVector(cInMatrix.YAxis());
	cCamera.SetField(dLength, dLength);

	pcImpl->View().SmoothTransition(cCamera);
}