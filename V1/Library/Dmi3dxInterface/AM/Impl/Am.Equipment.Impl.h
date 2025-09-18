#pragma once

#include "../AM.h"

#include "Am.Impl.h"

namespace AM  
{
	class EquipmentImpl : public Impl
	{
	public:
		EquipmentImpl();

		std::unique_ptr<Impl> Clone() const override;

		std::string_view m_title;
		bool m_builtIn = false;  // BUIL

		// DSCO unset
		// PTSP unset
		// INSC unset
	};
}