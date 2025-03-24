#include "stdafx.h"

#include "Ast.h"
#include "Cls.UserIoManager.h"
#include "Wnd.View.h"

#include <Signal2d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

ClsUserIoManager::ClsUserIoManager()
	: CommandBarCtl(*this)
	, DioContainer(*this)
	, TaskIoBarCtl(*this)
{
}



bool ClsUserIoManager::Initialize(CWnd* pParentWnd)
{
	if (UseCommandBar) {
		CommandBarCtl.Initialize(pParentWnd);
	}
	if (UseTaskIoBar) {
		TaskIoBarCtl.Initialize(pParentWnd);
	}
	if (UseDynamicInput) {
		DioContainer.Initialize(pParentWnd);
	}

	return true;
}



void ClsUserIoManager::ReceiveSignal(Json::Object* pData)
{
#define OnAction(x) SgnUserIO::Action::Put##x: Put##x(data); break

	Json::Object& data = *pData;
	SgnUserIO::Action action = (SgnUserIO::Action)data.GetInteger(SKW_ACTION, -1);

	if (action == SgnUserIO::Action::SetDynamicInput) {
		if (UseDynamicInput) {
			DioContainer.ReceiveSignal(pData);
		}
	}
	else if (action == SgnUserIO::Action::StandbyCommand) {
		StandbyCommand(data);
	}
	else {
		switch (action) {
		case OnAction(Command);
		case OnAction(Prompt);
		case OnAction(Error);
		case OnAction(Echo);

		default:
			DEBUG_STOP;
			break;
		}
	}

	REMOVE_POINTER(pData);

	AdjustLayout();

#undef OnAction
}



void ClsUserIoManager::ViewChanged(WndView* pView)
{
	if (UseCommandBar) {
		if (pView != nullptr && pView->ViewType == Wnd::EViewType::View2d) {
			CommandBarCtl.ShowWindow(SW_SHOW);
		}
		else {
			CommandBarCtl.ShowWindow(SW_HIDE);
		}
	}
}



void ClsUserIoManager::AdjustLayout()
{
	if (UseCommandBar) {
		CommandBarCtl.AdjustLayout();
	}
	if (UseDynamicInput) {
		DioContainer.AdjustLayout();
	}
	if (UseTaskIoBar) {
		TaskIoBarCtl.AdjustLayout();
	}
}

//--------------------------------------------------------------------------------------------------

bool ClsUserIoManager::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (UseCommandBar && CommandBarCtl.OnChar(nChar, nRepCnt, nFlags)) {
		return true;
	}
	if (UseDynamicInput && DioContainer.OnChar(nChar, nRepCnt, nFlags)) {
		return true;
	}
	if (UseTaskIoBar && TaskIoBarCtl.OnChar(nChar, nRepCnt, nFlags)) {
		return true;
	}

	return false;
}



bool ClsUserIoManager::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (UseCommandBar && CommandBarCtl.OnKeyDown(nChar, nRepCnt, nFlags)) {
		return true;
	}
	if (UseDynamicInput && DioContainer.OnKeyDown(nChar, nRepCnt, nFlags)) {
		return true;
	}
	if (UseTaskIoBar && TaskIoBarCtl.OnKeyDown(nChar, nRepCnt, nFlags)) {
		return true;
	}

	return false;
}



bool ClsUserIoManager::OnMouseLeave()
{
	if (UseDynamicInput && DioContainer.OnMouseLeave()) {
		return true;
	}

	return false;
}



bool ClsUserIoManager::OnMouseMove(UINT nFlags, CPoint point)
{
	if (UseDynamicInput && DioContainer.OnMouseMove(nFlags, point)) {
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------

bool ClsUserIoManager::PutCommand(Json::Object& data)
{
	Params.Command = data.GetString(SKW_COMMAND);

	if (UseCommandBar && CommandBarCtl.SetCommand()) {
		return true;
	}
	if (UseDynamicInput && DioContainer.SetCommand()) {
		return true;
	}
	if (UseTaskIoBar && TaskIoBarCtl.SetCommand()) {
		return true;
	}

	return false;
}



bool ClsUserIoManager::PutPrompt(Json::Object& data)
{
	ASSERT(Params.Command.IsEmpty() == false);

	Params.Options = data.GetInteger(SKW_OPTIONS);
	Params.Prompt = Ast::Local(data.GetString(SKW_PROMPT));
	Params.Keyword = Ast::Local(data.GetString(SKW_KEYWORD));
	Params.Keywords.clear();

	if (Params.Keyword.IsEmpty() == false) {
		WStringArray keywords;
		WStr::Split(Params.Keyword, L'/', keywords);

		for (auto keyword : keywords) {
			CString code = WStr::Front(keyword, L'(', false, false);
			CString key = WStr::Back(keyword, L'(', L')', false, false);
			Params.Keywords.push_back(code);
			Params.Keywords.push_back(key);
		}
	}

	if (UseCommandBar && CommandBarCtl.SetPrompt()) {
		return true;
	}
	if (UseDynamicInput && DioContainer.SetPrompt()) {
		return true;
	}
	if (UseTaskIoBar && TaskIoBarCtl.SetPrompt()) {
		return true;
	}

	return false;
}



bool ClsUserIoManager::PutEcho(Json::Object& data)
{
	CString value = Ast::Local(data.GetString(SKW_VALUE));

	if (UseCommandBar && CommandBarCtl.SetEcho(value)) {
		return true;
	}
	if (UseDynamicInput && DioContainer.SetEcho(value)) {
		return true;
	}
	if (UseTaskIoBar && TaskIoBarCtl.SetEcho(value)) {
		return true;
	}

	return false;
}



bool ClsUserIoManager::PutError(Json::Object& data)
{
	CString value = Ast::Local(data.GetString(SKW_VALUE));

	if (UseCommandBar && CommandBarCtl.SetError(value)) {
		return true;
	}
	if (UseDynamicInput && DioContainer.SetError(value)) {
		return true;
	}
	if (UseTaskIoBar && TaskIoBarCtl.SetError(value)) {
		return true;
	}

	return false;
}



bool ClsUserIoManager::StandbyCommand(Json::Object& data)
{
	Params.Clear();
	CString prompt = Ast::Local(data.GetString(SKW_PROMPT));

	if (UseCommandBar && CommandBarCtl.StandbyCommand(prompt)) {
		return true;
	}
	if (UseDynamicInput && DioContainer.StandbyCommand(prompt)) {
		return true;
	}
	if (UseTaskIoBar && TaskIoBarCtl.StandbyCommand(prompt)) {
		return true;
	}

	return false;
}
