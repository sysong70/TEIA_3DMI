#include "stdafx.h"
#include "DmiC3dModeler.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <model.h>

using namespace std;

DmiC3dModeler::DmiC3dModeler()
{
}


DmiC3dModeler::~DmiC3dModeler()
{
}

bool DmiC3dModeler::GetModel(DWORD_PTR nId, MbModel *& pcModel)
{
	auto cIterator = m_mapModelMap.find(nId);
	if(m_mapModelMap.end() == cIterator) {
		pcModel = new MbModel();
		if(nullptr == pcModel) {
			return false;
		}

		m_mapModelMap.insert(make_pair(nId, pcModel));

		return true;
	}

	pcModel = cIterator->second;

	return true;
}

bool DmiC3dModeler::DeleteModel(DWORD_PTR nId)
{
	auto cIterator = m_mapModelMap.find(nId);
	if(m_mapModelMap.end() == cIterator) {
		return false;
	}

	MbModel * pcModel = cIterator->second;
	if(nullptr == pcModel) {
		return false;
	}

	m_mapModelMap.erase(nId);

	::DeleteItem(pcModel);
// 	pcModel->DeleteItems();
// 	delete pcModel;

	return true;
}
