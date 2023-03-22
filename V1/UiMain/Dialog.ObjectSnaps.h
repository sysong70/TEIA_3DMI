#pragma once

#include "Dialog.Standard.H"
#include "Control.CheckListBox.h"



namespace Dialog
{
	class ObjectSnaps : public Standard
	{
		DECLARE_DYNCREATE(ObjectSnaps);

	public:

		ObjectSnaps();

		~ObjectSnaps() override;

	protected:

		void OnCancel() override;

		BOOL OnInitDialog() override;

		void OnOK() override;

		DECLARE_MESSAGE_MAP()

	protected:

		void ConstructBody(const CRect& boundary) override;

		void ConstructFooter(const CRect& boundary) override;

	private: // Controls

		Control::CheckListBox m_list;
		CBCGPButton m_wndOk;
		CBCGPButton m_wndCancel;
	};
}
