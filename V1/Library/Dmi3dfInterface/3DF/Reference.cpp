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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<ReferenceKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const ReferenceKeyImpl * pcInThat) {
			KeyImpl::Copy(pcInThat);
		}
	};
};

H3DF::ReferenceKey::ReferenceKey()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ReferenceKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::ReferenceKey::ReferenceKey(HC_KEY nInKey)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<ReferenceKeyImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcImpl = static_cast<ReferenceKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->SetKeyValue(nInKey);
}

H3DF::ReferenceKey::ReferenceKey(Key const & cInKey)
{
	if (staticType != Type()) {
		return;
	}

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<ReferenceKeyImpl>();
	auto pcImpl = static_cast<ReferenceKeyImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const KeyImpl *>(cInKey.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->KeyImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}
}

H3DF::ReferenceKey::ReferenceKey(ReferenceKey const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.GetImpl()) ? cInThat.GetImpl()->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

H3DF::ReferenceKey::~ReferenceKey()
{
	int i = 0;
}

ReferenceKey & H3DF::ReferenceKey::operator=(ReferenceKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

Key H3DF::ReferenceKey::GetTarget() const
{
	auto pcImpl = static_cast<ReferenceKeyImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	HC_KEY nKey = HC_Show_Reference_Geometry(pcImpl->KeyValue());
	Key cKey(nKey);

	return cKey;
}