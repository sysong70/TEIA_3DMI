#pragma once

#define LOGMANAGER_MAX_COUNT	10

#define LOGMANAGER_3DF_ERROR_LOG_ID		5

class LogManager
{
public:
	~LogManager();

	static void Log(LPCWSTR mesg, ...);
	static void Log(LPCSTR chMessage, ...);
	static void Log(int nId, LPCWSTR mesg, ...);
	static void Log(int nId, LPCSTR mesg, ...);
	static void Log(int nId, int nLogLevle, LPCWSTR mesg, ...);
	static void Log(int nId, int nLogLevle, LPCSTR mesg, ...);

	static void CreateLog(int nId, const WCHAR * pchFilePathName, bool bExistsFileDelete = true);

	static LogManager * GetInstance();

	static void SetComment(CString strComment) { m_strLogManagerComment[m_nCurrentId] = strComment; }

	static void SetLogFileNamePrefix(CString strPrefix) { m_strLogFileNamePrefix[m_nCurrentId] = strPrefix; }
	static void SetWriteLog(bool bFlag) { m_bWriteLogFlag[m_nCurrentId] = bFlag; }
	static void SetWriteLog(int nId, bool bFlag) { m_bWriteLogFlag[nId] = bFlag; }
	static void SetWriteTimeLog(bool bFlag) { m_bWriteTimeFlag[m_nCurrentId] = bFlag; }
	static void SetWriteTimeLog(int nId, bool bFlag) { m_bWriteTimeFlag[nId] = bFlag; }

	static bool IsWriteLog() { return m_bWriteLogFlag; }

	// FilePathName을 지정할 경우 특정 위치에 저장한다.
	static void SetFilePathName(CString strFilePathName) { m_strFilePathName[m_nCurrentId] = strFilePathName; }
	static void SetFilePathName(int nId, CString strFilePathName) { m_strFilePathName[nId] = strFilePathName; }

	// true인 경우 file을 새롭게 생성한다.
	static void SetCreateFile(bool bFlag) { m_bCreateFileFlag[m_nCurrentId] = bFlag; }
	static void SetCreateFile(int nId, bool bFlag) { m_bCreateFileFlag[nId] = bFlag; }

	static void ResetTabIndex();
	static void ResetTabIndex(int nId);
	static void IncreaseTabIndex();
	static void IncreaseTabIndex(int nId);
	static void DecreaseTabIndex();
	static void DecreaseTabIndex(int nId);

	static int GetCurrentId() { return m_nCurrentId; }
	static void SetCurrentId(int nId) { m_nCurrentId = nId; }
	static void SetLogLevel(int nLogLevel) { m_nLogLevel[m_nCurrentId] = nLogLevel; }
	static void SetLogLevel(int nId, int nLogLevel) { m_nLogLevel[nId] = nLogLevel; }
	static int GetLogLevel() { return m_nLogLevel[m_nCurrentId]; }
	static int GetLogLevel(int nId) { return m_nLogLevel[nId]; }

	static CString HexStr(DWORD_PTR nValue);
	static CString BoolStr(bool bValue);

public:
	class Init 
	{
	public:

		Init();
		//:Ken
		~Init();
	};

	static CString GetExecuteDirectory();
	static bool CreateFolder(CString strPath);

protected:
	
	bool CStringToChar(CString strText, char *& pchText);
	bool CStringToChar(CString strText, char *& pchText, int & nTextSize);

	int Open();
	int Open(int nId);

	void WriteLog(CString strMessage);
	void WriteLog(int nId, CString strMessage);

private:
	static Init StaticInitializer;

	static bool m_bInstanceFlag;
	static LogManager * m_pcLogManger;

	static int m_nFileHandle[LOGMANAGER_MAX_COUNT];

	static CString m_strFilePathName[LOGMANAGER_MAX_COUNT];
	static CString m_strLogManagerComment[LOGMANAGER_MAX_COUNT];
	static CString m_strLogFileNamePrefix[LOGMANAGER_MAX_COUNT];
		
	static bool m_bWriteTimeFlag[LOGMANAGER_MAX_COUNT];
	static bool m_bWriteLogFlag[LOGMANAGER_MAX_COUNT];
	static bool m_bCreateFileFlag[LOGMANAGER_MAX_COUNT]; // true인 경우 file을 새롭게 생성한다.

	static int m_nCurrentId;
	static int m_nTabIndex[LOGMANAGER_MAX_COUNT];
	static int m_nLogLevel[LOGMANAGER_MAX_COUNT];
};