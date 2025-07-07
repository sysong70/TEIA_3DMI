#pragma once

#include "../3DF.h"

#include <map>
#include <memory>

#define IMPL(ClassName) \
    auto impl = static_cast<ClassName##Impl *>(m_pcImpl.get()); \
    DEBUG_VALID(impl)

#define IMPL_AS(ClassName, varName) \
    varName = static_cast<ClassName##Impl *>(m_pcImpl.get()); \
    DEBUG_VALID(varName)

#define INIT_IMPL(ClassName) \
	m_pcImpl = std::make_unique<ClassName##Impl>(); \
	DEBUG_VALID(m_pcImpl)

namespace H3DF
{
	class Impl
	{
	public:
		Impl() = default;
		virtual ~Impl() = default;

		// 순수 가상 함수(pure virtual function)임. 그래서 추상 class로 취급함.
		virtual std::unique_ptr<Impl> Clone() const = 0;
		virtual void Copy(const Impl * pcInThat);
		virtual bool Equal(const Impl * pcInThat) const;

		H3DF::Type Type() const { return m_eType; }
		void SetType(H3DF::Type eType) { m_eType = eType; }

	protected:
		H3DF::Type m_eType = H3DF::Type::None;
	};

	struct HoopsOption {
		CStringA name;
		BOOL enabled = TRUE;
		std::map<CStringA, CStringA> options;
	};

	std::vector<HoopsOption> ParseHoopsOptionString(const CStringA & input);
}
