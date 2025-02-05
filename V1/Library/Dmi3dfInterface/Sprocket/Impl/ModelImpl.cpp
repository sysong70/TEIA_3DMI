#include "StdAfx.h"

#include "ModelImpl.h"

#include "Common_Define.h"

#include "../../3DF/Bounding.h"
#include "../../3DF/AttributeLock.h"
#include "../../3DF/Visibility.h"
#include "../../3DF/Condition.h"
#include "../../3DF/LineAttribute.h"
#include "../../3DF/DrawingAttribute.h"
#include "../../3DF/ColorInterpolation.h"

#include "../../3DF/3DF.Utility.h"

using namespace H3DF;

//== Model Class ===================================================================================

H3DF::ModelImpl::ModelImpl()
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

H3DF::ModelImpl::~ModelImpl()
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

// #Model: Init 및 Style 정의 (Show, NoShow)
void H3DF::ModelImpl::Init()
{
	HBaseModel::Init();

	m_cInclude = m_cSegmentKey.Subsegment("model_include");
	m_cInclude.GetAttributeLockControl().SetLock(AttributeLock::Type::Visibility);
	m_cInclude.SetVisibility(L"off");

	m_cModels = m_cSegmentKey.Subsegment("models");
	m_cMeasurements = m_cSegmentKey.Subsegment("measurements");
	m_cMarkups = m_cSegmentKey.Subsegment("markups");

	// 화면에 표시되는 통상적인 Line Weigth를 설정한다.	
	float fLineWeight = 0.0005f;
	Line::SizeUnits eUnits = Line::SizeUnits::WindowRelative;
	m_cModels.GetLineAttributeControl().SetWeight(fLineWeight, eUnits);

	BoundingKit cBounding;
	cBounding.SetExclusion(true);
	m_cInclude.SetBounding(cBounding);

	m_cIncludeSegment = m_cInclude.Subsegment("include");
	m_cIncludeModel = m_cIncludeSegment.Subsegment("model");
	m_cIncludeStyles = m_cIncludeModel.Subsegment("styles");

	// Show Condtion용 Style 생성
	m_cShowStyle = m_cIncludeStyles.Subsegment("show_style");
	m_cShowStyle.GetVisibilityControl().SetFaces(true).SetLines(true).SetText(true);

	m_cShowWireFrameStyle = m_cIncludeStyles.Subsegment("show_wireframe_style");
	m_cShowWireFrameStyle.GetVisibilityControl().SetLines(true);

	m_cShowVertexStyle = m_cIncludeStyles.Subsegment("show_vertex_style");
	m_cShowVertexStyle.GetVisibilityControl().SetVertices(true);

	// No Show Condtion용 Style 생성
	m_cNoShowStyle = m_cIncludeStyles.Subsegment("noshow_style");
	m_cNoShowStyle.GetVisibilityControl().SetFaces(false).SetLines(false).SetVertices(false).SetText(false);

	m_cNoShowWireFrameStyle = m_cIncludeStyles.Subsegment("noshow_wireframe_style");
	m_cNoShowWireFrameStyle.GetVisibilityControl().SetLines(false);

	m_cNoShowVertexStyle = m_cIncludeStyles.Subsegment("noshow_vertex_style");
	m_cNoShowVertexStyle.GetVisibilityControl().SetVertices(false);

	m_cModels.GetStyleControl().PushSegment(m_cShowStyle);

// 	입력된 Matrial을 Face에 적용한다.
// 	MaterialMappingKit cMaterialMapping;
// 	cMaterialMapping.SetFaceMaterial(cInKit);
// 
// 	cStyleSegment.SetMaterialMapping(cMaterialMapping);
// 
// 	H3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

}

SegmentKey H3DF::ModelImpl::GetSegmentKey()
{
	return m_cSegmentKey;
}

SegmentKey const H3DF::ModelImpl::GetSegmentKey() const
{
	return m_cSegmentKey;
}

void H3DF::ModelImpl::SetBRepGeometry(bool bBrepFlag)
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

void H3DF::ModelImpl::UpdateModelHandedness()
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