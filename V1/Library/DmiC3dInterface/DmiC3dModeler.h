#pragma once

#include <map>

class MbModel;

class DmiC3dModeler
{
public:
	DmiC3dModeler();
	~DmiC3dModeler();

	bool GetModel(DWORD_PTR nId, MbModel *& pcModel);
	bool DeleteModel(DWORD_PTR nId);

private:
	std::map<DWORD_PTR, MbModel *> m_mapModelMap;
};

