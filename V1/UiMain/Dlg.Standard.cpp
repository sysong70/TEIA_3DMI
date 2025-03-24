#include "stdafx.h"

#include "Ast.AppResources.h"
#include "Dlg.Standard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

IMPLEMENT_DYNAMIC(DlgStandard, DlgBase)

BEGIN_MESSAGE_MAP(DlgStandard, DlgBase)
END_MESSAGE_MAP()



DlgStandard::DlgStandard(CStringA dialogName, CWnd* pParent /*=nullptr*/)
	: DlgBase(IDD_DMI_STANDARD, pParent)
	, DesignData(nullptr)
{
	if (dialogName.IsEmpty() == false) {
		DesignData = &(TheAppResources.GetDialog(dialogName));
	}
}

DlgStandard::DlgStandard(UINT nIDTemplate, CStringA dialogName, CWnd* pParent)
	: DlgBase(nIDTemplate, pParent)
	, DesignData(nullptr)
{
	if (dialogName.IsEmpty() == false) {
		DesignData = &(TheAppResources.GetDialog(dialogName));
	}
}



BOOL DlgStandard::OnInitDialog()
{
	__super::OnInitDialog();

	SetWindowText(GetWinTitle());

	return TRUE;
}



Json::Object& DlgStandard::GetDefaultButtons()
{
	return TheAppResources.GetDialog("DefaultButtons");
}



Json::Object& DlgStandard::GetUiData()
{
	DEBUG_VALID(DesignData);
	return *DesignData;
}



CSize DlgStandard::GetWinSize()
{
	Json::Value* pValue = GetUiData().FindValue("size");
	if (pValue != nullptr) {
		Json::Object& data = pValue->AsObject();
		return globalUtils.ScaleByDPI(Ast::GetSize(data));
	}
	else {
		DEBUG_STOP;
		return {};
	}
}



CString DlgStandard::GetWinTitle()
{
	Json::Value* pValue = GetUiData().FindValue("title");
	if (pValue != nullptr) {
		return Ast::Local(pValue->ToString());
	}
	else {
		DEBUG_STOP;
		return L"";
	}
}
