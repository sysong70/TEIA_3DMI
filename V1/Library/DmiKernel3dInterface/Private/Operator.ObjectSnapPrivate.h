#pragma once

#include "../Kernel.h"

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include <3DF/Window.h>
#include <3DF/Selection.h>
#include <3DF/Segment.h>

#include <HOpCameraOrbit.h>

namespace KERNEL
{
	namespace Operator
	{
		class SnapItem;

		class ObjectSnapPrivate : public PrivateImpl
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

			ObjectSnapPrivate(H3DF::WindowKey * pcWindow);

			int NoButtonDownAndMove(int nFlags, int x, int y);

			void SetObjectSnapMode(DWORD nInSnapMode);

		protected:
			void CalculationObjectSnapPoint(H3DF::SelectionResults & cInItems);
			bool CalculationLienObjectSnapPoint(const H3DF::SelectionItem * pcInSelectionItem, const H3DF::WindowPoint & cInPoint);
			void CalculationLienAndLineObjectSnapPoint(const H3DF::SelectionItem * cInItems1, const H3DF::SelectionItem * cInItems2,
				const H3DF::MatrixKit & cMatrix1, const H3DF::MatrixKit & cMatrix2);

		public:
			void DrawSnapItems();
			//void DrawSnapItem(SnapItem * pcInItem, CamerInformation & cInCameraInfo, bool bUpdate = true);

			void DrawSnapPoint(SnapPoint & cSnapPoint, CamerInformation & cInCameraInfo);
			void DrawSnapPoint(H3DF::Point2D center, Status eInStatus, OSnap::Type eInType, double dUnit);

			double PixelToWorld(double unit);
			bool ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo);

		private:
			bool AddSnapItems(SnapItem * psInSnapItem);
			bool AddSnapItem(SnapItem * psInSnapItem, H3DF::Point cInSnapPoint, OSnap::Type eInType);
			void ClearSnapItems(bool bUpdate);
			void ResetSnapItem();

		protected:
			H3DF::SelectionResults m_cNewHighlightSelection;
			H3DF::SelectionResults m_cOldHighlightSelection;
			H3DF::SelectionResults m_cHighlightSelection;

			H3DF::WindowKey * m_pcWindow = nullptr;

			H3DF::SegmentKey m_cSnapPointSegment;

			float m_fSnapRadius = 8;

			H3DF::Point m_cPrevPoint;

			DWORD m_nOSnapMode; // Object snap 

			DWORD m_nPrevMouseMoveTickCount;
			DWORD m_nSelectPickCount;

			std::vector<SnapItem *> m_vSnapItems;
			SnapItem m_cNearSnapItem;
			
			int m_nTotalSnapItemCount = 5;
		};
	}
}