#include "StdAfx.h"

#include "3DF.Operator.ObjectSnap.h"
#include "Private/3DF.SelectionPrivate.h"

#include "3DF.Window.h"

#include "3DF.Line.h"

#include "../3DF.Signal.Connector.h"

#include <Common_Define.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

#define MARKER_OPCODE_START_FILL -1
#define MARKER_OPCODE_RESTART_FILL -2
#define MARKER_OPCODE_END_FILL -3
#define MARKER_OPCODE_ELLIPSE -4
#define MARKER_OPCODE_CIRCULAR_ARC -5
#define MARKER_OPCODE_INFINITE_LINE -6
#define MARKER_OPCODE_INFINITE_RAY -7
#define MARKER_OPCODE_START_CONTRAST_COLOR -8
#define MARKER_OPCODE_START_NORMAL_COLOR -9
#define MARKER_OPCODE_START_RGB_COLOR -10
#define MARKER_OPCODE_START_INDEXED_COLOR -11
#define MARKER_OPCODE_TERMINATE 0
#define MARKER_OPCODE_POINT 1
#define MARKER_OPCODE_LINE_2 2
#define MARKER_OPCODE_LINE_3 3
#define MARKER_OPCODE_LINE_4 4
#define MARKER_OPCODE_LINE_5 5
#define MARKER_OPCODE_LINE_6 6
#define MARKER_OPCODE_LINE_7 7
#define MARKER_OPCODE_LINE_8 8
#define MARKER_OPCODE_LINE_9 9
#define MARKER_OPCODE_LINE_10 10
#define MARKER_OPCODE_LINE_11 11
#define MARKER_OPCODE_LINE_12 12
#define MARKER_OPCODE_LINE_13 13

USING_3DF_NAMESPACE

Operator::ObjectSnap::ObjectSnap(WindowKey * pcWindow)
{
	m_pcWindow = pcWindow;
}

void Operator::ObjectSnap::DrawObjectSnapPoint(_3DF::SelectionResults & cInItems)
{
	HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetConstructionKey()); {

		//HC_Flush_Geometry(".");
		HC_Flush_Contents(".", "geometry, segment");

		for (POSITION pcPosition = cInItems.GetHeadPosition(); nullptr != pcPosition; ) {
			SelectionItem * pcItem = cInItems.GetNext(pcPosition);
			SelectionItemPrivate * pcImpl = (SelectionItemPrivate *)pcItem->GetImpl();

			WorldPoint cWorldPoint = pcImpl->cWorldPoint;

			Key * pcKey = nullptr;
			pcItem->ShowSelectedItem(pcKey);

			if (_3DF::Type::LineKey == pcKey->Type()) {

				LineKey cLine = LineKey(*pcKey);

				WorldPoint cNearPoint;
				if (true == cLine.NearPoint(*m_pcWindow, cWorldPoint, cNearPoint)) {
					DrawNearPoint("NearPoint", cNearPoint, RGB(255, 255, 0), 0.4);

					//int nViewId = m_pcWindow->ViewId();
					Connector::GetInstance(m_pcWindow->ViewId()).statusBar.ShowCoordinate(cWorldPoint.x, cWorldPoint.y, cWorldPoint.z);
				}

				WorldPointArray aPoints;
				cLine.ShowPoints(aPoints);

				if (2 == aPoints.GetCount()) {
					Point cPo[2];

					cPo[0] = aPoints[0];
					cPo[1] = aPoints[1];

					HC_Insert_Polyline(2, cPo);
				}
				else if (2 < aPoints.GetCount()) {
					Point cPo[3];

					cPo[0] = aPoints[0];
					cPo[1] = aPoints[(aPoints.GetCount() - 1) / 2];
					cPo[2] = aPoints[aPoints.GetCount() - 1];

					DrawMidPoint("Temp1", cPo[1], RGB(255, 255, 0), 0.4);
					DrawEndPoint("Temp2", cPo[0], RGB(255, 255, 0), 0.4);
					DrawEndPoint("Temp3", cPo[2], RGB(255, 255, 0), 0.4);

					//HC_Insert_Polyline(3, cPo);

// 					HC_Insert_Line(cPo[0].x, cPo[0].y, cPo[0].z, cPo[1].x, cPo[1].y, cPo[1].z);
// 					HC_Insert_Line(cPo[1].x, cPo[1].y, cPo[1].z, cPo[2].x, cPo[2].y, cPo[2].z);
// 
// 					HC_Insert_Marker(cPo[0].x, cPo[0].y, cPo[0].z);
// 					HC_Insert_Marker(cPo[1].x, cPo[1].y, cPo[1].z);
// 					HC_Insert_Marker(cPo[2].x, cPo[2].y, cPo[2].z);
				}
			}
		}
	} HC_Close_Segment();

	m_pcWindow->GetBaseView()->Update();
}

