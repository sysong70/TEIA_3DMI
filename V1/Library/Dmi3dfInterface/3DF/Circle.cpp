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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<CircleKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const CircleKitImpl * pcInThat)
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
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<CircleKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::CircleKit::CircleKit(CircleKit const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);

}

CircleKit const & H3DF::CircleKit::operator = (CircleKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

CircleKit & H3DF::CircleKit::SetCenter(Point const & cInCenter)
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	pcImpl->m_cCenter = cInCenter;
	return *this;
}

CircleKit & H3DF::CircleKit::SetRadius(double dInRadius)
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	pcImpl->m_dRadius = dInRadius;
	return *this;
}

CircleKit & H3DF::CircleKit::SetXAxis(Vector const & cInAxis)
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	pcImpl->m_cXAxis = cInAxis;
	return *this;
}

CircleKit & H3DF::CircleKit::SetYAxis(Vector const & cInAxis)
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	pcImpl->m_cYAxis = cInAxis;
	return *this;
}

bool H3DF::CircleKit::ShowCenter(Point & cOutCenter) const
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	cOutCenter = pcImpl->m_cCenter;
	return true;
}

bool H3DF::CircleKit::ShowRadius(float & cOutRadius) const
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	cOutRadius = (float)pcImpl->m_dRadius;
	return true;
}

bool H3DF::CircleKit::ShowXAxis(Vector & cOutAxis) const
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	cOutAxis = pcImpl->m_cXAxis;
	return true;
}

bool H3DF::CircleKit::ShowYAxis(Vector & cOutAxis) const
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	cOutAxis = pcImpl->m_cYAxis;
	return true;
}

bool H3DF::CircleKit::ShowNormal(Vector & cOutNormal) const
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	cOutNormal = pcImpl->m_cXAxis.Cross(pcImpl->m_cYAxis);
	return true;
}

bool H3DF::CircleKit::ShowPoint(float fInAngle, Point & cOutPoint) const
{
	auto pcImpl = static_cast<CircleKitImpl *>(m_pcImpl.get());
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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<CircleKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const CircleKeyImpl * pcInThat) {
			KeyImpl::Copy(pcInThat);
			m_cCircleKit = pcInThat->m_cCircleKit;
		}

		CircleKit m_cCircleKit;
	};
};

H3DF::CircleKey::CircleKey() : GeometryKey(INVALID_KEY)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<CircleKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::CircleKey::CircleKey(HC_KEY nInKey)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<CircleKeyImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetKeyValue(nInKey);
}

H3DF::CircleKey::CircleKey(Key const & cInKey)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<CircleKeyImpl>();
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const KeyImpl *>(cInKey.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->KeyImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}
}

H3DF::CircleKey::CircleKey(CircleKey const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);

}

CircleKey & H3DF::CircleKey::operator=(CircleKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

CircleKey & H3DF::CircleKey::SetCenter(Point const & cInCenter)
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	pcImpl->m_cCircleKit.SetCenter(cInCenter);	
	return *this;
}

CircleKey & H3DF::CircleKey::SetRadius(double dInRadius)
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	pcImpl->m_cCircleKit.SetRadius(dInRadius);	
	return *this;
}

CircleKey & H3DF::CircleKey::SetXAxis(Vector const & cInAxis)
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	pcImpl->m_cCircleKit.SetXAxis(cInAxis);
	return *this;
}

CircleKey & H3DF::CircleKey::SetYAxis(Vector const & cInAxis)
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	pcImpl->m_cCircleKit.SetYAxis(cInAxis);
	return *this;
}

bool H3DF::CircleKey::ShowCenter(Point & cOutCenter) const
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	return pcImpl->m_cCircleKit.ShowCenter(cOutCenter);
}

bool H3DF::CircleKey::ShowRadius(float & cOutRadius) const
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	return pcImpl->m_cCircleKit.ShowRadius(cOutRadius);
}

bool H3DF::CircleKey::ShowXAxis(Vector & cOutAxis) const
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	return pcImpl->m_cCircleKit.ShowXAxis(cOutAxis);
}

bool H3DF::CircleKey::ShowYAxis(Vector & cOutAxis) const
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	return pcImpl->m_cCircleKit.ShowYAxis(cOutAxis);
}

bool H3DF::CircleKey::ShowNormal(Vector & cOutNormal) const
{
	auto pcImpl = static_cast<CircleKeyImpl *>(m_pcImpl.get());
	return pcImpl->m_cCircleKit.ShowNormal(cOutNormal);
}
