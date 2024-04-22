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
			int LButtonDown(HEventInfo & cInEvent);
			int LButtonUp(HEventInfo & cInEvent);
			int LButtonDownAndMove(HEventInfo & cInEvent);
			
			int RButtonDown(HEventInfo & cInEvent);
			int RButtonUp(HEventInfo & cInEvent);
			int RButtonDownAndMove(HEventInfo & cInEvent);
		
			int MouseWheel(HEventInfo & cInEvent);
		};
	}
}