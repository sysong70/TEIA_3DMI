#pragma once

#include "Layout.Base.h"
#include <vector>



namespace Layout
{
	class Stack : public Base
	{
	public:

		enum class EOrientation
		{
			Horizontal,
			Vertical,
		};

		Stack();

		virtual ~Stack();

		int CalculateWidth() override;

		int CalculateHeight() override;

		void Update(CRect parent) override;

	public:

		void Orientation(EOrientation value);

	protected:

		EOrientation m_eOrientation = EOrientation::Horizontal;
		Bases m_children;
	};
}
