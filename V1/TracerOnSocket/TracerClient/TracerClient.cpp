#include "TracerClient.h"
#include <cassert>

#pragma comment(lib, "Ws2_32.lib")

#ifndef TRACE
#define TRACE
#endif



CTracerClient::CTracerClient()
{
    Initialize();
}



CTracerClient::~CTracerClient()
{
    Close();
}



void CTracerClient::Initialize()
{
    WSADATA wsa;
    m_bInitialized = WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
    assert(m_bInitialized);
}



void CTracerClient::Close()
{
    m_bInitialized = false;
    m_bConnected = false;

    if (m_pAddrInfo) {
        freeaddrinfo(m_pAddrInfo);
        m_pAddrInfo = nullptr;
    }

    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }

    WSACleanup();
}



bool CTracerClient::Connect(const char* address, const char* port)
{
    if (m_bInitialized == false) {
        return false;
    }

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    int result = getaddrinfo(address, port, &hints, &m_pAddrInfo);
    if (result != 0) {
        //:TODO - check result
        return false;
    }

    // Create a SOCKET for connecting to server
    m_socket = socket(m_pAddrInfo->ai_family, m_pAddrInfo->ai_socktype, m_pAddrInfo->ai_protocol);
    if (m_socket == INVALID_SOCKET) {
        //:TODO - WSAGetLastError();
        return false;
    }

    // Connect to server
    result = connect(m_socket, m_pAddrInfo->ai_addr, (int)m_pAddrInfo->ai_addrlen);
    if (result == SOCKET_ERROR) {
        return false;
    }

    return m_bConnected = true;
}



void CTracerClient::WriteLine(const wchar_t* message)
{
    if (m_bConnected) {
        int length = (int)::wcslen(message);
        int needed = ::WideCharToMultiByte(CP_UTF8, 0, message, length, nullptr, 0, nullptr, nullptr);
        assert(needed < 4096);

        if (needed > 0) {
            char buffer[4096];
            ::WideCharToMultiByte(CP_UTF8, 0, message, length, buffer, needed, nullptr, nullptr);
            buffer[needed] = '\n';

            int result = send(m_socket, buffer, needed, 0);
            if (result == SOCKET_ERROR) {
                Close();
            }
        }
    }
    else {
        TRACE(message);
    }
}
