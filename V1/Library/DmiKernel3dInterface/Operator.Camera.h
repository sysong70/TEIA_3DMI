#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>
#include <3DF/3DF.Operator.CameraControl.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class Camera : public OperatorBase
		{
		public:
			Camera(const DocView * pcInDocView);

			int MouseMove(HEventInfo & cInEvent);

			int LButtonDown(HEventInfo & cInEvent);
			int LButtonUp(HEventInfo & cInEvent);

			int RButtonDown(HEventInfo & cInEvent);
			int RButtonUp(HEventInfo & cInEvent);

			int MouseWheel(HEventInfo & cInEvent);
			
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