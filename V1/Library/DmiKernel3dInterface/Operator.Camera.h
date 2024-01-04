#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>
#include <3DF/Operator.CameraControl.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class Camera : public OperatorBase
		{
		public:
			Camera(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			int MouseMove(int nFlags, int x, int y);

			int LButtonDown(int nFlags, int x, int y);
			int LButtonUp(int nFlags, int x, int y);

			int RButtonDown(int nFlags, int x, int y);
			int RButtonUp(int nFlags, int x, int y);

			int MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop);

			//== View Control 관련 함수 ==============================================================
			H3DF::Camera::Mode CameraMode();
			void SetPanViewControl();
			void SetOrbitViewControl();
			void SetOrbitTurntableViewControl();
			void SetZoomArea();
			void FitWorld();
		};
	}
}