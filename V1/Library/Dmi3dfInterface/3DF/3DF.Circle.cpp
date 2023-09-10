#include "StdAfx.h"

#include "3DF.Circle.h"

#include "Math.h"
#include "Point.h"

#include "Private/3DF.KeyPrivate.h"

#include <HTools.h>

USING_3DF_NAMESPACE

class CircleKitPrivate : public PrivateImpl
{
public:
	void Copy(CircleKitPrivate * pcInThat)
	{
		m_cCenter = pcInThat->m_cCenter;
		m_dRadius = pcInThat->m_dRadius;
		m_cNormal = pcInThat->m_cNormal;
	}

	Point m_cCenter;
	double m_dRadius = -1.0;
	Vector m_cNormal;
};

CircleKit::CircleKit()
{
	m_pcImpl = new CircleKitPrivate();
}

CircleKit::CircleKit(CircleKit const & cInThat)
{
	m_pcImpl = new CircleKitPrivate();
	Set(cInThat);
}

void CircleKit::Set(CircleKit const & cInThat)
{
	CircleKitPrivate * pcImpl = (CircleKitPrivate *)m_pcImpl;
	CircleKitPrivate * pcInThatImpl = (CircleKitPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

CircleKit const & CircleKit::operator=(CircleKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

CircleKit & CircleKit::SetCenter(Point const & cInCenter)
{
	CircleKitPrivate * pcImpl = (CircleKitPrivate *)m_pcImpl;
	pcImpl->m_cCenter = cInCenter;
	return *this;
}

CircleKit & CircleKit::SetRadius(double dInRadius)
{
	CircleKitPrivate * pcImpl = (CircleKitPrivate *)m_pcImpl;
	pcImpl->m_dRadius = dInRadius;
	return *this;
}

CircleKit & CircleKit::SetNormal(Vector const & cInNormal)
{
	CircleKitPrivate * pcImpl = (CircleKitPrivate *)m_pcImpl;
	pcImpl->m_cNormal = cInNormal;
	return *this;
}

bool CircleKit::ShowCenter(Point & cOutCenter) const
{
	CircleKitPrivate * pcImpl = (CircleKitPrivate *)m_pcImpl;
	cOutCenter = pcImpl->m_cCenter;
	return true;
}

bool CircleKit::ShowRadius(float & cOutRadius) const
{
	CircleKitPrivate * pcImpl = (CircleKitPrivate *)m_pcImpl;
	cOutRadius = (float)pcImpl->m_dRadius;
	return true;
}

bool CircleKit::ShowNormal(Vector & cOutNormal) const
{
	CircleKitPrivate * pcImpl = (CircleKitPrivate *)m_pcImpl;
	cOutNormal = pcImpl->m_cNormal;
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

CircleKey::CircleKey()
{
	m_pcImpl = new CircleKeyPrivate();
}

CircleKey::CircleKey(Key const & cInKey)
{
	CircleKeyPrivate * pcImpl = new CircleKeyPrivate();
	m_pcImpl = pcImpl;

	((KeyPrivate *)pcImpl)->Copy((KeyPrivate *)(cInKey.GetImpl()));

	// 외부에서 들어오는 Key는 CircleKey가 아닐 수 있으므로, CircleKey로 변경한다.
	pcImpl->SetType(H3DF::Type::CircleKey);
}

CircleKey::CircleKey(CircleKey const & cInThat)
{
	m_pcImpl = new CircleKeyPrivate();
	Set(cInThat);
}

void CircleKey::Set(CircleKey const & cInThat)
{
	if (nullptr == m_pcImpl || nullptr == cInThat.m_pcImpl) {
		return;
	}

	SetKeyValue(cInThat.KeyValue());

	CircleKeyPrivate * pcImpl = (CircleKeyPrivate *)m_pcImpl;
	CircleKeyPrivate * pcInThatImpl = (CircleKeyPrivate *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

CircleKey & CircleKey::operator=(CircleKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

CircleKey & CircleKey::SetCenter(Point const & cInCenter)
{
	CircleKeyPrivate * pcImpl = (CircleKeyPrivate *)m_pcImpl;
	pcImpl->m_cCircleKit.SetCenter(cInCenter);	
	return *this;
}

CircleKey & CircleKey::SetRadius(double dInRadius)
{
	CircleKeyPrivate * pcImpl = (CircleKeyPrivate *)m_pcImpl;
	pcImpl->m_cCircleKit.SetRadius(dInRadius);	
	return *this;
}

CircleKey & CircleKey::SetNormal(Vector const & cInNormal)
{
	CircleKeyPrivate * pcImpl = (CircleKeyPrivate *)m_pcImpl;
	pcImpl->m_cCircleKit.SetNormal(cInNormal);
	return *this;
}