#include "StdAfx.h"

#include "Point.h"

#include "Window.h"

USING_3DF_NAMESPACE

//== ObjectPoint class =============================================================================
H3DF::ObjectPoint::ObjectPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "world", &cInPoint, "object", this);
	} HC_Close_Segment();
}

H3DF::ObjectPoint::ObjectPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "local window", &cInPoint, "object", this);
	} HC_Close_Segment();
}

H3DF::ObjectPoint::ObjectPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer window", &cInPoint, "object", this);
	} HC_Close_Segment();
}

H3DF::ObjectPoint::ObjectPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer pixels", &cInPoint, "object", this);
	} HC_Close_Segment();
}

H3DF::ObjectPoint::ObjectPoint(WorldPoint const & cInPoint) {}
H3DF::ObjectPoint::ObjectPoint(InnerWindowPoint const & cInPoint) {}
H3DF::ObjectPoint::ObjectPoint(WindowPoint const & cInPoint) {}
H3DF::ObjectPoint::ObjectPoint(PixelPoint const & cInPoint) {}

//== WorldPoint class ==============================================================================

H3DF::WorldPoint::WorldPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "object", &cInPoint, "world", this);
	} HC_Close_Segment();
}

H3DF::WorldPoint::WorldPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "local window", &cInPoint, "world", this);
	} HC_Close_Segment();
}

H3DF::WorldPoint::WorldPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer window", &cInPoint, "world", this);
	} HC_Close_Segment();
}

H3DF::WorldPoint::WorldPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer pixels", &cInPoint, "world", this);
	} HC_Close_Segment();
}

H3DF::WorldPoint::WorldPoint(ObjectPoint const & cInPoint) {}
H3DF::WorldPoint::WorldPoint(InnerWindowPoint const & cInPoint) {}
H3DF::WorldPoint::WorldPoint(WindowPoint const & cInPoint) {}
H3DF::WorldPoint::WorldPoint(PixelPoint const & cInPoint) {}

//== H3DF::InnerWindowPoint::InnerWindowPoint class ========================================================================

H3DF::InnerWindowPoint::InnerWindowPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.KeyValue()); {
		HC_Compute_Coordinates(".", "object", &cInPoint, "local window", this);
	} HC_Close_Segment();
}

H3DF::InnerWindowPoint::InnerWindowPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint) {
	HC_Open_Segment_By_Key(cInWindow.KeyValue()); {
		HC_Compute_Coordinates(".", "world", &cInPoint, "local window", this);
	} HC_Close_Segment();
}

H3DF::InnerWindowPoint::InnerWindowPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.KeyValue()); {
		HC_Compute_Coordinates(".", "outer window", &cInPoint, "local window", this);
	} HC_Close_Segment();
}

H3DF::InnerWindowPoint::InnerWindowPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.KeyValue()); {
		HC_Compute_Coordinates(".", "outer pixels", &cInPoint, "local window", this);
	} HC_Close_Segment();
}

H3DF::InnerWindowPoint::InnerWindowPoint(ObjectPoint const & cInPoint) {}
H3DF::InnerWindowPoint::InnerWindowPoint(WorldPoint const & cInPoint) {}
H3DF::InnerWindowPoint::InnerWindowPoint(WindowPoint const & cInPoint) {}
H3DF::InnerWindowPoint::InnerWindowPoint(PixelPoint const & cInPoint) {}

//== WindowPoint class =============================================================================

H3DF::WindowPoint::WindowPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "object", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

H3DF::WindowPoint::WindowPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "world", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

H3DF::WindowPoint::WindowPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "local window", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

H3DF::WindowPoint::WindowPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer pixels", &cInPoint, "outer window", this);
	} HC_Close_Segment();
}

H3DF::WindowPoint::WindowPoint(ObjectPoint const & cInPoint) {}
H3DF::WindowPoint::WindowPoint(WorldPoint const & cInPoint) {}
H3DF::WindowPoint::WindowPoint(InnerWindowPoint const & cInPoint) {}
H3DF::WindowPoint::WindowPoint(PixelPoint const & cInPoint) {}

//== PixelPoint class ==============================================================================

H3DF::PixelPoint::PixelPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "object", &cInPoint, "outer pixels", this);
	} HC_Close_Segment();
}

H3DF::PixelPoint::PixelPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "world", &cInPoint, "outer pixels", this);
	} HC_Close_Segment();
}

H3DF::PixelPoint::PixelPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "local window", &cInPoint, "outer pixels", this);
	} HC_Close_Segment();
}

H3DF::PixelPoint::PixelPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint)
{
	HC_Open_Segment_By_Key(cInWindow.GetSceneKey()); {
		HC_Compute_Coordinates(".", "outer window", &cInPoint, "outer pixels", this);
	} HC_Close_Segment();
}

H3DF::PixelPoint::PixelPoint(ObjectPoint const & cInPoint) {}
H3DF::PixelPoint::PixelPoint(WorldPoint const & cInPoint) {}
H3DF::PixelPoint::PixelPoint(InnerWindowPoint const & cInPoint) {}
H3DF::PixelPoint::PixelPoint(WindowPoint const & cInPoint) {}