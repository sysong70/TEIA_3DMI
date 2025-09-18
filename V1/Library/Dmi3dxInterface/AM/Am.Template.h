#pragma once

#include "Am.h"

#include "Am.Object.h"
#include "Am.Kit.h"

namespace AM
{
	class TemplateKit : public Kit
	{
	public:
		TemplateKit();

		void setDescription(std::string_view description) noexcept;

		void setProperty(std::string_view property) noexcept;
	};

	class Template : public Object
	{
	public:
		Template();
		virtual ~Template();

		static const AM::Type staticType = AM::Type::Template;
		AM::Type ObjectType() const { return staticType; }

		void setDescription(std::string_view description) noexcept;

		void setProperty(std::string_view property) noexcept;
	};
}