#include "StdAfx.h"

#include "3DF.Model.h"

#include "Common_Define.h"

#include <HIOManager.h>

#include "3DF.Selection.h"

USING_3DF_NAMESPACE

Model::Model() :
	m_cSegmentKey(GetModelKey())
{
	SetBRepGeometry(false);
	m_pcMultiSelectManager = new MultiSelectManager();

	m_eModelHandedness = ModelHandedness::NotSet;
	m_pcTopologyManager = new BREP_Topology;
	m_pPMIConnector = nullptr;
	m_pcConnector = nullptr;
	m_pcPRCAsmModelFile = nullptr;
	m_pcPRCDeleteModelCallback = nullptr;
}

Model::~Model()
{
	if(nullptr != m_pcMultiSelectManager) {
		REMOVE_POINTER(m_pcMultiSelectManager);
	}

	if(nullptr != m_pcTopologyManager) {
		REMOVE_POINTER(m_pcTopologyManager);
	}

	if(nullptr != m_pcConnector) {
		REMOVE_POINTER(m_pcConnector);
	}

	if(nullptr != m_pPMIConnector) {
		REMOVE_POINTER(m_pPMIConnector);
	}

	if(nullptr != m_pcPRCAsmModelFile && nullptr != m_pcPRCDeleteModelCallback) {
		m_pcPRCDeleteModelCallback(m_pcPRCAsmModelFile);
	}
}

void Model::SetBRepGeometry(bool bBrepFlag)
{
	if(true == bBrepFlag)
	{
		HC_KEY key = GetModelKey();
		HC_Open_Segment_By_Key(key);
		{
			HC_Set_User_Value(1L);
		}
		HC_Close_Segment();
	}

	HBaseModel::SetBRepGeometry(bBrepFlag);
}