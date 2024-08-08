#pragma once

#include "../Kernel.h"

#include "Command.Set.Impl.h"

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include <3DF/Window.h>
#include <3DF/Segment.h>
#include <3DF/Selection.h>
#include <3DF/Highlight.h>

#include <HOpCameraOrbit.h>

namespace KERNEL
{
	namespace Command
	{
		class HighlightObjectSnapImpl : public SetImpl
		{
		public:
			enum class Status
			{
				Selected,
				Normal,
			};

			struct CamerInformation
			{
				H3DF::MatrixKit cMatrix;
				H3DF::Point cOrigin;
				H3DF::Vector cXAixs, cYAixs;
				double dObjectSnapRadius = 0.0;
			};

			class SnapPoint
			{
			public:
				SnapPoint() {};
				SnapPoint(const SnapPoint & cInThat);
				SnapPoint & operator = (const SnapPoint & cInThat);

				H3DF::WorldPoint cPoint;
				OSnap::Type eType = OSnap::Type::None;
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

			HighlightObjectSnapImpl(const Session * pcInSession);

			Command::Result::Type LButtonDownAndMove(Event & cInEvent);
			int NoButtonDownAndMove(Event & cInEvent);
			bool DoDynamicHighlighting(H3DF::WindowPoint cMousePoint, H3DF::SelectionItem & cOutSelection);

			void SetObjectSnapMode(DWORD nInSnapMode);
			void SetSelectionFilter(DWORD nInSelFilter);

			void Reset(bool bUpdate);

		protected:
			void ApplySelectionFilter(H3DF::SelectionResults & cInSelections, H3DF::SelectionResults & cOutSelections);

			void CalculationObjectSnapPoint(H3DF::SelectionResults & cInItems);
			bool CalculationLienObjectSnapPoint(const H3DF::SelectionItem * pcInSelectionItem, const H3DF::WindowPoint & cInPoint);
			void CalculationLienAndLineObjectSnapPoint(const H3DF::SelectionItem * cInItems1, const H3DF::SelectionItem * cInItems2,
				const H3DF::MatrixKit & cMatrix1, const H3DF::MatrixKit & cMatrix2);

		public:
			void DrawSnapItems();

			void DrawSnapPointTypeText(SnapPoint & cSnapPoint, CamerInformation & cInCameraInfo, bool bSemgmentOpen = true);
			void DrawSnapPointTypeText(H3DF::Point2D center, Status eInStatus, OSnap::Type eInType, double dUnit);

			double PixelToWorld(double unit);
			bool ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo);

		private:
			bool AddSnapItems(SnapItem * psInSnapItem);
			bool AddSnapItem(SnapItem * psInSnapItem, H3DF::Point cInSnapPoint, OSnap::Type eInType);
			void ClearSnapItems(bool bUpdate);
			void ResetSnapItem();

		public:
			H3DF::SelectionResults m_cOSnapRelationSelItem;
			H3DF::SelectionResults m_cSelectionResult;
			H3DF::SelectionItem m_cDynamicHighlightSelItem;

			// DoDynamicHighlighting용 Highlight control
			H3DF::HighlightControl m_cDynHighlightControl;
			H3DF::HighlightControl m_cDynLineHighlightCtrl;
			H3DF::HighlightControl m_cDynPmiHighlightCtrl;

			H3DF::SegmentKey m_cSnapPointSegment;

			float m_fSnapRadius = 8;
			float m_fLineWeight = 10;

			H3DF::Point m_cPrevMousePixelPoint;

			DWORD m_nOSnapMode; // Object snap 
			DWORD m_nSelFilter;

			DWORD m_nPrevMouseMoveTickCount;
			DWORD m_nSelectPickCount;

			std::vector<SnapItem *> m_vSnapItems;
			SnapItem m_cNearSnapItem;
			
			SnapPoint m_cSelectSnapPoint;

			int m_nTotalSnapItemCount = 5;
		};
	}
}