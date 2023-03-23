#pragma once

#include "3DF.h"
#include "3DF.Math.h"

#include "3DF.Selection.h"

#include <HOpCameraOrbit.h>

OPEN_3DF_NAMESPACE

class WindowKey;

namespace Operator
{
	class ObjectSnap
	{
	public:
		ObjectSnap(WindowKey * pcWindow);

		void DrawObjectSnapPoint(_3DF::SelectionResults & cInItems);

		void DrawCenterMark(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawBox(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawEndPoint(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawMidPoint(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawNearPoint(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);

		static void CreateGlyph();

	protected:
		WindowKey * m_pcWindow = nullptr;
	};
}
CLOSE_3DF_NAMESPACE