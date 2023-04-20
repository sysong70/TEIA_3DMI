#pragma once

#include "3DF.h"
#include "3DF.Math.h"

#include "HTools.h"
#include "HGlobals.h"

OPEN_3DF_NAMESPACE

// using WindowPointArray = CAtlArray<WindowPoint>;
// using WorldPointArray = CAtlArray<WorldPoint>;
// using PixelPointArray = CAtlArray<PixelPoint>;

class WorldPoint : public Point
{
public:
	WorldPoint(float px = 0.0f, float py = 0.0f, float pz = 0.0f) : Point(px, py, pz) {}
	WorldPoint(Point const & cInPoint) :Point(cInPoint) {}
	WorldPoint(HPoint cInPoint) : Point(cInPoint.x, cInPoint.y, cInPoint.z) {}

// 	WorldPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint);
// 	WorldPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint);
// 	WorldPoint(WindowKey const & cInWindow, CameraPoint const & cInPoint);
// 	WorldPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint);
// 	WorldPoint(WindowKey const & cInWindow, InnerPixelPoint const & cInPoint);
 	WorldPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint);
 	WorldPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint);

	WorldPoint & operator = (HPoint const & cInPoint) { x = cInPoint.x, y = cInPoint.y, z = cInPoint.z; return *this; }

private:
// 	WorldPoint(ObjectPoint const & cInPoint); // Prevents implicit conversion to other points
// 	WorldPoint(CameraPoint const & cInPoint); // Prevents implicit conversion to other points
// 	WorldPoint(InnerWindowPoint const & cInPoint); // Prevents implicit conversion to other points
// 	WorldPoint(InnerPixelPoint const & cInPoint); // Prevents implicit conversion to other points
 	WorldPoint(WindowPoint const & cInPoint); // Prevents implicit conversion to other points
 	WorldPoint(PixelPoint const & cInPoint); // Prevents implicit conversion to other points
};

class WindowPoint : public Point
{
public:
	WindowPoint(float px = 0.0f, float py = 0.0f, float pz = 0.0f) : Point(px, py, pz) {}
	WindowPoint(Point const & cInPoint) :Point(cInPoint) {}
	WindowPoint(HPoint cInPoint) : Point(cInPoint.x, cInPoint.y, cInPoint.z) {}

// 	WindowPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint);
 	WindowPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint);
// 	WindowPoint(WindowKey const & cInWindow, CameraPoint const & cInPoint);
// 	WindowPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint);
// 	WindowPoint(WindowKey const & cInWindow, InnerPixelPoint const & cInPoint);
// 	WindowPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint);
 	WindowPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint);

	WindowPoint & operator = (HPoint const & cInPoint) { x = cInPoint.x, y = cInPoint.y, z = cInPoint.z; return *this; }

private:
// 	WindowPoint(ObjectPoint const & cInPoint); // Prevents implicit conversion to other points
 	WindowPoint(WorldPoint const & cInPoint); // Prevents implicit conversion to other points
// 	WindowPoint(CameraPoint const & cInPoint); // Prevents implicit conversion to other points
// 	WindowPoint(InnerWindowPoint const & cInPoint); // Prevents implicit conversion to other points
// 	WindowPoint(InnerPixelPoint const & cInPoint); // Prevents implicit conversion to other points
 	WindowPoint(PixelPoint const & cInPoint); // Prevents implicit conversion to other points
};

class PixelPoint : public Point
{
public:
	PixelPoint(float px = 0.0f, float py = 0.0f, float pz = 0.0f) : Point(px, py, pz) {}
	PixelPoint(Point const & cInPoint) :Point(cInPoint) {}
	PixelPoint(HPoint cInPoint) : Point(cInPoint.x, cInPoint.y, cInPoint.z) {}
	
//	PixelPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint);
	PixelPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint);
//	PixelPoint(WindowKey const & cInWindow, CameraPoint const & cInPoint);
//	PixelPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint);
//	PixelPoint(WindowKey const & cInWindow, InnerPixelPoint const & cInPoint);
	PixelPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint);
//	PixelPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint);

	PixelPoint & operator = (HPoint const & cInPoint) { x = cInPoint.x, y = cInPoint.y, z = cInPoint.z; return *this; }

private:
//	PixelPoint(ObjectPoint const & cInPoint); // Prevents implicit conversion to other points
	PixelPoint(WorldPoint const & cInPoint); // Prevents implicit conversion to other points
//	PixelPoint(CameraPoint const & cInPoint); // Prevents implicit conversion to other points
//	PixelPoint(InnerWindowPoint const & cInPoint); // Prevents implicit conversion to other points
//	PixelPoint(InnerPixelPoint const & cInPoint); // Prevents implicit conversion to other points
	PixelPoint(WindowPoint const & cInPoint); // Prevents implicit conversion to other points

};

CLOSE_3DF_NAMESPACE