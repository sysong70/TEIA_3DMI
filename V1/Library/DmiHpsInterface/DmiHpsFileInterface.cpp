#include "stdafx.h"
#include "DmiHpsFileInterface.h"

#include "DmiHpsInterface.h"

#include "DmiModelHandler.h"
#include "DmiHpsView.h"

#include "Json.h"

std::mutex g_cLogMessageMutex;

//== RtHpsFileInterface Class ======================================================================

DmiHpsFileInterface::DmiHpsFileInterface(DmiHpsInterface * pcHpsInterface)
{
	m_pcHpsInterface = pcHpsInterface;
}

DmiHpsFileInterface::~DmiHpsFileInterface()
{
	if(nullptr != m_pcImportStatusEventHandler) {
		delete m_pcImportStatusEventHandler;
	}
}

bool DmiHpsFileInterface::FileImport(CString strFilePathName, DmiModelHandler * pcModelHandler)
{
	CHECK_POINTER(pcModelHandler);

	if(nullptr == m_pcImportStatusEventHandler) {
		m_pcImportStatusEventHandler = new ImportStatusEventHandler(this, nullptr);
		CHECK_POINTER(m_pcImportStatusEventHandler);
	}

	try
	{
		HPS::UTF8 chFileName(strFilePathName);
		m_cImportNotifier = HPS::Exchange::File::Import(chFileName, HPS::Exchange::ImportOptionsKit());

		HPS::Database::GetEventDispatcher().Subscribe(*m_pcImportStatusEventHandler, HPS::Object::ClassID<HPS::ImportStatusEvent>());

//		m_cImportNotifier.Wait();
// 		pcModelHandler->GetCADModel().Delete();
// 		pcModelHandler->GetCADModel() = m_cImportNotifier.GetCADModel();
	}
	catch(const HPS::IOException & /* ioe */)
	{
		// handle error
	}

	return true;
}

bool DmiHpsFileInterface::EndFileImport(DmiModelHandler * pcModelHandler, DmiHpsView * pcHpsView)
{
	CHECK_POINTER(pcModelHandler);

// 	pcModelHandler->GetCADModel().Delete();
// 	pcModelHandler->GetCADModel() = m_cImportNotifier.GetCADModel();

	if(false == pcHpsView->AttachModelHandler(pcModelHandler)) {
		return false;
	}

	HPS::UpdateNotifier cUpdateNotifier = pcHpsView->GetCanvas().UpdateWithNotifier(HPS::Window::UpdateType::Exhaustive);
	cUpdateNotifier.Wait();

	return true;
}

//== HPS Import 관련 함수 ============================================================================

// 1. 주어진 File을 Import시작 하는 함수 
bool DmiHpsFileInterface::ImportExchangeFile(CString strFilePathName, DmiModelHandler * pcModelHandler, std::chrono::system_clock::time_point * pcTimes)
{
	m_strFilePathName = strFilePathName;
	bool bSuccess = false;
	m_cImportStatus = HPS::IOResult::Failure;
	std::string strMessage;

	try
	{
		m_pcImportStatusEventHandler = new ImportStatusEventHandler(this, pcTimes);
		assert(m_pcImportStatusEventHandler);

		HPS::Database::GetEventDispatcher().Subscribe(*m_pcImportStatusEventHandler, HPS::Object::ClassID<HPS::ImportStatusEvent>());

		HPS::Exchange::ImportOptionsKit cOptionsKit;
		// Import Option 설정
		// cOptionsKit.SetBRepMode(HPS::Exchange::BRepMode::BRepAndTessellation);

		// File Format 확인
		HPS::UTF8 strFileName(strFilePathName);
		HPS::Exchange::File::Format cFormat = HPS::Exchange::File::GetFormat(strFileName);
		//! [file_format]

		HPS::UTF8Array selectedConfig;
		HPS::Exchange::ConfigurationArray configs;
		if(cFormat == HPS::Exchange::File::Format::CATIAV4 && !cOptionsKit.ShowConfiguration(selectedConfig)
			&& !(configs = HPS::Exchange::File::GetConfigurations(strFileName)).empty())
		{
			// If this is a CATIA V4 file w/ configurations a configuration must be specified to perform the import.
			// Since no configuration was specified, we'll pick the first one and import it, otherwise, the import will throw an exception.
			getConfiguration(configs, selectedConfig);
			cOptionsKit.SetConfiguration(selectedConfig);
		}


		//CHPSExchangeProgressDialog dlg(this, cNotifier, strFileName);

		// Exchange를 이용한 File Import
		m_cImportNotifier = HPS::Exchange::File::Import(strFileName, cOptionsKit);
		m_bStartImportExchangeFlag = true;

 		m_pcWorkerThread = new WorkerThread();
 		m_pcWorkerThread->StartTimer(100);

// 		worker.SetTimerProcess(std::bind(&className::method, this));
// 		worker.SetTimerProcess(&funcName);

		m_pcWorkerThread->SetTimerProcess(std::bind(&DmiHpsFileInterface::SendLogMessage, this));// &this->SendLogMessage());
		//m_pcWorkerThread->SetTimerProcess(this->SendLogMessage());

		//m_cImportNotifier.Wait();
		//dlg.DoModal();
		//bSuccess = dlg.WasImportSuccessful();

		m_cImportStatus = m_cImportNotifier.Status();
	}
	catch(HPS::IOException const & cException)
	{
		m_cImportStatus = cException.result;
		m_strExceptionMessage = cException.what();
	}

	return true;
}

