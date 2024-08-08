#include "StdAfx.h"

#include "Command.Camera.h"

#include "Impl/Command.Set.Impl.h"

#include "Kernel.Session.h"

#include "Command.Select.h"

#include "Signal.Connector.h"
#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

#include <Sprocket/3DF.View.h>
#include <Sprocket/3DF.Canvas.h>
#include <Sprocket/3DF.Model.h>

#include <3DF/Window.h>
#include <3DF/Bounding.h>
#include <3DF/Camera.h>
#include <3DF/VisualEffects.h>
#include <3DF/Facility.AppOptions.h>
#include <3DF/Operator.CameraControl.h>

#include <Json.h>

using namespace H3DF;
using namespace KERNEL;

namespace KERNEL
{
	namespace Command
	{
		class CameraImpl : public SetImpl
		{
		public:
			CameraImpl(const Session * pcInSession);
			~CameraImpl();

			void Copy(CameraImpl * pcInThat) {
				SetImpl::Copy(pcInThat);
			}

			H3DF::Operator::CameraControl & CameraControl() { return *m_pcCameraControl; }
			H3DF::Camera::Mode CameraMode();

		protected:
			H3DF::Operator::CameraControl * m_pcCameraControl = nullptr;
		};
	}
}

KERNEL::Command::CameraImpl::CameraImpl(const Session * pcInSession)
	: SetImpl(pcInSession)
{
	m_pcCameraControl = new H3DF::Operator::CameraControl(Window(), View().GetNavigationCube());
}

KERNEL::Command::CameraImpl::~CameraImpl()
{
	if (nullptr != m_pcCameraControl) {
		delete m_pcCameraControl;
		m_pcCameraControl = nullptr;
	}
}

//== View Control 관련 함수 ==========================================================================

H3DF::Camera::Mode KERNEL::Command::CameraImpl::CameraMode()
{
	return CameraControl().CameraMode();
}

//== Camera class ==================================================================================

KERNEL::Command::Camera::Camera(const Session * pcInSession) :
	Set(pcInSession)
{
	CameraImpl * pcImpl = new CameraImpl(pcInSession);
	DEBUG_VALID(pcImpl);

	m_pcImpl = pcImpl;

	pcImpl->CameraControl();
}

Command::Result::Type KERNEL::Command::Camera::MouseMove(Event & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (true == cInEvent.LButton()) {
		return Command::Result::Convert(pcImpl->CameraControl().LButtonDownAndMove(cInEvent.OperatorEvent()));
	}
	else if (true == cInEvent.RButton()) {
		return Command::Result::Convert(pcImpl->CameraControl().RButtonDownAndMove(cInEvent.OperatorEvent()));
	}

	return Command::Result::Type::Pass;
}

Command::Result::Type KERNEL::Command::Camera::LButtonDown(Event & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return Command::Result::Convert(pcImpl->CameraControl().LButtonDown(cInEvent.OperatorEvent()));
}

Command::Result::Type KERNEL::Command::Camera::LButtonUp(Event & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::SelectionItem & cItem = pcImpl->GetSession().Select().DynamicHighlightSelectionItem();

	return Command::Result::Convert(pcImpl->CameraControl().LButtonUp(cInEvent.OperatorEvent(), cItem));
}

Command::Result::Type KERNEL::Command::Camera::RButtonDown(Event & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return Command::Result::Convert(pcImpl->CameraControl().RButtonDown(cInEvent.OperatorEvent()));
}

Command::Result::Type KERNEL::Command::Camera::RButtonUp(Event & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return Command::Result::Convert(pcImpl->CameraControl().RButtonUp(cInEvent.OperatorEvent()));
}

Command::Result::Type KERNEL::Command::Camera::MouseWheel(Event & cInEvent)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return Command::Result::Convert(pcImpl->CameraControl().MouseWheel(cInEvent.OperatorEvent()));
}

//== View Control 관련 함수 ==========================================================================

H3DF::Camera::Mode KERNEL::Command::Camera::CameraMode()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->CameraMode();
}

void KERNEL::Command::Camera::SetPanViewControl()
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

void KERNEL::Command::Camera::SetOrbitViewControl()
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

void KERNEL::Command::Camera::SetOrbitTurntableViewControl()
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

void KERNEL::Command::Camera::SetZoomArea()
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

void KERNEL::Command::Camera::FitWorld()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->CameraControl().SetCameraMode(H3DF::Camera::Mode::Multi);
	pcImpl->CameraControl().FitWorld();
}

void KERNEL::Command::Camera::FitWorldOnly()
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->CameraControl().FitWorld();
}

void KERNEL::Command::Camera::SetCamera(H3DF::CameraKit & cInCamera)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->View().SmoothTransition(cInCamera);
}

void KERNEL::Command::Camera::SetCameraFitSelection(H3DF::MatrixKit & cInMatrix, H3DF::SegmentKey & cInSegment)
{
	auto * pcImpl = dynamic_cast<CameraImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	H3DF::BoundingKit cBounding;
	if (false == cInSegment.ShowBounding(cBounding)) {
		// 입력된 Segment를 이용해서, Bounding을 구하지 못한 경우 Model의 Bounding을 구한다.
		SegmentKey cModelSegment = pcImpl->GetSession().Canvas().GetModel().GetSegmentKey();
		SetCameraFitSelection(cInMatrix, cModelSegment);
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