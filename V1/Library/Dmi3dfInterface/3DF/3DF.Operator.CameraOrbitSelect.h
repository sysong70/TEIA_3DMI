#pragma once

#include "3DF.h"
#include "3DF.Math.h"

#include "3DF.Selection.h"

#include "3DF.Operator.ObjectSnap.h"

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
		int OnLButtonUp(HEventInfo & cInEvent) override;
		int OnLButtonDown(HEventInfo & cInEvent) override;
		int OnLButtonDownAndMove(HEventInfo & cInEvent) override;
		int OnNoButtonDownAndMove(HEventInfo & cInEvent) override;
	private:
		int HOpCameraOrbit_OnLButtonDownAndMove(HEventInfo & event);

	protected:
		bool m_bOrbitMode;
		DWORD m_nSelectPickCount;
		DWORD m_nMouseDownTickCount;
		HPoint m_cMouseDownPoint;

		WindowKey * m_pcWindow = nullptr;

		SelectionResults m_cNewHighlightSelection;
		SelectionResults m_cOldHighlightSelection;
		SelectionResults m_cHighlightSelection;

	private:
		HPoint m_cClickPoint;

		std::vector<ObjectSnap::SnapItem *> m_vSnapItems;
	};
}

CLOSE_3DF_NAMESPACE