#pragma once

#include "3DF.h"
#include "Math.h"

#include "Selection.h"

#include "Operator.ObjectSnap.h"

#include "NavigationCube.h"

#include <HOpCameraOrbit.h>

#include "HTools.h"
#include "HGlobals.h"

namespace H3DF
{
	class WindowKey;

	namespace Operator
	{
		class CameraSelect : public HOpCameraOrbit
		{
		public:
			CameraSelect(WindowKey * pcWindow, NavigationCube & cNaviCube, int DoRepeat = 0, int DoCapture = 1);
			//CameraSelect(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);
			~CameraSelect();

			const char * GetName() override;
			HBaseOperator * Clone() override;

			//== Mouse Event 처리 =======================================================================
			int OnMouseWheel(HEventInfo & cInEvent) override;

			int OnLButtonUp(HEventInfo & cInEvent) override;
			int OnLButtonDown(HEventInfo & cInEvent) override;
			int OnLButtonDownAndMove(HEventInfo & cInEvent) override;
			int OnRButtonDown(HEventInfo & hevent) override;
			int OnRButtonDownAndMove(HEventInfo & hevent) override;
			int OnRButtonUp(HEventInfo & hevent) override;

			int OnNoButtonDownAndMove(HEventInfo & cInEvent) override;

		private:
			int HOpCameraOrbit_OnLButtonDownAndMove(HEventInfo & event);

			int HOpCameraPan_OnLButtonDown(HEventInfo & event);
			int HOpCameraPan_OnLButtonDownAndMove(HEventInfo & event);
			int HOpCameraPan_OnLButtonUp(HEventInfo & event);

			int HBaseView_OnMouseWheel(HEventInfo & event, bool bUdpate);

			bool valid_float(float f);
			bool valid_point(HPoint const & p);

		protected:
			bool m_bOrbitMode;
			DWORD m_nSelectPickCount;
			DWORD m_nMouseDownTickCount;
			HPoint m_cMouseDownPoint;

			WindowKey * m_pcWindow = nullptr;

			SelectionResults m_cNewHighlightSelection;
			SelectionResults m_cOldHighlightSelection;
			SelectionResults m_cHighlightSelection;

			Operator::ObjectSnap m_cObjectSnapOperator;

			NavigationCube * m_pcNaviCube = nullptr;

		private:
			HPoint m_cClickPoint;

			double  m_dFirstPoint[3];
		};
	}
}