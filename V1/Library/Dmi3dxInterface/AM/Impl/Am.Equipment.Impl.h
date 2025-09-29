#pragma once

#include "../AM.h"

#include "Am.Impl.h"
#include "Am.Object.Impl.h"

#include "../Am.Template.h"

namespace AM  
{
	class EquipmentImpl : public ObjectImpl
	{
	public:
		EquipmentImpl();

		EquipmentImpl(const EquipmentImpl &) = default;
		EquipmentImpl & operator=(const EquipmentImpl &) = default;

		EquipmentImpl(EquipmentImpl &&) noexcept = default;
		EquipmentImpl & operator=(EquipmentImpl &&) noexcept = default;

		std::unique_ptr<Impl> Clone() const override;

		Result writeDatal(std::ostream * os) noexcept override;

		std::string_view m_title;
		bool m_builtIn = false;  // BUIL

		std::vector<std::unique_ptr<AM::Template>> m_templates;

		// DSCO unset
		// PTSP unset
		// INSC unset
	};
}