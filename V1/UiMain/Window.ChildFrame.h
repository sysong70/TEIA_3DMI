#pragma once

#include "Window.h"

//--------------------------------------------------------------------------------------------------

namespace Window
{
	class ChildFrame : public CBCGPMDIChildWnd
	{
		DECLARE_DYNCREATE(ChildFrame)

	public:

		ChildFrame();

		~ChildFrame() override;

	protected:

		void ActivateFrame(int nCmdShow) override;

		BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	#ifdef _DEBUG
		void AssertValid() const override;

		void Dump(CDumpContext& dc) const override;
	#endif

	protected:

		DECLARE_MESSAGE_MAP()
	};
}