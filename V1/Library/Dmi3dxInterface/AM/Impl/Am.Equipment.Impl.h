#pragma once

#include "../AM.h"

#include "Am.Impl.h"

#include "../Am.Template.h"

namespace AM  
{
	class EquipmentImpl : public Impl
	{
	public:
		EquipmentImpl();

		std::unique_ptr<Impl> Clone() const override;

		std::string_view m_title;
		bool m_builtIn = false;  // BUIL

		std::vector<Template> m_templates;

		// DSCO unset
		// PTSP unset
		// INSC unset
	};
}