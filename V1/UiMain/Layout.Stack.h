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

		enum class EFlow
		{
			LeftToRight,
			RightToLeft,
			TopToBottom,
			BottomToTop,
		};

		Stack();

		virtual ~Stack();

		void HorizontalAlignment(EHorizontalAlignment value) override;
		EHorizontalAlignment HorizontalAlignment() override;

		void VerticalAlignment(EVerticalAlignment value) override;
		EVerticalAlignment VerticalAlignment() override;

		int CalculateWidth(EBoxModel box = EBoxModel::Margin) override;

		int CalculateHeight(EBoxModel box = EBoxModel::Margin) override;

		void Update() override;

	public:

		void AddChild(Base* pValue);

		void RemoveChild(Base* pValue);

		void Orientation(EOrientation value);

		void Flow(EFlow value);

	protected:

		EOrientation m_eOrientation = EOrientation::Horizontal;
		EFlow m_eFlow = EFlow::LeftToRight;
		Bases m_children;
	};
}
