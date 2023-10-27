#include "stdafx.h"

#include "3DF.View.h"
#include "Private/View.Private.h"

#include "Signal.Connector.h"

#include <Common_Define.h>
#include <Path.h>

#include "3DF.Canvas.h"
#include "Private/Canvas.Private.h"

#include "3DF/Segment.h"
#include "3DF/Private/SegmentPrivate.h"

#include "3DF/3DF.Utility.h"

#include "Import/DLL.Interface.h"

#include <HIOUtilityHsf.h>
#include <HConstantFrameRate.h>
#include <HIOUtilityPointCloud.h>

#include <chrono>

using namespace H3DF;
using namespace std::chrono;

H3DF::View::View()
{
	m_pcImpl = new ViewPrivate();
	if (nullptr == m_pcImpl) {
		assert(false);
	}
}

H3DF::View::View(View const & cInThat)
{
	m_pcImpl = new ViewPrivate();
	if (nullptr == m_pcImpl) {
		assert(false);
	}

	Set(cInThat);
}

H3DF::View::~View()
{

}

void H3DF::View::Set(View const & cInThat)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	ViewPrivate * pcInThatImpl = (ViewPrivate *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

View const & H3DF::View::operator = (View const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::View::Update(Json::Object & cInObject) const
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if (false == pcImpl->IsInitNavigationCube()) {
		Json::Array & cArray = cInObject.GetArray(SKW_RECT);
		int nLeft = cArray[0]->ToInteger();
		int nTop = cArray[1]->ToInteger();
		int nRight = cArray[2]->ToInteger();
		int nBottom = cArray[3]->ToInteger();

		pcImpl->InitNavigationCube(nRight, nBottom);
	}

	if (pcImpl->GetBaseView()->GetViewActive() && !pcImpl->GetBaseView()->GetSuppressUpdate())
	{
		HC_Control_Update_By_Key(pcImpl->GetBaseView()->GetViewKey(), "redraw everything");
		pcImpl->GetBaseView()->GetConstantFrameRateObject()->SetActivityType(GeneralActivity);

//		pcCanvas->GetIntRectangle(&rectangle);
// 		m_pHView->Notify(HSignalPaint, &rectangle);
// 		m_pHView->ResetIdleTime();

		if (false == pcImpl->GetBaseView()->GetFirstUpdate()) {
			pcImpl->GetBaseView()->ForceUpdate();
		}
		else {
			pcImpl->GetBaseView()->Update();
		}
	}
}

void H3DF::View::Update(Json::Object & cInObject, Window::UpdateType eInType, H3DF::Time dInTimeLimit) const
{
	Update(cInObject);
}

//== View 관련 함수 ==================================================================================

// 1. View 초기화, 전달받은 View Id를 이용해서 초기화 작업 실시하고 전달된 정보에 파일 정보가 있으면 File Open을 실시한다.
void H3DF::View::Destruct()
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if (nullptr != pcImpl->m_pcBaseView) {
		delete pcImpl->m_pcBaseView;
	}

	if (nullptr != pcImpl->m_pcWindow) {
		delete pcImpl->m_pcWindow;
	}
}

void H3DF::View::Resize(int x, int y)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->Resize(x, y);
}

//== Command 관련 함수 ===========================================================================

// 명령어 취소 함수, Select된 Object도 취소됨.
void H3DF::View::CancelCommands()
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->DeSelectAll();
}

void H3DF::View::CancelCommands() const
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->DeSelectAll();
}

// == Action Function ==============================================================================

// 2. Left Button 처리 함수
bool H3DF::View::LButtonDown(int nFlags, int x, int y)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->LButtonDown(nFlags, x, y);
}

bool H3DF::View::LButtonUp(int nFlags, int x, int y)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->LButtonUp(nFlags, x, y);
}

// 3. Middle Button 처리 함수
bool H3DF::View::MButtonDown(int nFlags, int x, int y)
{
	assert(m_pcCanvas);
	return true;
	//return pcCanvas->MButtonDown(nFlags, x, y);
}

