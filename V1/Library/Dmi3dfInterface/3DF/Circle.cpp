#include "StdAfx.h"

#include "Circle.h"

#include "Math.h"
#include "Point.h"

#include "Impl/KeyImpl.h"
#include "Impl/GeometryImpl.h"

#include <HTools.h>

using namespace H3DF;

namespace H3DF
{
	class CircleKitImpl : public Impl
	{
	public:
		void Copy(CircleKitImpl * pcInThat)
		{
			m_cCenter = pcInThat->m_cCenter;
			m_dRadius = pcInThat->m_dRadius;
			m_cYAxis = pcInThat->m_cYAxis;
		}

		Point m_cCenter;
		double m_dRadius = -1.0;
		Vector m_cYAxis;
		Vector m_cXAxis;
	};
}

H3DF::CircleKit::CircleKit()
{
	m_pcImpl = new CircleKitImpl();
}

H3DF::CircleKit::CircleKit(CircleKit const & cInThat)
{
	m_pcImpl = new CircleKitImpl();
	Set(cInThat);
}

void H3DF::CircleKit::Set(CircleKit const & cInThat)
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	CircleKitImpl * pcInThatImpl = static_cast<CircleKitImpl *>(cInThat.m_pcImpl);
	pcImpl->Copy(pcInThatImpl);
}

CircleKit const & H3DF::CircleKit::operator = (CircleKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

CircleKit & H3DF::CircleKit::SetCenter(Point const & cInCenter)
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	pcImpl->m_cCenter = cInCenter;
	return *this;
}

CircleKit & H3DF::CircleKit::SetRadius(double dInRadius)
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	pcImpl->m_dRadius = dInRadius;
	return *this;
}

CircleKit & H3DF::CircleKit::SetXAxis(Vector const & cInAxis)
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	pcImpl->m_cXAxis = cInAxis;
	return *this;
}

CircleKit & H3DF::CircleKit::SetYAxis(Vector const & cInAxis)
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	pcImpl->m_cYAxis = cInAxis;
	return *this;
}

bool H3DF::CircleKit::ShowCenter(Point & cOutCenter) const
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	cOutCenter = pcImpl->m_cCenter;
	return true;
}

bool H3DF::CircleKit::ShowRadius(float & cOutRadius) const
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	cOutRadius = (float)pcImpl->m_dRadius;
	return true;
}

bool H3DF::CircleKit::ShowXAxis(Vector & cOutAxis) const
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	cOutAxis = pcImpl->m_cXAxis;
	return true;
}

bool H3DF::CircleKit::ShowYAxis(Vector & cOutAxis) const
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	cOutAxis = pcImpl->m_cYAxis;
	return true;
}

bool H3DF::CircleKit::ShowNormal(Vector & cOutNormal) const
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	cOutNormal = pcImpl->m_cXAxis.Cross(pcImpl->m_cYAxis);
	return true;
}

bool H3DF::CircleKit::ShowPoint(float fInAngle, Point & cOutPoint) const
{
	CircleKitImpl * pcImpl = static_cast<CircleKitImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	double dRadius = pcImpl->m_dRadius;

	Point2D cPoint;
	cPoint.x = float(dRadius * cos(fInAngle));
	cPoint.y = float(dRadius * sin(fInAngle));

	cOutPoint = cPoint.LiftPoint(pcImpl->m_cCenter, pcImpl->m_cXAxis, pcImpl->m_cYAxis);

	return true;
}

//== CircleKey =====================================================================================
namespace H3DF {

	class CircleKeyImpl : public GeometryKeyImpl
	{
	public:
		void Copy(CircleKeyImpl * pcInThat) {
			KeyImpl::Copy(pcInThat);
			m_cCircleKit = pcInThat->m_cCircleKit;
		}

		CircleKit m_cCircleKit;
	};
};

H3DF::CircleKey::CircleKey() : GeometryKey(INVALID_KEY)
{
	m_pcImpl = new CircleKeyImpl();
}

H3DF::CircleKey::CircleKey(HC_KEY nInKey) : GeometryKey(INVALID_KEY)
{
	CircleKeyImpl * pcImpl = new CircleKeyImpl();
	pcImpl->SetKeyValue(nInKey);

	m_pcImpl = pcImpl;
}

H3DF::CircleKey::CircleKey(Key const & cInKey) : GeometryKey(INVALID_KEY)
{
	CircleKeyImpl * pcImpl = new CircleKeyImpl();
	m_pcImpl = pcImpl;

	if(H3DF::Type::CircleKey != cInKey.ObjectType()) {
		DEBUG_STOP;
		return;
	}

	((KeyImpl *)pcImpl)->Copy((KeyImpl *)(cInKey.GetImpl()));
}

H3DF::CircleKey::CircleKey(CircleKey const & cInThat) : GeometryKey(INVALID_KEY)
{
	m_pcImpl = new CircleKeyImpl();
	Set(cInThat);
}

void H3DF::CircleKey::Set(CircleKey const & cInThat)
{
	if (nullptr == m_pcImpl || nullptr == cInThat.m_pcImpl) {
		return;
	}

	SetKeyValue(cInThat.KeyValue());

	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	CircleKeyImpl * pcInThatImpl = static_cast<CircleKeyImpl *>(cInThat.m_pcImpl);

	pcImpl->Copy(pcInThatImpl);
}

CircleKey & H3DF::CircleKey::operator=(CircleKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

CircleKey & H3DF::CircleKey::SetCenter(Point const & cInCenter)
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	pcImpl->m_cCircleKit.SetCenter(cInCenter);	
	return *this;
}

CircleKey & H3DF::CircleKey::SetRadius(double dInRadius)
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	pcImpl->m_cCircleKit.SetRadius(dInRadius);	
	return *this;
}

CircleKey & H3DF::CircleKey::SetXAxis(Vector const & cInAxis)
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	pcImpl->m_cCircleKit.SetXAxis(cInAxis);
	return *this;
}

CircleKey & H3DF::CircleKey::SetYAxis(Vector const & cInAxis)
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	pcImpl->m_cCircleKit.SetYAxis(cInAxis);
	return *this;
}

bool H3DF::CircleKey::ShowCenter(Point & cOutCenter) const
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowCenter(cOutCenter);
}

bool H3DF::CircleKey::ShowRadius(float & cOutRadius) const
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowRadius(cOutRadius);
}

bool H3DF::CircleKey::ShowXAxis(Vector & cOutAxis) const
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowXAxis(cOutAxis);
}

bool H3DF::CircleKey::ShowYAxis(Vector & cOutAxis) const
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowYAxis(cOutAxis);
}

bool H3DF::CircleKey::ShowNormal(Vector & cOutNormal) const
{
	CircleKeyImpl * pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowNormal(cOutNormal);
}