// 2. Impoert 상태 요청에 대응하는 함수
bool DmiHpsFileInterface::RequestImportExchangeNotifierStatus()
{
	if(false == m_bStartImportExchangeFlag) {
		return false;
	}

	m_cImportStatus = m_cImportNotifier.Status();

	// #Require_convert_new_version
/*
	Json::Object cObject;
	cObject.SetDwordPtr("CommandId", (DWORD_PTR) DmiHps::Command::ReturnImportExchangeNotifierStatus);
	cObject.SetDwordPtr((DWORD_PTR) DmiHps::Command::Result, (DWORD_PTR) ConvertResult(m_cImportStatus));

	m_pcHpsInterface->SendCommandCallback(cObject);
*/

	return true;
}

// 2-1. HPS Result를 DMI Result로 변경하는 함수

// #Require_convert_new_version
/*
DmiHps::IoResult DmiHpsFileInterface::ConvertResult(HPS::IOResult eInResult)
{
	switch(eInResult)
	{
		case HPS::IOResult::Success:
			return DmiHps::IoResult::Success;
			break;

		case HPS::IOResult::Failure:
			return DmiHps::IoResult::Failure;
			break;

		case HPS::IOResult::FileNotFound:
			return DmiHps::IoResult::FileNotFound;
			break;

		case HPS::IOResult::UnableToOpenFile:
			return DmiHps::IoResult::UnableToOpenFile;
			break;

		case HPS::IOResult::InvalidOptions:
			return DmiHps::IoResult::InvalidOptions;
			break;

		case HPS::IOResult::InvalidSegment:
			return DmiHps::IoResult::InvalidSegment;
			break;

		case HPS::IOResult::InProgress:
			return DmiHps::IoResult::InProgress;
			break;

		case HPS::IOResult::Canceled:
			return DmiHps::IoResult::Canceled;
			break;

		case HPS::IOResult::UnableToLoadLibraries:
			return DmiHps::IoResult::UnableToLoadLibraries;
			break;

		case HPS::IOResult::VersionIncompatibility:
			return DmiHps::IoResult::VersionIncompatibility;
			break;

		case HPS::IOResult::InitializationFailed:
			return DmiHps::IoResult::InitializationFailed;
			break;

		case HPS::IOResult::UnsupportedFormat:
			return DmiHps::IoResult::UnsupportedFormat;
			break;

		case HPS::IOResult::InvalidFile:
			return DmiHps::IoResult::InvalidFile;
			break;

		default:
			assert(false);
			break;
	}

	return DmiHps::IoResult::Failure;
}
*/

