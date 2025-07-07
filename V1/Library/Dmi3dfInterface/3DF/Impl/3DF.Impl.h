#pragma once

#include <memory>

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
}
