#pragma once

#include "StaticRxObject.h"
#include "DbDatabaseReactor.h"
#include "Ed/EdCommandStack.h"

class OdDbCommandContext;

//--------------------------------------------------------------------------------------------------

class CommandReactor
	: public OdStaticRxObject<OdDbDatabaseReactor>
	, public OdStaticRxObject<OdEdCommandStackReactor>
{
	ODRX_NO_HEAP_OPERATORS();

	OdDbCommandContext* m_pCommandContext;
	bool m_bModified;
	OdString m_sLastInput;

public:

	CommandReactor(OdDbCommandContext* pCmdCtx);

	~CommandReactor();

public:

	void setLastInput(const OdString& sLastInput);

	const OdString& lastInput() const;

	bool isDatabaseModified() const;

	void setModified();

public: // OdDbDatabaseReactor

	void objectOpenedForModify(const OdDbDatabase*, const OdDbObject*) override;

	void headerSysVarWillChange(const OdDbDatabase*, const OdString&) override;

public: // OdEdCommandStackReactor

	void commandWillStart(OdEdCommand* pCmd, OdEdCommandContext* /*pCmdCtx*/) override;

	void commandCancelled(OdEdCommand*, OdEdCommandContext*) override;

	void commandFailed(OdEdCommand*, OdEdCommandContext*) override;

	OdEdCommandPtr unknownCommand(const OdString& sCmdName, OdEdCommandContext*) override;

private:

	void undoCommand();
};