// 3. File을 Import를 끝내고 다음 처리를 하는 함수. 성공 및 실패한 경우등을 처리함.
bool DmiHpsFileInterface::CompleteImportExchangeFile(DmiModelHandler * pcModelHandler)
{
	m_cImportStatus = m_cImportNotifier.Status();

	pcModelHandler->DeleteCadModel();

	if(HPS::IOResult::Success != m_cImportStatus)
	{
		CAtlString strErrorMessage;
		if(HPS::IOResult::Failure == m_cImportStatus) {
			strErrorMessage.Format(_T("Error loading file %s:\n\n\t%s"), m_strFilePathName, std::wstring(m_strExceptionMessage.begin(), m_strExceptionMessage.end()).data());
		}
		else {
			strErrorMessage = GetErrorString(m_cImportStatus, m_strFilePathName);
		}
		
		// Error Message 출력 처리 필요
		//GetCHPSFrame()->MessageBox(str.GetString(), _T("File import error"), MB_ICONERROR | MB_OK);
	}
	else {
		pcModelHandler->SetCadModel(m_cImportNotifier.GetCADModel());
	}

	return true;
}

// 4. Impoert Log 요청에 대응하는 함수
bool DmiHpsFileInterface::ImportExchangeLog()
{
	std::lock_guard<std::mutex> lock(g_cLogMessageMutex);

	// #Require_convert_new_version
/*
	Json::Object cObject;
	cObject.SetDwordPtr("CommandId", (DWORD_PTR) DmiHps::Command::ImportExchangeLog);

	Json::Array & cFileLogArray = cObject.GenArray("FileLog");

	for(auto & strLogText : m_destrLogMessageDeque) {
		cFileLogArray.AddString(strLogText);
	}

	m_destrLogMessageDeque.clear();

	m_pcHpsInterface->SendCommandCallback(cObject);
*/

	return true;
}

void DmiHpsFileInterface::getConfiguration(HPS::Exchange::ConfigurationArray const & configs, HPS::UTF8Array & selectedConfig)
{
	if(configs.empty())
		return;

	selectedConfig.push_back(configs[0].GetName());
	getConfiguration(configs[0].GetSubconfigurations(), selectedConfig);
}

//== Progress 관련 함수 ==============================================================================

void DmiHpsFileInterface::SendLogMessage()
{
	int i = 0;
}

bool DmiHpsFileInterface::InProgressImportStatus()
{
	m_cImportStatus = m_cImportNotifier.Status();

	if(HPS::IOResult::InProgress == m_cImportStatus) {
		return true;
	}
	else
	{
		HPS::Database::GetEventDispatcher().UnSubscribe(*m_pcImportStatusEventHandler);
		delete m_pcImportStatusEventHandler;
	}

	return false;
}

bool DmiHpsFileInterface::IsSuccessImportStatus()
{
	if(HPS::IOResult::Success == m_cImportStatus) {
		return true;
	}

	return false;
}

bool DmiHpsFileInterface::GetImportLogMessage(CString & strJsonReturnMessage)
{
	std::lock_guard<std::mutex> lock(g_cLogMessageMutex);

	// #Require_convert_new_version
/*
	if(false == m_destrLogMessageDeque.empty())
	{
		Json::Object cObject;
		cObject.SetString("Type", L"ImportLogMessage");

		Json::Array & arcMessageArray = cObject.GenArray("MessageArray");

		for(const auto & strMessage : m_destrLogMessageDeque) {
			arcMessageArray.AddString(strMessage);
		}

		cObject.Stringify(strJsonReturnMessage);

		m_destrLogMessageDeque.clear();
	}
*/

	return true;
}

bool DmiHpsFileInterface::GetImportStatusMessage(CString & strReturnMessage)
{
	strReturnMessage = m_strMessage;

	return true;
}

void DmiHpsFileInterface::AddLogEntry(CString chLogEntry)
{
	std::lock_guard<std::mutex> lock(g_cLogMessageMutex);
	m_destrLogMessageDeque.push_back(chLogEntry);
}

CString DmiHpsFileInterface::GetMessage(bool bRemoveMessageFlag)
{ 
	CString strMessage = m_strMessage;

	if(true == bRemoveMessageFlag) {
		m_strMessage.Empty();
	}
	
	return strMessage;
}

