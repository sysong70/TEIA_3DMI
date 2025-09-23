#pragma once

#include "../AM.h"

#include "Am.Impl.h"

namespace AM  
{
	class TemplateKitImpl : public Impl
	{
	public:
		TemplateKitImpl();

		// 복사 금지
		TemplateKitImpl(const TemplateKitImpl &) = default;
		TemplateKitImpl & operator=(const TemplateKitImpl &) = default;

		// 이동 허용
		TemplateKitImpl(TemplateKitImpl &&) noexcept = default;
		TemplateKitImpl & operator=(TemplateKitImpl &&) noexcept = default;

		std::unique_ptr<Impl> Clone() const override;

		std::string_view m_description;
		std::string_view m_property = "EQUI";
		double m_userWeight = 0;

		std::vector<std::unique_ptr<AM::Geometry>> m_geometries;
	};

	class TemplateImpl : public Impl
	{
	public:
		TemplateImpl();

		std::unique_ptr<Impl> Clone() const override;

		TemplateKitImpl m_kit;
	};
}