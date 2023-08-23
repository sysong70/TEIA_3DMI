#pragma once

#include "3DF.h"
#include "3DF.BaseView.h"
#include "3DF.Segment.h"
#include "3DF.Selection.h"

OPEN_3DF_NAMESPACE

class NavigationCube
{
public:

	NavigationCube(H3DF::BaseView * view = nullptr, WindowKey * pcInWindow = nullptr);

	~NavigationCube();

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

	HC_KEY CreatePlaneShell(const char* name, const char* text, Triple pos, Triple angle);

	HC_KEY CreateEdgeShell(const char* name, Triple pos, Triple angle);

	HC_KEY CreateCornerShell(const char* name, Triple pos, Triple angle);

	HC_KEY CreateAxis(const char* name, const char* text, HPoint axisEnd, HPoint textCenter, COLORREF rgb);

	void SetWindowSize(double width, double height, bool openSegment = true);

	WindowKey* m_pcWindow = nullptr;
	H3DF::BaseView* m_pView = nullptr;
	H3DF::Point2D m_windowSize;

	HC_KEY m_parentSegment = HC_ERROR_KEY;
	HC_KEY m_cubeSegment = HC_ERROR_KEY;

	SegmentKey m_cSegments[(int)H3DF::ViewMode::Count];

	SelectionResults m_cOldHighlightSelection;
};

CLOSE_3DF_NAMESPACE
