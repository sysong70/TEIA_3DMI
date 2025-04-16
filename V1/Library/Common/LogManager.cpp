#include "stdafx.h"

// CString용 Header
#include <atlstr.h>

#include "LogManager.h"

#include "Path.h"

#include <ShlObj_core.h>
#include <ctime>
#include <io.h>
#include <fcntl.h>

bool LogManager::m_bInstanceFlag;
LogManager * LogManager::m_pcLogManger;

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

LogManager::Init::Init()
{
	m_bInstanceFlag = false;
	m_pcLogManger = nullptr;

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
	}
}

// KEN
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

void LogManager::Log(LPCSTR chMessage, ...)
{
	if (false == m_bWriteLogFlag[m_nCurrentId]) {
		return;
	}

	va_list cArgList;
	va_start(cArgList, chMessage);

	CStringA strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(CString(strBuffer));
}

void LogManager::Log(int nId, LPCWSTR chMessage, ...)
{
	if (false == m_bWriteLogFlag[nId]) {
		return;
	}

	va_list cArgList;
	va_start(cArgList, chMessage);

	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(nId, strBuffer);
}

void LogManager::Log(int nId, LPCSTR chMessage, ...)
{
	if (false == m_bWriteLogFlag[nId]) {
		return;
	}

	va_list cArgList;
	va_start(cArgList, chMessage);

	CStringA strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(nId, CString(strBuffer));
}

void LogManager::Log(int nId, int nLogLevle, LPCWSTR chMessage, ...)
{
	if (false == m_bWriteLogFlag[nId]) {
		return;
	}

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

void LogManager::Log(int nId, int nLogLevle, LPCSTR chMessage, ...)
{
	if (false == m_bWriteLogFlag[nId]) {
		return;
	}

	if (0 > m_nLogLevel[nId] || m_nLogLevel[nId] < nLogLevle) {
		return;
	}

	va_list cArgList;
	va_start(cArgList, chMessage);

	CStringA strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(nId, CString(strBuffer));
}

void LogManager::CreateLog(int nId, const WCHAR * pchFilePathName, bool bExistsFileDelete)
{
	SetFilePathName(nId, pchFilePathName);
	SetCreateFile(nId, true);

	if (true == bExistsFileDelete) {
		if (NULL != m_nFileHandle[nId]) {
			_close(m_nFileHandle[nId]);
			m_nFileHandle[nId] = NULL;
		}

		_wremove(pchFilePathName);
	}

	if (NULL == m_nFileHandle[nId]) {
		m_nFileHandle[nId] = LogManager::GetInstance()->Open(nId);
	}

	SetWriteTimeLog(nId, true);
	Log(nId, L"Create Log");
	SetWriteTimeLog(nId, false);

/*
	SetCurrentId(nId);

	SetFilePathName(nId, pchFilePathName);
	SetCreateFile(nId, true);
	SetWriteLog(nId, true);
	ResetTabIndex(nId);

	SetWriteTimeLog(nId, true);
	Log(nId, L"Create Log");
	SetWriteTimeLog(nId, false);*/
}

void LogManager::WriteLog(CString strMessage)
{
	WriteLog(m_nCurrentId, strMessage);
}

void LogManager::WriteLog(int nId, CString strMessage)
{
	if(false == m_bWriteLogFlag[nId]) {
		return;
	}

	CString strBuffer, strBuffer2;

	if (NULL == m_nFileHandle[nId]) {
		m_nFileHandle[nId] = LogManager::Open(nId);
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
		strTabText += L"   ";
	}

	if(true == m_bWriteTimeFlag[nId]) {
		CString strTimeText;
		strTimeText.Format(L"%04d-%02d-%02d %02d:%02d:%02d", cCurTime.tm_year + 1900, cCurTime.tm_mon + 1, cCurTime.tm_mday,
			cCurTime.tm_hour, cCurTime.tm_min, cCurTime.tm_sec);

		if(true == m_strLogManagerComment[nId].IsEmpty()) {
			strBuffer.Format(L"%s%s  %s", strTimeText, strTabText, strMessage);
		}
		else {
			strBuffer.Format(L"%s%s  %s [%s]", strTimeText, strTabText, strMessage, m_strLogManagerComment[nId]);
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

	_commit(m_nFileHandle[nId]);

	delete[] pchText;
}

int LogManager::Open()
{
	return Open(m_nCurrentId);
}

int LogManager::Open(int nId)
{
	if (true == m_strFilePathName[nId].IsEmpty()) {
		DEBUG_STOP;
		return 0;
	}

	CString strFilePath;
	CString strFilePathName;
	CString strFileName;
	WCHAR chReturn[255];
	ZeroMemory(chReturn, sizeof(chReturn));

	strFilePathName = m_strFilePathName[nId];
	strFilePath = Path::GetDirectory(strFilePathName);
	strFileName = Path::GetFileName(strFilePathName);

	::CreateDirectory(strFilePath, nullptr);

	strFilePathName = strFilePath + strFileName;

	// 파일이 존재하는 경우 파일을 새롭게 생성한다.
	if(true == m_bCreateFileFlag[nId]) {
		// 경로에 있는 파일을 삭제
		_wremove(strFilePathName);
		m_bCreateFileFlag[nId] = false;
	}

	int nFileHandle = 0;
	//_O_RDWR | _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE
	
	//setlocale(LC_ALL, "");
// 	setlocale(LC_ALL, "ko-KR");
 	//setlocale(LC_ALL, ".utf8");

	errno_t nErrorNo = _wsopen_s(&nFileHandle, strFilePathName, _O_CREAT | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE); // | _O_U8TEXT
	if(0 != nErrorNo) {
		return 0;
	}

	_lseek(nFileHandle, 0L, SEEK_END);

	return nFileHandle;
}

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

	GetModuleFileName(NULL, szBuffer, sizeof(szBuffer)); // get process file name
	_wsplitpath_s(szBuffer, Drive, _MAX_DRIVE, Path, _MAX_PATH, Filename, _MAX_FNAME, Ext, _MAX_EXT); // get drive, path, file, ext name

	CString strFilePath;
	strFilePath.Format(L"%s%s", Drive, Path);

	return strFilePath;
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

	int nSize = WideCharToMultiByte(CP_UTF8, 0, strText, -1, NULL, 0, NULL, NULL);
	pchText = new char[nSize];
	if(nullptr == pchText) {
		return false;
	}

	nTextSize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR) strText, -1, pchText, nSize, NULL, NULL);

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
