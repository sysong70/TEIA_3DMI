#pragma once

#include "../AM.h"

#include "../Am.Object.h"

#include "../../3DX.h"

#include <map>
#include <memory>
#include <filesystem>

#define SET_IMPL(ClassName) \
	AM::Impl::setImpl(*this, std::unique_ptr<ClassName##Impl>());

#define IMPL(ClassName) \
	auto * impl = static_cast<ClassName##Impl *>(Impl::getImpl(*this).get()); \
	if (nullptr == impl) { DEBUG_STOP; return; } 

#define IMPL_AS(ClassName, varName) \
    varName = static_cast<ClassName##Impl *>(m_pcImpl.get()); \
    DEBUG_VALID(varName)

#define INIT_IMPL(ClassName) \
	m_pcImpl = std::make_unique<ClassName##Impl>(); \
	DEBUG_VALID(m_pcImpl)

namespace AM
{
	class Impl
	{
	public:
		Impl() = default;
		virtual ~Impl() = default;

		// Object::impl_에 대한 유일한 게이트
		static std::unique_ptr<Impl> const & getImpl(Object const & object) noexcept  { 
			return object.m_impl; 
		}

		static std::unique_ptr<Impl> & getImpl(Object & object) noexcept { 
			return object.m_impl;
		}

		static void setImpl(Object & object, std::unique_ptr<Impl> impl) noexcept { 
			object.m_impl = std::move(impl);
		}

		static void resetImpl(Object & object) noexcept { 
			object.m_impl.reset(); 
		}


		// 순수 가상 함수(pure virtual function)임. 그래서 추상 class로 취급함.
		virtual std::unique_ptr<Impl> Clone() const = 0;
		virtual void Copy(const Impl * pcInThat);
		virtual bool Equal(const Impl * pcInThat) const;

		AM::Type Type() const noexcept { return m_type; }
		void SetType(AM::Type type) { m_type = type; }

		Result Bind(std::ostream * os) noexcept;

	private:
		AM::Type m_type = AM::Type::None;
		std::ostream * m_os = nullptr;
	};

#define AM_IMPL_COMMON(Derived, EnumType)                                \
    Derived() { this->SetType(EnumType); }                               \
    Derived(Derived const&) = default;                                   \
    std::unique_ptr<AM::Impl> Clone() const override {                   \
        return std::make_unique<Derived>(*this);                         \
    }
}

