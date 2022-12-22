#pragma once

class DmiC3dModeler;

class __declspec(dllexport) DmiC3dInterface
{
public:
	DmiC3dInterface();
	~DmiC3dInterface();

	bool ExecuteCommand(DWORD_PTR nJsonObject);

	bool GetModel(DWORD_PTR nId, DWORD_PTR & pcModel);
	bool DeleteModel(DWORD_PTR nId);

private:
	DmiC3dModeler * m_pcC3dModeler;
};