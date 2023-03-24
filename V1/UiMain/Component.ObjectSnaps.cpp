#include "stdafx.h"
#include "Component.ObjectSnaps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetObjectSnaps

namespace PresetObjectSnaps
{
	CSize Size(20, 20);
	CSize HalfSize(10, 10);
	CRect Boundary(-10, -10, 10, 10);
}



Component::ObjectSnaps::ObjectSnaps()
{
}



Component::ObjectSnaps::~ObjectSnaps()
{
	Clear();
}



void Component::ObjectSnaps::Add(Signal::ObjectSnapPoint point)
{
	m_points.push_back(point);
	m_geometries.AddGeometry(MakeGeometry(point));
}



void Component::ObjectSnaps::Add(int id, int x, int y, Signal::EObjectSnap type)
{
	Add({ id, x, y, type });
}



void Component::ObjectSnaps::Set(Json::Object* pData)
{
	Json::Object& data = *pData;
	Json::Array& items = data.GetArray(SKW_ITEMS);

	for (auto item : items.GetBuffer()) {
		Json::Object& child = item->AsObject();

		Add(child.GetInteger(SKW_ID), child.GetInteger(SKW_X), child.GetInteger(SKW_Y),
			(Signal::EObjectSnap)child.GetInteger(SKW_TYPE));
	}

	REMOVE_POINTER(pData);
}



void Component::ObjectSnaps::Clear()
{
	m_points.clear();
	m_geometries.Clear();
}



void Component::ObjectSnaps::Draw(CBCGPGraphicsManager* manager)
{
	if (m_geometries.GetGeometries().GetSize() > 0) {
		manager->DrawGeometry(m_geometries, m_brush, 2);
	}

	manager->DrawLine(0, 0, 500, 500, CBCGPBrush(CBCGPColor::Red), 5);
}



int Component::ObjectSnaps::PointIn(CPoint point)
{
	for (auto item : m_points) {
		CRect rect(item.X, item.Y, item.X, item.Y);
		rect.InflateRect(PRESET::Boundary);

		if (rect.PtInRect(point)) {
			return item.Id;
		}
	}

	return -1;
}



CBCGPGeometry* Component::ObjectSnaps::MakeGeometry(Signal::ObjectSnapPoint& point)
{
	//:TODO
	CRect rect(point.X, point.Y, point.X, point.Y);
	rect.InflateRect(PRESET::Boundary);

	CBCGPEllipseGeometry* pGeom = new CBCGPEllipseGeometry(CBCGPRect(rect.left, rect.top, rect.right, rect.bottom));
	return pGeom;

	//switch (point.Type) {
	//// cross + circle
	//case Signal::EObjectSnap::Point: break;
	//// square
	//case Signal::EObjectSnap::End: break;
	//// triangle
	//case Signal::EObjectSnap::Mid: break;
	//// cross
	//case Signal::EObjectSnap::Intersection: break;
	//// L
	//case Signal::EObjectSnap::Perpendicular: break;
	//// circle
	//case Signal::EObjectSnap::Center: break;
	//// diamond
	//case Signal::EObjectSnap::Quadrant: break;
	//// snadglass
	//case Signal::EObjectSnap::Near: break;

	//case Signal::EObjectSnap::OnSurface: break;
	//case Signal::EObjectSnap::BoundaryCenter: break;
	//case Signal::EObjectSnap::Axis: break;
	//default:
	//}

	//RETURN_NULL;
}
