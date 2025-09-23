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
	m_pcImpl = std::make_unique<ClassName##Impl>(); \
	DEBUG_VALID(m_pcImpl)

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

	template <class ImplT, class ObjT>
	ImplT & ensureImpl(ObjT & obj) {
		if (auto * p = static_cast<ImplT *>(Impl::getImpl(obj).get()))
			return *p;
		Impl::setImpl(obj, std::make_unique<ImplT>());
		return *static_cast<ImplT *>(Impl::getImpl(obj).get());
	}

	template <class ImplT, class ObjT>
	const ImplT * peekImpl(const ObjT & obj) noexcept {
		return static_cast<const ImplT *>(Impl::getImpl(obj).get());
	}
}

