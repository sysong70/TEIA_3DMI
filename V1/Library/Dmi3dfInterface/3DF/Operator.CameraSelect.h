#pragma once

#include "3DF.h"
#include "Math.h"

#include "Selection.h"

#include "Operator.ObjectSnap.h"

#include "NavigationCube.h"

#include <HOpCameraOrbit.h>
#include <HOpCameraPan.h>
#include <HOpCameraZoomBox.h>

#include "HTools.h"
#include "HGlobals.h"

class CameraZoomBox : public HOpCameraZoomBox
{
public:
	CameraZoomBox(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);

	int OnLButtonUp(HEventInfo & hevent) override;
};


namespace H3DF
{
	class WindowKey;

	namespace Operator
	{
		
		class CameraSelect : public HBaseOperator
		//class CameraSelect : public HOpCameraOrbit
		{
		public:
			CameraSelect(WindowKey * pcWindow, NavigationCube & cNaviCube, int DoRepeat = 0, int DoCapture = 1);
			//CameraSelect(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);
			~CameraSelect();

			const char * GetName() override;
			HBaseOperator * Clone() override;

			void SetViewControlMode(ViewControl::Mode eMode);

			//== Mouse Event 처리 =======================================================================
			int OnMouseWheel(HEventInfo & cInEvent) override;

			int OnLButtonUp(HEventInfo & cInEvent) override;
			int OnLButtonDown(HEventInfo & cInEvent) override;
			int OnLButtonDownAndMove(HEventInfo & cInEvent) override;
			int OnRButtonDown(HEventInfo & hevent) override;
			int OnRButtonDownAndMove(HEventInfo & hevent) override;
			int OnRButtonUp(HEventInfo & hevent) override;

			int OnNoButtonDownAndMove(HEventInfo & cInEvent) override;

		protected:
			int OnZoomBoxLButtonUp(HEventInfo & cInEvent);

		protected:
			DWORD m_nSelectPickCount;
			DWORD m_nMouseDownTickCount;
			HPoint m_cMouseDownPoint;

			WindowKey * m_pcWindow = nullptr;

			SelectionResults m_cNewHighlightSelection;
			SelectionResults m_cOldHighlightSelection;
			SelectionResults m_cHighlightSelection;

			Operator::ObjectSnap m_cObjectSnapOperator;

			NavigationCube * m_pcNaviCube = nullptr;

			ViewControl::Mode m_eViewControlMode = ViewControl::Mode::Multi;

		private:
			HPoint m_cClickPoint;
			double  m_dFirstPoint[3];

			HOpCameraOrbit m_cCameraOrbit;
			HOpCameraPan m_cCameraPan;
			// HOpCameraZoomBox m_cCameraZoomBox;
			CameraZoomBox m_cCameraZoomBox;
		};
	}
}