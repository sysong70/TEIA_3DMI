#pragma once

#include "3DF.h"
#include "3DF.Math.h"

#include "3DF.Selection.h"

#include <HOpCameraOrbit.h>

OPEN_3DF_NAMESPACE

class WindowKey;

namespace Operator
{
	class CameraOrbitSelect : public HOpCameraOrbit
	{
	public:
		CameraOrbitSelect(WindowKey * pcWindow, int DoRepeat = 0, int DoCapture = 1);
		CameraOrbitSelect(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);
		~CameraOrbitSelect();

		const char * GetName() override;
		HBaseOperator * Clone() override;

		//== Mouse Event 처리 =======================================================================
		int OnLButtonUp(HEventInfo & cEvent) override;
		int OnLButtonDown(HEventInfo & cEvent) override;
		int OnLButtonDownAndMove(HEventInfo & cEvent) override;
		int OnNoButtonDownAndMove(HEventInfo & cEvent) override;

	protected:
		bool m_bOrbitMode;
		DWORD m_nSelectPickCount;
		DWORD m_nMouseDownTickCount;
		HPoint m_cMouseDownPoint;

		WindowKey * m_pcWindow = nullptr;

		SelectionResults m_cNewHighlightSelection;
		SelectionResults m_cOldHighlightSelection;
		SelectionResults m_cHighlightSelection;
	};
}

CLOSE_3DF_NAMESPACE