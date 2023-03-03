#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>



class CTracerClient
{
public:

	CTracerClient();

	~CTracerClient();

protected:

	void Initialize();

	void Close();

public:

	bool Connect(const char* hostAddress = "127.0.0.1", const char* hostPort = "8080");

	bool IsValid() {
		return m_bConnected;
	}

	void WriteLine(const wchar_t* message);

private:

	struct addrinfo* m_pAddrInfo = nullptr;
	SOCKET m_socket = INVALID_SOCKET;
	bool m_bInitialized = false;
	bool m_bConnected = false;
};

