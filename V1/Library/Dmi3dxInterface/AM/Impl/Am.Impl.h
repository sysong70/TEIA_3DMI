#pragma once

#include "../AM.h"

#include "../Am.Object.h"

#include "../../3DX.h"

#include <map>
#include <memory>
#include <filesystem>

// 내부 유니크 이름 도우미
#define AM_CONCAT_INNER(a, b) a##b
#define AM_CONCAT(a, b)       AM_CONCAT_INNER(a, b)
#define AM_UNIQUE(base)       AM_CONCAT(base, __LINE__)

#define ENSURE_IMPL(ClassName)                                           \
    ([&]() -> ClassName##Impl* {                                              \
        auto* p = static_cast<ClassName##Impl*>(AM::Impl::getImpl(*this).get()); \
        if (!p) {                                                             \
            AM::Impl::setImpl(*this, std::make_unique<ClassName##Impl>());    \
            p = static_cast<ClassName##Impl*>(AM::Impl::getImpl(*this).get()); \
            if (!p) { DEBUG_STOP; return nullptr; }                           \
        }                                                                     \
        return p;                                                             \
    }())

#define GET_IMPL(Obj, ClassName)                                     \
    ([&]() -> ClassName##Impl* {                                          \
        auto* p = static_cast<ClassName##Impl*>(Impl::getImpl(Obj).get());\
        if (!p) { DEBUG_STOP; return nullptr; }                           \
        return p;                                                         \
    }())

#define IMPL_AS(ClassName, varName) \
    varName = static_cast<ClassName##Impl *>(m_pcImpl.get()); \
    DEBUG_VALID(varName)

#define INIT_IMPL(ClassName) \
	AM::Impl::setImpl(*this, std::make_unique<ClassName##Impl>());

#define CLONE_IMPL(ClassName) \
	auto impl = std::make_unique<ClassName>(); \
	DEBUG_VALID(impl.get()); \
	return impl;

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

	private:
		AM::Type m_type = AM::Type::None;
	};
}

