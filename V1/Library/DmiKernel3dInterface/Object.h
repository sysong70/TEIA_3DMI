#pragma once

#include "Kernel.h"

namespace KERNEL
{
	class API_KERNEL Object
	{
	public:
		Object();
		virtual ~Object();

		KERNEL::Type Type() const;

		KERNEL::Type ObjectType() const { return KERNEL::Type::None; };
		Impl * GetImpl() { return m_pcImpl; }
		const Impl * GetImpl() const { return m_pcImpl; }

		Impl * GetImpl1() { return m_pcImpl; }
		const Impl * GetImpl1() const { return m_pcImpl; }

	protected:
		friend class Impl;
		Impl * m_pcImpl = nullptr;
		Impl * m_pcImpl1 = nullptr;
	};
}