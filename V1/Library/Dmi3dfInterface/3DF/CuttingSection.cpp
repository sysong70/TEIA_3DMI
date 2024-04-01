#include <StdAfx.h>

#include "CuttingSection.h"

#include "Impl/KeyImpl.h"

#include <Common_Define.h>

using namespace H3DF;

//== CuttingSectionKit Class =======================================================================

namespace H3DF
{
	class CuttingSectionKitImpl : public Impl
	{
	public:
		void Copy(CuttingSectionKitImpl * that)
		{
			m_arPlanes = that->m_arPlanes;
		}

		PlaneArray m_arPlanes;
	};
}

H3DF::CuttingSectionKit::CuttingSectionKit()
{
	m_pcImpl = new CuttingSectionKitImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::CuttingSectionKit::CuttingSectionKit(CuttingSectionKit const & cInKit)
{
	m_pcImpl = new CuttingSectionKitImpl();
	DEBUG_VALID(m_pcImpl);
	Set(cInKit);
}

void H3DF::CuttingSectionKit::Set(CuttingSectionKit const & cInThat)
{
	CuttingSectionKitImpl * pcImpl = (CuttingSectionKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	CuttingSectionKitImpl * pcInThatImpl = (CuttingSectionKitImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

CuttingSectionKit & H3DF::CuttingSectionKit::operator = (CuttingSectionKit const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::CuttingSectionKit::Show(CuttingSectionKit & cOutKit) const
{
	cOutKit = *this;
}

bool H3DF::CuttingSectionKit::Empty() const
{
	CuttingSectionKitImpl * pcImpl = (CuttingSectionKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_arPlanes.clear();

	return true;
}

bool H3DF::CuttingSectionKit::Equals(CuttingSectionKit const & cInKit) const
{
	CuttingSectionKitImpl * pcImpl = (CuttingSectionKitImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	CuttingSectionKitImpl * pcInKitImpl = (CuttingSectionKitImpl *)cInKit.m_pcImpl;
	DEBUG_VALID(pcInKitImpl);

	if (pcImpl->m_arPlanes.size() != pcInKitImpl->m_arPlanes.size()) {
		return false;
	}

	for (size_t nIndex = 0; nIndex < pcImpl->m_arPlanes.size(); nIndex++) {
		if (pcImpl->m_arPlanes[nIndex] != pcInKitImpl->m_arPlanes[nIndex]) {
			return false;
		}
	}

	return true;
}

bool H3DF::CuttingSectionKit::operator==(CuttingSectionKit const & cInKit) const
{
	return Equals(cInKit);
}

bool H3DF::CuttingSectionKit::operator!=(CuttingSectionKit const & cInKit) const
{
	return !Equals(cInKit);
}

CuttingSectionKit & H3DF::CuttingSectionKit::SetPlanes(H3DF::Plane const & cInPlane)
{
	CuttingSectionKitImpl * pcImpl = (CuttingSectionKitImpl *)m_pcImpl;

	// Clear the existing planes and add the new one.
	pcImpl->m_arPlanes.clear();
	pcImpl->m_arPlanes.push_back(cInPlane);

	return *this;
}

// Sets multiple cutting planes for this CuttingSectionKit.
CuttingSectionKit & H3DF::CuttingSectionKit::SetPlanes(H3DF::PlaneArray const & cInPlanes)
{
	CuttingSectionKitImpl * pcImpl = (CuttingSectionKitImpl *)m_pcImpl;

	pcImpl->m_arPlanes.clear();
	pcImpl->m_arPlanes = cInPlanes;

	return *this;
}

CuttingSectionKit & H3DF::CuttingSectionKit::SetVisualization(CuttingSection::Mode eInMode, RGBAColor const & cInColor, float fInScale)
{
	return *this;
}

//== CuttingSectionKey Class =======================================================================

namespace H3DF
{
	class CuttingSectionKeyImpl : public KeyImpl
	{
	public:
		void Copy(CuttingSectionKeyImpl * that)
		{
			m_arPlanes = that->m_arPlanes;
		}

		PlaneArray m_arPlanes;
	};
}


H3DF::CuttingSectionKey::CuttingSectionKey() : GeometryKey(INVALID_KEY)
{
	m_pcImpl = new CuttingSectionKeyImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::CuttingSectionKey::CuttingSectionKey(Key const & cInThat) : GeometryKey(INVALID_KEY)
{
	CuttingSectionKeyImpl * pcImpl = new CuttingSectionKeyImpl();
	DEBUG_VALID(pcImpl);
	m_pcImpl = pcImpl;

	((KeyImpl *)pcImpl)->Copy((KeyImpl *)(cInThat.GetImpl()));

	// 외부에서 들어오는 Key는 CuttingSectionKey가 아닐 수 있으므로, ShellKey로 변경한다.
	pcImpl->SetType(H3DF::Type::CuttingSectionKey);
}

H3DF::CuttingSectionKey::CuttingSectionKey(CuttingSectionKey const & cInThat) : GeometryKey(INVALID_KEY)
{
	m_pcImpl = new CuttingSectionKeyImpl();
	Set(cInThat);
}

void H3DF::CuttingSectionKey::Set(CuttingSectionKey const & cInThat)
{
	CuttingSectionKeyImpl * pcImpl = (CuttingSectionKeyImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);
	CuttingSectionKeyImpl * pcInThatImpl = (CuttingSectionKeyImpl *)cInThat.m_pcImpl;
	DEBUG_VALID(pcInThatImpl);

	pcImpl->Copy(pcInThatImpl);
}

CuttingSectionKey & H3DF::CuttingSectionKey::operator = (CuttingSectionKey const & cInThat)
{
	Key::Set(cInThat);
	Set(cInThat);
	return *this;
}