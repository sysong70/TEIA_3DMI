#pragma once

#include "3DF.h"
#include "3DF.Math.h"

#include "3DF.Selection.h"
#include "3DF.Segment.h"

#include <HOpCameraOrbit.h>

OPEN_3DF_NAMESPACE

class WindowKey;

namespace Operator
{
	class ObjectSnap
	{
	public:
		enum class Type
		{
			EndPoint,
			MidPoint,
			NearPoint,
			Center,
			None
		};

		enum class Status
		{
			Selected,
			Normal,
		};

		struct SnapItem
		{
			WorldPoint cPoint;
			Type eType = Type::None;
			Status eStatus = Status::Normal;
		};

		struct CamerInformation
		{
			MatrixKit cMatrix;
			Point cOrigin;
			Vector cXAixs, cYAixs;
			double dObjectSnapRadius = 0.0;
		};

		ObjectSnap(WindowKey * pcWindow);

		int NoButtonDownAndMove(HEventInfo & cInEvent);

		//== Object Snap 계산 =======================================================================
		void CalculationObjectSnapPoint(TDF::SelectionResults & cInItems);

	protected:
		bool CalculationLienObjectSnapPoint(const Key & cInLineKey, const WindowPoint & cInPoint, const MatrixKit & cModelingMatrix);
		void CalculationLienAndLineObjectSnapPoint(LineKey & cLine1, LineKey & cLine2, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2);

		//== Object Snap Draw ======================================================================
	public:
		void DrawSnapItems(bool bUpdate = true);
		void DrawSnapItems(CamerInformation & cInCameraInfo, bool bUpdate = true);
		void DrawSnapItem(SnapItem * pcInItem, CamerInformation & cInCameraInfo, bool bUpdate = true);
		bool ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo);

		void DrawSnapItems1();
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

		void ClearSnapItems(bool bUpdate);

		//== Utility Function ======================================================================
		bool AddSnapItem(Key & cInKey, Point cSnapPoint, Type eType);
		void ResetSnapItem();

	protected:
		SelectionResults m_cNewHighlightSelection;
		SelectionResults m_cOldHighlightSelection;
		SelectionResults m_cHighlightSelection;

		WindowKey * m_pcWindow = nullptr;

		SegmentKey m_cSnapPointSegment;

		float m_fSnapRadius = 10;

		std::vector<ObjectSnap::SnapItem *> m_vSnapItems;
	};
}
CLOSE_3DF_NAMESPACE