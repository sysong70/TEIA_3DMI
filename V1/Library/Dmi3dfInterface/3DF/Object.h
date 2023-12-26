#pragma once

#include "3DF.h"

namespace H3DF
{
	class API_3DF Object
	{
	public:
		Object();
		virtual ~Object();

		virtual H3DF::Type Type() const;

		H3DF::Type ObjectType() const { return H3DF::Type::None; };
		Impl * GetImpl() { return m_pcImpl; }
		const Impl * GetImpl() const { return m_pcImpl; }

	protected:
		friend class Impl;
		friend class KeyImpl;

		Impl * m_pcImpl = nullptr;
	};
}
