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
		PrivateImpl * GetImpl() { return m_pcImpl; }
		const PrivateImpl * GetImpl() const { return m_pcImpl; }

	protected:
		friend class PrivateImpl;
		PrivateImpl * m_pcImpl = nullptr;
	};
}