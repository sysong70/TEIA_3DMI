#pragma once

#include "Impl/Am.DatalConverter.Impl.h"

#include <gsl/gsl>

namespace AM
{
	class DatalConverterKit
	{
	public:
		DatalConverterKit();
	};

	class DatalConverter : public DatalConverterImpl
	{
	public:
		DatalConverter();

		// Brep data를 입력받아서 Primitve 별로 처리를 해야함.
		[[nodiscard]] bool ExportBrep(gsl::not_null<const A3DTopoBrepData *> brepData) noexcept;
	};
}