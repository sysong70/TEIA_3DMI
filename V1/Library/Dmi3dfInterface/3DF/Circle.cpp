#include "StdAfx.h"

#include "Circle.h"

#include "Math.h"
#include "Point.h"

#include "Private/KeyPrivate.h"

#include <HTools.h>

using namespace H3DF;

namespace H3DF
{
	class CircleKitPrivate : public PrivateImpl
	{
	public:
		void Copy(CircleKitPrivate * pcInThat)
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
	m_pcImpl = new CircleKitPrivate();
}

H3DF::CircleKit::CircleKit(CircleKit const & cInThat)
{
	m_pcImpl = new CircleKitPrivate();
	Set(cInThat);
}

void H3DF::CircleKit::Set(CircleKit const & cInThat)
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	CircleKitPrivate * pcInThatImpl = static_cast<CircleKitPrivate *>(cInThat.m_pcImpl);
	pcImpl->Copy(pcInThatImpl);
}

CircleKit const & H3DF::CircleKit::operator = (CircleKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

CircleKit & H3DF::CircleKit::SetCenter(Point const & cInCenter)
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	pcImpl->m_cCenter = cInCenter;
	return *this;
}

CircleKit & H3DF::CircleKit::SetRadius(double dInRadius)
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	pcImpl->m_dRadius = dInRadius;
	return *this;
}

CircleKit & H3DF::CircleKit::SetXAxis(Vector const & cInAxis)
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	pcImpl->m_cXAxis = cInAxis;
	return *this;
}

CircleKit & H3DF::CircleKit::SetYAxis(Vector const & cInAxis)
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	pcImpl->m_cYAxis = cInAxis;
	return *this;
}

bool H3DF::CircleKit::ShowCenter(Point & cOutCenter) const
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	cOutCenter = pcImpl->m_cCenter;
	return true;
}

bool H3DF::CircleKit::ShowRadius(float & cOutRadius) const
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	cOutRadius = (float)pcImpl->m_dRadius;
	return true;
}

bool H3DF::CircleKit::ShowXAxis(Vector & cOutAxis) const
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	cOutAxis = pcImpl->m_cXAxis;
	return true;
}

bool H3DF::CircleKit::ShowYAxis(Vector & cOutAxis) const
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	cOutAxis = pcImpl->m_cYAxis;
	return true;
}

bool H3DF::CircleKit::ShowNormal(Vector & cOutNormal) const
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	cOutNormal = pcImpl->m_cXAxis.Cross(pcImpl->m_cYAxis);
	return true;
}

bool H3DF::CircleKit::ShowPoint(float fInAngle, Point & cOutPoint) const
{
	CircleKitPrivate * pcImpl = static_cast<CircleKitPrivate *>(m_pcImpl);
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

	class CircleKeyPrivate : public H3DF::KeyPrivate
	{
	public:
		CircleKeyPrivate() { m_eType = H3DF::Type::CircleKey; }

		void Copy(CircleKeyPrivate * pcInThat) {
			KeyPrivate::Copy(pcInThat);
			m_cCircleKit = pcInThat->m_cCircleKit;
		}

		CircleKit m_cCircleKit;
	};
};

H3DF::CircleKey::CircleKey()
{
	m_pcImpl = new CircleKeyPrivate();
}

H3DF::CircleKey::CircleKey(Key const & cInKey)
{
	CircleKeyPrivate * pcImpl = new CircleKeyPrivate();
	m_pcImpl = pcImpl;

	((KeyPrivate *)pcImpl)->Copy((KeyPrivate *)(cInKey.GetImpl()));

	// 외부에서 들어오는 Key는 CircleKey가 아닐 수 있으므로, CircleKey로 변경한다.
	pcImpl->SetType(H3DF::Type::CircleKey);
}

H3DF::CircleKey::CircleKey(CircleKey const & cInThat)
{
	m_pcImpl = new CircleKeyPrivate();
	Set(cInThat);
}

void H3DF::CircleKey::Set(CircleKey const & cInThat)
{
	if (nullptr == m_pcImpl || nullptr == cInThat.m_pcImpl) {
		return;
	}

	SetKeyValue(cInThat.KeyValue());

	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	CircleKeyPrivate * pcInThatImpl = static_cast<CircleKeyPrivate *>(cInThat.m_pcImpl);

	pcImpl->Copy(pcInThatImpl);
}

CircleKey & H3DF::CircleKey::operator=(CircleKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

CircleKey & H3DF::CircleKey::SetCenter(Point const & cInCenter)
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	pcImpl->m_cCircleKit.SetCenter(cInCenter);	
	return *this;
}

CircleKey & H3DF::CircleKey::SetRadius(double dInRadius)
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	pcImpl->m_cCircleKit.SetRadius(dInRadius);	
	return *this;
}

CircleKey & H3DF::CircleKey::SetXAxis(Vector const & cInAxis)
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	pcImpl->m_cCircleKit.SetXAxis(cInAxis);
	return *this;
}

CircleKey & H3DF::CircleKey::SetYAxis(Vector const & cInAxis)
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	pcImpl->m_cCircleKit.SetYAxis(cInAxis);
	return *this;
}

bool H3DF::CircleKey::ShowCenter(Point & cOutCenter) const
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowCenter(cOutCenter);
}

bool H3DF::CircleKey::ShowRadius(float & cOutRadius) const
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowRadius(cOutRadius);
}

bool H3DF::CircleKey::ShowXAxis(Vector & cOutAxis) const
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowXAxis(cOutAxis);
}

bool H3DF::CircleKey::ShowYAxis(Vector & cOutAxis) const
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowYAxis(cOutAxis);
}

bool H3DF::CircleKey::ShowNormal(Vector & cOutNormal) const
{
	CircleKeyPrivate * pcImpl = static_cast<CircleKeyPrivate *>(m_pcImpl);
	return pcImpl->m_cCircleKit.ShowNormal(cOutNormal);
}