//== Object Snap Point를 그리는 함수 ==================================================================
void Operator::ObjectSnap::DrawCenterMark(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
	HC_Open_Segment(pchSegmentName);
	{
/*
		if (true == bTempDrawFlag) {
			HC_Set_Rendering_Options("no simple reflection");
			HC_Set_Rendering_Options("no simple shadow, no frame buffer effects");
			HC_Set_Rendering_Options("depth range = (0.1,0.2)");
			HC_Set_Visibility("shadows = off");
			HC_Set_Visibility("cutting planes = off");
			HC_Set_User_Options("on top");
		}
*/

		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapCenterMark");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
}

void Operator::ObjectSnap::DrawBox(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
	HC_Open_Segment(pchSegmentName);
	{
		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapBox");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
}

void Operator::ObjectSnap::DrawEndPoint(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
	HC_Open_Segment(pchSegmentName);
	{
		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapEndPoint");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
}

void Operator::ObjectSnap::DrawMidPoint(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
	HC_Open_Segment(pchSegmentName);
	{
		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapMidPoint");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
}

void Operator::ObjectSnap::DrawNearPoint(const char * pchSegmentName, Point cPoint, COLORREF nColor, double dWeight)
{
	HC_Open_Segment(pchSegmentName);
	{
		HC_Set_Variable_Marker_Size("3 oru");

		HC_Set_Visibility("marker = on");
		HC_Set_Marker_Size(dWeight);
		HC_Set_Color_By_Value("geometry", "rgb", GetRValue(nColor) / 255.0, GetGValue(nColor) / 255.0, GetBValue(nColor) / 255.0);

		HC_Set_Marker_Symbol("ObjectSnapNearPoint");
		HC_Insert_Marker(cPoint.x, cPoint.y, cPoint.z);
	}
	HC_Close_Segment();
}

void Operator::ObjectSnap::CreateGlyph()
{
	HC_Open_Segment("/"); {
		const char chEndPointData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			MARKER_OPCODE_LINE_5,
			-127, 127,
			127, 127,
			127, -127,
			-127, -127,
			-127, 127,

			MARKER_OPCODE_LINE_5,
			-107, 107,
			107, 107,
			107, -107,
			-107, -107,
			-107, 107,

			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapEndPoint", sizeof(chEndPointData), chEndPointData);

		const char chMidPointData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			MARKER_OPCODE_LINE_4,
			0, 127,
			-127, -93,
			127, -93,
			0, 127,

			MARKER_OPCODE_LINE_4,
			0, 87,
			-92, -73,
			92, -73,
			0, 87,

			MARKER_OPCODE_LINE_4,
			0, 97,
			-101, -77,
			101, -77,
			0, 97,

			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapMidPoint", sizeof(chMidPointData), chMidPointData);

		const char chNearPointData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			MARKER_OPCODE_LINE_5,
			-127, 127,
			127, 127,
			-127, -127,
			127, -127,
			-127, 127,

// 			MARKER_OPCODE_LINE_5,
// 			-127, 127,
// 			127, 127,
// 			-127, -127,
// 			127, -127,
// 			-127, 127,

// 			-127, 127,
// 			127, 127,
// 			127, -127,
// 			-127, -127,
// 			-127, 127,

			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapNearPoint", sizeof(chNearPointData), chNearPointData);

		const char chBoxData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			MARKER_OPCODE_LINE_5,
			-127, 127,
			127, 127,
			127, -127,
			-127, -127,
			-127, 127,
			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapBox", sizeof(chBoxData), chBoxData);

		const char chCenterMarkData[] = {
			127,	// radius size (최대 127까지만 사용한다.)
			0, 0,	// offset origin

			// 우측 상단 원호
			MARKER_OPCODE_CIRCULAR_ARC,
			127, 0,
			90, 90,
			0, 127,

			// 좌측 상단 원호 채움
			MARKER_OPCODE_START_FILL,
			MARKER_OPCODE_CIRCULAR_ARC,
			0, 127,
			-90, 90,
			-127, 0,
			MARKER_OPCODE_LINE_2,
			-127, 0,
			0, 0,
			MARKER_OPCODE_LINE_2,
			0, 0,
			0,127,
			MARKER_OPCODE_END_FILL,

			// 좌측 하단 원호
			MARKER_OPCODE_CIRCULAR_ARC,
			-127, 0,
			-90, -90,
			0, -127,

			// 우측 하단 원호 채움
			MARKER_OPCODE_START_FILL,
			MARKER_OPCODE_CIRCULAR_ARC,
			0, -127,
			90, -90,
			127,0,
			MARKER_OPCODE_LINE_2,
			127, 0,
			0, 0,
			MARKER_OPCODE_LINE_2,
			0, 0,
			0, -127,
			MARKER_OPCODE_END_FILL,

			MARKER_OPCODE_TERMINATE
		};

		HC_Define_Glyph("ObjectSnapCenterMark", sizeof(chCenterMarkData), chCenterMarkData);

	} HC_Close_Segment();
}