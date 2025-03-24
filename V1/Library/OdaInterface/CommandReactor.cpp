#include "stdafx.h"

#include "CommandReactor.h"

#include "DbCommandContext.h"

//**************************************************************************************************

CommandReactor::CommandReactor(OdDbCommandContext* pCmdCtx)
	: m_pCommandContext(pCmdCtx)
	, m_bModified(false)
{
	ODA_ASSERT(m_pCommandContext);
	::odedRegCmds()->addReactor(this);
	m_pCommandContext->database()->addReactor(this);
}



CommandReactor::~CommandReactor()
{
	::odedRegCmds()->removeReactor(this);
	if (!m_bModified) {
		m_pCommandContext->database()->removeReactor(this);
	}
}



void CommandReactor::setLastInput(const OdString& sLastInput)
{
	m_sLastInput = sLastInput;
}



const OdString& CommandReactor::lastInput() const
{
	return m_sLastInput;
}



bool CommandReactor::isDatabaseModified() const
{
	return m_bModified;
}



void CommandReactor::setModified()
{
	m_bModified = true;
	m_pCommandContext->database()->removeReactor(this);
}



void CommandReactor::objectOpenedForModify(const OdDbDatabase*, const OdDbObject*)
{
	setModified();
}



void CommandReactor::headerSysVarWillChange(const OdDbDatabase*, const OdString&)
{
	setModified();
}



void CommandReactor::commandWillStart(OdEdCommand* pCmd, OdEdCommandContext*)
{
	m_sLastInput.makeUpper();

	if (GetBit(pCmd->flags(), OdEdCommand::kNoHistory) == false) {
		//theApp.setRecentCmd(m_sLastInput);
	}
	if (GetBit(pCmd->flags(), OdEdCommand::kNoUndoMarker) == false) {
		m_pCommandContext->database()->startUndoRecord();
	}
}



void CommandReactor::commandCancelled(OdEdCommand*, OdEdCommandContext*)
{
	undoCommand();
}



void CommandReactor::commandFailed(OdEdCommand*, OdEdCommandContext*)
{
	undoCommand();
}



OdEdCommandPtr CommandReactor::unknownCommand(const OdString& sCmdName, OdEdCommandContext*)
{
	//CDwgViewer* pViewer = OdDbDatabaseDocPtr(m_pCommandContext->database())->document()->getViewer();
	//if (pViewer) {
	//	OdEdCommandPtr pRes = pViewer->command(sCmdName);
	//	if (pRes.get()) {
	//		return pRes;
	//	}
	//}

	OdString sMsg;
	sMsg.format(L"Unknown command \"%ls\".", sCmdName.c_str());
	m_pCommandContext->userIO()->putString(sMsg);

	return OdEdCommandPtr();
}



void CommandReactor::undoCommand()
{
	OdDbDatabase* pDb = m_pCommandContext->database();

	//try {
	//	pDb->disableUndoRecording(true);
	//	pDb->undo();
	//	pDb->disableUndoRecording(false);
	//}
	//catch (const OdError& err) {
	//	theApp.reportError(_T("Can't repair database"), err);
	//}
}
