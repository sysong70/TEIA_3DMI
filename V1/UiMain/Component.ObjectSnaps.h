#pragma once

#include "Component.h"
#include <Signal.h>
#include <Json.h>
#include <vector>



namespace Component
{
	class ObjectSnaps
	{
	public:

		ObjectSnaps();

		~ObjectSnaps();

		void Add(Signal::ObjectSnapPoint point);

		void Add(int id, int x, int y, Signal::EObjectSnap type);

		void Set(Json::Object* pData);

		void Clear();

		void Draw(CBCGPGraphicsManager* manager);

		int PointIn(CPoint point);

	private:

		Signal::ObjectSnapPoints m_points;

		CBCGPGeometryGroup m_geometries;
		CBCGPBrush m_brush = CBCGPColor::Red;

		CBCGPGeometry* MakeGeometry(Signal::ObjectSnapPoint& point);
	};
}

