#include "StdAfx.h"

#include "3DF.Circle.h"

#include "3DF.Math.h"
#include "3DF.Point.h"

#include "Private/3DF.KeyPrivate.h"

#include <HTools.h>

USING_3DF_NAMESPACE

class CircleKitPrivate : public PrivateImpl
{
public:
	void Copy(CircleKitPrivate * that)
	{
	}
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


//== CircleKey =====================================================================================
namespace TDF {

	class CircleKeyPrivate : public TDF::KeyPrivate
	{
	public:
		TDF::Type Type() const override { return TDF::Type::CircleKey; }

		void Copy(CircleKeyPrivate * pcInThat) {
			KeyPrivate::Copy(pcInThat);
		}
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