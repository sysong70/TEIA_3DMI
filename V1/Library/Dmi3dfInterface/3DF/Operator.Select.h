#pragma once

#include "3DF.h"
#include "Math.h"

#include "NavigationCube.h"

#include <HOpCameraOrbit.h>
#include <HOpCameraOrbitTurntable.h>
#include <HOpCameraPan.h>
#include <HOpCameraZoomBox.h>

#include "HTools.h"
#include "HGlobals.h"

namespace H3DF
{
	class WindowKey;

	namespace Operator
	{
		class Select : public HBaseOperator
		{
		public:
			Select(WindowKey * pcWindow, NavigationCube & cNaviCube, int DoRepeat = 0, int DoCapture = 1);
			~Select();

			const char * GetName() override;
			HBaseOperator * Clone() override;

			//== Mouse Event 처리 =======================================================================
			int OnLButtonUp(HEventInfo & cInEvent) override;
			int OnLButtonDown(HEventInfo & cInEvent) override;
			int OnLButtonDownAndMove(HEventInfo & cInEvent) override;

			int OnNoButtonDownAndMove(HEventInfo & cInEvent) override;

		protected:
			DWORD m_nSelectTickCount;
			DWORD m_nMouseDownTickCount;
			HPoint m_cMouseDownPoint;

			WindowKey * m_pcWindow = nullptr;

			SelectionResults m_cNewHighlightSelection;
			SelectionResults m_cOldHighlightSelection;
			SelectionResults m_cHighlightSelection;

			NavigationCube * m_pcNaviCube = nullptr;

		private:
			HPoint m_cClickPoint;
			double  m_dFirstPoint[3];
		};
	}
}