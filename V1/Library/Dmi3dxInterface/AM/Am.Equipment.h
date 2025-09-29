#pragma once

#include "Am.h"

#include "Am.Object.h"

namespace AM
{
	class Equipment : public Object
	{
	public:
		Equipment();
		virtual ~Equipment();

		static const AM::Type staticType = AM::Type::Equipment;
		AM::Type ObjectType() const { return staticType; }

		Equipment & setTitile(std::string_view title);

		Equipment & setBuiltIn(bool builtIn);

		Result writeDatal(std::ostream * os) noexcept override;

		//== Template ==============================================================================
		Template & insertTemplate(TemplateKit const & kit);
	};
}