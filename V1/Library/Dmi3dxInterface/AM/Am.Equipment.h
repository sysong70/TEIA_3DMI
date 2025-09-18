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

		void setTitile(std::string_view title);

		void setBuiltIn(bool builtIn);


		//== Template ==============================================================================
		Template InsertTemplate(TemplateKit const & kit);

	};
}