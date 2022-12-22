#pragma once

#include <type_traits>

// 참조 문헌
// https://blog.benoitblanchon.fr/getprocaddress-like-a-boss/

class ProcPtr {
public:
	explicit ProcPtr(FARPROC ptr) { _ptr = ptr; }

	template <typename T, typename = std::enable_if_t<std::is_function_v<T>>>

	operator T *() const 
	{
		return reinterpret_cast<T *>(_ptr);
	}

private:
	FARPROC _ptr;
};

class DllHelper {
public:
	explicit DllHelper(LPCTSTR strFileName)
	{
		SetLastError(0);

		m_nLastErrorCode = 0;

		m_hModule = LoadLibrary(strFileName);

		if (nullptr == m_hModule) {
			m_nLastErrorCode = GetLastError();
		}
	}

	~DllHelper() 
	{ 
		if (nullptr != m_hModule) {
			FreeLibrary(m_hModule);
		}
	}

	ProcPtr GetFunction(LPCSTR proc_name) const
	{
		return ProcPtr(GetProcAddress(m_hModule, proc_name));
	}

	ProcPtr operator[](LPCSTR proc_name) const 
	{
		return ProcPtr(GetProcAddress(m_hModule, proc_name));
	}

	DWORD GetErrorCode() { return m_nLastErrorCode; }

	HMODULE GetHmodule() { return m_hModule; } 

private:
	HMODULE m_hModule;
	DWORD m_nLastErrorCode;
};

//+ == 사용법 =============================================================
/*
TestDLG는 사전에 정의되어 있는 함수라야 함.
_declspec (dllexport) bool TestDLG(CString strText);

class TestApi {
	DllHelper _dll{ L"..\\MFCLibrary1\\Debug\\MFCLibrary1.dll" };

public:
	decltype(TestDLG) * cDLG = _dll["TestDLG"];
};

int main() {
	TestApi testApi;
	testApi.cDLG(L"Test Dll Helper");
}
*/