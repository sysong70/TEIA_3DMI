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

			//== Mouse Event 처리 ===================================================================
			int LButtonDown(int nFlags, int x, int y);
			int LButtonUp(int nFlags, int x, int y);
			int LButtonDownAndMove(int nFlags, int x, int y);
			
			int RButtonDown(int nFlags, int x, int y);
			int RButtonUp(int nFlags, int x, int y);
			int RButtonDownAndMove(int nFlags, int x, int y);
		
			int MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop);
		};
	}
}