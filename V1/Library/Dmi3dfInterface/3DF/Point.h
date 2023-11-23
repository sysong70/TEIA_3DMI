#pragma once

#include "3DF.h"
#include "Math.h"

// using WindowPointArray = CAtlArray<WindowPoint>;
// using WorldPointArray = CAtlArray<WorldPoint>;
// using PixelPointArray = CAtlArray<PixelPoint>;
class HPoint;

namespace H3DF
{
	class API_3DF ObjectPoint : public Point
	{
	public:
		ObjectPoint(float px = 0.0f, float py = 0.0f, float pz = 0.0f) : Point(px, py, pz) {}
		ObjectPoint(Point const & cInPoint) :Point(cInPoint) {}
		ObjectPoint(HPoint & cInPoint);

		//ObjectPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint);
		ObjectPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint);
		//ObjectPoint(WindowKey const & cInWindow, CameraPoint const & cInPoint);
		ObjectPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint);
		//ObjectPoint(WindowKey const & cInWindow, InnerPixelPoint const & cInPoint);
		ObjectPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint);
		ObjectPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint);

	private:

		ObjectPoint(WorldPoint const & cInPoint); // Prevent implicit conversion to other types of points
		//ObjectPoint(CameraPoint const & cInPoint); // Prevent implicit conversion to other types of points
		ObjectPoint(InnerWindowPoint const & cInPoint); // Prevent implicit conversion to other types of points
		//ObjectPoint(InnerPixelPoint const & cInPoint); // Prevent implicit conversion to other types of points
		ObjectPoint(WindowPoint const & cInPoint); // Prevent implicit conversion to other types of points
		ObjectPoint(PixelPoint const & cInPoint); // Prevent implicit conversion to other types of points

	};

	class API_3DF WorldPoint : public Point
	{
	public:
		WorldPoint(float px = 0.0f, float py = 0.0f, float pz = 0.0f) : Point(px, py, pz) {}
		WorldPoint(Point const & cInPoint) :Point(cInPoint) {}
		WorldPoint(HPoint & cInPoint);

		WorldPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint);
		// 	WorldPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint);
		// 	WorldPoint(WindowKey const & cInWindow, CameraPoint const & cInPoint);
		WorldPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint);
		// 	WorldPoint(WindowKey const & cInWindow, InnerPixelPoint const & cInPoint);
		WorldPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint);
		WorldPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint);

		//WorldPoint & operator = (HPoint const & cInPoint) { x = cInPoint.x, y = cInPoint.y, z = cInPoint.z; return *this; }

	private:
		WorldPoint(ObjectPoint const & cInPoint); // Prevents implicit conversion to other points
		// 	WorldPoint(CameraPoint const & cInPoint); // Prevents implicit conversion to other points
		WorldPoint(InnerWindowPoint const & cInPoint); // Prevents implicit conversion to other points
		// 	WorldPoint(InnerPixelPoint const & cInPoint); // Prevents implicit conversion to other points
		WorldPoint(WindowPoint const & cInPoint); // Prevents implicit conversion to other points
		WorldPoint(PixelPoint const & cInPoint); // Prevents implicit conversion to other points
	};

	class API_3DF InnerWindowPoint : public Point
	{
	public:
		InnerWindowPoint(float px = 0.0f, float py = 0.0f, float pz = 0.0f) : Point(px, py, pz) {}
		InnerWindowPoint(Point const & cInPoint) :Point(cInPoint) {}
		InnerWindowPoint(HPoint & cInPoint);

		InnerWindowPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint);
		InnerWindowPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint);
		//InnerWindowPoint(WindowKey const & cInWindow, CameraPoint const & cInPoint);
		//InnerWindowPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint);
		//InnerWindowPoint(WindowKey const & cInWindow, InnerPixelPoint const & cInPoint);
		InnerWindowPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint);
		InnerWindowPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint);

	private:
		InnerWindowPoint(ObjectPoint const & cInPoint); // Prevents implicit conversion to other points
		InnerWindowPoint(WorldPoint const & cInPoint); // Prevents implicit conversion to other points
		//InnerWindowPoint(CameraPoint const & cInPoint); // Prevents implicit conversion to other points
		//InnerWindowPoint(InnerPixelPoint const & cInPoint); // Prevents implicit conversion to other points
		InnerWindowPoint(WindowPoint const & cInPoint); // Prevents implicit conversion to other points
		InnerWindowPoint(PixelPoint const & cInPoint); // Prevents implicit conversion to other points
	};

	class API_3DF WindowPoint : public Point
	{
	public:
		WindowPoint(float px = 0.0f, float py = 0.0f, float pz = 0.0f) : Point(px, py, pz) {}
		WindowPoint(Point const & cInPoint) :Point(cInPoint) {}
		WindowPoint(HPoint & cInPoint);

		WindowPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint);
		WindowPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint);
		// 	WindowPoint(WindowKey const & cInWindow, CameraPoint const & cInPoint);
		WindowPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint);
		// 	WindowPoint(WindowKey const & cInWindow, InnerPixelPoint const & cInPoint);
		// 	WindowPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint);
		WindowPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint);

		//WindowPoint & operator = (HPoint const & cInPoint) { x = cInPoint.x, y = cInPoint.y, z = cInPoint.z; return *this; }

	private:
		WindowPoint(ObjectPoint const & cInPoint); // Prevents implicit conversion to other points
		WindowPoint(WorldPoint const & cInPoint); // Prevents implicit conversion to other points
		// 	WindowPoint(CameraPoint const & cInPoint); // Prevents implicit conversion to other points
		WindowPoint(InnerWindowPoint const & cInPoint); // Prevents implicit conversion to other points
		// 	WindowPoint(InnerPixelPoint const & cInPoint); // Prevents implicit conversion to other points
		WindowPoint(PixelPoint const & cInPoint); // Prevents implicit conversion to other points
	};

	class API_3DF PixelPoint : public Point
	{
	public:
		PixelPoint(float px = 0.0f, float py = 0.0f, float pz = 0.0f) : Point(px, py, pz) {}
		PixelPoint(Point const & cInPoint) :Point(cInPoint) {}
		PixelPoint(HPoint & cInPoint);

		PixelPoint(WindowKey const & cInWindow, ObjectPoint const & cInPoint);
		PixelPoint(WindowKey const & cInWindow, WorldPoint const & cInPoint);
		//	PixelPoint(WindowKey const & cInWindow, CameraPoint const & cInPoint);
		PixelPoint(WindowKey const & cInWindow, InnerWindowPoint const & cInPoint);
		//	PixelPoint(WindowKey const & cInWindow, InnerPixelPoint const & cInPoint);
		PixelPoint(WindowKey const & cInWindow, WindowPoint const & cInPoint);
		//	PixelPoint(WindowKey const & cInWindow, PixelPoint const & cInPoint);

			//PixelPoint & operator = (HPoint const & cInPoint) { x = cInPoint.x, y = cInPoint.y, z = cInPoint.z; return *this; }

	private:
		PixelPoint(ObjectPoint const & cInPoint); // Prevents implicit conversion to other points
		PixelPoint(WorldPoint const & cInPoint); // Prevents implicit conversion to other points
		//	PixelPoint(CameraPoint const & cInPoint); // Prevents implicit conversion to other points
		PixelPoint(InnerWindowPoint const & cInPoint); // Prevents implicit conversion to other points
		//	PixelPoint(InnerPixelPoint const & cInPoint); // Prevents implicit conversion to other points
		PixelPoint(WindowPoint const & cInPoint); // Prevents implicit conversion to other points
	};
}