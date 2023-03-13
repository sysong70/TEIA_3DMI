#pragma once

#include "3DF.h"
#include "3DF.Math.h"

#include "3DF.Selection.h"

#include <HOpCameraOrbit.h>

OPEN_3DF_NAMESPACE

class WindowKey;

class OpCameraOrbitSelect : public HOpCameraOrbit
{
public:
	OpCameraOrbitSelect(WindowKey * pcWindow, int DoRepeat = 0, int DoCapture = 1);
	OpCameraOrbitSelect(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);
	~OpCameraOrbitSelect();

	const char * GetName() override;
	HBaseOperator * Clone() override;

	//== Mouse Event 처리 ===========================================================================
	int OnLButtonDown(HEventInfo & cEvent) override;
	int OnLButtonUp(HEventInfo & cEvent) override;
	int OnLButtonDownAndMove(HEventInfo & cEvent) override;
	int OnNoButtonDownAndMove(HEventInfo & cEvent) override;

	//== Selection 관련 함수 =========================================================================
protected:
	int OnDaynamicHighlightMouseMove(HEventInfo & cEvent);

protected:
	bool m_bOrbitMode;
	DWORD m_nSelectPickCount;
	DWORD m_nMouseDownTickCount;
	HPoint m_cMouseDownPoint;

	WindowKey * m_pcWindow = nullptr;

	SelectionResults m_cNewHighlightSelection;
	SelectionResults m_cOldHighlightSelection;
};

CLOSE_3DF_NAMESPACE