void DmiHpsFileInterface::SetImportMessage(CString strMessage) 
{ 
	m_strMessage = strMessage; 

	// #Require_convert_new_version
/*
	Json::Object cObject;
	cObject.SetDwordPtr("CommandId", (DWORD_PTR) DmiHps::Command::ImportMessage);
	cObject.SetString("Message", strMessage);

	m_pcHpsInterface->SendCommandCallback(cObject);*/
}

CAtlString DmiHpsFileInterface::GetErrorString(HPS::IOResult status, LPCTSTR lpszPathName)
{
	CAtlString strErrorMessage;

	switch(status)
	{
		case HPS::IOResult::FileNotFound:
		{
			strErrorMessage.Format(_T("Could not locate file %s"), lpszPathName);
		}	break;

		case HPS::IOResult::UnableToOpenFile:
		{
			strErrorMessage.Format(_T("Unable to open file %s"), lpszPathName);
		}	break;

		case HPS::IOResult::InvalidOptions:
		{
			strErrorMessage.Format(_T("Invalid options"));
		}	break;

		case HPS::IOResult::InvalidSegment:
		{
			strErrorMessage.Format(_T("Invalid segment"));
		}	break;

		case HPS::IOResult::UnableToLoadLibraries:
		{
			strErrorMessage.Format(_T("Unable to load libraries"));
		}	break;

		case HPS::IOResult::VersionIncompatibility:
		{
			strErrorMessage.Format(_T("Version incompatability"));
		}	break;

		case HPS::IOResult::InitializationFailed:
		{
			strErrorMessage.Format(_T("Initialization failed"));
		}	break;

		case HPS::IOResult::UnsupportedFormat:
		{
			strErrorMessage.Format(_T("Unsupported format"));
		}	break;

		case HPS::IOResult::Canceled:
		{
			strErrorMessage.Format(_T("IO Canceled"));
		}	break;

		case HPS::IOResult::Failure:
		default:
		{
			strErrorMessage.Format(_T("Error loading file %s"), lpszPathName);
		}
	}

	return strErrorMessage;
}

//== ImportStatusEventHandler Class ================================================================
namespace
{
	bool isAbsolutePath(char const * path)
	{
		bool starts_with_drive_letter = strlen(path) >= 3 && isalpha(path[0]) && path[1] == ':' && strchr("/\\", path[2]);

		bool starts_with_network_drive_prefix = strlen(path) >= 2 && strchr("/\\", path[0]) && strchr("/\\", path[1]);

		return starts_with_drive_letter || starts_with_network_drive_prefix;
	}
}

ImportStatusEventHandler::ImportStatusEventHandler(DmiHpsFileInterface * pcHpsFileInterface, std::chrono::system_clock::time_point * pcTimes)
{
	m_pcHpsFileInterface = pcHpsFileInterface;
	m_pcTimes = pcTimes;
}

ImportStatusEventHandler::~ImportStatusEventHandler()
{
	Shutdown();
}

HPS::EventHandler::HandleResult ImportStatusEventHandler::Handle(HPS::Event const * pcEvent)
{
	HPS::UTF8 message = static_cast<HPS::ImportStatusEvent const *>(pcEvent)->import_status_message;

	if(message.IsValid())
	{
		bool update_message = true;

		if(message == HPS::UTF8("Import and Tessellation")) {
			m_pcTimes[0] = std::chrono::system_clock::now();
			m_pcHpsFileInterface->SetImportMessage(L"Stage 1/3 : Import and Tessellation");
		}
		else if(message == HPS::UTF8("Creating Graphics Database")) {
			m_pcTimes[1] = std::chrono::system_clock::now();
			m_pcHpsFileInterface->SetImportMessage(L"Stage 2/3 : Creating Graphics Database");
		}
		else if(isAbsolutePath(message))
		{
			WCHAR chBuffer[4098];
			message.ToWStr(chBuffer);

			CString strMessage(chBuffer);

			strMessage = strMessage.Right(strMessage.GetLength() - strMessage.ReverseFind('\\') - 1);

			m_pcHpsFileInterface->AddLogEntry(strMessage);
		}
	}

	return HPS::EventHandler::HandleResult::Handled;
}