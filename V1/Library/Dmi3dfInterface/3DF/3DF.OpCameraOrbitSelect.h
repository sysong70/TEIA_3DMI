#pragma once

#include "3DF.h"
#include "3DF.Math.h"

#include <HOpCameraOrbit.h>

OPEN_3DF_NAMESPACE

class OpCameraOrbitSelect : public HOpCameraOrbit
{
public:
	OpCameraOrbitSelect(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);
	~OpCameraOrbitSelect();

	const char * GetName() override;
	HBaseOperator * Clone() override;

	//== Mouse Event 처리 ===========================================================================
	int OnLButtonDown(HEventInfo & cEvent) override;
	int OnLButtonUp(HEventInfo & cEvent) override;
	int OnLButtonDownAndMove(HEventInfo & cEvent) override;

	//== Select 관련 함수 ============================================================================
protected:
	int SelectButtonDown(HEventInfo & cEvent);
	void HandleSelection(HEventInfo & cEvent);

	bool m_bOrbitMode;
	DWORD m_nSelectPickCount;
	DWORD m_nMouseDownTickCount;
	HPoint m_cMouseDownPoint;
	
	enum SelType{
		None,
		Shell,
		Region,
		Marker,
		Line
	};
};

CLOSE_3DF_NAMESPACE