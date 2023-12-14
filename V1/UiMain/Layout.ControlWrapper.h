#pragma once

#include "Layout.Base.h"
#include "Control.h"



namespace Layout
{
	class ControlWrapper : public Base
	{
	public:

		enum class EContent
		{
			Unknown = 0,
			Button,
			Label,
			CheckBox,
			Radio,
			IconComboBox,
			Tree,
		};

		ControlWrapper(CWnd* pValue, EContent type);

		virtual ~ControlWrapper();

		void HorizontalAlignment(EHorizontalAlignment value) override;
		EHorizontalAlignment HorizontalAlignment() override;

		void HorizontalContentAlignment(EHorizontalAlignment value) override;
		EHorizontalAlignment HorizontalContentAlignment() override;

		void VerticalAlignment(EVerticalAlignment value) override;
		EVerticalAlignment VerticalAlignment() override;

		void VerticalContentAlignment(EVerticalAlignment value) override;
		EVerticalAlignment VerticalContentAlignment() override;

		void Alignment(EHorizontalAlignment h, EVerticalAlignment v) override;

		void ContentAlignment(EHorizontalAlignment h, EVerticalAlignment v) override;

		int CalculateWidth(EBoxModel box = EBoxModel::Margin) override;

		int CalculateHeight(EBoxModel box = EBoxModel::Margin) override;

		void Update();

	protected:

		void Content(CWnd* pValue, EContent type);

	protected:

		CWnd* m_pContent = nullptr;
		EContent m_eType = EContent::Unknown;
	};
}
