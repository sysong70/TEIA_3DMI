#pragma once

#include "3DF.h"

#include <memory>

#pragma warning(push)
#pragma warning(disable:4251)

namespace H3DF
{
	class API_3DF Object
	{
	public:

		Object();
		Object(Object const & cInThat);
		virtual ~Object();

		static const H3DF::Type staticType = H3DF::Type::None;
		H3DF::Type ObjectType() const { return staticType; }
		virtual H3DF::Type Type() const;

		Object const & operator = (Object const & cInThat);

		Object(Object && cInThat) noexcept;
		Object & operator=(Object && cInThat) noexcept;

		bool HasType(H3DF::Type eInMask) const;

		Impl * GetImpl() { return m_pcImpl.get(); }
		const Impl * GetImpl() const { return m_pcImpl.get(); }

	protected:
		friend class Impl;
		friend class KeyImpl;

		std::unique_ptr<Impl> m_pcImpl;
	};
}

#pragma warning(pop)