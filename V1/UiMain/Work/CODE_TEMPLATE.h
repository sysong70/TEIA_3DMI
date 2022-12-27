#pragma once

#include "PARENT.H"



namespace NAMESPACE
{
	class CLASS : public CWnd
	{
	public:

		CLASS();

		~CLASS() override;

	protected:

		void PostNcDestroy() override;

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		DECLARE_MESSAGE_MAP()
	};
}
