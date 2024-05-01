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

		Impl * GetImpl() { return m_pcImpl; }
		const Impl * GetImpl() const { return m_pcImpl; }

	protected:
		friend class Impl;
		Impl * m_pcImpl = nullptr;
	};
}