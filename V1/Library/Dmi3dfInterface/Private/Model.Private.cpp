#include "StdAfx.h"

#include "Model.Private.h"

#include "Common_Define.h"

#include "../3DF/3DF.Utility.h"

using namespace H3DF;

//== Model Class ===================================================================================

H3DF::ModelPrivate::ModelPrivate()
	: HBaseModel()
{
	m_cSegmentKey.Set(GetModelKey());

	SetBRepGeometry(false);

	m_eModelHandedness = ModelHandedness::NotSet;
	m_pcTopologyManager = new BREP_Topology;
	m_pPMIConnector = nullptr;
	m_pcConnector = nullptr;
	m_pcPRCAsmModelFile = nullptr;
	m_pcPRCDeleteModelCallback = nullptr;
}

H3DF::ModelPrivate::~ModelPrivate()
{
	if (nullptr != m_pcTopologyManager) {
		REMOVE_POINTER(m_pcTopologyManager);
	}

	if (nullptr != m_pcConnector) {
		REMOVE_POINTER(m_pcConnector);
	}

	if (nullptr != m_pPMIConnector) {
		REMOVE_POINTER(m_pPMIConnector);
	}

	if (nullptr != m_pcPRCAsmModelFile && nullptr != m_pcPRCDeleteModelCallback) {
		m_pcPRCDeleteModelCallback(m_pcPRCAsmModelFile);
	}
}

SegmentKey H3DF::ModelPrivate::GetSegmentKey()
{
	return m_cSegmentKey;
}

SegmentKey const H3DF::ModelPrivate::GetSegmentKey() const
{
	return m_cSegmentKey;
}

void H3DF::ModelPrivate::SetBRepGeometry(bool bBrepFlag)
{
	if (true == bBrepFlag)
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

void H3DF::ModelPrivate::UpdateModelHandedness()
{
	// see if handedness attribute was defined in the model,
	// if yes set our member variable
	// what is the polygon handedness for this model
	HC_Open_Segment_By_Key(GetModelKey()); {
		if (HC_Show_Existence("heuristics"))
		{
			unsigned int token_num = 0;
			char heuristics[MVO_BUFFER_SIZE], token[MVO_BUFFER_SIZE];
			HC_Show_Heuristics(heuristics);
			while (HC_Parse_String(heuristics, ",", token_num++, token))
			{
				if (strstr(token, "polygon handedness"))
				{
					HC_Parse_String(token, "=", token_num++, token);
					if (strstr(token, "left"))
						m_eModelHandedness = ModelHandedness::Left;
					else if (strstr(token, "right"))
						m_eModelHandedness = ModelHandedness::Right;
					else
						m_eModelHandedness = ModelHandedness::None;
				}
			}
		}
		else {
			m_eModelHandedness = ModelHandedness::NotSet;
		}
	} HC_Close_Segment();
}