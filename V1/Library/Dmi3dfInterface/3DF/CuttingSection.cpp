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
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<CuttingSectionKitImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const CuttingSectionKitImpl * that) {
			m_arPlanes = that->m_arPlanes;
			m_eMode = that->m_eMode;
			m_bModeSet = that->m_bModeSet;
			m_cColor = that->m_cColor;
			m_bColorSet = that->m_bColorSet;
			m_fScale = that->m_fScale;
			m_bScaleSet = that->m_bScaleSet;
		}

		PlaneArray m_arPlanes;

		CuttingSection::Mode m_eMode = CuttingSection::Mode::None; bool m_bModeSet = false;
		RGBAColor m_cColor; bool m_bColorSet = false;
		float m_fScale = 1.0f; bool m_bScaleSet = false;
	};
}

H3DF::CuttingSectionKit::CuttingSectionKit()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<CuttingSectionKitImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::CuttingSectionKit::CuttingSectionKit(CuttingSectionKit const & cInKit)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInKit.m_pcImpl) ? cInKit.m_pcImpl->Clone() : nullptr;
}

CuttingSectionKit & H3DF::CuttingSectionKit::operator = (CuttingSectionKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::CuttingSectionKit::Show(CuttingSectionKit & cOutKit) const
{
	cOutKit = *this;
}

bool H3DF::CuttingSectionKit::Empty() const
{
	auto pcImpl = static_cast<CuttingSectionKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arPlanes.clear();

	return true;
}

bool H3DF::CuttingSectionKit::Equals(CuttingSectionKit const & cInKit) const
{
	auto pcImpl = static_cast<CuttingSectionKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	auto pcInKitImpl = static_cast<CuttingSectionKitImpl *>(cInKit.m_pcImpl.get());
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
	auto pcImpl = static_cast<CuttingSectionKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	// Clear the existing planes and add the new one.
	pcImpl->m_arPlanes.clear();
	pcImpl->m_arPlanes.push_back(cInPlane);

	return *this;
}

// Sets multiple cutting planes for this CuttingSectionKit.
CuttingSectionKit & H3DF::CuttingSectionKit::SetPlanes(H3DF::PlaneArray const & cInPlanes)
{
	auto pcImpl = static_cast<CuttingSectionKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arPlanes.clear();
	pcImpl->m_arPlanes = cInPlanes;

	return *this;
}

/*
CuttingSectionKit & H3DF::CuttingSectionKit::SetVisualization(CuttingSection::Mode eInMode, RGBAColor const & cInColor, float fInScale)
{
	auto pcImpl = static_cast<CuttingSectionKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_eMode = eInMode;
	pcImpl->m_bModeSet = true;

	pcImpl->m_cColor = cInColor;
	pcImpl->m_bColorSet = true;

	pcImpl->m_fScale = fInScale;
	pcImpl->m_bScaleSet = true;

	return *this;
}
*/

CuttingSectionKit & H3DF::CuttingSectionKit::SetVisualization(RGBAColor const & cInColor)
{
	auto pcImpl = static_cast<CuttingSectionKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cColor = cInColor;
	pcImpl->m_bColorSet = true;

	return *this;
}

//== CuttingSectionKey Class =======================================================================

namespace H3DF
{
	class CuttingSectionKeyImpl : public KeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<CuttingSectionKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const CuttingSectionKeyImpl * that) {
			m_arPlanes = that->m_arPlanes;
		}

		PlaneArray m_arPlanes;
	};
}


H3DF::CuttingSectionKey::CuttingSectionKey()
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = std::make_unique<CuttingSectionKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::CuttingSectionKey::CuttingSectionKey(Key const & cInThat)
{
	// cInThat이 올바른 Impl(CuttingSectionKeyImpl)을 가지고 있으면 복제
	if (cInThat.GetImpl()) {
		// 만약 CuttingSectionKeyImpl이 KeyImpl에서 파생된 구조라면 dynamic_cast에 의해서 nullptr이 아닌 정상적인 값이 넘어옴
		auto pcSrcImpl = dynamic_cast<const CuttingSectionKeyImpl *>(cInThat.GetImpl());
		if (nullptr != pcSrcImpl) {
			m_pcImpl = pcSrcImpl->Clone();
		}
		else {
			// 타입이 다를 경우 예외 처리 또는 방어적 초기화
			m_pcImpl = std::make_unique<CuttingSectionKeyImpl>();
			static_cast<CuttingSectionKeyImpl *>(m_pcImpl.get())->Copy((CuttingSectionKeyImpl *) (cInThat.GetImpl()));
		}
	}
	else {
		m_pcImpl = std::make_unique<CuttingSectionKeyImpl>();
		static_cast<CuttingSectionKeyImpl *>(m_pcImpl.get())->Copy((CuttingSectionKeyImpl *) (cInThat.GetImpl()));
	}
}

H3DF::CuttingSectionKey::CuttingSectionKey(CuttingSectionKey const & cInThat)
{
	if (staticType != Type()) {
		return;
	}

	m_pcImpl = (nullptr == cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

CuttingSectionKey & H3DF::CuttingSectionKey::operator = (CuttingSectionKey const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}