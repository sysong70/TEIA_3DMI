#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>
#include <3DF/Operator.CameraControl.h>

#include "Command.h"
#include "Command.Set.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Command
	{
		class Camera : public Set
		{
		public:
			Camera(const Session * pcInSession);

			Command::Result::Type MouseMove(Event & cInEvent);

			Command::Result::Type LButtonDown(Event & cInEvent);
			Command::Result::Type LButtonUp(Event & cInEvent);

			Command::Result::Type RButtonDown(Event & cInEvent);
			Command::Result::Type RButtonUp(Event & cInEvent);

			Command::Result::Type MouseWheel(Event & cInEvent);
			
			//== View Control 관련 함수 ==============================================================
			H3DF::Camera::Mode CameraMode();
			void SetPanViewControl();
			void SetOrbitViewControl();
			void SetOrbitTurntableViewControl();
			void SetZoomArea();
			void FitWorld();
			void FitWorldOnly();

			void SetCamera(H3DF::CameraKit & cInCamera);
			void SetCameraFitSelection(H3DF::MatrixKit & cInMatrix, H3DF::SegmentKey & cInSegment);
		};
	}
}