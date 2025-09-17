#pragma once

#include "../3DF.h"

#include "ControlImpl.h"

namespace H3DF
{
	class TextAttributeControlImpl : public ControlImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<TextAttributeControlImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const TextAttributeControlImpl * pcInThat) {
		}
	};
}