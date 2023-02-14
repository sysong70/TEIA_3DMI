#include "stdafx.h"

// CString용 Header
#include <atlstr.h>

#include "LogManager.h"

#include <ShlObj_core.h>
#include <ctime>
#include <io.h>
#include <fcntl.h>

bool LogManager::m_bInstanceFlag;
LogManager * LogManager::m_pcLogManger;

CString LogManager::m_strProductName;
CString LogManager::m_strProductVersion;
CString LogManager::m_strFilePathName[LOGMANAGER_MAX_COUNT];
CString LogManager::m_strLogManagerComment[LOGMANAGER_MAX_COUNT];
CString LogManager::m_strLogFileNamePrefix[LOGMANAGER_MAX_COUNT];
bool LogManager::m_bWriteTimeFlag[LOGMANAGER_MAX_COUNT];
bool LogManager::m_bWriteLogFlag[LOGMANAGER_MAX_COUNT];
bool LogManager::m_bCreateFileFlag[LOGMANAGER_MAX_COUNT]; // true인 경우 file을 새롭게 생성한다.
int LogManager::m_nCurrentId;
int LogManager::m_nTabIndex[LOGMANAGER_MAX_COUNT];
int LogManager::m_nLogLevel[LOGMANAGER_MAX_COUNT];

int LogManager::m_nFileHandle[LOGMANAGER_MAX_COUNT];
bool LogManager::m_bFileCloseFlag[LOGMANAGER_MAX_COUNT];

LogManager::Init::Init()
{
	m_bInstanceFlag = false;
	m_pcLogManger = nullptr;

	m_strProductName = L"";
	m_strProductVersion = L"";

	m_nCurrentId = 0;

	for(int nIndex = 0; nIndex < LOGMANAGER_MAX_COUNT; nIndex++)
	{
		m_strFilePathName[nIndex] = L"";
		m_strLogFileNamePrefix[nIndex] = L"";
		m_strLogManagerComment[nIndex] = L"";

		m_bCreateFileFlag[nIndex] = false;
		m_bWriteTimeFlag[nIndex] = true;
		m_bWriteLogFlag[nIndex] = false;
		m_nTabIndex[nIndex] = 0;
		m_nLogLevel[nIndex] = -1;
		m_nFileHandle[nIndex] = NULL;
		m_bFileCloseFlag[nIndex] = true;
	}
}

//:Ken
LogManager::Init::~Init()
{
	if (m_bInstanceFlag && m_pcLogManger != nullptr) {
		delete m_pcLogManger;
		m_pcLogManger = nullptr;
	}
}

LogManager::Init LogManager::StaticInitializer;

LogManager::~LogManager()
{
	//this->Close();
}

LogManager * LogManager::GetInstance()
{
	if(!m_bInstanceFlag)
	{
		m_pcLogManger = new LogManager();
		m_bInstanceFlag = true;
	}
	return m_pcLogManger;
}

void LogManager::Log(LPCWSTR chMessage, ...)
{
	if(false == m_bWriteLogFlag[m_nCurrentId]) {
		return;
	}

	va_list cArgList;
	va_start(cArgList, chMessage);

	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(strBuffer);
}

void LogManager::Log(int nId, LPCWSTR chMessage, ...)
{
	va_list cArgList;
	va_start(cArgList, chMessage);

	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(nId, strBuffer);
}

void LogManager::Log(int nId, int nLogLevle, LPCWSTR chMessage, ...)
{
	if(0 > m_nLogLevel[nId] || m_nLogLevel[nId] < nLogLevle) {
		return;
	}

	va_list cArgList;
	va_start(cArgList, chMessage);
	
	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(nId, strBuffer);
}

void LogManager::WriteLog(CString strMessage)
{
	WriteLog(m_nCurrentId, strMessage);
}

