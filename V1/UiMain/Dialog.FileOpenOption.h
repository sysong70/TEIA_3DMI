#pragma once

#include "Dialog.PropertyList.h"
#include "Data.FileOption.h"
#include "Window.Document.h"

OPEN_DIALOG_NAMESPACE

class FileOpenOption : public PropertyList
{
public:

	//:WARNING
	friend class CHelper;

	FileOpenOption(CStringA dialogName, Window::DocType eType = Window::DocType::Unknown, CWnd* pParent = nullptr);

	~FileOpenOption() override;

public:

	Data::Base* GetResultData(bool bNewInstance = false) override;

	Json::Object* GetResult() override;

protected:

	CRect ConstructBody(CRect bound) override;

	CRect ConstructFooter(CRect bound) override;

protected:

	BOOL OnInitDialog() override;

	void OnCancel() override;

	void OnOK() override;

	afx_msg LRESULT	OnCommandClicked(WPARAM wp, LPARAM lp);

	afx_msg LRESULT	OnMenuItemSelected(WPARAM wp, LPARAM lp);

	afx_msg LRESULT	OnPropertyChanged(WPARAM wp, LPARAM lp);

	void OnResetDefaults();

	DECLARE_MESSAGE_MAP()

private:

	enum class Extra
	{
		ResetDefaults = 11000,
	};

	CBCGPButton m_wndResetDefaults;

	CBCGPButton m_wndSetToDefaults;

	Window::DocType m_eDocType;

	Data::ImportOption m_data;
};

CLOSE_DIALOG_NAMESPACE
