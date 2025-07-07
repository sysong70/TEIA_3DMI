#include "StdAfx.h"

#include "Text.h"

#include "./Impl/GeometryImpl.h"

using namespace H3DF;

namespace H3DF
{
	class TextKeyImpl : public GeometryKeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<TextKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy (const Impl * pcInThat) override
		{
			if (pcInThat == nullptr) {
				DEBUG_RETURN;
			}

			GeometryKeyImpl::Copy(pcInThat);

			auto pcImpl = static_cast<const TextKeyImpl *>(pcInThat);
		}
	};
}

H3DF::TextKey::TextKey()
{
	m_pcImpl = std::make_unique<TextKeyImpl>();
	DEBUG_VALID(m_pcImpl);
}

H3DF::TextKey::TextKey(Key const & cInThat)
{
	m_pcImpl = std::make_unique<TextKeyImpl>();
	DEBUG_VALID(m_pcImpl);

	auto pcInKeyImpl = static_cast<const KeyImpl *>(cInThat.GetImpl());

	// 명시적 캐스팅을 해야 합니다.
	static_cast<KeyImpl *>(m_pcImpl.get())->Copy(pcInKeyImpl);
}

H3DF::TextKey::TextKey(TextKey const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
	DEBUG_VALID(m_pcImpl);
}

TextKey & H3DF::TextKey::operator=(TextKey const & cInOther)
{
	if (nullptr != cInOther.m_pcImpl) {
		m_pcImpl = cInOther.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

TextKey & H3DF::TextKey::SetBackground(bool bInState, CStringA strName)
{
	return *this;
}

TextKey & H3DF::TextKey::SetBackground(CStringA strName)
{
	return *this;
}

TextKey & H3DF::TextKey::SetBackground(bool bInState)
{
	return *this;
}

bool H3DF::TextKey::ShowBackground(bool & bOutState, CString & cOutName) const
{
	//HC_Show_Text()

	return false;
}

