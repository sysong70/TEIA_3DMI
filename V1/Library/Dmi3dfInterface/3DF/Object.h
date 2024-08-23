#pragma once

#include "3DF.h"

#include <memory>

namespace H3DF
{
	class API_3DF Object
	{
	public:
		Object();
		Object(Object const & cInThat);
		virtual ~Object();

		virtual H3DF::Type Type() const;
		virtual H3DF::Type ObjectType() const { return H3DF::Type::None; }

		void Set(Object const & cInThat);
		Object const & operator = (Object const & cInThat);

		Object(Object && cInThat) noexcept ;
		Object & operator=(Object && cInThat) noexcept;

		bool HasType(H3DF::Type eInMask) const;

		Impl * GetImpl() { return m_pcImpl; }
		const Impl * GetImpl() const { return m_pcImpl; }

	protected:
		friend class Impl;
		friend class KeyImpl;

		Impl * m_pcImpl = nullptr;
	};
}
