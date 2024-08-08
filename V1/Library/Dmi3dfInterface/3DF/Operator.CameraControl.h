#pragma once

#include "3DF.h"
#include "Math.h"

#include "Selection.h"

#include "NavigationCube.h"

namespace H3DF
{
	class WindowKey;

	namespace Operator
	{
		class Event;

		class API_3DF CameraControl : public Object
		{
		public:
			CameraControl(WindowKey const & cInWindow, NavigationCube & cNaviCube);
			~CameraControl();

			void SetCameraMode(H3DF::Camera::Mode eMode);
			H3DF::Camera::Mode CameraMode();

			void FitWorld();

			void SetCamera(H3DF::CameraKit & cInCameraKit);
			void SetCameraFitSelection(H3DF::MatrixKit & cInMatrix, SegmentKey & cInSegment);

			//== Mouse Event 처리 ===================================================================
			Operator::Result LButtonDown(Operator::Event & cInEvent);
			Operator::Result LButtonUp(Operator::Event & cInEvent, SelectionItem & cInItem);
			Operator::Result LButtonDownAndMove(Operator::Event & cInEvent);
			
			Operator::Result RButtonDown(Operator::Event & cInEvent);
			Operator::Result RButtonUp(Operator::Event & cInEvent);
			Operator::Result RButtonDownAndMove(Operator::Event & cInEvent);
		
			Operator::Result MouseWheel(Operator::Event & cInEvent);
		};
	}
}