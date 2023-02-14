#include "stdafx.h"

#include "DmiC3dInterface.h"

#include "DmiC3dModeler.h"

#include <Json.h>

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <model.h>

using namespace c3d;

#define CHECK_HPS_VISUALIZE if(nullptr == m_pcHpsVisualize) { assert(FALSE);  return false; } 

DmiC3dInterface::DmiC3dInterface()
{
	m_pcC3dModeler = new DmiC3dModeler();
}

DmiC3dInterface::~DmiC3dInterface()
{
	//:Ken
	delete m_pcC3dModeler;
}

bool DmiC3dInterface::ExecuteCommand(DWORD_PTR nJsonObject)
{
	return true;
}

// 1. ID를 이용해서 기존에 생성되어 있는 Model을 가져온다.
// 2. ID와 일치하는 Model이 없는 경우 새로운 ID를 갖는 Model을 생성한다.
bool DmiC3dInterface::GetModel(DWORD_PTR nId, DWORD_PTR & pcModel)
{
	if(nullptr != m_pcC3dModeler) {
		RETURN_FALSE;
	}

	MbModel * pcTempModel = nullptr;
	if(true == m_pcC3dModeler->GetModel(nId, pcTempModel)) {
		pcModel = (DWORD_PTR) pcTempModel;
		return true;
	}

	return false;
}

bool DmiC3dInterface::DeleteModel(DWORD_PTR nId)
{
	if(nullptr != m_pcC3dModeler) {
		RETURN_FALSE;
	}

	if(true == m_pcC3dModeler->DeleteModel(nId)) {
		return true;
	}

	return false;
}