#include "StdAfx.h"

#include "Reference.h"

#include "Math.h"
#include "Math.Matrix.h"
#include "Point.h"

#include "Impl/KeyImpl.h"
#include "Impl/GeometryImpl.h"

#include <HTools.h>

using namespace H3DF;

//== ReferenceKey =======================================================================================
namespace H3DF {

	class ReferenceKeyImpl : public GeometryKeyImpl
	{
	public:
		ReferenceKeyImpl() { m_eType = H3DF::Type::ReferenceKey; }
		~ReferenceKeyImpl();

		void Copy(ReferenceKeyImpl * pcInThat) {
			KeyImpl::Copy(pcInThat);
		}
	};
};

H3DF::ReferenceKeyImpl::~ReferenceKeyImpl()
{
	int i = 0;
}

H3DF::ReferenceKey::ReferenceKey() : GeometryKey(INVALID_KEY)
{
	m_pcImpl = new ReferenceKeyImpl();
}

H3DF::ReferenceKey::ReferenceKey(HC_KEY nInKey) : GeometryKey(INVALID_KEY)
{
	ReferenceKeyImpl * pcImpl = new ReferenceKeyImpl();
	pcImpl->SetKeyValue(nInKey);

	m_pcImpl = pcImpl;
}

H3DF::ReferenceKey::ReferenceKey(Key const & cInKey)
{
	ReferenceKeyImpl * pcImpl = new ReferenceKeyImpl();
	m_pcImpl = pcImpl;

	((KeyImpl *)pcImpl)->Copy((KeyImpl *)(cInKey.GetImpl()));

	// 외부에서 들어오는 Key는 ReferenceKey가 아닐 수 있으므로, ReferenceKey로 변경한다.
	pcImpl->SetType(H3DF::Type::ReferenceKey);
}

H3DF::ReferenceKey::ReferenceKey(ReferenceKey const & cInThat)
{
	m_pcImpl = new ReferenceKeyImpl();
	Set(cInThat);
}

H3DF::ReferenceKey::~ReferenceKey()
{
	int i = 0;
}

void H3DF::ReferenceKey::Set(ReferenceKey const & cInThat)
{
	if (nullptr == m_pcImpl || nullptr == cInThat.m_pcImpl) {
		return;
	}

	ReferenceKeyImpl * pcImpl = (ReferenceKeyImpl *)m_pcImpl;
	ReferenceKeyImpl * pcInThatImpl = (ReferenceKeyImpl *)cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

ReferenceKey & H3DF::ReferenceKey::operator=(ReferenceKey const & cInThat)
{
	Set(cInThat);
	return *this;
}

Key H3DF::ReferenceKey::GetTarget() const
{
	ReferenceKeyImpl * pcImpl = (ReferenceKeyImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = HC_Show_Reference_Geometry(pcImpl->KeyValue());
	Key cKey(nKey);

	return cKey;
}