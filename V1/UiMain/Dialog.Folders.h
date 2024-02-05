#pragma once

#include "Dialog.Standard.h"



namespace Dialog
{
	class Folders : public Standard
	{
	protected:

		class EditListBox : public CBCGPEditListBox
		{
		public:

			EditListBox();

		protected:

			void OnBrowse() override;

			void OnClickButton(int iButton) override;

			void OnSelectionChanged() override;
		};

	public:

		Folders();

		~Folders() override;

		CString& GetValue();

		void SetValue(CString value);

	protected:

		void OnCancel() override;

		BOOL OnInitDialog() override;

		void OnOK() override;

		DECLARE_MESSAGE_MAP()

	protected:

		void ConstructBody(const CRect& boundary) override;

		void ConstructFooter(const CRect& boundary) override;

	private: // Body controls

		EditListBox m_listBox;
		CString m_value; // setting before OnInitDialog

	private: // Footer controls

		CBCGPButton m_wndOk;
		CBCGPButton m_wndCancel;

	private:
	};
}
