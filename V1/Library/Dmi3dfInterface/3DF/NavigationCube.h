#pragma once

#include "3DF.h"

#include "Segment.h"
#include "Selection.h"

#include <HBaseView.h>
#include <HUtility.h>
#include <HTools.h>
#include <HSelectionSet.h>

namespace H3DF
{
	class BaseView;

	class NavigationCube : public Object
	{
	public:

		NavigationCube(H3DF::BaseView * view = nullptr, WindowKey * pcInWindow = nullptr);

		~NavigationCube();

		void Set(NavigationCube const & cInThat);
		NavigationCube const & operator = (NavigationCube const & cInThat);

		int LButtonUp(HEventInfo & cInEvent);

		int LButtonDownAndMove(HEventInfo & cInEvent);

		int NoButtonDownAndMove(HEventInfo & cInEvent);

		void SetView(H3DF::BaseView * view, WindowKey * pcInWindow);

		void SetVisible(bool axis, bool cube);

		bool IsValid();

	public:

		void Create(float width, float height, HC_KEY parent);
		//:WARNING - on changing preference
		void Recreate();

		HC_KEY HitTest(float x, float y, float z);

		void Transform();

		void OnSize(float width, float height);

	private:

		struct Triple
		{
			double x;
			double y;
			double z;
		};

		void OpenCubeSegment();

		void CloseCubeSegment();

		void CreateAxis();

		void CreateCube();

		void CreateCubeWire();

		HC_KEY CreatePlaneShell(const char * name, const char * text, Triple pos, Triple angle);

		HC_KEY CreateEdgeShell(const char * name, Triple pos, Triple angle);

		HC_KEY CreateCornerShell(const char * name, Triple pos, Triple angle);

		HC_KEY CreateAxis(const char * name, const char * text, HPoint axisEnd, HPoint textCenter, COLORREF rgb);

		void SetWindowSize(double width, double height, bool openSegment = true);
	};
}