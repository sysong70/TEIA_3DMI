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
	class SnapItem;

	class ObjectSnap
	{
	public:
		enum class Type
		{
			EndPoint,
			MidPoint,
			NearPoint,
			Center,
			Intersection,
			None
		};

		enum class Status
		{
			Selected,
			Normal,
		};

		struct CamerInformation
		{
			MatrixKit cMatrix;
			Point cOrigin;
			Vector cXAixs, cYAixs;
			double dObjectSnapRadius = 0.0;
		};

		class SnapPoint
		{
		public:
			SnapPoint() {};
			SnapPoint(const SnapPoint & cInThat);
			SnapPoint & operator = (const SnapPoint & cInThat);

			WorldPoint cPoint;
			Type eType = Type::None;
			Status eStatus = Status::Normal;
		};

		class SnapItem
		{
		public:
			SnapItem() {};

			bool operator == (const SnapItem & cInThat) const;

			std::vector<SnapPoint> vcSnapPoints;
			std::vector <H3DF::SelectionItem> vcItems;
		};

		ObjectSnap(WindowKey * pcWindow);

		int NoButtonDownAndMove(HEventInfo & cInEvent);

	protected:

		void CalculationObjectSnapPoint(H3DF::SelectionResults& cInItems);
		bool CalculationLienObjectSnapPoint(const SelectionItem * pcInSelectionItem, const WindowPoint & cInPoint);
		void CalculationLienAndLineObjectSnapPoint(const SelectionItem * cInItems1, const SelectionItem * cInItems2, 
			const MatrixKit & cMatrix1, const MatrixKit & cMatrix2);

	public:

		void DrawSnapItems(bool bUpdate = true);
		//void DrawSnapItem(SnapItem * pcInItem, CamerInformation & cInCameraInfo, bool bUpdate = true);

		void DrawSnapPoint(SnapPoint & cSnapPoint, CamerInformation & cInCameraInfo);
		void DrawSnapPoint(Point2D center, Status eInStatus, Type eInType, double dUnit);

		double PixelToWorld(double unit);
		bool ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo);

	private:
		bool AddSnapItems(SnapItem * psInSnapItem);
		bool AddSnapItem(SnapItem * psInSnapItem, Point cInSnapPoint, Type eInType);
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

		DWORD m_nPrevMouseMoveTickCount;
		DWORD m_nSelectPickCount;

		std::vector<SnapItem *> m_vSnapItems;

		int m_nTotalSnapItemCount = 5;
	};
}

CLOSE_3DF_NAMESPACE