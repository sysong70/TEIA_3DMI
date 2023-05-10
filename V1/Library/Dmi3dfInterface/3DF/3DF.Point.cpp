#include "StdAfx.h"

#include "3DF.Point.h"

#include "3DF.Window.h"

USING_3DF_NAMESPACE

//== ObjectPoint class =============================================================================
TDF::ObjectPoint::ObjectPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "world", &cInPoint, "object", this);
	} HC_Close_Segment();
}

TDF::ObjectPoint::ObjectPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer window", &cInPoint, "object", this);
	} HC_Close_Segment();
}

TDF::ObjectPoint::ObjectPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer pixels", &cInPoint, "object", this);
	} HC_Close_Segment();
}

TDF::ObjectPoint::ObjectPoint(WorldPoint const & cInPoint) {}
TDF::ObjectPoint::ObjectPoint(WindowPoint const & cInPoint) {}
TDF::ObjectPoint::ObjectPoint(PixelPoint const & cInPoint) {}

//== WorldPoint class ==============================================================================

TDF::WorldPoint::WorldPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "object", &cInPoint, "world", this);
	} HC_Close_Segment();
}

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

TDF::WorldPoint::WorldPoint(ObjectPoint const & cInPoint) {}
TDF::WorldPoint::WorldPoint(WindowPoint const & cInPoint) {}
TDF::WorldPoint::WorldPoint(PixelPoint const & cInPoint) {}


//== WindowPoint class =============================================================================

TDF::WindowPoint::WindowPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "object", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

TDF::WindowPoint::WindowPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "world", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

TDF::WindowPoint::WindowPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer pixels", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

TDF::WindowPoint::WindowPoint(ObjectPoint const & cInPoint) {}
TDF::WindowPoint::WindowPoint(WorldPoint const & cInPoint) {}
TDF::WindowPoint::WindowPoint(PixelPoint const & cInPoint) {}


//== PixelPoint class ==============================================================================

PixelPoint::PixelPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "object", &cInPoint, "outer pixels", this);
	} HC_Close_Segment();
}

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

PixelPoint::PixelPoint(ObjectPoint const & cInPoint) {}
PixelPoint::PixelPoint(WorldPoint const & cInPoint) {}
PixelPoint::PixelPoint(WindowPoint const & cInPoint) {}