bool H3DF::View::MButtonUp(int nFlags, int x, int y)
{
	assert(m_pcCanvas);
	return true;
	//return pcCanvas->MButtonUp(nFlags, x, y);
}

// 4. Right Button 처리 함수
bool H3DF::View::RButtonUp(int nFlags, int x, int y)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->RButtonUp(nFlags, x, y);
}

bool H3DF::View::RButtonDown(int nFlags, int x, int y)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->RButtonDown(nFlags, x, y);
}

// 5. Mouse Move 처리 함수
bool H3DF::View::MouseMove(int nFlags, int x, int y)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->MouseMove(nFlags, x, y);
}

// 6. Mouse Wheel 처리 함수
bool H3DF::View::MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->MouseWheel(nFlags, zDelta, x, y, nLeft, nTop);
}

bool H3DF::View::Char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->Char(nChar, nRepCnt, nFlags);
}

bool H3DF::View::KeyboardInput(Json::Object & input)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->KeyboardInput(input);
}

bool H3DF::View::ExecuteKeyboardSignal(int nAction, Json::Object& cInObject)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->KeyboardInput(cInObject);
}

//== Select 관련 함수 ========================================================================
void H3DF::View::SetSubentitySelectLevel()
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->SetSubentitySelectLevel();
}

void H3DF::View::SaveHsfFile(CString strFilePathName, Canvas * pcHoopsView)
{
/*
	HIOUtilityHsf cUtilityHsf;

	HC_KEY nModelKey = pcHoopsView->GetBaseView()->GetModelKey();

	HC_Open_Segment_By_Key(nModelKey);

	HOutputHandlerOptions cOptions;
	HStreamFileToolkit * mytool = new HStreamFileToolkit;
	cOptions.ExtendedData(mytool);

	int sflags = 0;
	sflags |= TK_Full_Resolution_Vertices;
	sflags |= TK_Full_Resolution_Normals;
	sflags |= TK_Full_Resolution_Parameters;

	mytool->SetWriteFlags(sflags);

	HFileOutputResult eResult = cUtilityHsf.FileOutputByKey(strFilePathName, nModelKey, &cOptions);

	HC_Close_Segment();

	delete mytool;
*/
}

void H3DF::View::LoadPointCloudFile(CString strFilePathName)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	SegmentKey cViewKey(pcImpl->GetBaseView()->GetViewKey());
	SegmentKeyPrivate::LocalOpen(cViewKey);
	HC_Set_Driver_Options("eye dome lighting = (on, strength=1.0)");
	SegmentKeyPrivate::LocalClose(cViewKey);

	HInputHandlerOptions cOptions;
	cOptions.m_tk = pcImpl->GetBaseView()->GetModel()->GetStreamFileTK();
	cOptions.m_pHBaseView = pcImpl->GetBaseView();
	
	//cOptions.m_pExtendedData = &cPointCloudOptions;

	//m_point_cloud_options = (HPointCloudOptions *)options->m_pExtendedData;

	SegmentKey cModelKey(pcImpl->GetBaseView()->GetModelKey());
	SegmentKey cPointCloudSegment = cModelKey.Subsegment(L"_3dmi_point_cloud");

	HIOUtilityPointCloud cPointCloud;
	
	// 라이브러리를 사용해야 하므로 미리 cPointCloudSegment를 Open하도록 한다.
	SegmentKeyPrivate::LocalOpen(cPointCloudSegment);
	cPointCloud.FileInputByKey(Utility::ToChar(strFilePathName), cPointCloudSegment.KeyValue(), &cOptions);

	HC_UnSet_Marker_Symbol();
	HC_Set_Marker_Size(0.2);

	SegmentKeyPrivate::LocalOpen(cPointCloudSegment);

	// Point Clouse Segment의 하부를 검색해서 색상을 변경함.
	// Library에서 나오는 색상은 기본적으로 Black으로 나옴.
	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetVertexColor(RGBAColor(0.75, 0.75, 0.75)); // Gray Color 설정
	Utility::ChangeSubSegmentColor(cPointCloudSegment, cMaterialMapping, true);
}