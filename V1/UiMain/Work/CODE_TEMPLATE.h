#pragma once

#include "parent_.h"

//--------------------------------------------------------------------------------------------------

namespace namespace_
{
	class class_ : public CWnd
	{
	public:

		class_();

		~class_() override;

	protected:

		void PostNcDestroy() override;

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		DECLARE_MESSAGE_MAP()
	};
}