void LogManager::WriteLog(int nId, CString strMessage)
{
	CString strBuffer, strBuffer2;

	if(true == m_bFileCloseFlag[nId]) {
		m_nFileHandle[nId] = LogManager::Open(nId);
	}
	else {
		if(NULL == m_nFileHandle[nId]) {
			m_nFileHandle[nId] = LogManager::Open(nId);
		}
	}

	// Log 파일을 생성하지 못한경우 저장하지 않는다.
	if(NULL == m_nFileHandle[nId]) {
		return;
	}

	struct tm cCurTime;
	time_t cLocalCurrentTime = time(nullptr);
	_localtime64_s(&cCurTime, &cLocalCurrentTime);

	CString strTabText;
	for(int nIndex = 0; nIndex < m_nTabIndex[nId]; nIndex++) {
		strTabText += L"\t";
	}

	if(true == m_bWriteTimeFlag[nId]) {
		CString strTimeText;
		strTimeText.Format(L"%04d-%02d-%02d %02d:%02d:%02d", cCurTime.tm_year + 1900, cCurTime.tm_mon + 1, cCurTime.tm_mday,
			cCurTime.tm_hour, cCurTime.tm_min, cCurTime.tm_sec);

		if(true == m_strLogManagerComment[nId].IsEmpty()) {
			strBuffer.Format(L"%s%s   %s", strTabText, strTimeText, strMessage);
		}
		else {
			strBuffer.Format(L"%s%s   %s [%s]", strTabText, strTimeText, strMessage, m_strLogManagerComment[nId]);
			m_strLogManagerComment[nId].Empty();
		}
	}
	else {
		if(true == m_strLogManagerComment[nId].IsEmpty()) {
			strBuffer.Format(L"%s%s", strTabText, strMessage);
		}
		else {
			strBuffer.Format(L"%s%s [%s]", strTabText, strMessage, m_strLogManagerComment[nId]);
			m_strLogManagerComment[nId].Empty();
		}
	}

	strBuffer += "\n";

	int nTextSize = 0;
	char * pchText = nullptr;

	if(true == CStringToChar(strBuffer, pchText, nTextSize)) {
		size_t nSize = strlen(pchText);
		int nBytesWritten = _write(m_nFileHandle[nId], pchText, (UINT)nSize);
	}

	if(true == m_bFileCloseFlag[nId]) {
		_close(m_nFileHandle[nId]);
		m_nFileHandle[nId] = NULL;
	}

	delete[] pchText;
}

int LogManager::Open()
{
	return Open(m_nCurrentId);
}

int LogManager::Open(int nId)
{
	CString strFilePath;
	CString strFilePathName;
	CString strFileName;
	WCHAR chReturn[255];
	ZeroMemory(chReturn, sizeof(chReturn));

	if(true == m_strFilePathName[nId].IsEmpty()) {
		CString strAppDataDirectory = GetAppDataFolderPath();
		strFilePath.Format(L"%sSystemLog\\", strAppDataDirectory);

		struct tm cCurTime;
		time_t cLocalCurrentTime = time(nullptr);
		_localtime64_s(&cCurTime, &cLocalCurrentTime);

		::CreateDirectory(strFilePath, nullptr);

		CString strTimeText;
		strTimeText.Format(L"%04d%02d%02d", cCurTime.tm_year + 1900, cCurTime.tm_mon + 1, cCurTime.tm_mday);

		if(true == m_strLogFileNamePrefix[nId].IsEmpty()) {
			strFileName.Format(L"%s.log", strTimeText);
		}
		else {
			strFileName.Format(L"%s %s.log", m_strLogFileNamePrefix[nId], strTimeText);
		}

		strFilePathName = strFilePath + strFileName;
	}
	else {
		strFilePathName = m_strFilePathName[nId];
	}

	if(true == m_bCreateFileFlag[nId]) {
		_wremove(strFilePathName);
		m_bCreateFileFlag[nId] = false;
	}

	int nFileHandle = 0;
	//_O_RDWR | _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE
	errno_t nErrorNo = _wsopen_s(&nFileHandle, strFilePathName, _O_CREAT | _O_RDWR, _SH_DENYRD, _S_IREAD | _S_IWRITE); // | _O_U8TEXT
	if(0 != nErrorNo) {
		return 0;
	}

	_lseek(nFileHandle, 0L, SEEK_END);

	return nFileHandle;
}

/*
void LogManager::Close()
{
}
*/

