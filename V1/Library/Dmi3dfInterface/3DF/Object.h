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

		static const H3DF::Type staticType = H3DF::Type::None;
		virtual H3DF::Type ObjectType() const { return staticType; }

		bool HasType(H3DF::Type eInMask) const;

		Impl * GetImpl() { return m_pcImpl; }
		const Impl * GetImpl() const { return m_pcImpl; }

	protected:
		friend class Impl;
		friend class KeyImpl;

		Impl * m_pcImpl = nullptr;
	};
}
