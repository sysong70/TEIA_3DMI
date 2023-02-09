#pragma once

typedef void (*SendSignalFunc)(const wchar_t* content);



extern "C"
{
	INTERFACE_API bool Initialize();

	INTERFACE_API bool Terminate();
	// Pipe for renderer to ui
	INTERFACE_API void AssignSendSignalFunc(SendSignalFunc fp);
	// Pipe for ui to renderer
	INTERFACE_API bool ReceiveSignal(const wchar_t* content);
}