// 다중 디렉토리도 생성함.
bool LogManager::CreateFolder(CString strPath)
{
	WCHAR chPathBuffer[MAX_PATH];

	size_t len = wcslen(strPath);

	for(size_t i = 0; i < len; i++)
	{
		chPathBuffer[i] = *(strPath.GetBuffer() + i);
		if(_T('\\') == chPathBuffer[i] || _T('/') == chPathBuffer[i])
		{
			chPathBuffer[i + 1] = NULL;
			if(FALSE == PathFileExists(chPathBuffer))
			{
				if(FALSE == ::CreateDirectory(chPathBuffer, NULL))
				{
					if(GetLastError() != ERROR_ALREADY_EXISTS) {
						return false;
					}
				}
			}
		}
	}

	return true;
}

CString LogManager::GetExecuteDirectory()
{
	TCHAR szBuffer[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Path[_MAX_PATH];
	TCHAR Filename[_MAX_FNAME];
	TCHAR Ext[_MAX_EXT];

	GetModuleFileName(NULL, szBuffer, sizeof(szBuffer));	// get process file name
	_wsplitpath_s(szBuffer, Drive, Path, Filename, Ext);	// get drive, path, file, ext name

	CString strFilePath;
	strFilePath.Format(L"%s%s", Drive, Path);

	return strFilePath;
}

CString LogManager::GetAppDataFolderPath()
{
	CString strLicenseFilePathName;
	WCHAR chAppDataPath[MAX_PATH] = {0,};

	// Get path for each computer, non-user specific and non-roaming data.
	// Vista 이상 : C:/ProgramData/MDS 2015
	// XP : C:/Documents and Settings/All Users/Application Data/MDS 2015
	if(S_OK == SHGetFolderPath(nullptr, CSIDL_COMMON_APPDATA, NULL, 0, chAppDataPath)) {
		strLicenseFilePathName = chAppDataPath;
		strLicenseFilePathName += L"\\" + m_strProductName + L"\\" + m_strProductName + L" " + m_strProductVersion + L"\\";

		if(0 != _wchdir(strLicenseFilePathName))
		{
			if(false == CreateFolder(strLicenseFilePathName))
			{
				return L"";
			}
		}
	}

	return strLicenseFilePathName;
}

void LogManager::ResetTabIndex()
{
	ResetTabIndex(m_nCurrentId);
}

void LogManager::ResetTabIndex(int nId)
{
	m_nTabIndex[nId] = 0;
}

void LogManager::IncreaseTabIndex()
{
	IncreaseTabIndex(m_nCurrentId);
}

void LogManager::IncreaseTabIndex(int nId)
{
	m_nTabIndex[nId]++;
}

void LogManager::DecreaseTabIndex()
{
	DecreaseTabIndex(m_nCurrentId);
}

void LogManager::DecreaseTabIndex(int nId)
{
	m_nTabIndex[nId]--;
	if(0 > m_nTabIndex[nId]) {
		m_nTabIndex[nId] = 0;
	}
}

void LogManager::SetFileCloseFlag(bool bFlag)
{
	SetFileCloseFlag(m_nCurrentId, bFlag);
}

void LogManager::SetFileCloseFlag(int nId, bool bFlag) 
{ 
	m_bFileCloseFlag[nId] = bFlag; 

	if(true == bFlag) {
		if(NULL != m_nFileHandle[nId]) {
			_close(m_nFileHandle[nId]);
			m_nFileHandle[nId] = NULL;
		}
	}
}

bool LogManager::CStringToChar(CString strText, char *& pchText)
{
	int nTextSize = 0;
	return CStringToChar(strText, pchText, nTextSize);
}

bool LogManager::CStringToChar(CString strText, char *& pchText, int & nTextSize)
{
	if(true == strText.IsEmpty()) {
		return false;
	}

	int nSize = WideCharToMultiByte(CP_ACP, 0, strText, -1, NULL, 0, NULL, NULL);
	pchText = new char[nSize];
	if(nullptr == pchText) {
		return false;
	}

	nTextSize = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) strText, -1, pchText, nSize, NULL, NULL);

	if(0 == nTextSize) {
		delete[] pchText;
		return false;
	}

	return true;
}

CString LogManager::HexStr(DWORD_PTR nValue)
{
	CString strText;
	strText.Format(L"0x%llx", nValue);
	return strText;
}

CString LogManager::BoolStr(bool bValue)
{
	if(true == bValue) {
		return L"true";
	}

	return L"false";
}