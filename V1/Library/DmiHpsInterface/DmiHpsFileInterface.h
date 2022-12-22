#pragma once

#include "../../Common/Common_Define.h"
#include "../../Signal/WorkerThread.h"

#include <mutex>

class DmiModelHandler;
class DmiHpsView;
class ImportStatusEventHandler;
class DmiHpsInterface;

class WorkerThread;

using WorkerThreadTimerProcess = void(*)();

class DmiHpsFileInterface
{
public:
	DmiHpsFileInterface(DmiHpsInterface * pcHpsInterface);
	~DmiHpsFileInterface();

	bool FileImport(CString strFilePathName, DmiModelHandler * pcModelHandler);
	bool EndFileImport(DmiModelHandler * pcModelHandler, DmiHpsView * pcHpsView);

	bool ImportExchangeFile(CString strFilePathName, DmiModelHandler * pcModelHandler, std::chrono::system_clock::time_point * pcTimes);
	bool CompleteImportExchangeFile(DmiModelHandler * pcModelHandler);
	bool RequestImportExchangeNotifierStatus();
	bool ImportExchangeLog();
	
	void SendLogMessage();
	bool InProgressImportStatus();
	bool IsSuccessImportStatus();
	bool GetImportLogMessage(CString & strJsonReturnMessage);
	bool GetImportStatusMessage(CString & strReturnMessage);

	void AddLogEntry(CString chLogEntry);

	CString GetMessage(bool bRemoveMessageFlag = true);
	void SetImportMessage(CString strMessage);

protected:
	void getConfiguration(HPS::Exchange::ConfigurationArray const & configs, HPS::UTF8Array & selectedConfig);

	HPS::Exchange::ImportNotifier & GetImportNotifier() { return m_cImportNotifier; }

	CAtlString GetErrorString(HPS::IOResult status, LPCTSTR lpszPathName);

	// #Require_convert_new_version
	//DmiHps::IoResult ConvertResult(HPS::IOResult eInResult);

private:
	CString m_strMessage;
	std::string m_strExceptionMessage;
	CString m_strFilePathName;

	std::deque<CString> m_destrLogMessageDeque;

	ImportStatusEventHandler * m_pcImportStatusEventHandler;

	HPS::Exchange::ImportNotifier m_cImportNotifier;
	HPS::IOResult m_cImportStatus;

	bool m_bStartImportExchangeFlag = false;

	DmiHpsInterface * m_pcHpsInterface = nullptr;

	WorkerThread * m_pcWorkerThread = nullptr;
};

class ImportStatusEventHandler : public HPS::EventHandler
{
public:
	ImportStatusEventHandler(DmiHpsFileInterface * pcHpsFileInterface, std::chrono::system_clock::time_point * pcTimes);
	virtual ~ImportStatusEventHandler();

	HandleResult Handle(HPS::Event const * pcEvent) override;

	DmiHpsFileInterface * m_pcHpsFileInterface;

	std::chrono::system_clock::time_point * m_pcTimes = nullptr;
};
