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

	protected:

		void CalculationObjectSnapPoint(TDF::SelectionResults& cInItems);
		bool CalculationLienObjectSnapPoint(const Key & cInLineKey, const WindowPoint & cInPoint, const MatrixKit & cModelingMatrix);
		void CalculationLienAndLineObjectSnapPoint(LineKey & cLine1, LineKey & cLine2, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2);

	public:

		void DrawSnapItems(bool bUpdate = true);
		void DrawSnapItem(SnapItem * pcInItem, CamerInformation & cInCameraInfo, bool bUpdate = true);
		void DrawSnapPoint(double dRadius, Point2D center, SnapItem* pItem);
		bool ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo);

	private:

		bool AddSnapItem(Key & cInKey, Point cSnapPoint, Type eType);
		void ClearSnapItems(bool bUpdate);
		void ResetSnapItem();

	protected:

		SelectionResults m_cNewHighlightSelection;
		SelectionResults m_cOldHighlightSelection;
		SelectionResults m_cHighlightSelection;

		WindowKey * m_pcWindow = nullptr;

		SegmentKey m_cSnapPointSegment;

		float m_fSnapRadius = 8;

		Point m_cPrevPoint;

		std::vector<ObjectSnap::SnapItem *> m_vSnapItems;
	};
}
CLOSE_3DF_NAMESPACE