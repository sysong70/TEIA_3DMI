#include "StdAfx.h"

#include "3DF.Point.h"

#include "3DF.Window.h"

USING_3DF_NAMESPACE

//== WorldPoint class ==============================================================================

TDF::WorldPoint::WorldPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer window", &cInPoint, "world", this);
	} HC_Close_Segment();
}

TDF::WorldPoint::WorldPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer pixels", &cInPoint, "world", this);
	} HC_Close_Segment();
}

WorldPoint::WorldPoint(WindowPoint const & cInPoint) {}
WorldPoint::WorldPoint(PixelPoint const & cInPoint) {}


//== WindowPoint class =============================================================================
WindowPoint::WindowPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "world", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

WindowPoint::WindowPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer pixels", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

WindowPoint::WindowPoint(WorldPoint const & cInPoint) {}
WindowPoint::WindowPoint(PixelPoint const & cInPoint) {}


//== PixelPoint class ==============================================================================

PixelPoint::PixelPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "world", &cInPoint, "outer pixels", this);
	} HC_Close_Segment();
}

PixelPoint::PixelPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer window", &cInPoint, "outer pixels", this);
	} HC_Close_Segment();
}

PixelPoint::PixelPoint(WorldPoint const & cInPoint) {}
PixelPoint::PixelPoint(WindowPoint const & cInPoint) {}