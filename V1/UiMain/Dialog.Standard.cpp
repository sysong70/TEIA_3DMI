#include "stdafx.h"
#include "Dialog.Standard.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

using namespace Dialog;

IMPLEMENT_DYNAMIC(Standard, Base)

BEGIN_MESSAGE_MAP(Standard, Base)
END_MESSAGE_MAP()



Dialog::Standard::Standard(CStringA dialogName, CWnd* pParent /*=nullptr*/)
	: Base(IDD_DMI_STANDARD, pParent)
	, m_pDialogData(nullptr)
{
	if (dialogName.IsEmpty() == false) {
		m_pDialogData = &(TheAppResources.GetDialog(dialogName));
	}
}

Dialog::Standard::Standard(UINT nIDTemplate, CStringA dialogName, CWnd* pParent)
	: Base(nIDTemplate, pParent)
	, m_pDialogData(nullptr)
{
	if (dialogName.IsEmpty() == false) {
		m_pDialogData = &(TheAppResources.GetDialog(dialogName));
	}
}



Dialog::Standard::~Standard()
{
}



BOOL Dialog::Standard::OnInitDialog()
{
	__super::OnInitDialog();

	SetWindowText(GetWinTitle());

	return TRUE;
}



Json::Object& Dialog::Standard::GetDefaultButtons()
{
	return TheAppResources.GetDialog("DefaultButtons");
}



Json::Object& Dialog::Standard::GetUiData()
{
	DEBUG_VALID(m_pDialogData);
	return *m_pDialogData;
}



CSize Dialog::Standard::GetWinSize()
{
	Json::Value* pValue = GetUiData().FindValue("size");
	if (pValue != nullptr) {
		Json::Object& data = pValue->AsObject();
		return globalUtils.ScaleByDPI(Facility::GetSize(data));
	}
	else {
		DEBUG_STOP;
		return {};
	}
}



CString Dialog::Standard::GetWinTitle()
{
	Json::Value* pValue = GetUiData().FindValue("title");
	if (pValue != nullptr) {
		return Facility::Local(pValue->ToString());
	}
	else {
		DEBUG_STOP;
		return L"";
	}
}
