#pragma once

#include "../AM.h"

#include "Am.Impl.h"

namespace AM  
{
	class TemplateKitImpl : public Impl
	{
	public:
		TemplateKitImpl();

		std::unique_ptr<Impl> Clone() const override;

		std::string_view m_description;
		std::string_view m_property = "EQUI";
		double m_userWeight = 0;
	};

	class TemplateImpl : public Impl
	{
	public:
		TemplateImpl();

		std::unique_ptr<Impl> Clone() const override;

		std::string_view m_description;
		std::string_view m_property = "EQUI";
		double m_userWeight = 0;
	};
}