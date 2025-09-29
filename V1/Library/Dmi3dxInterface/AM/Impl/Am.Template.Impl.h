#pragma once

#include "../AM.h"

#include "Am.Impl.h"
#include "Am.Object.Impl.h"

namespace AM  
{
	class TemplateKitImpl : public Impl
	{
	public:
		TemplateKitImpl();

		TemplateKitImpl(const TemplateKitImpl &) = default;
		TemplateKitImpl & operator=(const TemplateKitImpl &) = default;

		TemplateKitImpl(TemplateKitImpl &&) noexcept = default;
		TemplateKitImpl & operator=(TemplateKitImpl &&) noexcept = default;

		std::unique_ptr<Impl> Clone() const override;

		std::string_view m_description;
		std::string_view m_property = "EQUI";
		double m_userWeight = 0;

		std::vector<std::unique_ptr<AM::Geometry>> m_geometries;
	};

	class TemplateImpl : public ObjectImpl
	{
	public:
		TemplateImpl();

		std::unique_ptr<Impl> Clone() const override;

		Result writeDatal(std::ostream * os) noexcept override;

		TemplateKitImpl m_kit;
	};
}