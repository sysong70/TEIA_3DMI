#pragma once

#include "Am.h"

#include "Am.Object.h"
#include "Am.Kit.h"

#include "Am.Box.h"
#include "Am.Cylinder.h"

namespace AM
{
	class TemplateKit : public Kit
	{
	public:
		TemplateKit();

		// 복사 허용
		TemplateKit(const TemplateKit &) = default;
		TemplateKit & operator=(const TemplateKit &) = default;

		// 이동 허용
		TemplateKit(TemplateKit &&) noexcept = default;
		TemplateKit & operator=(TemplateKit &&) noexcept = default;

		TemplateKit & setDescription(std::string_view description);

		TemplateKit & setProperty(std::string_view property);
	};

	class Template : public Object
	{
	public:
		Template();
		virtual ~Template();

		static const AM::Type staticType = AM::Type::Template;
		AM::Type ObjectType() const { return staticType; }

		// 복사 허용
		Template(const Template &) = default;
		Template & operator=(const Template &) = default;

		// 이동 허용
		Template(Template &&) noexcept = default;
		Template & operator=(Template &&) noexcept = default;

		Result writeDatal(std::ostream * os) noexcept override;

		Template & set(TemplateKit const & kit);

		Template & setDescription(std::string_view description);

		Template & setProperty(std::string_view property);

		Box insertBox(BoxKit const & kit);

		Cylinder insertCylinder(CylinderKit const & kit);
	};
}