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
		enum class SnapType
		{
			EndPoint,
			MidPoint,
			NearPoint,
			Center,
			None
		};

		struct SnapItem
		{
			Point cPoint;
			SnapType eType = SnapType::None;
		};

		ObjectSnap(WindowKey * pcWindow);

		//== Object Snap 계산 =======================================================================
		void CalculationObjectSnapPoint(TDF::SelectionResults & cInItems);
		bool CalculationLienObjectSnapPoint(const Key & cInLineKey, const WindowPoint & cInPoint, const MatrixKit & cModelingMatrix);
		void CalculationLienAndLineObjectSnapPoint(LineKey & cLine1, LineKey & cLine2, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2);

		//== Object Snap Draw ======================================================================
		
		void DrawSnapItems();

		void DrawObjectSnapPoint(TDF::SelectionResults & cInItems);

		void DrawCenterMark(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawBox(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawEndPoint(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawMidPoint(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawNearPoint(const char * pchSegmentName, Point cPoint, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawCircle(SegmentKey & cConstruction, Point cPoint, Vector cViewNormal, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);
		void DrawRectangle(SegmentKey & cConstruction, Point cPoint, Vector cViewNormal, COLORREF = RGB(0, 0, 255), double dWeight = 1.0);

		static void CreateGlyph();
		//:Ken
		static void LoadResource();

		//== Utility Function ======================================================================
		bool AddSnapItem(Key & cInKey, Point cSnapPoint, SnapType eType);
		void ResetSnapItem();
	protected:

		WindowKey * m_pcWindow = nullptr;

		CAtlList<SnapItem *> m_aSnapItems;
	};
}
CLOSE_3DF_NAMESPACE