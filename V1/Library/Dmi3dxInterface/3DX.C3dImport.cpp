#include "stdafx.h"

#include "3DX.C3dImport.h"

#include "Dmi3dx.h"

#include "Surface/DmiSurface.h"
#include "Surface/DmiCylinderSurface.h"
#include "Surface/DmiConeSurface.h"
#include "Surface/DmiSphereSurface.h"
#include "Surface/DmiTorusSurface.h"
#include "Surface/DmiRuledSurface.h"

#include "Curve/RtCurve.h"

#include "TopoItem/DmiOrientedEdge.h"

#include <tool_enabler.h>

#include <model.h>
#include <solid.h>
#include <action_solid.h>
#include "../../C3D/Kernel/Include/instance.h"
#include <space_instance.h>
#include <plane_instance.h>
#include <assisting_item.h>

#include <attr_common_attribute.h>

// Surface
#include <surf_plane.h>
#include <surf_cylinder_surface.h>
#include <surf_spline_surface.h>
#include <surf_curve_bounded_surface.h>
#include <surf_extrusion_surface.h>
#include <surf_elevation_surface.h>

// 3D curve
#include <curve3d.h>
#include <cur_line_segment3d.h>
#include <cur_line3d.h>
#include <cur_arc3d.h>
#include <cur_nurbs3d.h>
#include <cur_surface_intersection.h>
#include <cur_surface_curve.h>
#include <cur_contour3d.h>
#include <cur_polyline3d.h>

// 2D curve
#include <cur_line_segment.h>
#include <cur_nurbs.h>
#include <cur_reparam_curve.h>
#include <cur_projection_curve.h>

#include <mesh.h>
#include <mesh_primitive.h>

#include <conv_model_exchange.h>

#include <utility>

#include "C3dTracer.h"
#include "LogManager.h"

#include <thread>
#include <mutex>
#include <concurrent_vector.h>

//----- Convert용 Object -----
#include "Convert/ConvFaceShell.h"
#include "Convert/ConvFace.h"
#include "Convert/ConvLoop.h"
#include "Convert/ConvCoEdge.h"
#include "Convert/ConvEdge.h"
#include "Convert/ConvVertex.h"
#include "Convert/ConvCurve.h"
#include "Convert/ConvSurface.h"

#include "3DXSignal.Interface.h"
//#include "Dmi3dxInterface.h"
#include <Common_Define.h>

using namespace c3d;
using namespace std::chrono;

#ifdef _DEBUG
	#define USED_LOG_MANAGER
#endif

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RETURN(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return false; } }

#ifndef INITIALIZE_A3D_API
#include <A3DSDKIncludes.h>
#endif // !INITIALIZE_A3D_API

USING_3DX_NAMESPACE

C3dImport::C3dImport(_3DXSignal::Interface * pc3dxInterface) :
	ImportBase(pc3dxInterface)
{
	m_pcModel = nullptr;

	const char key[] = "DATAFACE.2022011820220418.[WIN][cnv][mdl][slv][vsn][bsh]";
	const char signature[] = "C5VDnJM+TOB5l8G/MIUWYuazUUHXhMe3ZGySCpSQlX6DQ8jsNgDNekBVJHjBTUa0quUheau/jBbwUftVyOpLdw==";
	EnableMathModules(key, (int) strlen(key), signature, (int) strlen(signature));
}

C3dImport::~C3dImport()
{
}

// == 명령어 처리 관련 함수 ============================================================================

// 1. 전달 받은 명령어 처리
bool C3dImport::ExecuteCommand(Json::Object * pcObject)
{
	CString strCommand = pcObject->GetString("Command");

	if(L"FileImport" == strCommand) {

		m_nId = pcObject->GetDwordPtr("Id");

		CString strFilePathName = pcObject->GetString("Value");

		DWORD_PTR nPtr = (DWORD_PTR) pcObject->GetDwordPtr("Model");

		m_pcModel = (MbModel *) (DWORD_PTR) pcObject->GetDwordPtr("Model");

		return FileImport(strFilePathName);

		// Thread로 File을 C3dImport하는 함수는 사용하지 않는다.
		//m_pcThreadFileImport = new std::thread(ThreadFileImport, this, strFilePathName);
		//m_cThreadFileImport.join();
		//return true;

		// 		std::thread cThreadFileImport(ThreadFileImport, this, strFilePathName);
		// 		cThreadFileImport.join();


				// #temp
		//		return FileImport(strFilePathName);
	}

	return false;
}

// 2. 명령어를 상위 Command Analyzer로 전달전달 받은 명령어 처리
bool C3dImport::SendExecuteCommand(Json::Object & cObject)
{
	if(nullptr == m_pc3dxInterface) {
		return false;
	}

	return m_pc3dxInterface->SendExecuteCommand(cObject);
}

bool C3dImport::FileImport(CString strFilePathName)
{
	system_clock::time_point cTime1 = system_clock::now();
	long long tick = cTime1.time_since_epoch().count();

	m_strCadFileName = strFilePathName.Right(strFilePathName.GetLength() - strFilePathName.ReverseFind('\\') - 1);

	// ===== C3dImport 옵션을 설정 =====
	A3DRWParamsLoadData cParamsLoadData;
	A3D_INITIALIZE_DATA(A3DRWParamsLoadData, cParamsLoadData);

	SetDefaultParamsLoadData(cParamsLoadData);

	// #option : ReadGeomTessMode
	cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomOnly;
	//cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomAndTess;
	//cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadTessOnly;

	// Report용 Callback 함수 설정
	SetCallbacksReport();

	A3DUTF8Char * pchUtf8Path = nullptr;
	if(false == CStringToUtf8(strFilePathName, pchUtf8Path)) {
		return false;
	}

	A3DAsmModelFile * pcAsmModelFile = nullptr;
	A3DStatus eStatus = A3DAsmModelFileLoadFromFile(pchUtf8Path, &cParamsLoadData, &pcAsmModelFile);
	delete[] pchUtf8Path;

	if(A3D_SUCCESS != eStatus && A3D_LOAD_MULTI_MODELS_CADFILE != eStatus && A3D_LOAD_MISSING_COMPONENTS != eStatus)
	{
		// 초기화를 해주지 않으면 다음번에 계속 오류가 난다 
		CString strErrorMessage;
		Reset(strErrorMessage);
		SetLastErrorMessage(L"Model File Load Error", eStatus);
		return false;
	}

	/*
		Json::Object cObject;
		cObject.SetDwordPtr("Id", m_nId);
		cObject.SetString("Type", MSG_HPS_TYPE);
		cObject.SetDwordPtr("CommandId", RtHps::CommandId::CreateInsertModelThread); // Model을 삽입할 수 있는 Thread 생성
		SendExcuteCommand(cObject);
	*/

	//Math::SetMultithreadedMode(mtm_Standard);

	bool bStatus = ConvertAsmModelFile(pcAsmModelFile);

	A3DAsmModelFileDelete(pcAsmModelFile);

	system_clock::time_point cTime2 = system_clock::now();
	/*
		cObject.SetDwordPtr("CommandId", RtHps::CommandId::DesteructInsertModelThread); // Model을 삽입할 수 있는 Thread 정지
		SendExcuteCommand(cObject);
	*/
	// Thread에서 돌아가기 때문에 언제 종료될지 모르는 상태임.
	// 그래서 Update를 여기서 처리한다.

	// #Require_convert_new_version
/*
	Json::Object cObject;
	cObject.SetDwordPtr("Id", m_nId);
	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetDwordPtr(L"Model", (DWORD_PTR) m_pcModel);
	cObject.SetString("Command", L"ImportDataUpdate");
	SendExecuteCommand(cObject);

	system_clock::time_point cTime3 = system_clock::now();

	Json::Object cResultMsgObject;
	cResultMsgObject.SetDwordPtr("Id", m_nId);
	cResultMsgObject.SetString("Type", MSG_APP_TYPE);
	cResultMsgObject.SetInteger("MSG", MSG_FILE_IMPORT_INFORMATION);
	cResultMsgObject.SetLongLong("TimeTick1", cTime1.time_since_epoch().count());
	cResultMsgObject.SetLongLong("TimeTick2", cTime2.time_since_epoch().count());
	cResultMsgObject.SetLongLong("TimeTick3", cTime3.time_since_epoch().count());

	Json::Array & cHpsError = cObject.GetArray("HpsError");

	Json::Array & cHpsErrorArray = cResultMsgObject.GenArray("HpsError");

	for(int nIndex = 0; nIndex < cHpsError.GetSize(); nIndex++) {
		CString strErrMsg = cHpsError.GetAt(nIndex)->AsString();
		cHpsErrorArray.AddString(strErrMsg);
	}

	SendExecuteCommand(cResultMsgObject);
*/

	return bStatus;
}

bool C3dImport::IncrementalFileImport(CString strFilePathName)
{
	m_strCadFileName = strFilePathName.Right(strFilePathName.GetLength() - strFilePathName.ReverseFind('\\') - 1);

	CString strDllPath = Dmi3dx::GetExecuteDirectory() + L"3DX";

	A3DSDKHOOPSExchangeLoader cHoopsExchangeLoader(strDllPath);
	if(A3D_SUCCESS != cHoopsExchangeLoader.m_eSDKStatus) {
		return false;
	}

	// Report용 Callback 함수 설정
	SetCallbacksReport();

	A3DUTF8Char * pchUtf8Path = nullptr;
	if(false == CStringToUtf8(strFilePathName, pchUtf8Path)) {
		return false;
	}

	A3DImport cImport(pchUtf8Path);
	cImport.m_sLoadData.m_sIncremental.m_bLoadStructureOnly = true;

	A3DStatus nStatus = cHoopsExchangeLoader.Import(cImport);
	if(nStatus != A3D_SUCCESS && nStatus != A3D_LOAD_MISSING_COMPONENTS) {
		return false;
	}

	// ===== C3dImport 옵션을 설정 =====
	A3DRWParamsLoadData cParamsLoadData;
	A3D_INITIALIZE_DATA(A3DRWParamsLoadData, cParamsLoadData);

	SetDefaultParamsLoadData(cParamsLoadData);

	cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomOnly;

	A3DAsmModelFile * pcAsmModelFile = nullptr;
	A3DStatus eStatus = A3DAsmModelFileLoadFromFile(pchUtf8Path, &cParamsLoadData, &pcAsmModelFile);
	delete[] pchUtf8Path;

	if(A3D_SUCCESS != eStatus && A3D_LOAD_MULTI_MODELS_CADFILE != eStatus && A3D_LOAD_MISSING_COMPONENTS != eStatus)
	{
		// 초기화를 해주지 않으면 다음번에 계속 오류가 난다 
		CString strErrorMessage;
		Reset(strErrorMessage);
		SetLastErrorMessage(L"Model File Load Error", eStatus);
		return false;
	}

	bool bStatus = ConvertAsmModelFile(pcAsmModelFile);;

	A3DAsmModelFileDelete(pcAsmModelFile);

	return bStatus;
}

// == 3DX 설정 관련 함수 ==============================================================================
bool C3dImport::SetDefaultParamsLoadData(A3DRWParamsLoadData & cParamsLoadData)
{
	// General 변수
	cParamsLoadData.m_sGeneral.m_bReadSolids = true;
	cParamsLoadData.m_sGeneral.m_bReadSurfaces = true;
	cParamsLoadData.m_sGeneral.m_bReadWireframes = true;
	cParamsLoadData.m_sGeneral.m_bReadPmis = true;
	cParamsLoadData.m_sGeneral.m_bReadAttributes = true;
	cParamsLoadData.m_sGeneral.m_bReadHiddenObjects = false;
	// #option : 축이나 Axis를 읽어드리는 옵션
	cParamsLoadData.m_sGeneral.m_bReadConstructionAndReferences = false;
	cParamsLoadData.m_sGeneral.m_bReadActiveFilter = true;

	cParamsLoadData.m_sGeneral.m_eReadingMode2D3D = kA3DRead_3D;
	cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadTessOnly;
	cParamsLoadData.m_sGeneral.m_eDefaultUnit = kA3DUnitUnknown;

	cParamsLoadData.m_sGeneral.m_bReadFeature = false; // Allows reading of model feature tree. version 10.2

	// PMI 변수
	//cParamsLoadData.m_sPmi.m_bAlwaysSubstituteFont = true;
// 	cParamsLoadData.m_sPmi.m_pcSubstitutionFont = new A3DUTF8Char[sizeof("Myriad CAD")];
// 	printf(cParamsLoadData.m_sPmi.m_pcSubstitutionFont, "%s", "Myriad CAD");

	// Tessellation 변수
	//cParamsLoadData.m_sTessellation.m_eTessellationLevelOfDetail = kA3DTessLODMedium;
	//cParamsLoadData.m_sTessellation.m_bDoNotComputeNormalsInAccurateTessellation = false;	// Compute Normal

	// Assembly 변수
	cParamsLoadData.m_sAssembly.m_bUseRootDirectory = true;
	cParamsLoadData.m_sAssembly.m_bRootDirRecursive = true;

	// Multiple models 변수
	// cParamsLoadData.m_sMultiEntries

	// 특정 CAD Format 관련 변수
	// cParamsLoadData.m_sSpecifics

	// 특정 CAD Format 관련 변수
	// cParamsLoadData.m_sIncremental

	return true;
}

//== 1. C3D 변환 관련 함수 ============================================================================
bool C3dImport::ConvertAsmModelFile(A3DAsmModelFile * pcAsmModelFile)
{
	// #Import_Log : ExcuteFunction.log
#ifdef USED_LOG_MANAGER
	CreateLog(2, L"d:\\Temp\\ExcuteFunction.log");
	m_cA3dTracer.CreateLog(L"D:\\Temp\\A3dXInfo.log");
	//CreateLog(1, L"d:\\Temp\\AssyStruct.log");
#endif

	Log(2, L"ConvertAsmModelFile: %s", LogHexStr((DWORD_PTR) pcAsmModelFile));

	LogIncreaseTabIndex(2);

	// 	UINT nProcessCount = std::thread::hardware_concurrency();
	// 	m_pcThreadPoolpool = new ThreadPool(nProcessCount);

		// Model File Data를 가져오는 부분
	A3DAsmModelFileData cModelFileData;
	A3D_INITIALIZE_DATA(A3DAsmModelFileData, cModelFileData);
	A3DStatus nStatus = A3DAsmModelFileGet(pcAsmModelFile, &cModelFileData);
	if(A3D_SUCCESS != nStatus) {
		SetLastErrorMessage(L"Asm Model File Get Error", nStatus);
		return false;
	}

	// ===== Model Scale 처리 부분 =====
	// Matrix 안에 있는 Scale 값을 사용해야 한다 ( info.dModelScale은 더 이상 사용하지 않는다) [12/14/2016 이인호]
	// Model Scale을 사용해야 한다 ( POccurrence Scale을 맞지 않는다 ) [6/26/2017 이인호]
	// cImportInfo.dModelScale = cModelFileData.m_bUnitFromCAD ? cModelFileData.m_dUnit : 1.0;
	// cImportInfo.eModellerType = cModelFileData.m_eModellerType;
	m_bGlobalDataFlag = false;
	A3DGlobal * pcGlobal = nullptr;

	nStatus = A3DGlobalGetPointer(&pcGlobal);
	if(A3D_SUCCESS == nStatus) {
		A3D_INITIALIZE_DATA(A3DGlobalData, m_cGlobalData);
		nStatus = A3DGlobalGet(pcGlobal, &m_cGlobalData);
		if(A3D_SUCCESS == nStatus) {
			m_bGlobalDataFlag = true;
		}
	}

	// 최초의 Attribute 생성
	A3DMiscCascadedAttributes * pcAttrs = nullptr;
	A3DMiscCascadedAttributesData cAttrsData;
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, cAttrsData);

	A3DMiscCascadedAttributesCreate(&pcAttrs);
	A3DMiscCascadedAttributesGet(pcAttrs, &cAttrsData);

	ConvertModelFileData(cModelFileData, pcAttrs);

	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
	A3DMiscCascadedAttributesDelete(pcAttrs);

	A3DAsmModelFileGet(nullptr, &cModelFileData);
	A3DGlobalGet(nullptr, &m_cGlobalData);

	LogDecreaseTabIndex(2);

	return true;
}

// == 2. Product Occurrences 관련 함수 ===============================================================

// 2. 전체 Product Occurrence 처리
bool C3dImport::ConvertModelFileData(A3DAsmModelFileData & cModelFileData, A3DMiscCascadedAttributes * pcParentAttr)
{
	Log(2, L"ConvertModelFileData");

	LogIncreaseTabIndex(2);

	for(A3DUns32 i = 0; i < cModelFileData.m_uiPOccurrencesSize; ++i) {
		AssemblySPtr pcAssy;
		ConvertProductOccurrence(cModelFileData.m_ppPOccurrences[i], pcParentAttr, pcAssy);
	}

	LogDecreaseTabIndex(2);

	return true;
}

// 2-1. 개별 Product Occurrence 처리
bool C3dImport::ConvertProductOccurrence(A3DAsmProductOccurrence * pcPO, A3DMiscCascadedAttributes * pcParentAttr, AssemblySPtr & pcParentAssy)
{
	if(nullptr == pcPO) {
		return false;
	}

	// Product Occurrence Data 추출
	A3DAsmProductOccurrenceData cPOData;
	A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cPOData);
	A3DStatus eStatus = A3DAsmProductOccurrenceGet(pcPO, &cPOData);
	if(A3D_SUCCESS != eStatus) {
		SetLastErrorMessage(L"Asm Product Occurrence Get Error", eStatus);
		return false;
	}

	// subAssembly를 로드할 때는 무시해야한다
	if(cPOData.m_uiProductFlags == A3D_PRODUCT_FLAG_CONFIG || cPOData.m_uiProductFlags & A3D_PRODUCT_FLAG_SUPPRESSED)
	{
		A3DAsmProductOccurrenceGet(nullptr, &cPOData);
		return true;
	}

	// Attribute 생성
	// Parent에서 받은(계단식으로) Attribute를 이용해서, Attribute를 생성
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcPO, pcParentAttr, &pcAttrs, &cAttrsData);

	CString strPoName;
	GetName(pcPO, strPoName);

	// 위치 정보 Matrix를 가져온다.
	MbMatrix3D cMatrix;
	GetProductOccurrenceTransformMatrix(&cPOData, cMatrix);

	AssemblySPtr pcAssy(new MbAssembly());
	SetItemName(pcAssy, strPoName);

	// Parent Assy가 null인 경우 최상단 Node라고 생각한다.
	if(true == pcParentAssy.is_null()) {
		// 생성된 Assy를 Model에 추가시켜준다.
		MbItem * pcItem = m_pcModel->AddItem(*pcAssy);
	}
	else {
		pcParentAssy->AddInstance(*pcAssy, cMatrix);
	}

	if(true == strPoName.IsEmpty()) {
		strPoName.Format(L"Product %d", m_nProductId);
	}

	m_nProductId++;

	Log(2, L"ConvertProductOccurrence: %s", strPoName);

	LogIncreaseTabIndex(2);

	CString strOriginalFilePath;
	CString strFilePath;
	if(nullptr != cPOData.m_pPrototype) {
		GetProductOccurrenceFilePath(cPOData.m_pPrototype, strFilePath, strOriginalFilePath);
	}
	else {
		GetProductOccurrenceFilePath(pcPO, strFilePath, strOriginalFilePath);
	}

	// 처리 필요
	// ParseAttributes(pcPO);

	// Layer Filter를 가져오는 함수
	GetProductOccurrenceLayerFilter(cPOData);

	//GetProductOccurrentPMI(cPOData, pcAttrs);

	GetProductOccurrencePart(&cPOData, pcAttrs, pcAssy);

	GetProductOccurrenceChildren(&cPOData, pcAttrs, pcAssy);

	A3DAsmProductOccurrenceGet(nullptr, &cPOData);

	LogDecreaseTabIndex(2);

	return true;
}

// 2-1-1. Assembly Product Occurence의 파일 이름 경로를 가져옴.
// Returns the original file name stored in the CAD file
bool C3dImport::GetProductOccurrenceFilePath(A3DAsmProductOccurrence * pcPO, CString & strFilePath, CString & strOriginalFilePath)
{
	A3DUTF8Char * pcOriginalFilePath = nullptr;
	A3DStatus ret = A3DAsmProductOccurrenceGetOriginalFilePathName(pcPO, &pcOriginalFilePath);
	if(ret == A3D_SUCCESS)
	{
		if(nullptr != pcOriginalFilePath) {
			Utf8ToCString(pcOriginalFilePath, strOriginalFilePath);
			strOriginalFilePath.Replace(_T("/"), _T("\\"));	// 마지막에 "/" 이 붙는다 -_-;
		}
	}

	A3DUTF8Char * pcUtf8FilePath = nullptr;
	A3DStatus eStatus = A3DAsmProductOccurrenceGetFilePathName(pcPO, &pcUtf8FilePath);
	if(A3D_SUCCESS == eStatus) {
		if(nullptr != pcUtf8FilePath) {
			Utf8ToCString(pcUtf8FilePath, strFilePath);
			strFilePath.Replace(_T("/"), _T("\\"));	// 마지막에 "/" 이 붙는다 -_-;
		}
	}

	return true;
}

// 2-1-2. Layer Filter를 가져오는 함수
bool C3dImport::GetProductOccurrenceLayerFilter(const A3DAsmProductOccurrenceData & cProductOccurrenceData)
{
	if(0 == cProductOccurrenceData.m_uiDisplayFilterSize > 0) {
		return true;
	}

	for(A3DUns32 nIndex = 0; nIndex < cProductOccurrenceData.m_uiDisplayFilterSize; nIndex++)
	{
		A3DAsmFilter * pcFilter = cProductOccurrenceData.m_ppDisplayFilters[nIndex];

		// Type 확인 
		A3DEEntityType eType;
		A3DEntityGetType(pcFilter, &eType);
		ASSERT(kA3DTypeAsmFilter == eType);

		CString strFilterName;
		GetName(pcFilter, strFilterName);

		//ParseAttributes(pFilter);

		A3DAsmFilterData cFilterData;
		A3D_INITIALIZE_DATA(A3DAsmFilterData, cFilterData);
		A3DAsmFilterGet(pcFilter, &cFilterData);

		/*
				// 데이터가 있는지 확인한다
				ASSERT(cFilterData.m_sEntityFilterItem.m_uiSize == 0);

				//if ( cFilterData.m_bIsActive)
				//{
				A3D_LAYER_FILTER * pNewLayerFilter = new A3D_LAYER_FILTER;

				pNewLayerFilter->bIsActive = cFilterData.m_bIsActive;
				pNewLayerFilter->bIsInclusive = cFilterData.m_sLayerFilterItem.m_bIsInclusive;
				ASSERT(pNewLayerFilter->bIsInclusive);
				pNewLayerFilter->nSize = cFilterData.m_sLayerFilterItem.m_uiSize;

				if(pNewLayerFilter->nSize > 0)
				{
					pNewLayerFilter->pLayerIndexes = new UINT[pNewLayerFilter->nSize];
					ZeroMemory(pNewLayerFilter->pLayerIndexes, sizeof(UINT) * pNewLayerFilter->nSize);

					for(UINT j = 0; j < pNewLayerFilter->nSize; j++)
					{
						UINT nLayer = cFilterData.m_sLayerFilterItem.m_puiLayerIndexes[j];
						pNewLayerFilter->pLayerIndexes[j] = nLayer;

						//UINT nLayerFlags = 0;	// show layer
						//if ( !info.layerMap.Lookup( nLayer, nLayerFlags))	// this layer is not registered
						//	info.layerMap.SetAt( nLayer, nLayerFlags);		// 모든 layer를 모아둔다
					}
				}

				// LayerFilter Name
				pNewLayerFilter->strName = strFilterName;
				info.layerFilterArray.Add(pNewLayerFilter);
		*/

		A3DAsmFilterGet(nullptr, &cFilterData);
	}

	return true;
}

// 2-1-3. Assembly Product Occurence의 위치를 가져오는 함수 / ProductOccurrenceGetLocation
bool C3dImport::GetProductOccurrenceTransformMatrix(A3DAsmProductOccurrenceData const * pcPoData, MbMatrix3D & cTransMatrix)
{
	if(nullptr == pcPoData) {
		return false;
	}

	A3DMiscTransformation * pcLocation = nullptr;

	bool bFlag = false;

	if(nullptr != pcPoData->m_pLocation)
	{
		pcLocation = pcPoData->m_pLocation;
		bFlag = true;
	}
	else if(nullptr != pcPoData->m_pPrototype)
	{
		A3DAsmProductOccurrenceData sPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sPrototypeData);

		if(A3D_SUCCESS == A3DAsmProductOccurrenceGet(pcPoData->m_pPrototype, &sPrototypeData))
		{
			A3DMiscCartesianTransformation * pPrototypeLocation = nullptr;
			if(true == GetProductOccurrenceTransformMatrix(&sPrototypeData, cTransMatrix)) {
				bFlag = true;
			}
		}

		A3DAsmProductOccurrenceGet(nullptr, &sPrototypeData);

		return bFlag;
	}
	else
	{
		A3DAsmProductOccurrence * pcExtPO = nullptr;
		if(true == GetProductOccurrenceExternalData(pcPoData, pcExtPO))
		{
			A3DAsmProductOccurrenceData sExternalDataData;
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sExternalDataData);

			if(A3D_SUCCESS == A3DAsmProductOccurrenceGet(pcExtPO, &sExternalDataData)) {
				if(nullptr != sExternalDataData.m_pLocation) {
					pcLocation = sExternalDataData.m_pLocation;
					bFlag = true;
				}
			}

			A3DAsmProductOccurrenceGet(nullptr, &sExternalDataData);
		}
	}

	bool bStatus = bFlag;
	if(true == bFlag) {
		bStatus = GetMatrix(pcLocation, cTransMatrix);
	}

	Log(2, L"GetProductOccurrenceTransformMatrix: %s", LogBoolStr(bStatus));

	return bStatus;
}

// 2-1-5. Product의 PMI 처리
bool C3dImport::GetProductOccurrentPMI(const A3DAsmProductOccurrenceData & sPOccData, A3DMiscCascadedAttributes * pcAttrs)
{
	Log(2, L"GetProductOccurrentPMI");

	LogIncreaseTabIndex(2);

	//assert(m_arcPmiEntities.size() == 0);

	if(sPOccData.m_uiViewsSize == 0 && sPOccData.m_uiAnnotationsSize == 0 && sPOccData.m_pPrototype)
	{
		A3DAsmProductOccurrenceData sData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sData);
		if(A3DAsmProductOccurrenceGet(sPOccData.m_pPrototype, &sData) != A3D_SUCCESS) {
			LogDecreaseTabIndex(2);
			return false;
		}

		GetProductOccurrentPMI(sData, pcAttrs);

		A3DAsmProductOccurrenceGet(nullptr, &sData);
	}


	for(UINT i = 0; i < sPOccData.m_uiAnnotationsSize; ++i) {
		ConvertAnnotationEntity(sPOccData.m_ppAnnotations[i], pcAttrs);
	}

	/*
	#ifdef _DEBUG
		for(A3DUns32 j = 0; j < sPOccData.m_uiViewsSize; j++)
		{
			A3DMkpViewData viewData;
			A3D_INITIALIZE_DATA(A3DMkpViewData, viewData);

			A3DMkpViewGet(sPOccData.m_ppViews[j], &viewData);
			int ab = 0;

			A3DMkpViewGet(nullptr, &viewData);
		}
	#endif
	*/

	LogDecreaseTabIndex(2);

	return true;
}

// 2-2. Child ProductOccurrenceData 처리 함수 (Assembly 처리)
bool C3dImport::GetProductOccurrenceChildren(const A3DAsmProductOccurrenceData * pcPoData, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy)
{
	Log(2, L"GetProductOccurrenceChildren");

	LogIncreaseTabIndex(2);

	if(nullptr == pcPoData) {
		ASSERT(false);
		return false;
	}

	bool bAddedOccurrencesFlag = false;

	if(0 < pcPoData->m_uiPOccurrencesSize) {
		for(unsigned int nIndex = 0; nIndex < pcPoData->m_uiPOccurrencesSize; nIndex++) {
			ConvertProductOccurrence(pcPoData->m_ppPOccurrences[nIndex], pcParentAttr, pcParentAssy);
		}

		bAddedOccurrencesFlag = true;
	}
	/*
		else
		{
			HC_KEY added_prototype_key = INVALID_KEY;
			added_occurrences = PrototypeGetOccurrencesRecursive(pcPoData->m_pPrototype, info, pcAttribute, added_prototype_key);
		}
	*/
	A3DAsmProductOccurrence * pcExtPO = nullptr;
	A3DInt32 nStatus = GetProductOccurrenceExternalData(pcPoData, pcExtPO);
	if(pcExtPO != nullptr)
	{
		A3DAsmProductOccurrenceData sExtData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sExtData);
		nStatus = A3DAsmProductOccurrenceGet(pcExtPO, &sExtData);

		if(true == bAddedOccurrencesFlag) {
			ConvertProductOccurrence(pcExtPO, pcParentAttr, pcParentAssy);
		}
		else {
			GetProductOccurrenceChildren(&sExtData, pcParentAttr, pcParentAssy);
		}

		nStatus = A3DAsmProductOccurrenceGet(nullptr, &sExtData);

		bAddedOccurrencesFlag = true;
	}

	LogDecreaseTabIndex(2);

	return bAddedOccurrencesFlag;
}

// 2-2-1. Product Occurrence 확장데이터를 가져옴.
bool C3dImport::GetProductOccurrenceExternalData(A3DAsmProductOccurrenceData const * pcPoData, A3DAsmProductOccurrence *& pcExternalData)
{
	if(nullptr == pcPoData) {
		return false;
	}

	Log(2, L"GetProductOccurrenceExternalData");

	LogIncreaseTabIndex(2);

	if(pcPoData->m_pExternalData == nullptr && pcPoData->m_pPrototype != nullptr)
	{
		A3DAsmProductOccurrenceData sProductPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sProductPrototypeData);

		A3DStatus eStatus = A3DAsmProductOccurrenceGet(pcPoData->m_pPrototype, &sProductPrototypeData);

		bool bFlag = GetProductOccurrenceExternalData(&sProductPrototypeData, pcExternalData);

		A3DAsmProductOccurrenceGet(nullptr, &sProductPrototypeData);

		LogDecreaseTabIndex(2);

		return bFlag;
	}
	else {
		pcExternalData = pcPoData->m_pExternalData;
	}

	LogDecreaseTabIndex(2);

	return true;
}

//== 3. Part 관련 함수 ===============================================================================

// 3-1. 현재 product의 part를 가져오는 함수 --
bool C3dImport::GetProductOccurrencePart(const A3DAsmProductOccurrenceData * pcPoData,
	A3DMiscCascadedAttributes * pcParentAttr, AssemblySPtr & pcParentAssy)
{
	if(nullptr == pcPoData) {
		return false;
	}

	Log(2, L"GetProductOccurrencePart");

	LogIncreaseTabIndex(2);

	A3DStatus eStatus = A3D_SUCCESS;

	// Product Occurrence Data에 Part 정보가 있는 경우
	if(nullptr != pcPoData->m_pPart) {
		bool bStatus = ConvertPart(pcPoData->m_pPart, pcParentAttr, pcParentAssy);
		LogDecreaseTabIndex(2);
		return bStatus;
	}

	if(true == GetPrototypePartRecursive(pcPoData->m_pPrototype, pcParentAttr, pcParentAssy, false)) {
		LogDecreaseTabIndex(2);
		return true;
	}

	bool bPartAdded = false;

	if(0 == pcPoData->m_uiPOccurrencesSize)
	{
		if(nullptr != pcPoData->m_pExternalData)
		{
			A3DAsmProductOccurrenceData cExternalProductOccurrenceData;
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cExternalProductOccurrenceData);

			eStatus = A3DAsmProductOccurrenceGet(pcPoData->m_pExternalData, &cExternalProductOccurrenceData);

			if(nullptr != cExternalProductOccurrenceData.m_pPart) {
				bPartAdded = ConvertPart(cExternalProductOccurrenceData.m_pPart, pcParentAttr, pcParentAssy);
			}
			else {
				bPartAdded = GetPrototypePartRecursive(pcPoData->m_pExternalData, pcParentAttr, pcParentAssy, false);
			}

			eStatus = A3DAsmProductOccurrenceGet(nullptr, &cExternalProductOccurrenceData);
		}
		else {
			bPartAdded = GetPrototypePartRecursive(pcPoData->m_pPrototype, pcParentAttr, pcParentAssy, true);
		}
	}

	LogDecreaseTabIndex(2);

	return bPartAdded;
}

// 3-2. Prototype Product Occurrence의 Part를 제귀적으로 가져옴.
bool C3dImport::GetPrototypePartRecursive(A3DAsmProductOccurrence * pcPrototypeProductOccurrence, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy, bool bUsedExternalData)
{
	if(nullptr == pcPrototypeProductOccurrence) {
		return false;
	}

	Log(2, L"GetPrototypePartRecursive");

	LogIncreaseTabIndex(2);

	//-----------------------------------------------------------------------------------
	CString strFilePath, strOriginalFilePath;
	GetProductOccurrenceFilePath(pcPrototypeProductOccurrence, strFilePath, strOriginalFilePath);

	// 	info.strAssemblyFilePath = strFilePath;	// prototype chain을 따라서 경로를 얻어야 한다 
	// 	info.strAssemblyOriginalFilePath = strOriginalFilePath;	// prototype chain을 따라서 경로를 얻어야 한다 

	bool bAddedPartFlag = false;

	A3DAsmProductOccurrenceData cProductPrototypeData;
	A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cProductPrototypeData);
	A3DInt32 nStatus = A3DAsmProductOccurrenceGet(pcPrototypeProductOccurrence, &cProductPrototypeData);

	if(false == bUsedExternalData && nullptr != cProductPrototypeData.m_pPart)
	{
		if(false == IsPartVisible(cProductPrototypeData.m_pPart, pcParentAttr)) {
			return true;
		}

		bAddedPartFlag = ConvertPart(cProductPrototypeData.m_pPart, pcParentAttr, pcParentAssy);

		/*
				// Hidden Part는 Reference를 사용할 수 없다
				if(true == IsPartVisible(cProductPrototypeData.m_pPart, pcParentAttr))
				{
					return true;
					// Include 처리를 위한 함수

					if(GetIncludePrototypePart(pcPrototypeProductOccurrence, bUsedExternalData, prototype_key))
					{
						A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData);

						added_prototype_key = prototype_key;	// this
						bIncluded = true;
						return true;
					}
				}


				HC_Open_Segment_By_Key(info.nModelKey);
				{
					HC_Open_Segment("model_include");

					// merge할 때 include에 같은 이름을 가진 segment가 생성되지 않도록
					DWORD dwFileKeyValue = (DWORD) info.nFileKey;	// Unique Value
					CString strFileKey;
					strFileKey.Format(_T("%d"), dwFileKeyValue);
					Safe_Open_Segment(strFileKey, _T("File_Include"), info);
					{
						prototype_key = ParsePart(cProductPrototypeData.m_pPart, info, pcAttribute);
					}
					HC_Close_Segment();

					HC_Close_Segment();
				}
				HC_Close_Segment();

				// 변경점
				if(prototype_key != INVALID_KEY)
				{
					// PMI와 링크되어 있는 경우 Include를 사용해선 안된다
					// SubAssembly를 로드할 때도 Reference를 만들지 않는다
					// Part가 숨겨진 경우 Reference로 만들지 않는다
					if(info.bLinkedWithPMI || IsSubAssemblyLoad(info) || info.bPartHidden)
					{
						//char path[MVO_BUFFER_SIZE];
						//char path1[MVO_BUFFER_SIZE];
						//HC_Show_Segment(prototype_key, path);

						HC_Move_By_Key(prototype_key, ".");

						//HC_Show_Segment(prototype_key, path1);

						SetPartUUID(prototype_key);
					}
					else
					{
						include_key = IncludeProtoTypePart(prototype_key);
						bIncluded = true;	// reference
					}

					added_part = true;
				}
		*/
	}
	else if(true == bUsedExternalData && nullptr != cProductPrototypeData.m_pExternalData)
	{
		GetProductOccurrenceFilePath(cProductPrototypeData.m_pExternalData, strFilePath, strOriginalFilePath);
		/*
				GetProductOccurrenceFilePathName(cProductPrototypeData.m_pExternalData, strFilePath);
				info.strAssemblyFilePath = strFilePath;	// prototype chain을 따라서 경로를 얻어야 한다
				info.strAssemblyOriginalFilePath = strOriginalFilePath;	// prototype chain을 따라서 경로를 얻어야 한다
		*/

		A3DAsmProductOccurrenceData cExternalDataProductOccurrenceData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cExternalDataProductOccurrenceData);
		nStatus = A3DAsmProductOccurrenceGet(cProductPrototypeData.m_pExternalData, &cExternalDataProductOccurrenceData);

		if(nullptr != cExternalDataProductOccurrenceData.m_pPart)
		{
			if(false == IsPartVisible(cExternalDataProductOccurrenceData.m_pPart, pcParentAttr))
			{
				return false;
			}

			bAddedPartFlag = ConvertPart(cExternalDataProductOccurrenceData.m_pPart, pcParentAttr, pcParentAssy);
			/*
						// Hidden Part는 Reference를 사용할 수 없다
						if(true == IsPartVisible(sData.m_pPart, pcAttribute))
						{
							if(IncludeProtoTypePart(pcPrototypeProductOccurrence, bUsedExternalData, prototype_key))
							{
								A3DAsmProductOccurrenceGet(nullptr, &sData);
								A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData);

								added_prototype_key = prototype_key;	// this
								bIncluded = true;
								return true;
							}
						}
						HC_Open_Segment_By_Key(info.nModelKey);
						{
							HC_Open_Segment("model_include");

							// merge할 때 include에 같은 이름을 가진 segment가 생성되지 않도록
							DWORD dwFileKeyValue = (DWORD) info.nFileKey;	// Unique Value
							CString strFileKey;
							strFileKey.Format(_T("%d"), dwFileKeyValue);
							Safe_Open_Segment(strFileKey, _T("File_Include"), info);
							{
								prototype_key = ParsePart(cProductOccurrenceData.m_pPart, info, pcParentAttr);
							}
							HC_Close_Segment();

							HC_Close_Segment();
						}
						HC_Close_Segment();

						// 변경점
						if(prototype_key != INVALID_KEY)
						{
							// PMI와 링크되어 있는 경우 Include를 사용해선 안된다
							// SubAssembly를 로드할 때도 Reference를 만들지 않는다
							// Part가 숨겨진 경우 Reference로 만들지 않는다
							if(info.bLinkedWithPMI || IsSubAssemblyLoad(info) || info.bPartHidden)
							{
								HC_Move_By_Key(prototype_key, ".");

								SetPartUUID(prototype_key);
							}
							else
							{
								include_key = IncludeProtoTypePart(prototype_key);
								bIncluded = true;	// reference
							}

							added_part = true;
						}*/
		}

		nStatus = A3DAsmProductOccurrenceGet(nullptr, &cExternalDataProductOccurrenceData);
	}
	else
	{
		bAddedPartFlag = GetPrototypePartRecursive(cProductPrototypeData.m_pPrototype, pcParentAttr,
			pcParentAssy, bUsedExternalData);
	}

	nStatus = A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData);

	/*
		if(added_part)
		{
			ASSERT(prototype_key != INVALID_KEY);

			if(bIncluded)
			{
				(bUsedExternalData ? m_cExternalPartKeyHash.InsertItem(pcPrototypeProductOccurrence, prototype_key) : m_cPartKeyHash.InsertItem(pcPrototypeProductOccurrence, prototype_key));
			}
			//else if (IsSubAssemblyLoad(info))	// reference가 모두 로드되므로 하나만 처리한다
			//{
				//(use_external_data ? m_cDeadExternalPartHash.InsertItem(prototype) : m_cDeadPartHash.InsertItem(prototype));
			//}

			added_prototype_key = prototype_key;
		}
		else
		{
			if(include_key != INVALID_KEY)
			{
				ASSERT(FALSE);
				HC_Delete_By_Key(include_key);
			}

			if(prototype_key != INVALID_KEY)
			{
				ASSERT(FALSE);
				HC_Delete_By_Key(prototype_key);
			}

			// 구조만 읽을 때는 DeadHash에 넣지 않는다
			if(!info.bLoadStructureOnly && !info.bPartHidden)
			{
				(bUsedExternalData ? m_cDeadExternalPartHash.InsertItem(pcPrototypeProductOccurrence) : m_cDeadPartHash.InsertItem(pcPrototypeProductOccurrence));
			}

			added_prototype_key = INVALID_KEY;
		}
	*/

	LogDecreaseTabIndex(2);

	return bAddedPartFlag;
}

// 3-2-1. Part Visibility 판단.
bool C3dImport::IsPartVisible(A3DAsmPartDefinition * pcPartDefinition, A3DMiscCascadedAttributes * pcParentAttr)
{
	Log(2, L"IsPartVisible");

	LogIncreaseTabIndex(2);

	A3DMiscCascadedAttributes * pcAttribute;
	A3DMiscCascadedAttributesData sAttrData;
	CreateAndPushCascadedAttributes(pcPartDefinition, pcParentAttr, &pcAttribute, &sAttrData);

	bool bVisible = (1 == sAttrData.m_bShow) ? true : false;

	A3DMiscCascadedAttributesGet(nullptr, &sAttrData);
	A3DMiscCascadedAttributesDelete(pcAttribute);

	LogDecreaseTabIndex(2);

	return bVisible;
}

// 3-3. Part 변환 --
bool C3dImport::ConvertPart(A3DAsmPartDefinition * pcPartDefinition, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy)
{
	CString strName;
	GetName(pcPartDefinition, strName);

	Log(2, L"ConvertPart: %s", strName);

	LogIncreaseTabIndex(2);

	A3DAsmPartDefinitionData cPartDefinitionData;
	A3D_INITIALIZE_DATA(A3DAsmPartDefinitionData, cPartDefinitionData);
	A3DStatus eStatus = A3DAsmPartDefinitionGet(pcPartDefinition, &cPartDefinitionData);
	if(A3D_SUCCESS != eStatus) {
		SetLastErrorMessage(L"A3DAsmPartDefinitionGet Error", eStatus);
		return false;
	}

	// 데이터가 없으면 종료한다 
	if(0 == cPartDefinitionData.m_uiRepItemsSize && 0 == cPartDefinitionData.m_uiAnnotationsSize) {
		A3DAsmPartDefinitionGet(nullptr, &cPartDefinitionData);
		return false;
	}

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcPartDefinition, pcParentAttr, &pcAttrs, &cAttrsData);

	if(A3D_FALSE == cAttrsData.m_bShow)
	{
		/*
				cImportInfo.bPartHidden = TRUE;

				if (!cImportInfo.bReadHiddenEntity)	// 데이터를 읽지 않는다  [11/11/2016 이인호]
				{
					A3DAsmPartDefinitionGet(nullptr, &cPartDefinitionData);
					return INVALID_KEY;
				}
		*/
	}

	BOOL bHasData = TRUE;

	// ConvertAttributes(pcPartDefinition);

	// RepItem 변환시작 ------------------------------------------------------------------------------------------------

	//cImportInfo.shellAttrNodes.RemoveAll();

	if(nullptr == cAttrsData.m_pCoordinateSystem) {
		// ASSERT(false);
	}

	/*
		// PMI 데이터를 먼저 처리한다 ( link )
		if ( cPartDefinitionData.m_uiViewsSize > 0)
		{
			HC_KEY viewKey = Safe_Open_Segment( _T("Views"), _T("Views"), cImportInfo);
			ASSERT(viewKey != INVALID_KEY);
			{
				for ( A3DUns32 i = 0; i < cPartDefinitionData.m_uiViewsSize; ++i)
				{
					//TraverseView(sData.m_ppViews[i], info);
					ConvertView(cPartDefinitionData.m_ppViews[i], cImportInfo, pcAttrs);
				}
			}
			HC_Close_Segment();
		}

		// PMI 데이터를 먼저 처리한다 ( link )
		// deal only with remaining markups not attached to a view
		//assert(m_arcPmiEntities.size() == 0);

		if ( cPartDefinitionData.m_uiAnnotationsSize > 0)
		{
			HC_KEY pmiKey = Safe_Open_Segment( _T("PMI"), _T("PMI"), cImportInfo);
			ASSERT(pmiKey != INVALID_KEY);
			{
				for ( A3DUns32 i = 0; i < cPartDefinitionData.m_uiAnnotationsSize; ++i)
				{
					HC_KEY tester = 0;
					int res = vhash_lookup_item(m_pcAnnotationHash, (void *)cPartDefinitionData.m_ppAnnotations[i], (void **)&tester);
					if (!(res == VHASH_STATUS_SUCCESS))
					{
						tester = 1;
						vhash_insert_item(m_pcAnnotationHash, (void *)cPartDefinitionData.m_ppAnnotations[i], (void *)tester);

						ConvertAnnotationEntity(cPartDefinitionData.m_ppAnnotations[i], cImportInfo, pcAttrs);

						cImportInfo.bLinkedWithPMI = TRUE;
					}
				}
			}
			HC_Close_Segment();

			CheckAndDeleteSegment(pmiKey);
		}

		//------------------------------------------------------------------

		m_lspcBrepFaces = new_vlist(malloc, free);
	*/

	// 위치 정보 Matrix를 가져온다.
	MbMatrix3D cMatrix;

	AssemblySPtr pcAssy(new MbAssembly());
	SetItemName(pcAssy, strName);
	pcParentAssy->AddItem(*pcAssy);

	for(A3DUns32 nIndex = 0; nIndex < cPartDefinitionData.m_uiRepItemsSize; ++nIndex) {
		ParseRiRepresentationItem(nIndex, cPartDefinitionData.m_ppRepItems[nIndex], pcAttrs, pcAssy);
	}

	A3DAsmPartDefinitionGet(nullptr, &cPartDefinitionData);

	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
	A3DMiscCascadedAttributesDelete(pcAttrs);

	LogDecreaseTabIndex(2);

	return true;
}

// 3-4. Representation Item 변환 --
bool C3dImport::ParseRiRepresentationItem(int nIndex, A3DRiRepresentationItem * pcRiRepItem, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy)
{
	Log(2, L"ParseRiRepresentationItem[%d]", nIndex);

	LogIncreaseTabIndex(2);

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcRiRepItem, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DGraphRgbColorData cGraphRgbColorData;
	A3D_INITIALIZE_DATA(A3DGraphRgbColorData, cGraphRgbColorData);
	A3DGlobalGetGraphRgbColorData(cAttrsData.m_sStyle.m_uiRgbColorIndex, &cGraphRgbColorData);

	// Hidden 처리할 것
	//info.bShow = cAttrsData.m_bShow;
	if(A3D_FALSE == cAttrsData.m_bShow)
	{
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return false;
	}

	// layerfilter가 없어도 layer값을 읽을 것
	//info.nLayer = (sAttrData.m_usLayer == 0xFFFF) ? -1 : sAttrData.m_usLayer;
	//ParseAttributes(pcRepresentationItem);

	A3DEEntityType eType;
	A3DStatus eStatus = A3DEntityGetType(pcRiRepItem, &eType);

	// Tessellation 상태를 변경할 때 
// 	if (eType == kA3DTypeRiBrepModel)
// 	{
// 		A3DRWParamsTessellationData tessData;
// 		A3D_INITIALIZE_DATA(A3DRWParamsTessellationData, tessData);
// 		tessData.m_eTessellationLevelOfDetail = kA3DTessLODExtraHigh;
// 
// 		//A3DStatus aa = A3DRiRepresentationItemComputeTessellation(pcRepresentationItem, &tessData);
// 		int a = 0;
// 	}

// 	A3DRiRepresentationItemData cRiRepItemData;
// 	A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, cRiRepItemData);
// 	eStatus = A3DRiRepresentationItemGet(pcRiRepItem, &cRiRepItemData);

	/*
		IwObject* pIwCreatedObject = nullptr;
		//BOOL bSolidModel = FALSE;

		info.bSolidModel = FALSE;
		info.pTopoBrepData = nullptr;

	#if defined(READ_FEATURE) && defined(_DEBUG)

		A3DUns32 nPersistID = 0;
		GetPersistentId(pcRepresentationItem, nPersistID);

	#endif
	*/

	switch(eType)
	{
		case kA3DTypeRiCurve:
			ConvertRiCurve(pcRiRepItem, pcAttrs, pcParentAssy);
			break;

		case kA3DTypeRiSet:
			ConvertRiSet(pcRiRepItem, pcAttrs, pcParentAssy);
			break;

		case kA3DTypeRiBrepModel:
			// #Chech_point : A3DCopyAndAdaptBrepModel
			ParseRiBrepModel(pcRiRepItem, pcAttrs, pcParentAssy, true);
			//ParseRiBrepModel(pcRiRepItem, pcAttrs, pcParentAssy, false);
			break;

		case kA3DTypeRiCoordinateSystem:
			ConvertRiCoordinateSystem(pcRiRepItem, pcAttrs, pcParentAssy);
			break;

		case kA3DTypeRiPolyBrepModel:
			ConvertRiPolyBrepModel(pcRiRepItem, pcAttrs, pcParentAssy);
			break;

			// 		case kA3DTypeRiPointSet:
			// 			//ConvertRiPointSet(pcRiRepItem, pcAttrs, cTransMatrix);
			// 			break;

		default:
		{
			CString strText;
			switch(eType)
			{
				case kA3DTypeRiPointSet:
					strText = L"RiPointSet";
					break;

				case kA3DTypeRiPlane:
					strText = L"RiPlane";
					break;

				default:
					strText.Format(L"%d", eType);
					break;
			}

			Log(2, L"ParseRiRepresentationItem Type: %s", strText);
		}
		break;

/*
		case kA3DTypeMiscCartesianTransformation:
		case kA3DTypeMiscEntityReference:
			ASSERT(FALSE);
			break;


		case kA3DTypeRiPointSet:
			ParseRiPointSet(pcRepresentationItem, info, sData.m_pCoordinateSystem, pcAttribute);
			break;
*/
	}

	if(eStatus == A3D_SUCCESS)		// Plane은 제외한다 
	{
/*
		if ( eType != kA3DTypeRiPlane)
		{
			//check to see if we need to regenerate tessellation
			if (sData.m_pTessBase == nullptr && (eType == kA3DTypeRiBrepModel || eType == kA3DTypeRiCurve))
			{
				A3DRWParamsTessellationData tessellation_data;
				A3D_INITIALIZE_DATA(A3DRWParamsTessellationData, tessellation_data);


				SetupTessellationData(info.pcImportOpt, tessellation_data);

				A3DRiRepresentationItemComputeTessellation(pcRepresentationItem, &tessellation_data);
				A3DRiRepresentationItemGet(pcRepresentationItem, &sData);
			}

			ParseTess(sData.m_pTessBase, pcRepresentationItem, info, pcAttribute, sData.m_pCoordinateSystem);

			// IwObject를 MvGeometry에 연결한다
			//if ( pIwCreatedObject)
			//{
				//info.pIwObjContainer->AddObject( pIwCreatedObject);

				// Shell인 경우 Solid인지 알려준다
			//}
		}
		else
		{
			// Plane은 Export때 다른 개체와 연결되지 않도록 변경한다
			OprPrcManipulator encorder;
			UINT nConnectID = (UINT)-1;
			encorder.SetConnectID( pcRepresentationItem, nConnectID);
		}
*/

		// A3DRiRepresentationItemGet(nullptr, &cRiRepItemData); // delete sData	
	}

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	LogDecreaseTabIndex(2);

	return true;
}

// 3-5. Ri Curve 변환 --
void C3dImport::ConvertRiCurve(A3DRiCurve * pcInputRiCurve, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy)
{
	CString strName;
	GetName(pcInputRiCurve, strName);

	Log(2, L"ConvertRiCurve: %s", strName);

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcInputRiCurve, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DRiCurveData cRiCurveData;
	A3D_INITIALIZE_DATA(A3DRiCurveData, cRiCurveData);
	A3DStatus nStatus = A3DRiCurveGet(pcInputRiCurve, &cRiCurveData);
	if(A3D_SUCCESS != nStatus) {
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return;
	}

	if(nullptr == cRiCurveData.m_pBody) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return;
	}

	A3DTopoSingleWireBodyData cSingleWireBodyData;
	A3D_INITIALIZE_DATA(A3DTopoSingleWireBodyData, cSingleWireBodyData);
	nStatus = A3DTopoSingleWireBodyGet(cRiCurveData.m_pBody, &cSingleWireBodyData);
	if(A3D_SUCCESS != nStatus) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return;
	}

	if(nullptr == cSingleWireBodyData.m_pWireEdge) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DTopoSingleWireBodyGet(nullptr, &cSingleWireBodyData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return;
	}

	// Scale을 구하기 위해서 Context Data에서 값을 가져온다.
	// 기본적으로 RiCurve에는 스케일이 들어있지 않음.
	ConvertTopoContextScale(cRiCurveData.m_pBody, m_dContextScale);

	A3DTopoWireEdgeData cWireEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoWireEdgeData, cWireEdgeData);
	nStatus = A3DTopoWireEdgeGet(cSingleWireBodyData.m_pWireEdge, &cWireEdgeData);
	if(A3D_SUCCESS != nStatus) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DTopoSingleWireBodyGet(nullptr, &cSingleWireBodyData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
	}

	if(nullptr == cWireEdgeData.m_p3dCurve) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DTopoSingleWireBodyGet(nullptr, &cSingleWireBodyData);
		A3DTopoWireEdgeGet(nullptr, &cWireEdgeData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
	}

	LogIncreaseTabIndex(2);

	SpaceCurveSPtr pcSpaceCurve;
	if(true == ConvCurveBase::ConvertSpaceCurveData(cWireEdgeData, m_dContextScale, m_dContextScale, pcSpaceCurve)) {
		SInstanceSPtr cSpaceInstance(new MbSpaceInstance(*pcSpaceCurve));

		// 1. 이름 설정
		if(false == strName.IsEmpty()) {
			SetItemName(cSpaceInstance, strName);
		}

		// 2. Color 설정
		COLORREF cColor;
		if(true == GetAttributeColor(cAttrsData, cColor)) {
			cSpaceInstance->SetColor(cColor);
		}

		// 3. Line Pattern 설정
		int nLinePattern;
		if(true == GetAttributeLinePattern(cAttrsData, nLinePattern)) {
			cSpaceInstance->SetStyle(nLinePattern);
		}

		pcParentAssy->AddItem(*cSpaceInstance);
	}

	LogDecreaseTabIndex(2);

	A3DRiCurveGet(nullptr, &cRiCurveData);
	A3DTopoSingleWireBodyGet(nullptr, &cSingleWireBodyData);
	A3DTopoWireEdgeGet(nullptr, &cWireEdgeData);
	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
}

// 3-6. Ri Set 변환
void C3dImport::ConvertRiSet(A3DRiSet * pcInputRiSet, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy)
{
	CString strName;
	GetName(pcInputRiSet, strName);

	m_nRiSetIndex++;
	Log(2, L"ConvertRiSet[%d]: %s", m_nRiSetIndex, strName);

	LogIncreaseTabIndex(2);

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcInputRiSet, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DRiSetData cRiSetData;
	A3D_INITIALIZE_DATA(A3DRiSetData, cRiSetData);

	A3DStatus nStatus = A3DRiSetGet(pcInputRiSet, &cRiSetData);

	if(A3D_SUCCESS != nStatus) {
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
	}

	for(A3DUns32 nIndex = 0; nIndex < cRiSetData.m_uiRepItemsSize; nIndex++)
	{
		// #import_debug : ConvertRiSet RepItemsSize 
		//if(2 == m_nRiSetIndex && 6 != nIndex) {
// 		if(28 != nIndex) {
// 			continue;
// 		}

		ParseRiRepresentationItem(nIndex, cRiSetData.m_ppRepItems[nIndex], pcAttrs, pcParentAssy);
	}

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	LogDecreaseTabIndex(2);
}

// 3-7. Ri Brep Model 변환 (B-Rep Model 및 Tessellation Model도 함께 처리된다.)
bool C3dImport::ParseRiBrepModel(A3DRiRepresentationItem * pcInputRiBrepModel, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy, bool bCopyModelFlag)
{
	MbSolid * pcSolid = nullptr;
	if(true == m_mpcSolidMap.Lookup((DWORD_PTR) pcInputRiBrepModel, pcSolid)) {
		Log(2, L"ParseRiBrepModel-Instance: [%s]", LogHexStr((DWORD_PTR) pcInputRiBrepModel));
		pcParentAssy->AddItem(*pcSolid);
		return true;
	}

	CString strName;
	GetName(pcInputRiBrepModel, strName);

	Log(2, L"ParseRiBrepModel: %s, [%s]", strName, LogHexStr((DWORD_PTR) pcInputRiBrepModel));

	LogIncreaseTabIndex(2);

	A3DRiBrepModel * pcRiBrepModel = nullptr;

	A3DRiRepresentationItemData cRiRepItemData;
	A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, cRiRepItemData);
	A3DRiRepresentationItemGet(pcInputRiBrepModel, &cRiRepItemData);

	// #option : Brep Model Tessellation Loading
// 	if(nullptr != cRiRepItemData.m_pTessBase) {
// 		ConvertTessBase(cRiRepItemData, pcInputRiBrepModel, pcParentAttr, pcParentAssy);
// 		return true;
// 	}

	if(false == bCopyModelFlag) {
		pcRiBrepModel = pcInputRiBrepModel;
	}
	else {
		// #option: 변환하기 쉽도록 Brep Data를 변경해주는 함수 (A3DCopyAndAdaptBrepModelData)
		A3DCopyAndAdaptBrepModelData cCopyAndAdaptBrepModelData;
		A3D_INITIALIZE_DATA(A3DCopyAndAdaptBrepModelData, cCopyAndAdaptBrepModelData);
// 		cCopyAndAdaptBrepModelData.m_bSplitFaces = A3D_TRUE;
// 		cCopyAndAdaptBrepModelData.m_bSplitClosedFaces = A3D_TRUE;
		// UV Curve가 있는 경우는 영향을 주지 않는다.
		cCopyAndAdaptBrepModelData.m_bForceComputeUV = A3D_TRUE;
		//cCopyAndAdaptBrepModelData.m_bForceComputeUV = A3D_FALSE;
		// 변환중 Error가 발생해도 강제적으로 변환을 계속 하도록 한다.
		cCopyAndAdaptBrepModelData.m_bContinueOnError = A3D_TRUE;

		// If `A3D_TRUE` and m_bForceComputeUV is set to `A3D_TRUE`, computed UV curves can cross seams. 
		// It will be automatically disabled if m_bSplitFaces or m_bSplitClosedFaces are set to `A3D_TRUE`. \version 9.0
		//cCopyAndAdaptBrepModelData.m_bAllowUVCrossingSeams = A3D_TRUE;

		A3DUns32 pcSurfaces[] = {
			kA3DTypeSurfPlane,
			kA3DTypeSurfCylinder,
			kA3DTypeSurfCone,
			kA3DTypeSurfSphere,
			kA3DTypeSurfTorus
		};

		cCopyAndAdaptBrepModelData.m_uiAcceptableSurfacesSize = 5;
		cCopyAndAdaptBrepModelData.m_puiAcceptableSurfaces = pcSurfaces;

		A3DUns32 nCurves[] = {
			kA3DTypeCrvCircle,
			kA3DTypeCrvLine
		};

		cCopyAndAdaptBrepModelData.m_uiAcceptableCurvesSize = 2;
		cCopyAndAdaptBrepModelData.m_puiAcceptableCurves = nCurves;

		A3DStatus nCopyResult = A3DCopyAndAdaptBrepModel(pcInputRiBrepModel, &cCopyAndAdaptBrepModelData, &pcRiBrepModel);
		if(A3D_SUCCESS != nCopyResult) {

			CString strText;
			switch(nCopyResult)
			{
				case A3D_TOOLS_NURBSCONVERT_SURFACE_FAILURE:
					strText = L"NURBS_CONVERT_SURFACE_FAILURE";
					break;

				case A3D_TOOLS_SPLIT_FAILURE:
					strText = L"TOOLS_SPLIT_FAILURE";
					break;

				case A3D_LOOP_BAD_ORIENTATION_DATA:
					strText = L"A3D_LOOP_BAD_ORIENTATION_DATA";
					break;

				default:
					strText.Format(L"%d", nCopyResult);
					break;
			}

			Log(2, L"A3DCopyAndAdaptBrepModel Error: %s", strText);
			ASSERT(false);
// 			pcRiBrepModel = pcInputRiBrepModel;
// 			return false;
		}
	}

	A3DRiBrepModelData cBrepModelData;
	A3D_INITIALIZE_DATA(A3DRiBrepModelData, cBrepModelData);
	A3DStatus nResult = A3DRiBrepModelGet(pcRiBrepModel, &cBrepModelData);

	if(A3D_SUCCESS != nResult) {
		SetLastErrorMessage(L"ParseRiBrepModel - RiBrepModelGet Error", nResult);
		return false;
	}

	// Scale을 구하기 위해서 Context Data에서 값을 가져온다.
	ConvertTopoContextScale(cBrepModelData.m_pBrepData, m_dContextScale);

	m_pcCurrentTopoBrepData = nullptr;

	A3DTopoBrepDataData cTopoBrepDataData;
	A3D_INITIALIZE_DATA(A3DTopoBrepDataData, cTopoBrepDataData);
	nResult = A3DTopoBrepDataGet(cBrepModelData.m_pBrepData, &cTopoBrepDataData);
	if(A3D_SUCCESS != nResult) {
		A3DRiBrepModelGet(nullptr, &cBrepModelData);
		SetLastErrorMessage(L"ParseRiBrepModel - TopoBrepDataGet Error", nResult);
		return false;
	}

	// Loop에서 Face를 검색할 때 사용하기 위해서 값을 저장해 놓는다.
	m_pcCurrentTopoBrepData = cBrepModelData.m_pBrepData;
	m_pcCurRiBrepModel = pcInputRiBrepModel;

	// TopoConnex가 다수인 경우도 있고, 그 아래에 있는 TopoShell도 다수인 경우가 있기 때문에, Solid Vector를 이용해서
	// 정보를 수집한다.
	c3d::SolidsSPtrVector vpSolidVector;

	ShellSPtr pcFaceShell(new MbFaceShell());

	m_mpcConvSurfaceMap.RemoveAll();
	m_mpcSurfBaseMap.RemoveAll();
	m_mpcConvCoEdgeMap.RemoveAll();
	m_mpcVertexMap.RemoveAll();

	for(A3DUns32 nIndex = 0; nIndex < cTopoBrepDataData.m_uiConnexSize; nIndex++)
	{
		// #import_debug : BrepModel Connex Index 조절
		// Solid[1]
// 		if(2 != nIndex) {
// 			continue;
// 		}

		ConvertTopoConnex(nIndex, cTopoBrepDataData.m_ppConnexes[nIndex], pcParentAttr, vpSolidVector);
	}

	A3DRiBrepModelGet(nullptr, &cBrepModelData);
	A3DTopoBrepDataGet(nullptr, &cTopoBrepDataData);

	LogDecreaseTabIndex(2);

	if(true == vpSolidVector.empty()) {
		return false;
	}

	bool bApplyMatrix = false;

	MbMatrix3D cMatrix;

	// RiRepItem 자체에 Matrix가 적용되는 경우 처리
	if(nullptr != cRiRepItemData.m_pCoordinateSystem) {
		A3DRiCoordinateSystemData cCoordinateSystemData;
		A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, cCoordinateSystemData);
		A3DStatus nResult = A3DRiCoordinateSystemGet(cRiRepItemData.m_pCoordinateSystem, &cCoordinateSystemData);
		if(A3D_SUCCESS != nResult) {
			return false;
		}

		A3DMiscTransformation * pcTransformation = cCoordinateSystemData.m_pTransformation;
		if(nullptr == pcTransformation) {
			A3DRiCoordinateSystemGet(nullptr, &cCoordinateSystemData);
			return false;
		}

		GetMatrix(pcTransformation, cMatrix);

		if(false == cMatrix.IsSingle()) {
			bApplyMatrix = true;
		}
	}

	if(1 == vpSolidVector.size()) {
		SetItemName(vpSolidVector[0], strName);

		// Unit Matrix인 경우 처리
		if(false == bApplyMatrix) {
			pcParentAssy->AddItem(*vpSolidVector[0]);
		}
		else {
			pcParentAssy->AddInstance(*vpSolidVector[0], cMatrix);
		}

		m_mpcSolidMap.SetAt((DWORD_PTR) pcInputRiBrepModel, vpSolidVector[0]);
	}
	else {
		AssemblySPtr pcAssy(new MbAssembly());
		SetItemName(pcAssy, strName);

		if(false == bApplyMatrix) {
			for(const auto & pcSolid : vpSolidVector) {
				pcAssy->AddItem(*pcSolid);
			}
		}
		else {
			for(const auto & pcSolid : vpSolidVector) {
				pcAssy->AddInstance(*pcSolid, cMatrix);
			}
		}

		pcParentAssy->AddItem(*pcAssy);
	}

	/*	// Face set을 이용해서 Solid를 만드는 경우
		SolidSPtr pcSolid(new MbSolid(pcFaceShell, nullptr));
		if(nullptr == pcSolid) {
			ASSERT(NULL);
			return false;
		}

		A3DMiscCascadedAttributes * pcAttrs;
		A3DMiscCascadedAttributesData cAttrsData;
		CreateAndPushCascadedAttributes(cBrepModelData.m_pBrepData, pcParentAttr, &pcAttrs, &cAttrsData);

		COLORREF cColor;
		bool bTransparencyDefined = false;
		float fTransparency = 1.0f;
		if(true == GetAttributeColor(cAttrsData, cColor, bTransparencyDefined, fTransparency)) {
			pcSolid->SetColor(cColor);

			if(true == bTransparencyDefined) {
				float a, d, sp, sh, t, e;
				pcSolid->GetVisual(a, d, sp, sh, t, e);
				pcSolid->SetVisual(a, d, sp, sh, fTransparency, e);
			}
		}

		pcParentAssy->AddItem(*pcSolid);
		m_mpcSolidMap.insert(std::make_pair(pcInputRiBrepModel, pcSolid));
	*/

	/*
		if(true == vpSolidVector.empty()) {
			return false;
		}

		if(1 == vpSolidVector.size()) {
			SolidSPtr pcSolid = vpSolidVector[0];
			return true;
		}

		AssemblySPtr pcSolidAssy(new MbAssembly());

		// Index 0번째 Solid를 Main로 생각해서 작업을 실시한다.
		SolidSPtr pcSolid = vpSolidVector[0];
		pcParentAssy->AddItem(*pcSolid);
		m_mpcSolidMap.insert(std::make_pair(pcInputRiBrepModel, pcSolid));

		if(1 == vpSolidVector.size()) {
			return true;
		}

		// Solid 갯수가 1개 이상인 경우 Part Solid를 생성하도록 한다.
		// 첫번째 solid는 메인으로 지정되었으므로 제거한다.
		vpSolidVector.erase(vpSolidVector.begin());
		MbSNameMaker cNames(ct_DetachSolid, MbSNameMaker::i_SideNone, 0);
		::DetachParts(*pcSolid, vpSolidVector, cNames);
	*/

	return true;
}

// 3-7-2. Scale을 구하기 위해서 Context Data에서 값을 가져온다.
bool C3dImport::ConvertTopoContextScale(const A3DTopoBody * pcBody, double & dTopoContextScale)
{
	// 아래쪽에서 문제가 생겨서 값을 가져올 수 없는 경우를 대비해서 Scale값을 1.0을 저장한다.
	dTopoContextScale = 1.0;

	// Scale을 구하기 위해서 Context Data를 가져온다.
	A3DTopoBodyData cTopoBodyData;
	A3D_INITIALIZE_DATA(A3DTopoBodyData, cTopoBodyData);
	A3DStatus nResult = A3DTopoBodyGet(pcBody, &cTopoBodyData);
	if(A3D_SUCCESS != nResult) {
		return false;
	}

	A3DTopoContextData cTopoContextData;
	A3D_INITIALIZE_DATA(A3DTopoContextData, cTopoContextData);
	nResult = A3DTopoContextGet(cTopoBodyData.m_pContext, &cTopoContextData);
	if(A3D_SUCCESS != nResult) {
		A3DTopoBodyGet(nullptr, &cTopoBodyData);
		return false;
	}

	if(A3D_TRUE == cTopoContextData.m_bHaveScale) {
		dTopoContextScale = cTopoContextData.m_dScale;
	}

	A3DTopoBodyGet(nullptr, &cTopoBodyData);
	A3DTopoContextGet(nullptr, &cTopoContextData);

	return true;
}

// 3-8. Topo Connex 변환 --
bool C3dImport::ConvertTopoConnex(int nIndex, A3DTopoConnex * pcTopoConnex, A3DMiscCascadedAttributes * pcParentAttr,
	SolidsSPtrVector & vpSolidVector)
{
	// Topo Connex에서는 Name이 나오지 않음.
	Log(2, L"ConvertTopoConnex[%d]: %s", nIndex, LogHexStr((DWORD_PTR) pcTopoConnex));

	LogIncreaseTabIndex(2);

	if(nullptr != pcTopoConnex) {
		RETURN_FALSE;
	}

	A3DTopoConnexData cTopoConnexData;
	A3D_INITIALIZE_DATA(A3DTopoConnexData, cTopoConnexData);
	A3DStatus eStatus = A3DTopoConnexGet(pcTopoConnex, &cTopoConnexData);
	if(A3D_SUCCESS != eStatus) {
		SetLastErrorMessage(L"ParseRiBrepModel - TopoConnexGet Error", eStatus);
		return false;
	}

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcTopoConnex, pcParentAttr, &pcAttrs, &cAttrsData);

	if(A3D_FALSE == cAttrsData.m_bShow) {
		A3DTopoConnexGet(nullptr, &cTopoConnexData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return false;
	}

	ShellSPtr pcFaceShell(new MbFaceShell());

	for(A3DUns32 nIndex = 0; nIndex < cTopoConnexData.m_uiShellSize; nIndex++) {
		if(true == ConvertMbFaceShell(cTopoConnexData.m_ppShells[nIndex], pcAttrs, pcFaceShell)) {
		}
	}

	SolidSPtr pcSolid;
	pcSolid = new MbSolid(pcFaceShell, nullptr);
	if(nullptr == pcSolid) {
		ASSERT(NULL);
		return false;
	}

	COLORREF cColor;
	if(true == GetAttributeColor(cAttrsData, cColor)) {
		pcSolid->SetColor(cColor);
	}

	float fTransparency = 1.0f;
	if(true == GetAttributeTransparency(cAttrsData, fTransparency)) {
		float a, d, sp, sh, t, e;
		pcSolid->GetVisual(a, d, sp, sh, t, e);
		pcSolid->SetVisual(a, d, sp, sh, fTransparency, e);
	}

	vpSolidVector.push_back(pcSolid);

	A3DTopoConnexGet(nullptr, &cTopoConnexData);
	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	LogDecreaseTabIndex(2);

	// #Require_convert_new_version
/*
	Json::Object cObject;
	cObject.SetDwordPtr("Id", m_nId);
	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetDwordPtr("CommandId", (DWORD_PTR) DmiHps::Command::InsertItem);
	cObject.SetDwordPtr("MbItem", (DWORD_PTR) ((MbItem *) pcSolid));

	SendExecuteCommand(cObject);
*/

#ifdef USED_LOG_MANAGER // #Import_Log : C3dTracer::SolidInformation
	C3dTracer::SolidInformation(*pcSolid, L"d:\\Temp\\SolidInfo.log", m_strCadFileName.GetBuffer());
#endif

	return true;
}

// 3-9. Topo Shell 변환 --
bool C3dImport::ConvertMbFaceShell(A3DTopoShell * pcTopoShell, A3DMiscCascadedAttributes * pcParentAttr,
	ShellSPtr & pcFaceShell)
{
	if(nullptr == pcTopoShell) {
		return false;
	}

#ifdef USED_LOG_MANAGER // #Import_Log : A3dTracer
	m_cA3dTracer.SetScale(m_dContextScale);
	//m_cA3dTracer.CreateLog(L"D:\\Temp\\A3dXInfo.log");
	m_cA3dTracer.A3DTopoShellLog(pcTopoShell);
#endif

	A3DTopoShellData cTopoShellData;
	A3D_INITIALIZE_DATA(A3DTopoShellData, cTopoShellData);
	A3DStatus nResult = A3DTopoShellGet(pcTopoShell, &cTopoShellData);
	if(A3D_SUCCESS != nResult) {
		SetLastErrorMessage(L"ConvertTopoShell - TopoShellGet Error", nResult);
		return false;
	}

	Log(2, L"ConvertMbFaceShell: %s, Face Size: %d", LogHexStr((DWORD_PTR) pcTopoShell), cTopoShellData.m_uiFaceSize);

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcTopoShell, pcParentAttr, &pcAttrs, &cAttrsData);

	if(A3D_FALSE == cAttrsData.m_bShow) {
		return false;
	}

	COLORREF cColor;
	bool bColorDefinedFlag = false;
	if(true == GetAttributeColor(cAttrsData, cColor)) {
		bColorDefinedFlag = true;
	}

	float fTransparency = 1.0f;
	bool bTransparencyDefineFlag = false;
	if(true == GetAttributeTransparency(cAttrsData, fTransparency)) {
		bTransparencyDefineFlag = true;
	}

	for(A3DUns32 nIndex = 0; nIndex < cTopoShellData.m_uiFaceSize; nIndex++)
	{
		// #import_debug : Face Index 조절
		//if(!(0 <= nIndex && nIndex <= 10)) {
		//if(536 != nIndex && 535 != nIndex) {
//       	if(1315 != nIndex) {
//       		continue;
//       	}

		FaceSPtr pcFace;
		// Face의 Sense 결정
		bool bOrientationWithShell = (1 == cTopoShellData.m_pucOrientationWithShell[nIndex]) ? true : false;

		LogIncreaseTabIndex(2);

		if(true == ConvertMbFace(nIndex, cTopoShellData.m_ppFaces[nIndex], bOrientationWithShell, pcAttrs, cAttrsData,
			cColor, bColorDefinedFlag, fTransparency, bTransparencyDefineFlag, pcFace)) {
			if(nullptr != pcFace) {
				pcFaceShell->AddFace(*pcFace);

				// #import_debug : Face 개별 Log 저장
/*
				if(2829 == nIndex) {
					CString strPathName;
					strPathName.Format(L"d:\\Temp\\FaceInfo-%d.log", nIndex);
					C3dTracer::FaceInformation(pcFace, strPathName, m_strCadFileName.GetBuffer());
				}
*/
			}
		}
		else {
			ASSERT(false);
		}

		LogDecreaseTabIndex(2);
	}

#ifdef _DEBUG
	// 	SimpleName testMainName = ct_Creator;
	// 	MbSNameMaker snMaker(testMainName);
	// 	MbCheckTopologyParams ctPars(true, snMaker);
	//	MbResultType res = pcFaceShell->CheckTopology(ctPars);
	//	int i = 0;
#endif

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return true;
}

bool C3dImport::ThreadConvertMbFaceShell(A3DTopoShell * pcTopoShell, A3DMiscCascadedAttributes * pcParentAttr,
	ShellSPtr & pcFaceShell)
{
	if(nullptr == pcTopoShell) {
		return false;
	}

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcTopoShell, pcParentAttr, &pcAttrs, &cAttrsData);

	if(A3D_FALSE == cAttrsData.m_bShow) {
		return false;
	}

	// 1. ConvFaceShell를 생성해서, Thread 작업을 준비한다.
	ConvFaceShell cConvFaceShell(pcTopoShell, m_dContextScale);
	if(false == cConvFaceShell.IsInit()) {
		Log(2, L"Error - cConvFace.IsInit()");
		return false;
	}

	Log(2, L"ThreadConvertMbFaceShell: %s, Face Size: %d", LogHexStr((DWORD_PTR) pcTopoShell), cConvFaceShell.m_cTopoShellData.m_uiFaceSize);

	// 2. ConvFaceShell에 들어있는 Surface를 검색해서 변환한다.
	cConvFaceShell.CollectAndConvertSurface(m_dContextScale);

	// 3. ConvCoEdge를 수집해서, MbCurveEdge를 생성하도록 한다.
	cConvFaceShell.CollectAndConvertTopoCoEdge(m_dContextScale);

	// 4. TopoVertex를 수집해서, MbVertex를 생성하도록 한다.
	cConvFaceShell.CollectAndConvertTopoVertex(m_dContextScale);

	// 5. Convert를 실행한다.
	if(false == cConvFaceShell.Convert()) {
		ASSERT(false);
		return false;
	}

#ifdef _DEBUG
	// 	A3dTracer cA3dTracer(m_dContextScale);
	// 	cA3dTracer.CreateLog(L"D:\\Temp\\A3dXInfo.log");
	// 	cA3dTracer.A3DTopoShellLog(pcTopoShell);
#endif

	ConvFaceVector & vpcConvFaceVector = cConvFaceShell.GetConvFaceVector();

	COLORREF cParentColor;
	bool bParentColorDefinedFlag = false;
	if(true == GetAttributeColor(cAttrsData, cParentColor)) {
		bParentColorDefinedFlag = true;
	}

	float fParentTransparency = 1.0f;
	bool bParentTransparencyDefineFlag = false;
	if(true == GetAttributeTransparency(cAttrsData, fParentTransparency)) {
		bParentTransparencyDefineFlag = true;
	}

	for(auto pcConvFace : vpcConvFaceVector) {
		MbFace * pcFace = pcConvFace->GetFace();
		pcFaceShell->AddFace(*pcFace);

		A3DTopoFace * pcTopoFace = pcConvFace->GetTopoFace();

		A3DMiscCascadedAttributes * pcFaceAttrs;
		A3DMiscCascadedAttributesData cFaceAttrsData;
		CreateAndPushCascadedAttributes(pcTopoFace, pcAttrs, &pcFaceAttrs, &cFaceAttrsData);

		COLORREF cColor;
		if(true == GetAttributeColor(cFaceAttrsData, cColor)) {
			bool bColoredFlag = true;
			if(true == bParentColorDefinedFlag) {
				if(cParentColor == cColor) {
					bColoredFlag = false;
				}
			}

			if(true == bColoredFlag) {
				pcFace->SetColor(cColor);
			}
		}

		float fTransparency = 1.0f;
		if(true == GetAttributeTransparency(cFaceAttrsData, fTransparency)) {
			bool bTransparencyDefineFlag = true;
			if(true == bParentTransparencyDefineFlag) {
				if(fParentTransparency == fTransparency) {
					bTransparencyDefineFlag = false;
				}
			}

			if(true == bTransparencyDefineFlag) {
				float a, d, sp, sh, t, e;
				pcFace->GetVisual(a, d, sp, sh, t, e);
				pcFace->SetVisual(a, d, sp, sh, fTransparency, e);
			}
		}

		A3DMiscCascadedAttributesDelete(pcFaceAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cFaceAttrsData);
	}

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return true;
}

// 3-10. Topo Face 변환
bool C3dImport::ConvertMbFace(A3DUns32 nIndex, A3DTopoFace * pcTopoFace, bool bOrientationWithShell, A3DMiscCascadedAttributes * pcParentAttr,
	A3DMiscCascadedAttributesData & cParentAttributesData, COLORREF cParentColor, bool bParentColorDefinedFlag,
	float fParentTransparency, bool bParentTransparencyDefineFlag, c3d::FaceSPtr & pcFace)
{
	A3DTopoFaceData cTopoFaceData;
	A3D_INITIALIZE_DATA(A3DTopoFaceData, cTopoFaceData);
	A3DStatus eResult = A3DTopoFaceGet(pcTopoFace, &cTopoFaceData);
	if(A3D_SUCCESS != eResult) {
		Log(2, L"Error - A3DTopoShellGet Fail: %d", eResult);
		return false;
	}

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcTopoFace, pcParentAttr, &pcAttrs, &cAttrsData);

	// Noshow 상태 확인
	if(A3D_FALSE == cAttrsData.m_bShow) {
		ASSERT(false);
		A3DTopoFaceGet(nullptr, &cTopoFaceData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return false;
	}

	// 1. Surface를 변환. MbLoop를 생성하기 위해서, Surface를 미리 만들어야 함.
	A3DSurfBase * pcSurfBase = cTopoFaceData.m_pSurface;
	SurfaceSPtr pcSurface;
	if(false == GetMbSurface(pcSurfBase, pcSurface)) {
		Log(2, L"ConvertTopoFace - GetMbSurface Error");
		return false;
	}

	// #import_debug : Base Surface Drawing
// 	SInstanceSPtr cSpaceInstance(new MbSpaceInstance(*pcSurface));
// 	m_pcModel->AddItem(*cSpaceInstance);

	std::list<LoopSPtr> vpcLoopList;
	// 2. Loop Data를 수집
	for(A3DUns32 nIndex = 0; nIndex < cTopoFaceData.m_uiLoopSize; nIndex++)
	{
		// 2-1. Loop를 생성, bFaceSense에 의해서 생성된 Curve와 Curve의 연결 순서가 반대로 된다.
		LoopSPtr pcLoop;
		if(false == ConvertMbLoop(cTopoFaceData.m_ppLoops[nIndex], pcSurface, bOrientationWithShell, pcLoop)) {
			ASSERT(false);
			continue;
		}

		// 2-2. Loop Vector에 Loop 저장
		if(nullptr != pcLoop) {
			// 외각 Loop의 Index가 0이 아닌경우 처리, Outter Index와 현재 Index가 같은경우, 그 Loop를 맨앞으로 보낸다.
			if(nIndex == cTopoFaceData.m_uiOuterLoopIndex) {
				vpcLoopList.push_front(pcLoop);
			}
			else {
				vpcLoopList.push_back(pcLoop);
			}

		}
	}

	// 3. Loop List가 비어있는 경우 Surface를 이용해서, Sruface Loop를 생성한다.
	if(true == vpcLoopList.empty()) {
		CreateSurfaceBorderLoop(pcSurface, vpcLoopList);
	}

	MbSurface * pcOriginSurface = pcSurface;

	/*
		SurfaceSPtr pcBoundedSurface;
		if(true == CreateBoundedSurface(pcSurface, vpcLoopVector[0], bOrientationWithShell, pcBoundedSurface)) {
			pcSurface = pcBoundedSurface;
		}
	*/

	// 4. 최외각 Loop를 이용해서, Surface의 Boundary를 Curve가 넘어가는지 여부를 확인하고 그런경우 Boundary Surface를 생성해서 사용하도록 한다.
	switch(pcSurface->IsA())
	{
		/*
				// 3-1. Periodic Surface만 적용하도록 한다.
				case st_CylinderSurface:
				case st_ConeSurface:
				case st_SphereSurface:
				{
					double dVMin1 = pcSurface->GetVMin();
					double dVMax1 = pcSurface->GetVMax();

					SurfaceSPtr pcBoundedSurface;

					if(TRUE == cTopoFaceData.m_bHasTrimDomain) {
						if(true == CreateBoundedSurface(pcSurface, cTopoFaceData.m_sSurfaceDomain, pcBoundedSurface)) {
							pcSurface = pcBoundedSurface;
						}
					}
					else if(true == CreateBoundedSurface(pcSurface, vpcLoopVector[0], bOrientationWithShell, pcBoundedSurface)) {
						double dVMin2 = pcSurface->GetVMin();
						double dVMax2 = pcSurface->GetVMax();

						pcSurface = pcBoundedSurface;
					}

					double dVMin3 = pcSurface->GetVMin();
					double dVMax3 = pcSurface->GetVMax();

					int i = 0;
				}
				break;
		*/
		// 3-2. Plane에서도 Face가 제대로 구성되지 않는 문제가 발생함.
		//      Torus는 Majon Minor Radius의 관계 때문에, 주어진 Domain을 사용하면 안되는 경우가 있음.
		case st_CylinderSurface:
		case st_ConeSurface:
		case st_SphereSurface:
		case st_TorusSurface:
		case st_Plane:
		{
			SurfaceSPtr pcBoundedSurface;
			if(true == CreateBoundedSurface(pcSurface, vpcLoopList.front(), bOrientationWithShell, false, pcBoundedSurface)) {
				pcSurface = pcBoundedSurface;
			}
		}
		break;

		default:
		{
			SurfaceSPtr pcBoundedSurface;
			if(true == CreateBoundedSurface(pcSurface, vpcLoopList.front(), bOrientationWithShell, true, pcBoundedSurface)) {
				pcSurface = pcBoundedSurface;
			}
		}
		break;
	}

	bool bSwapUVFlag = false;
	const DmiSurface * pcDmiSurface = dynamic_cast<const DmiSurface *>(pcOriginSurface);

	if(nullptr != pcDmiSurface) {
		bSwapUVFlag = pcDmiSurface->IsSwapUV();
		/*

				double dVCoeff = 0.0;

				if(false == bSwapUVFlag) {
					dVCoeff = pcDmiSurface->GetVCoeff();
				}
				else {
					dVCoeff = pcDmiSurface->GetUCoeff();
				}

				if(0 > dVCoeff) {
					bOrientationWithShell = !bOrientationWithShell;
				}
				else if(true == bSwapUVFlag) {
					bOrientationWithShell = !bOrientationWithShell;
				}
		*/
	}

	// 4. 생성된 Surface를 이용해서 Face를 생성한다.
	switch(pcOriginSurface->IsA())
	{
		// 3-1. Periodic Surface만 적용하도록 한다.
		case st_Plane:
		case st_CylinderSurface:
		case st_ConeSurface:
		case st_SphereSurface:
		case st_TorusSurface:
		{
			double dVCoeff = 0.0;
			if(false == bSwapUVFlag) {
				dVCoeff = pcDmiSurface->GetVCoeff();
			}
			else {
				dVCoeff = pcDmiSurface->GetUCoeff();
			}

			if(0.0 > pcDmiSurface->GetVCoeff() * pcDmiSurface->GetUCoeff()) {
				//if(0 > pcDmiSurface->GetVCoeff()) {
				bOrientationWithShell = !bOrientationWithShell;
				// 
// 				if(0.0 > dVCoeff) {
// 					bOrientationWithShell = !bOrientationWithShell;
// 				}
				// 1. SAT 파일등에서 V Coeff가 0보다 작은 경우 SwapUV 조건에 따라서 Face 방향을 뒤집어야 하는 경우가 있음
				// 2. Pipe.sat 경우에서는 Swap UV라고 뒤집으면 않됨.
// 				if (true == pcDmiSurface->IsSwapUV()) {
// 					bOrientationWithShell = !bOrientationWithShell;
// 				}
			}
		}
		break;

/*
		case st_TorusSurface:
			if(0.0 > pcDmiSurface->GetVCoeff() * pcDmiSurface->GetUCoeff()) {
				//if(0 > pcDmiSurface->GetVCoeff()) {
				bOrientationWithShell = !bOrientationWithShell;
				// 
// 				if(0.0 > dVCoeff) {
// 					bOrientationWithShell = !bOrientationWithShell;
// 				}
				// 1. SAT 파일등에서 V Coeff가 0보다 작은 경우 SwapUV 조건에 따라서 Face 방향을 뒤집어야 하는 경우가 있음
				// 2. Pipe.sat 경우에서는 Swap UV라고 뒤집으면 않됨.
// 				if (true == pcDmiSurface->IsSwapUV()) {
// 					bOrientationWithShell = !bOrientationWithShell;
// 				}
			}

			// V 경계면을 넘어가는 UV 커브가 있는 경우 Major Radius를 Minus로 설정한다.
			if(0.0 > ((MbTorusSurface *) pcOriginSurface)->GetMajorRadius()) {
				//bOrientationWithShell = !bOrientationWithShell;

				// SAT 파일등에서 V Coeff가 0보다 작은 경우 SwapUV 조건에 따라서 Face 방향을 뒤집어야 하는 경우가 있음
// 					if (true == pcDmiSurface->IsSwapUV()) {
// 						bOrientationWithShell = !bOrientationWithShell;
// 					}
			}
			break;*/
	}

	if(true == bSwapUVFlag) {
		bOrientationWithShell = !bOrientationWithShell;
	}

	pcFace = new MbFace(*pcSurface, bOrientationWithShell);

	COLORREF cColor;
	if(true == GetAttributeColor(cAttrsData, cColor)) {
		pcFace->SetColor(cColor);
	}
	else if(true == bParentColorDefinedFlag) {
		pcFace->SetColor(cParentColor);
	}

	float fTransparency = 1.0f;
	if(true == GetAttributeTransparency(cAttrsData, fTransparency)) {
		float a, d, sp, sh, t, e;
		pcFace->GetVisual(a, d, sp, sh, t, e);
		pcFace->SetVisual(a, d, sp, sh, fTransparency, e);
	}
	else if(true == bParentTransparencyDefineFlag) {
		float a, d, sp, sh, t, e;
		pcFace->GetVisual(a, d, sp, sh, t, e);
		pcFace->SetVisual(a, d, sp, sh, fParentTransparency, e);
	}

	if(nullptr == pcFace) {
		SetLastErrorMessage(L"ConvertSurfPlane - MbFace nullptr");
		return false;
	}

	// 5. Loop를 Face에 추가.
	for(MbLoop * pcLoopElement : vpcLoopList) {
		pcFace->AddLoop(*pcLoopElement);
	}


	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return true;
}

// 3-10-1. Loop의 Parameter 영역이 Surface의 영역을 벗어난 경우 Surface를 바탕으로 MbCurveBoundedSurface를 생성한다.
bool C3dImport::CreateBoundedSurface(SurfaceSPtr & pcSurface, LoopSPtr & pcLoop, bool bOrientationWithShell, bool bCheckBoundFlag, SurfaceSPtr & pcBoundedSurface)
{
	const MbRect cLoopRect = pcLoop->GetGabarit(pcSurface->GetSurface(), bOrientationWithShell);
	if(true == cLoopRect.IsEmpty()) {
		ASSERT(false);
		return false;
	}

	// 공차를 주어야 한다. Loop의 Bound Box가 약간이라도 벗어나면 문제가 생김.
	double dTolerance = 1.0e-6;

	bool bCreateBoundedSurface = false;

	if(true == bCheckBoundFlag) {
		double dUMin = pcSurface->GetUMin();
		double dUMax = pcSurface->GetUMax();
		double dVMin = pcSurface->GetVMin();
		double dVMax = pcSurface->GetVMax();

		if(cLoopRect.GetXMin() - dTolerance < dUMin) {
			bCreateBoundedSurface = true;
		}
		else if(cLoopRect.GetXMax() + dTolerance > dUMax) {
			bCreateBoundedSurface = true;
		}
		else if(cLoopRect.GetYMin() - dTolerance < dVMin) {
			bCreateBoundedSurface = true;
		}
		else if(cLoopRect.GetYMax() + dTolerance > dVMax) {
			bCreateBoundedSurface = true;
		}
	}
	else {
		bCreateBoundedSurface = true;
	}

	if(false == bCreateBoundedSurface) {
		return false;
	}

	MbRect2D cRect(cLoopRect.GetXMin(), cLoopRect.GetYMin(), cLoopRect.GetXMax(), cLoopRect.GetYMax());
	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, cRect);

	// 	PlaneContourSPtr pcLoopContour(&pcLoop->MakeContour(*pcSurface, bOrientationWithShell));
	// 	PlaneContoursSPtrVector pcloopContoursVector;
	// 	pcloopContoursVector.push_back(pcLoopContour);
	//	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, pcloopContoursVector, true);

	return true;
}

bool C3dImport::CreateBoundedSurface(MbSurface *& pcSurface, LoopSPtr & pcLoop, bool bOrientationWithShell, MbSurface *& pcBoundedSurface)
{
	const MbRect cLoopRect = pcLoop->GetGabarit(pcSurface->GetSurface(), bOrientationWithShell);
	if(true == cLoopRect.IsEmpty()) {
		ASSERT(false);
		return false;
	}

	// 공차를 주어야 한다. Loop의 Bound Box가 약간이라도 벗어나면 문제가 생김.
	double dTolerance = 1.0e-9;

	bool bCreateBoundedSurface = false;

	double dUMin = pcSurface->GetUMin();
	double dUMax = pcSurface->GetUMax();
	double dVMin = pcSurface->GetVMin();
	double dVMax = pcSurface->GetVMax();

	if(cLoopRect.GetXMin() - dTolerance < dUMin) {
		bCreateBoundedSurface = true;
	}
	else if(cLoopRect.GetXMax() + dTolerance > dUMax) {
		bCreateBoundedSurface = true;
	}
	else if(cLoopRect.GetYMin() - dTolerance < dVMin) {
		bCreateBoundedSurface = true;
	}
	else if(cLoopRect.GetYMax() + dTolerance > dVMax) {
		bCreateBoundedSurface = true;
	}

	bCreateBoundedSurface = true;

	if(false == bCreateBoundedSurface) {
		return false;
	}

	PlaneContourSPtr pcLoopContour(&pcLoop->MakeContour(*pcSurface, bOrientationWithShell));

	PlaneContoursSPtrVector pcloopContoursVector;
	pcloopContoursVector.push_back(pcLoopContour);

	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, pcloopContoursVector, false);

	dVMax = pcSurface->GetVMax();
	dVMin = pcSurface->GetVMin();

	return true;
}

// 3-10-2. 주어진 Domain을 이용해서 MbCurveBoundedSurface를 생성한다.
bool C3dImport::CreateBoundedSurface(SurfaceSPtr & pcSurface, const A3DDomainData & cDomainData, SurfaceSPtr & pcBoundedSurface)
{
	const DmiSurface * pcRtSurface = dynamic_cast<const DmiSurface *>((MbSurface *) pcSurface);

	double dUMin = cDomainData.m_sMin.m_dX * m_dContextScale * pcRtSurface->GetUCoeff();
	double dUMax = cDomainData.m_sMax.m_dX * m_dContextScale * pcRtSurface->GetUCoeff();
	double dVMin = cDomainData.m_sMin.m_dY * m_dContextScale * pcRtSurface->GetVCoeff();
	double dVMax = cDomainData.m_sMax.m_dY * m_dContextScale * pcRtSurface->GetVCoeff();

	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, dUMin, dUMax, dVMin, dVMax);

	dVMax = pcSurface->GetVMax();
	dVMin = pcSurface->GetVMin();

	return true;
}

// 3-10-3. 주어진 Plane Curve를 이용해서 MbCurveBoundedSurface를 생성한다.
bool C3dImport::CreateBoundedSurface(SurfaceSPtr & pcSurface, PlaneCurveSPtr & pcPlaneCurve, SurfaceSPtr & pcBoundedSurface)
{
	MbRect cCurveRect;
	pcPlaneCurve->CalculateGabarit(cCurveRect);
	if(true == cCurveRect.IsEmpty()) {
		ASSERT(false);
		return false;
	}

	// 공차를 주어야 한다. Loop의 Bound Box가 약간이라도 벗어나면 문제가 생김.
	double dTolerance = 1.0e-6;

	bool bCreateBoundedSurface = false;

	double dUMin = pcSurface->GetUMin();
	double dUMax = pcSurface->GetUMax();
	double dVMin = pcSurface->GetVMin();
	double dVMax = pcSurface->GetVMax();

	if(cCurveRect.GetXMin() - dTolerance < dUMin) {
		bCreateBoundedSurface = true;
	}
	else if(cCurveRect.GetXMax() + dTolerance > dUMax) {
		bCreateBoundedSurface = true;
	}
	else if(cCurveRect.GetYMin() - dTolerance < dVMin) {
		bCreateBoundedSurface = true;
	}
	else if(cCurveRect.GetYMax() + dTolerance > dVMax) {
		bCreateBoundedSurface = true;
	}

	bCreateBoundedSurface = true;

	if(false == bCreateBoundedSurface) {
		return false;
	}

	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, cCurveRect);

	return true;
}

bool C3dImport::CreateBoundedSurface(MbSurface *& pcSurface, const A3DDomainData & cDomainData, MbSurface *& pcBoundedSurface)
{
	const DmiSurface * pcRtSurface = dynamic_cast<const DmiSurface *>((MbSurface *) pcSurface);

	double dUMin = cDomainData.m_sMin.m_dX * m_dContextScale * pcRtSurface->GetUCoeff();
	double dUMax = cDomainData.m_sMax.m_dX * m_dContextScale * pcRtSurface->GetUCoeff();
	double dVMin = cDomainData.m_sMin.m_dY * m_dContextScale * pcRtSurface->GetVCoeff();
	double dVMax = cDomainData.m_sMax.m_dY * m_dContextScale * pcRtSurface->GetVCoeff();

	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, dUMin, dUMax, dVMin, dVMax);

	dVMax = pcSurface->GetVMax();
	dVMin = pcSurface->GetVMin();

	return true;
}

// 3-10-4. Create Surface Border, 외각 Loop 생성
// (Face에서 Loop Curve가 없는 경우 Surface의 외각 UV Curve를 생성해서 추가하도록 한다.)
bool C3dImport::CreateSurfaceBorderLoop(SurfaceSPtr & pcSurface, std::list<LoopSPtr> & vpcLoopList)
{
	LoopSPtr pcLoop;
	pcLoop = new MbLoop();
	if(nullptr == pcLoop) {
		return false;
	}

	MbCartPoint cMinPo, cMaxPo;

	cMinPo.x = pcSurface->GetUMin();
	cMinPo.y = pcSurface->GetVMin();

	cMaxPo.x = pcSurface->GetUMax();
	cMaxPo.y = pcSurface->GetVMax();

	// 1. Bottom Side Edge 생성
	MbCartPoint cSP, cEP;
	cSP = cMinPo;
	
	cEP.x = cMaxPo.x;
	cEP.y = cMinPo.y;

	MbLineSegment * pcBottomCurve = new MbLineSegment(cSP, cEP);

	IntersectionCurveSPtr pcInterCurve;
	pcInterCurve = new MbSurfaceIntersectionCurve(*pcSurface, *pcBottomCurve,
		*pcSurface, *pcBottomCurve, MbeCurveBuildType::cbt_Ordinary, true, false);

	EdgeSPtr pcCurveEdge;
	pcCurveEdge = new MbCurveEdge(*pcInterCurve, true);

	DmiOrientedEdge * pcOrientedEdge;
	pcOrientedEdge = new DmiOrientedEdge(*pcCurveEdge, true, true);

	pcLoop->AddEdge(*pcOrientedEdge);

	// 2. Right Side Edge 생성
	cSP.x = cMaxPo.x;
	cSP.y = cMinPo.y;

	cEP.x = cMaxPo.x;
	cEP.y = cMaxPo.y;

	MbLineSegment * pcRightCurve = new MbLineSegment(cSP, cEP);

	pcInterCurve = new MbSurfaceIntersectionCurve(*pcSurface, *pcRightCurve,
		*pcSurface, *pcRightCurve, MbeCurveBuildType::cbt_Ordinary, true, false);

	pcCurveEdge = new MbCurveEdge(*pcInterCurve, true);

	pcOrientedEdge = new DmiOrientedEdge(*pcCurveEdge, true, true);

	pcLoop->AddEdge(*pcOrientedEdge);

	// 3. Top Side Edge 생성
	cSP.x = cMaxPo.x;
	cSP.y = cMaxPo.y;

	cEP.x = cMinPo.x;
	cEP.y = cMaxPo.y;

	MbLineSegment * pcTopCurve = new MbLineSegment(cSP, cEP);

	pcInterCurve = new MbSurfaceIntersectionCurve(*pcSurface, *pcTopCurve,
		*pcSurface, *pcTopCurve, MbeCurveBuildType::cbt_Ordinary, true, false);

	pcCurveEdge = new MbCurveEdge(*pcInterCurve, true);

	pcOrientedEdge = new DmiOrientedEdge(*pcCurveEdge, true, true);

	pcLoop->AddEdge(*pcOrientedEdge);

	// 4. Left Side Edge 생성
	cSP.x = cMinPo.x;
	cSP.y = cMaxPo.y;

	cEP = cMinPo;

	MbLineSegment * pcLeftCurve = new MbLineSegment(cSP, cEP);

	pcInterCurve = new MbSurfaceIntersectionCurve(*pcSurface, *pcLeftCurve,
		*pcSurface, *pcLeftCurve, MbeCurveBuildType::cbt_Ordinary, true, false);

	pcCurveEdge = new MbCurveEdge(*pcInterCurve, true);

	pcOrientedEdge = new DmiOrientedEdge(*pcCurveEdge, true, true);

	pcLoop->AddEdge(*pcOrientedEdge);

	vpcLoopList.push_back(pcLoop);


	return true;
}

// 3-11. Topo Loop 변환.
bool C3dImport::ConvertMbLoop(A3DTopoLoop * pcTopoLoop, const MbSurface * pcSurface, bool bOrientationWithShell, LoopSPtr & pcLoop)
{
	if(nullptr != pcTopoLoop) {
		RETURN_FALSE;
	}

	// TopoLoop Data 수집
	A3DTopoLoopData cTopoLoopData;
	A3D_INITIALIZE_DATA(A3DTopoLoopData, cTopoLoopData);
	A3DStatus eResult = A3DTopoLoopGet(pcTopoLoop, &cTopoLoopData);
	if(A3D_SUCCESS != eResult) {
		SetLastErrorMessage(L"ConvertTopoLoop - A3DTopoLoopGet Error", eResult);
		return false;
	}

	// Loop Sense 설정
	bool bOrientationWithSurfaceFlag = (1 == cTopoLoopData.m_ucOrientationWithSurface) ? true : false;

	const DmiSurface * pcDmiSurface = dynamic_cast<const DmiSurface *>(pcSurface);

	// 	pcLoop = new MbLoop();
	// 	ASSERT_RETURN_FALSE(nullptr != pcLoop);
	std::list<const DmiOrientedEdge *> lpcOrientedEdgeList;

	for(A3DUns32 nIndex = 0; nIndex < cTopoLoopData.m_uiCoEdgeSize; nIndex++)
	{
		DmiOrientedEdge * pcOrientedEdge = nullptr;
		ConvertMbOrientedEdge(cTopoLoopData.m_ppCoEdges[nIndex], pcSurface, pcOrientedEdge);

		if(nullptr != pcOrientedEdge) {
			lpcOrientedEdgeList.push_back(pcOrientedEdge);
		}
	}

	// 생성된 OrientEdge 크기를 확인
	if(0 == lpcOrientedEdgeList.size()) {
		return false;
	}

	// Loop가 Open되어 있는지 여부를 확인해서 Open되어 있는 경우 Close되도록 Edge를 추가한다.
	// Cone Surface에 대해서만 확인한다. (Cone은 Pole까지 Surface가 있는 경우 Curve가 Open됨)
	MbeSpaceType eSpaceType = pcSurface->IsA();
	if(st_ConeSurface == eSpaceType) {// || st_TorusSurface == eSpaceType) {
		CheckOrientedEdgeOpenAndHealing(lpcOrientedEdgeList, pcSurface, 1.0e-4 / m_dContextScale);
		//CheckUVDomain(lpcOrientedEdgeList, &pcSurface);
	}

	pcLoop = new MbLoop();
	for(const DmiOrientedEdge * pcOrientedEdge : lpcOrientedEdgeList) {
		pcLoop->AddEdge(*pcOrientedEdge);
	}

	if(bOrientationWithShell != bOrientationWithSurfaceFlag) {
		pcLoop->PartialReverse();
	}

	return true;
}

// 3-11-1. OrientedEdge의 Open 여부를 확인하고 Open된 경우 Close되도록 Edge를 추가한다.
void C3dImport::CheckOrientedEdgeOpenAndHealing(std::list<const DmiOrientedEdge *> & lpcOrientedEdgeList, const MbSurface * pcBaseSurface, double dTolerance)
{
	auto cIterator = lpcOrientedEdgeList.begin();
	auto cCurIterator = cIterator;
	auto cNextIterator = cIterator;
	auto cEndIterator = std::prev(lpcOrientedEdgeList.end());

	const MbRect cRect;

	// 중간에 삽입되어도 순회하는데 문제는 없음. [2022/01/24 11:11 sysong]
	while(cIterator != lpcOrientedEdgeList.end()) {
		const DmiOrientedEdge * pcCurOrientedEdge = *cIterator;
		const DmiOrientedEdge * pcNextOrientedEdge = nullptr;

		cCurIterator = cIterator;
		++cIterator;
		cNextIterator = cIterator;

		// 마지막 요소라면 처음 요소와 비교한다.
		if(cCurIterator == cEndIterator) {
			pcNextOrientedEdge = *lpcOrientedEdgeList.begin();
		}
		else {
			pcNextOrientedEdge = *cIterator;
		}

		bool bConnectFlag = false;
		MbCartPoint cStartPoint, cEndPoint;

		if(true == CheckConectivity(pcCurOrientedEdge, pcNextOrientedEdge, pcBaseSurface, dTolerance, bConnectFlag)) {
			if(false == bConnectFlag) {
				const DmiOrientedEdge * pcCreateOrientedEdge = nullptr;
				if(true == CreateLineSegmentOrientedEdge(lpcOrientedEdgeList, pcCurOrientedEdge, pcNextOrientedEdge, pcBaseSurface, dTolerance,
					pcCreateOrientedEdge))
				{
					if(cCurIterator == cEndIterator) {
						lpcOrientedEdgeList.push_back(pcCreateOrientedEdge);
					}
					else {
						lpcOrientedEdgeList.insert(cIterator, pcCreateOrientedEdge);
					}
				}
			}
		}
	}
}

// 3-11-2. OrientedEdge Plane curve가 Line인지 여부를 확인
bool C3dImport::IsOrientedEdgeAllPlaneLine(std::list<const DmiOrientedEdge *> & lpcOrientedEdgeList, const MbSurface * pcBaseSurface)
{
	for(auto pcOrientEdge : lpcOrientedEdgeList) {
		const MbCurveEdge & cCurveEdge = pcOrientEdge->GetCurveEdge();
		const MbSurfaceIntersectionCurve & cIntersectionCurve = cCurveEdge.GetIntersectionCurve();

		if(pcBaseSurface == &cIntersectionCurve.GetCurveOneSurface()) {
			if(pt_LineSegment != cIntersectionCurve.GetCurveOneCurve().IsA()) {
				return false;
			}
		}
		else if(pcBaseSurface == &cIntersectionCurve.GetCurveTwoSurface()) {
			if(pt_LineSegment != cIntersectionCurve.GetCurveTwoCurve().IsA()) {
				return false;
			}
		}
	}

	return true;
}

// 3-11-4. 연결 상태 검사
bool C3dImport::CheckConectivity(const DmiOrientedEdge * pcCurEdge, const DmiOrientedEdge * pcNextEdge,
	const MbSurface * pcBaseSurface, double dTolerance, bool & bConnectFlag, const MbCurve ** pcConnectCurve)
{
	const MbCurveEdge & cCurCurveEdge = pcCurEdge->GetCurveEdge();
	const MbCurveEdge & cNextCurveEdge = pcNextEdge->GetCurveEdge();

	const MbSurfaceIntersectionCurve & cCurIntersectionCurve = cCurCurveEdge.GetIntersectionCurve();
	const MbSurfaceIntersectionCurve & cNextIntersectionCurve = cNextCurveEdge.GetIntersectionCurve();

	bool bCurEdgeUsedCurveOneFlag = pcCurEdge->IsUsedCurveOne();
	bool bNextEdgeUsedCurveOneFlag = pcNextEdge->IsUsedCurveOne();

	bool bCurSense = pcCurEdge->GetOrientation();
	bool bNextSense = pcNextEdge->GetOrientation();

	const MbCurve * pcCurCurve;
	const MbCurve * pcNextCurve;

	pcCurCurve = nullptr;
	pcNextCurve = nullptr;

	if(true == bCurEdgeUsedCurveOneFlag) {
		pcCurCurve = &cCurIntersectionCurve.GetCurveOneCurve();
	}
	else {
		pcCurCurve = &cCurIntersectionCurve.GetCurveTwoCurve();
	}

	if(true == bNextEdgeUsedCurveOneFlag) {
		pcNextCurve = &cNextIntersectionCurve.GetCurveOneCurve();
	}
	else {
		pcNextCurve = &cNextIntersectionCurve.GetCurveTwoCurve();
	}

	MbCartPoint cCurCurveEdnPoint, cNextCurveStartPoint;

	if(true == bCurSense) {
		if(nullptr != pcCurCurve) {
			pcCurCurve->GetEndPoint(cCurCurveEdnPoint);
		}
	}
	else {
		if(nullptr != pcCurCurve) {
			pcCurCurve->GetStartPoint(cCurCurveEdnPoint);
		}
	}

	if(true == bNextSense) {
		if(nullptr != pcNextCurve) {
			pcNextCurve->GetStartPoint(cNextCurveStartPoint);
		}
	}
	else {
		if(nullptr != pcNextCurve) {
			pcNextCurve->GetEndPoint(cNextCurveStartPoint);
		}
	}

	double dDistnace = DBL_MAX;
	bConnectFlag = false;

	if(nullptr != pcConnectCurve) {
		pcConnectCurve[0] = pcCurCurve;
		pcConnectCurve[1] = pcNextCurve;
	}

	dDistnace = cCurCurveEdnPoint.DistanceToPoint(cNextCurveStartPoint);
	if(dDistnace < dTolerance) {
		bConnectFlag = true;
		return true;
	}

	return true;
}

bool C3dImport::CheckConectivity_V1(const MbOrientedEdge * pcCurEdge, const MbOrientedEdge * pcNextEdge,
	const MbSurface * pcBaseSurface, double dTolerance, bool & bConnectFlag, const MbCurve ** pcConectiCurve)
{
	const MbCurveEdge & cCurCurveEdge = pcCurEdge->GetCurveEdge();
	const MbCurveEdge & cNextCurveEdge = pcNextEdge->GetCurveEdge();

	const MbSurfaceIntersectionCurve & cCurIntersectionCurve = cCurCurveEdge.GetIntersectionCurve();
	const MbSurfaceIntersectionCurve & cNextIntersectionCurve = cNextCurveEdge.GetIntersectionCurve();

	bool bCurSense = pcCurEdge->GetOrientation();
	bool bNextSense = pcNextEdge->GetOrientation();

	const MbCurve * pcCurCurve[2];
	const MbCurve * pcNextCurve[2];

	pcCurCurve[0] = nullptr;
	pcCurCurve[1] = nullptr;

	pcNextCurve[0] = nullptr;
	pcNextCurve[1] = nullptr;

	// Seam Edge의 경우 Curve의 Base Surface가 같음.
	if(pcBaseSurface == cCurIntersectionCurve.GetSurfaceOne()) {
		pcCurCurve[0] = &cCurIntersectionCurve.GetCurveOneCurve();
	}
	if(pcBaseSurface == cCurIntersectionCurve.GetSurfaceTwo()) {
		pcCurCurve[1] = &cCurIntersectionCurve.GetCurveTwoCurve();
	}

	if(pcBaseSurface == cNextIntersectionCurve.GetSurfaceOne()) {
		pcNextCurve[0] = &cNextIntersectionCurve.GetCurveOneCurve();
	}
	if(pcBaseSurface == cNextIntersectionCurve.GetSurfaceTwo()) {
		pcNextCurve[1] = &cNextIntersectionCurve.GetCurveTwoCurve();
	}

	MbCartPoint cCurCurveEdnPoint[2], cNextCurveStartPoint[2];

	if(true == bCurSense) {
		if(nullptr != pcCurCurve[0]) {
			pcCurCurve[0]->GetEndPoint(cCurCurveEdnPoint[0]);
		}
		if(nullptr != pcCurCurve[1]) {
			pcCurCurve[1]->GetEndPoint(cCurCurveEdnPoint[1]);
		}
	}
	else {
		if(nullptr != pcCurCurve[0]) {
			pcCurCurve[0]->GetStartPoint(cCurCurveEdnPoint[0]);
		}
		if(nullptr != pcCurCurve[1]) {
			pcCurCurve[1]->GetStartPoint(cCurCurveEdnPoint[1]);
		}
	}

	if(true == bNextSense) {
		if(nullptr != pcNextCurve[0]) {
			pcNextCurve[0]->GetStartPoint(cNextCurveStartPoint[0]);
		}
		if(nullptr != pcNextCurve[1]) {
			pcNextCurve[1]->GetStartPoint(cNextCurveStartPoint[1]);
		}
	}
	else {
		if(nullptr != pcNextCurve[0]) {
			pcNextCurve[0]->GetEndPoint(cNextCurveStartPoint[0]);
		}
		if(nullptr != pcNextCurve[1]) {
			pcNextCurve[1]->GetEndPoint(cNextCurveStartPoint[1]);
		}
	}

	double dDistnace = DBL_MAX;
	bConnectFlag = false;

	// 사전에 시작점 끝점은 Curve 방향에 따라서 입력되어 있음.
	for(int nCurCurveIndex = 0; nCurCurveIndex < 2; nCurCurveIndex++) {
		for(int nNextCurveIndex = 0; nNextCurveIndex < 2; nNextCurveIndex++) {
			if(nullptr != pcCurCurve[nCurCurveIndex] && nullptr != pcNextCurve[nNextCurveIndex]) {
				dDistnace = cCurCurveEdnPoint[nCurCurveIndex].DistanceToPoint(cNextCurveStartPoint[nNextCurveIndex]);
				if(dDistnace < dTolerance) {
					if(nullptr != pcConectiCurve) {
						pcConectiCurve[0] = pcCurCurve[nCurCurveIndex];
						pcConectiCurve[1] = pcNextCurve[nNextCurveIndex];
					}
					bConnectFlag = true;
					return true;
				}
			}
		}
	}

	return true;
}

// 3-12. Topo CoEdge 변환. MbOrientedEdge를 생성함.
bool C3dImport::ConvertMbOrientedEdge(A3DTopoCoEdge * pcTopoCoEdge, const MbSurface * pcSurface, DmiOrientedEdge *& pcOrientedEdge)
{
	if(nullptr != pcTopoCoEdge) {
		RETURN_FALSE;
	}

	// 1. TopoCodeEdge의 Data값을 얻어온다.
	A3DTopoCoEdgeData cTopoCoEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, cTopoCoEdgeData);
	A3DStatus eResult = A3DTopoCoEdgeGet(pcTopoCoEdge, &cTopoCoEdgeData);
	if(A3D_SUCCESS != eResult) {
		ASSERT(false);
		Log(2, L"ConvertTopoCoEdge - A3DTopoCoEdgeGet Error", eResult);
		return false;
	}

	bool bOriLoopFlag = (1 == cTopoCoEdgeData.m_ucOrientationWithLoop) ? true : false;
	bool bOriUVLoopFlag = (1 == cTopoCoEdgeData.m_ucOrientationUVWithLoop) ? true : false;

	A3DTopoEdge * pcTopoEdge = cTopoCoEdgeData.m_pEdge;
	A3DTopoCoEdge * pcNeighborTopoCoEdge = cTopoCoEdgeData.m_pNeighbor;

	bool bNeighborOriLoopFlag = true;
	bool bNeighborOriUVLoopFlag = true;

	A3DTopoCoEdgeData cNeighborTopoCoEdgeData;
	if(nullptr != pcNeighborTopoCoEdge) {
		A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, cNeighborTopoCoEdgeData);
		A3DStatus eResult = A3DTopoCoEdgeGet(pcNeighborTopoCoEdge, &cNeighborTopoCoEdgeData);
		if(A3D_SUCCESS != eResult) {
			Log(2, L"ConvertTopoCoEdge - A3DTopoCoEdgeGet Error: %d", eResult);
			return false;
		}

		bNeighborOriLoopFlag = (1 == cNeighborTopoCoEdgeData.m_ucOrientationWithLoop) ? true : false;
		bNeighborOriUVLoopFlag = (1 == cNeighborTopoCoEdgeData.m_ucOrientationUVWithLoop) ? true : false;
	}

	// 2. MbCurveEdge를 검색해서 찾는 경우 그 값을 이용해서 바로 OrientedEdge를 생성한다.
	EdgeSPtr pcCurveEdge;
	A3DTopoEdgeData cTopoEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoEdgeData, cTopoEdgeData);
	eResult = A3DTopoEdgeGet(pcTopoEdge, &cTopoEdgeData);
	if(A3D_SUCCESS != eResult) {
		ASSERT(false);
		Log(2, L"ConvertTopoCoEdge - A3DTopoEdgeGet Error", eResult);
		return false;
	}

	// 3. 사전에 생성된 Curve Edge가 있는 경우 조건을 판단해서 Orietn Edge를 생성해서 돌려 보내준다.
	// 이웃 CoEdge를 이용해서 이웃 Orient Edge를 찾아와서 거기에 들어 있는 CurveEdge를 이용해서, Orient Edge를 생성한다.
	if(nullptr != pcNeighborTopoCoEdge) {
		// 이웃 ConvCoEdge의 이웃 TopoCoEdge가 변환중인 TopoCoEdge와 같은 경우에는 CurveEdge를 이용한다.
		// 다중 3D Curve가 생성되는 경우가 있기 때문에 확인해야 함.
		if(cNeighborTopoCoEdgeData.m_pNeighbor == pcTopoCoEdge) {
			if(true == m_mpcCurveEdgeMap.Lookup((DWORD_PTR) pcTopoEdge, pcCurveEdge)) {
				MbSurfaceIntersectionCurve * pcInterCurve = (MbSurfaceIntersectionCurve *) &pcCurveEdge->GetIntersectionCurve();
				A3DTopoCoEdgeGet(nullptr, &cTopoCoEdgeData);
				pcOrientedEdge = new DmiOrientedEdge(*pcCurveEdge, bOriLoopFlag, false);
				return true;
			}
		}
	}

	// 4. Neighbor Surface 정보를 찾아온다. (pcNeighborTopoCoEdge를 이용해서 검색을 실시한다.)
	const A3DSurfBase * pcNeighborSurfBase = nullptr;
	const A3DTopoFace * pcNeighborTopoFace = nullptr;
	SurfaceSPtr pcNeighborSurface;

	if(nullptr != pcNeighborTopoCoEdge) {
		// 4-1. A3D 함수들을 이용해서 관련 정보를 수집한다. TopoCoEdge에서 A3DSurfBase를 찾아옴.
		if(false == GetSurfBaseFromTopoCoEdge(pcNeighborTopoCoEdge, pcNeighborSurfBase)) {
			return false;
		}

		// 4-2. 이웃 CoEdge의 Surface를 찾는다.
		if(false == GetMbSurface(pcNeighborSurfBase, pcNeighborSurface)) {
			ASSERT(false);
		}
	}

	// ----- 5. MbCurveEdge Create -----
	// 5-1. Plane Curve 생성
	PlaneCurveSPtr pcPlaneCurve;
	if(nullptr != cTopoCoEdgeData.m_pUVCurve) {
		if(false == ConvCurveBase::ConvertPlaneCurve(cTopoCoEdgeData.m_pUVCurve, pcSurface, m_dContextScale, pcPlaneCurve)) {
			ASSERT(false);
			return false;
		}
	}
	else {
		return false;
	}

	/*
		else if(nullptr != cTopoEdgeData.m_p3dCurve) {
			if(false == ConvCurveBase::ConvertProjectionPlaneCurve(cTopoEdgeData.m_p3dCurve, (MbSurface*)pcSurface, m_dContextScale, pcPlaneCurve)) {
				ASSERT(false);
				return false;
			}
		}
	*/

	// 5-2. Neighbor Plane Curve 생성
	PlaneCurveSPtr pcNeighborPlaneCurve;
	if(nullptr != pcNeighborTopoCoEdge && nullptr != pcNeighborSurface) {
		if(nullptr != cNeighborTopoCoEdgeData.m_pUVCurve) {
			if(false == ConvCurveBase::ConvertPlaneCurve(cNeighborTopoCoEdgeData.m_pUVCurve, pcNeighborSurface, m_dContextScale, pcNeighborPlaneCurve)) {
				ASSERT(false);
			}
		}
		/*
				else {
					A3DTopoEdgeData cNeighborTopoEdgeData;
					A3D_INITIALIZE_DATA(A3DTopoEdgeData, cNeighborTopoEdgeData);
					eResult = A3DTopoEdgeGet(cNeighborTopoCoEdgeData.m_pEdge, &cNeighborTopoEdgeData);
					if(A3D_SUCCESS != eResult) {
						ASSERT(false);
						Log(2, L"ConvertTopoCoEdge - A3DTopoEdgeGet Error", eResult);
						return false;
					}

					bool bStatus = ConvCurveBase::ConvertProjectionPlaneCurve(cNeighborTopoEdgeData.m_p3dCurve, (MbSurface *) pcSurface, m_dContextScale, pcNeighborPlaneCurve);
					A3DTopoEdgeGet(nullptr, &cNeighborTopoEdgeData);

					if(false == bStatus) {
						return false;
					}
				}
		*/
	}

	// 6. Vertex 정보를 검색하고, 저장된 값이 없으면 생성한다.
	A3DTopoVertex * pcStartTopoVertex = cTopoEdgeData.m_pStartVertex;
	A3DTopoVertex * pcEndTopoVertex = cTopoEdgeData.m_pEndVertex;

	VertexSPtr pcStartVertex, pcEndVertex;

	if(nullptr != pcStartTopoVertex && nullptr != pcEndTopoVertex) {
		// Start Vertex
		if(false == FindTopoVertex(pcStartTopoVertex, pcStartVertex)) {
			if(true == ConvertTopoVertex(pcStartTopoVertex, pcStartVertex)) {
				AddTopoVertexToMap(pcStartTopoVertex, pcStartVertex);
			}
		}

		// End Vertex
		if(false == FindTopoVertex(pcEndTopoVertex, pcEndVertex)) {
			if(true == ConvertTopoVertex(pcEndTopoVertex, pcEndVertex)) {
				AddTopoVertexToMap(pcEndTopoVertex, pcEndVertex);
			}
		}
	}

	if(bOriLoopFlag != bOriUVLoopFlag) {
		if(nullptr != pcPlaneCurve) {
			pcPlaneCurve->Inverse();
		}
	}

	if(bNeighborOriLoopFlag != bNeighborOriUVLoopFlag) {
		if(nullptr != pcNeighborPlaneCurve) {
			pcNeighborPlaneCurve->Inverse();
		}
	}

	/*
		if(false == bOriUVLoopFlag) {
			if(nullptr != pcPlaneCurve) {
				pcPlaneCurve->Inverse();
			}
		}

		if(false == bNeighborOriUVLoopFlag) {
			if(nullptr != pcNeighborPlaneCurve) {
				pcNeighborPlaneCurve->Inverse();
			}
		}
	*/
	bool bFlag1 = true;
	bool bFlag2 = true;

	MbCartPoint cPoint[2];
	pcPlaneCurve->GetStartPoint(cPoint[0]);
	pcPlaneCurve->GetEndPoint(cPoint[1]);

	// 7. 이웃 CoEdge에서 찾아온 Curve 정보를 이용해서 Intersection Curve를 생성
	IntersectionCurveSPtr pcInterCurve;
	if(nullptr != pcNeighborSurface && nullptr != pcNeighborPlaneCurve) { // 이웃 Face가 있는 경우 처리

		pcInterCurve = new MbSurfaceIntersectionCurve(*pcSurface, *pcPlaneCurve,
			*pcNeighborSurface, *pcNeighborPlaneCurve, MbeCurveBuildType::cbt_Ordinary, bFlag1, bFlag2);

		// cgt_Stitch를 사용하면 않됨.
// 		if(pcNeighborSurface == pcSurface) {
// 			pcInterCurve->SetGlueType(cgt_Stitch);
// 		}
	}
	else {
		//  Convert Ri에서 시작된 Face들은 Face Set이거나 Solid 구성이 아니기 때문에, 이웃 CorEdge가 없다.
		pcInterCurve = new MbSurfaceIntersectionCurve(*pcSurface, *pcPlaneCurve,
			*pcSurface, *pcPlaneCurve, MbeCurveBuildType::cbt_Ordinary, bFlag1, bFlag2);
	}

//  	if(bOriLoopFlag != bOriUVLoopFlag) {
//  		pcInterCurve->Inverse();
//  	}

//  	if(false == bOriLoopFlag) {
//  		pcInterCurve->Inverse();
//  	}

	// UG File에서는 Vertex값이 NULL인 경우가 나옴.
	if(nullptr == pcStartVertex || nullptr == pcEndVertex) {
		pcCurveEdge = new MbCurveEdge(*pcInterCurve, true);
	}
	else {
		pcCurveEdge = new MbCurveEdge(*pcStartVertex, *pcEndVertex, *pcInterCurve, true);
	}

// 	if(true == bOriLoopFlag) {
// 		pcCurveEdge = new MbCurveEdge(*pcStartVertex, *pcEndVertex, *pcInterCurve, true);
// 	}
// 	else {
// 		pcCurveEdge = new MbCurveEdge(*pcEndVertex, *pcStartVertex, *pcInterCurve, true);
// 	}

	//pcCurveEdge = new MbCurveEdge(*pcStartVertex, *pcEndVertex, *pcInterCurve, bOriLoopFlag);
	m_mpcCurveEdgeMap.SetAt((DWORD_PTR) pcTopoEdge, pcCurveEdge);

	pcOrientedEdge = new DmiOrientedEdge(*pcCurveEdge, bOriLoopFlag, true);

	return true;
}

// 3-13. Coordinate System 변환
bool C3dImport::ConvertRiCoordinateSystem(A3DRiRepresentationItem * pcRiRepItem, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy)
{
	A3DRiCoordinateSystem * pcRiCoordSystem = static_cast<A3DRiCoordinateSystem *>(pcRiRepItem);
	if(nullptr == pcRiCoordSystem) {
		return false;
	}

	A3DRiCoordinateSystemData cCoordinateSystemData;
	A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, cCoordinateSystemData);
	A3DStatus nResult = A3DRiCoordinateSystemGet(pcRiCoordSystem, &cCoordinateSystemData);
	if(A3D_SUCCESS != nResult) {
		return false;
	}

	A3DMiscTransformation * pcTransformation = cCoordinateSystemData.m_pTransformation;
	if(nullptr == pcTransformation) {
		A3DRiCoordinateSystemGet(nullptr, &cCoordinateSystemData);
		return false;
	}

	MbMatrix3D cMatrix;
	GetMatrix(pcTransformation, cMatrix);

	MbAssistingItem * pcAssistingItem = new MbAssistingItem(MbPlacement3D(cMatrix));
	pcParentAssy->AddItem(*pcAssistingItem);

	A3DRiCoordinateSystemGet(nullptr, &cCoordinateSystemData);

	return true;
}

// 3-14. Poly Brep model 변환
bool C3dImport::ConvertRiPolyBrepModel(A3DRiRepresentationItem * pcRiRepItem, A3DMiscCascadedAttributes * pcParentAttr,
	AssemblySPtr & pcParentAssy)
{
	MbMesh * pcMesh = nullptr;
	if(true == m_mpcMeshMap.Lookup((DWORD_PTR) pcRiRepItem, pcMesh)) {
		Log(2, L"ConvertRiPolyBrepModel-Instance: [%s]", LogHexStr((DWORD_PTR) pcRiRepItem));
		pcParentAssy->AddItem(*pcMesh);
		return true;
	}

	CString strName;
	GetName(pcRiRepItem, strName);

	if(true == strName.IsEmpty()) {
		strName.Format(L"Solid %d", m_nSolidId);
	}

	m_nSolidId++;

	Log(2, L"ConvertRiPolyBrepModel: %s, [%s]", strName, LogHexStr((DWORD_PTR) pcRiRepItem));

	A3DRiRepresentationItemData cRiRepItemData;
	A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, cRiRepItemData);
	A3DStatus nResult = A3DRiRepresentationItemGet(pcRiRepItem, &cRiRepItemData);
	if(nResult != A3D_SUCCESS) {
		return false;
	}

	A3DRiPolyBrepModelData cPolyBrepModelData;
	A3D_INITIALIZE_DATA(A3DRiPolyBrepModelData, cPolyBrepModelData);
	nResult = A3DRiPolyBrepModelGet(pcRiRepItem, &cPolyBrepModelData);
	if(nResult != A3D_SUCCESS) {
		LogDecreaseTabIndex(2);
		return false;
	}

	LogIncreaseTabIndex(2);

	bool bStatus = false;
	if(nullptr != cRiRepItemData.m_pTessBase) {
		bStatus = ConvertTessBase(cRiRepItemData, pcRiRepItem, pcParentAttr, pcMesh);
	}

	LogDecreaseTabIndex(2);

	if(nullptr != pcMesh) {
		pcParentAssy->AddItem(*pcMesh);
		m_mpcMeshMap.SetAt((DWORD_PTR) pcRiRepItem, pcMesh);
	}

	A3DRiRepresentationItemGet(nullptr, &cRiRepItemData);
	A3DRiPolyBrepModelGet(nullptr, &cPolyBrepModelData);

	return bStatus;
}

//== 4. Tessellation Mode 함수 =========================================================================

// 4-1. B-Rep Model의 Tessellation Data 변환
bool C3dImport::ConvertTessBase(A3DRiRepresentationItemData & cRiRepItemData, A3DRiRepresentationItem * pcInputRiBrepModel, A3DMiscCascadedAttributes * pcParentAttr,
	MbMesh *& pcMesh)
{
	A3DTessBase * pcTessBase = cRiRepItemData.m_pTessBase;
	if(nullptr == pcTessBase) {
		return false;
	}

	CString strName;
	GetName(pcInputRiBrepModel, strName);

	// 	Log(2, L"ConvertTessBase: %s, [%s]", strName, LogHexStr((DWORD_PTR) pcInputRiBrepModel));
	// 	LogIncreaseTabIndex(2);

	A3DTessBaseData cTessBaseData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, cTessBaseData);
	A3DStatus nStatus = A3DTessBaseGet(pcTessBase, &cTessBaseData);
	if(A3D_SUCCESS != nStatus) {
		return false;
	}

	A3DEEntityType eType;
	nStatus = A3DEntityGetType(pcTessBase, &eType);
	if(A3D_SUCCESS != nStatus) {
		return false;
	}

	switch(eType)
	{
		case kA3DTypeTess3D:
			ConvertTess3d(pcTessBase, pcInputRiBrepModel, pcParentAttr, &cTessBaseData, cRiRepItemData.m_pCoordinateSystem, pcMesh);
			break;

			// 		case kA3DTypeTess3DWire:
			// 			BuildWires(pcTessBase, pRepItem, cImportInfo, pcParentAttribute, &sData, pCoordinateSystem);
			// 			break;
			// 
			// 		case kA3DTypeTessMarkup:
			// 			BuildMarkup(pcTessBase, pRepItem, cImportInfo, pcParentAttribute, &sData);
			// 			break;
	}

	A3DTessBaseGet(nullptr, &cTessBaseData);

	//	LogDecreaseTabIndex(2);

	return true;
}

// 4-2. Tess3d 변환
bool C3dImport::ConvertTess3d(A3DTess3D * pcTess3d, A3DRiRepresentationItem * pcRiRepItem,
	A3DMiscCascadedAttributes * pcParentAttr, A3DTessBaseData * pcTessBaseData, A3DRiCoordinateSystem * pcCoordinateSystem, MbMesh *& pcMesh)
{
	double dUnitScale = 1.0;	// Tessellation은 1:1 비율로 들어온다 

	A3DTess3DData cTess3dData;
	A3D_INITIALIZE_DATA(A3DTess3DData, cTess3dData);

	A3DInt32 nStatus = A3DTess3DGet(pcTess3d, &cTess3dData);
	if(A3D_SUCCESS != nStatus) {
		return false;
	}

	A3DUns32 nFacesCount = cTess3dData.m_uiFaceTessSize;

	if(0 == nFacesCount) {
		A3DTess3DGet(nullptr, &cTess3dData);
		return false;
	}

	// Attribute of Cascade
	A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[0];
	A3DMiscCascadedAttributes * pcAttribute;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributesTessFace(pcRiRepItem, pcTess3d, &cTessFaceData, 0, pcParentAttr, &pcAttribute, &cAttrsData);

	const double cBigValue = 1.0e+12;
	bool bStrange = false;

	double dScale = 1.0;// cImportInfo.dModelScale * cImportInfo.dTessellationScale;

	A3DUns32 nTextureCoordCount = cTess3dData.m_uiTextureCoordSize / 2;
	/*
		HPoint * const texcor = new HPoint[nTextureCoordCount];

		for(UINT i = 0; i < nTextureCoordCount; ++i) {
			texcor[i].Set(cTess3dData.m_pdTextureCoords[i * 2], cTess3dData.m_pdTextureCoords[i * 2 + 1], 0);
		}
	*/

	A3DUns32 * pnTriangleIndices = cTess3dData.m_puiTriangulatedIndexes;

	UINT nTriangleFaceCount = 0;

	pcMesh = new MbMesh();

	COLORREF cColor;
	if(true == GetAttributeColor(cAttrsData, cColor)) {
		pcMesh->SetColor(cColor);
	}

	float fTransparency = 1.0f;
	if(true == GetAttributeTransparency(cAttrsData, fTransparency)) {
		float a, d, sp, sh, t, e;
		pcMesh->GetVisual(a, d, sp, sh, t, e);
		pcMesh->SetVisual(a, d, sp, sh, fTransparency, e);
	}

	// 	maPointIndexMap.InitHashTable(257);
	// 	maNormalIndexMap.InitHashTable(257);

	for(A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; ++nFaceIndex)
	{
		cTessFaceData = cTess3dData.m_psFaceTessData[nFaceIndex];

		A3DUns32 nTriSizeIndex = 0;	// 한 Triangle Type당 하나씩
		A3DUns32 nTriStartIndex = cTessFaceData.m_uiStartTriangulated;
		A3DUns32 nTriColorIndex = 0;

		// 3DX에서 돌려주는 Face Index는 Tess3d 전체의 Index를 돌려주는데, Grid를 구성하기 위해서는 부분적인 Point들의 Index가 필요함.
		// 별도의 순서를 갖는 Face Index를 구성하기 위해서 Map을 사용한다.
		MeshIndexMap maPointIndexMap;
		MeshIndexMap maNormalIndexMap;
		// 		maPointIndexMap.InitHashTable(1257);
		// 		maNormalIndexMap.InitHashTable(1257);

		MeshTrianglesVector cTrianglesVector;

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
		{
			nTriangleFaceCount += ConvertTessFaceDataTriangle(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, cTrianglesVector);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
		{
			nTriangleFaceCount += ConvertTessFaceDataTriangleFan(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, cTrianglesVector);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
		{
			nTriangleFaceCount += ConvertTessFaceDataTriangleStripe(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				nTriColorIndex, maPointIndexMap, maNormalIndexMap, cTrianglesVector);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
		{
			nTriangleFaceCount += ConvertTessFaceDataTriangleOneNormal(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, cTrianglesVector);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
		{
			nTriangleFaceCount += ConvertTessFaceDataTriangleFanOneNormal(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, cTrianglesVector);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
		{
			nTriangleFaceCount += ConvertTessFaceDataTriangleStripeOneNormal(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, cTrianglesVector);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataPolyfaceTextured)
			continue;

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured)
		{
			// nTriangleFaceCount += InsertFace_TriangleTextured(cTessFaceData, pcPoints, pcNormals, texcor, pnTriangleIndices, nTriangleSizeIndex, nTriangleStartIndex, cCreatedShell);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanTextured)
			continue;

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeTextured)
		{
			// nTriangleFaceCount += InsertFace_TriangleStripeTextured(cTessFaceData, pcPoints, pcNormals, texcor, pnTriangleIndices, nTriangleSizeIndex, nTriangleStartIndex, cCreatedShell);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormalTextured)
			continue;

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured)
			continue;

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
			continue;

		if(false == cTrianglesVector.empty()) {
			MbGrid * pcGrid = pcMesh->AddGrid();

			CreateAndPushCascadedAttributesTessFace(pcRiRepItem, pcTess3d, &cTessFaceData, 0, pcParentAttr, &pcAttribute, &cAttrsData);

			COLORREF cFaceColor;
			if(true == GetAttributeColor(cAttrsData, cFaceColor)) {
				pcGrid->SetColor(cFaceColor);
			}

			CreateMbGrid(pcTessBaseData->m_pdCoords, cTess3dData.m_pdNormals, dScale, maPointIndexMap, maNormalIndexMap, cTrianglesVector, pcGrid);
			CreateMbGridLoop(pcTessBaseData->m_pdCoords, cTess3dData, cTessFaceData, maPointIndexMap, pcGrid);
		}
	}

	return true;
}

// 4-2-1. Triangle Face Data 변환
UINT C3dImport::ConvertTessFaceDataTriangle(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriangleIndices, A3DUns32 & nTriangleSizeIndex, A3DUns32 & nTriangleStartIndex,
	MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, MeshTrianglesVector & cTrianglesVector)
{
	A3DUns32 nTriangleCount = cTessFaceData.m_puiSizesTriangulated[nTriangleSizeIndex++];

	A3DUns32 nFacePointIndex[3];
	A3DUns32 nFaceNormalIndex[3];

	MeshIndexMap maLocalNormalIndexMap;

	for(A3DUns32 nIndex = 0; nIndex < nTriangleCount; ++nIndex)
	{
		nFacePointIndex[0] = pnTriangleIndices[nTriangleStartIndex + 1] / 3;
		nFaceNormalIndex[0] = pnTriangleIndices[nTriangleStartIndex] / 3;
		nTriangleStartIndex += 2;

		MatchVertexNormal(maLocalNormalIndexMap, nFacePointIndex[0], nFaceNormalIndex[0]);

		nFacePointIndex[1] = pnTriangleIndices[nTriangleStartIndex + 1] / 3;
		nFaceNormalIndex[1] = pnTriangleIndices[nTriangleStartIndex] / 3;
		nTriangleStartIndex += 2;

		MatchVertexNormal(maLocalNormalIndexMap, nFacePointIndex[1], nFaceNormalIndex[1]);

		nFacePointIndex[2] = pnTriangleIndices[nTriangleStartIndex + 1] / 3;
		nFaceNormalIndex[2] = pnTriangleIndices[nTriangleStartIndex] / 3;
		nTriangleStartIndex += 2;

		MatchVertexNormal(maLocalNormalIndexMap, nFacePointIndex[2], nFaceNormalIndex[2]);

		ConvertTriangleVector(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, cTrianglesVector);
	}

	return nTriangleCount;
}

// 4-2-2. Triangle Fan Data 변환
UINT C3dImport::ConvertTessFaceDataTriangleFan(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, c3d::MeshTrianglesVector & cTrianglesVector)
{
	A3DUns32 nFacePointIndex[3];
	A3DUns32 nFaceNormalIndex[3];

	A3DUns32 nTriangleCount = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex++];
	A3DUns32 nTriFaceCount = 0;

	for(A3DUns32 stripe = 0; stripe < nTriangleCount; stripe++)
	{
		A3DUns32 nPointCount = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex++];
		A3DUns32 nTriStartIndex2 = nTriStartIndex;

		for(A3DUns32 nIndex = 0; nIndex < nPointCount - 1; ++nIndex)
		{
			if(!(nIndex % 2) || 1)
			{
				nFacePointIndex[0] = pnTriIndices[nTriStartIndex + 1] / 3;
				nFaceNormalIndex[0] = pnTriIndices[nTriStartIndex] / 3;

				nFacePointIndex[1] = pnTriIndices[nTriStartIndex2 + 1] / 3;
				nFaceNormalIndex[1] = pnTriIndices[nTriStartIndex2] / 3;

				nTriStartIndex2 += 2;

				nFacePointIndex[2] = pnTriIndices[nTriStartIndex2 + 1] / 3;
				nFaceNormalIndex[2] = pnTriIndices[nTriStartIndex2] / 3;
			}
			else
			{
				nFacePointIndex[2] = pnTriIndices[nTriStartIndex + 1] / 3;
				nFaceNormalIndex[2] = pnTriIndices[nTriStartIndex] / 3;

				nFacePointIndex[1] = pnTriIndices[nTriStartIndex2 + 1] / 3;
				nFaceNormalIndex[1] = pnTriIndices[nTriStartIndex2] / 3;

				nTriStartIndex2 += 2;

				nFacePointIndex[0] = pnTriIndices[nTriStartIndex2 + 1] / 3;
				nFaceNormalIndex[0] = pnTriIndices[nTriStartIndex2] / 3;
			}

			ConvertTriangleVector(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, cTrianglesVector);
			nTriFaceCount++;
		}

		nTriStartIndex += nPointCount * 2;
	}

	return nTriFaceCount;
}

// 4-2-3. Triangle Stripe Data 변환
UINT C3dImport::ConvertTessFaceDataTriangleStripe(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	A3DUns32 & nTriColorIndex, MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, MeshTrianglesVector & cTrianglesVector)
{
	A3DUns32 nFacePointIndex[3];
	A3DUns32 nFaceNormalIndex[3];

	// HPoint face4[3];

	A3DUns32 nStripesCount = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex++];
	A3DUns32 nTriFaceCount = 0;

	bool hasVertexColor = false; // INT2bool(cTessFaceData.m_uiRGBAVerticesSize);
	if(0 < cTessFaceData.m_uiRGBAVerticesSize) {
		hasVertexColor = true;
	}

	for(A3DUns32 nStripeIndex = 0; nStripeIndex < nStripesCount; nStripeIndex++)
	{
		A3DUns32 nPointCount = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex];
		int nColorIndex = nTriColorIndex;
		int nStartIndex = nTriStartIndex;

		bool single_normal = false;								// flag indicating if there are uniform vertex normals per tri-strip

		// determine if we have a normal per vertex or just one per triangle strip
		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal
			|| cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
		{
			if(cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex] & kA3DTessFaceDataNormalSingle)
			{
				single_normal = true;
			}
		}

		nTriSizeIndex++;

		// TRACE(L"Point Count, StartIndex, vcpos2 : %d, %d, %d\r\n", nPointCount, nLocalTriangleStartIndex, vcpos2);

		for(A3DUns32 i = 0; i < nPointCount - 2; i++)
		{
			if(!(i % 2))
			{
				nFacePointIndex[0] = pnTriIndices[nStartIndex + 1] / 3;
				nFaceNormalIndex[0] = pnTriIndices[nStartIndex] / 3;
				nStartIndex += 2;

				nFacePointIndex[1] = pnTriIndices[nStartIndex + 1] / 3;
				nFaceNormalIndex[1] = pnTriIndices[nStartIndex] / 3;
				nStartIndex += 2;

				nFacePointIndex[2] = pnTriIndices[nStartIndex + 1] / 3;
				nFaceNormalIndex[2] = pnTriIndices[nStartIndex] / 3;
				nStartIndex -= 2;

				if(hasVertexColor) {
					//AddVertexColor(face4, cTessFaceData.m_pucRGBAVertices, nColorIndex, false, true);
				}
			}
			else
			{
				nFacePointIndex[2] = pnTriIndices[nStartIndex + 1] / 3;
				nFaceNormalIndex[2] = pnTriIndices[nStartIndex] / 3;
				nStartIndex += 2;

				nFacePointIndex[1] = pnTriIndices[nStartIndex + 1] / 3;
				nFaceNormalIndex[1] = pnTriIndices[nStartIndex] / 3;
				nStartIndex += 2;

				nFacePointIndex[0] = pnTriIndices[nStartIndex + 1] / 3;
				nFaceNormalIndex[0] = pnTriIndices[nStartIndex] / 3;
				nStartIndex -= 2;

				if(hasVertexColor) {
					//AddVertexColor(face4, cTessFaceData.m_pucRGBAVertices, nColorIndex, true, true);
				}
			}

			/*
						// m_VColors가 nullptr 인 경우가 있다 -_-;
						if(hasVertexColor && cCreatedShell.m_VColors) {
							cCreatedShell.AddNextFaceWithDistinctNormalsAndColor(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, face4, 3);
						}
						else {
							cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
						}
			*/
			ConvertTriangleVector(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, cTrianglesVector);

			nTriFaceCount++;
		}

		nTriStartIndex += nPointCount * 2;
		nTriColorIndex += nPointCount * 3;
	}

	return nTriFaceCount;
}

// 4-2-4. Triangle Stripe One Normal 변환
UINT C3dImport::ConvertTessFaceDataTriangleOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, MeshTrianglesVector & cTrianglesVector)
{
	A3DUns32 nFacePointIndex[3];
	A3DUns32 nFaceNormalIndex[3];

	A3DUns32 nTriCount = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex++];
	//if (numtriangles>1)
	//	numtriangles = numtriangles;

	for(A3DUns32 i = 0; i < nTriCount; ++i)
	{
		int nStartIndex = nTriStartIndex;
		++nTriStartIndex;
		nFacePointIndex[0] = pnTriIndices[nTriStartIndex] / 3;
		nFaceNormalIndex[0] = pnTriIndices[nStartIndex] / 3;

		++nTriStartIndex;
		nFacePointIndex[1] = pnTriIndices[nTriStartIndex] / 3;
		nFaceNormalIndex[1] = pnTriIndices[nStartIndex] / 3;

		++nTriStartIndex;
		nFacePointIndex[2] = pnTriIndices[nTriStartIndex] / 3;
		nFaceNormalIndex[2] = pnTriIndices[nStartIndex] / 3;

		++nTriStartIndex;

		ConvertTriangleVector(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, cTrianglesVector);
	}

	return nTriCount;
}

// 4-2-5. Triangle Fan One Normal Data 변환
UINT C3dImport::ConvertTessFaceDataTriangleFanOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, MeshTrianglesVector & cTrianglesVector)
{
	A3DUns32 nFacePointIndex[3];
	A3DUns32 nFaceNormalIndex[3];

	bool bNormalSingleFlag;
	A3DUns32 nStripesCount = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex++];
	A3DUns32 nTriCount = 0;

	for(A3DUns32 nStripeIndex = 0; nStripeIndex < nStripesCount; ++nStripeIndex)
	{
		if(cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex] & kA3DTessFaceDataNormalSingle) {
			bNormalSingleFlag = true;
		}
		else {
			bNormalSingleFlag = false;
		}

		int nPointsCount = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex] & ~kA3DTessFaceDataNormalSingle;
		++nTriSizeIndex;

		A3DUns32 nStartIndex = nTriStartIndex;
		if(!bNormalSingleFlag)
		{
			nTriStartIndex += 2;
			for(int i = 0; i < nPointsCount - 1; ++i)
			{
				if(!(i % 2))
				{
					nFacePointIndex[0] = pnTriIndices[nTriStartIndex + 1] / 3;
					nFaceNormalIndex[0] = pnTriIndices[nTriStartIndex] / 3;
					nFacePointIndex[1] = pnTriIndices[nStartIndex + 1] / 3;
					nFaceNormalIndex[1] = pnTriIndices[nStartIndex] / 3;
					nStartIndex += 2;
					nFacePointIndex[2] = pnTriIndices[nStartIndex + 1] / 3;
					nFaceNormalIndex[2] = pnTriIndices[nStartIndex] / 3;
				}
				else
				{
					nFacePointIndex[2] = pnTriIndices[nTriStartIndex + 1] / 3;
					nFaceNormalIndex[2] = pnTriIndices[nTriStartIndex] / 3;
					nFacePointIndex[1] = pnTriIndices[nStartIndex + 1] / 3;
					nFaceNormalIndex[1] = pnTriIndices[nStartIndex] / 3;
					nStartIndex += 2;
					nFacePointIndex[0] = pnTriIndices[nStartIndex + 1] / 3;
					nFaceNormalIndex[0] = pnTriIndices[nStartIndex] / 3;
				}

				ConvertTriangleVector(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, cTrianglesVector);
				//cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
				nTriCount++;
			}
			nTriStartIndex += nPointsCount * 2;
		}
		else
		{
			nStartIndex += 2;
			for(int i = 0; i < nPointsCount - 2; ++i)
			{
				if(!(i % 2) || 1)
				{
					nFacePointIndex[0] = pnTriIndices[nTriStartIndex + 1] / 3;
					nFaceNormalIndex[0] = pnTriIndices[nTriStartIndex] / 3;
					nFacePointIndex[1] = pnTriIndices[nStartIndex] / 3;
					nFaceNormalIndex[1] = pnTriIndices[nTriStartIndex] / 3;
					++nStartIndex;
					nFacePointIndex[2] = pnTriIndices[nStartIndex] / 3;
					nFaceNormalIndex[2] = pnTriIndices[nTriStartIndex] / 3;
				}
				else
				{
					nFacePointIndex[2] = pnTriIndices[nTriStartIndex + 1] / 3;
					nFaceNormalIndex[2] = pnTriIndices[nTriStartIndex] / 3;
					nFacePointIndex[1] = pnTriIndices[nStartIndex] / 3;
					nFaceNormalIndex[1] = pnTriIndices[nTriStartIndex] / 3;
					++nStartIndex;
					nFacePointIndex[0] = pnTriIndices[nStartIndex] / 3;
					nFaceNormalIndex[0] = pnTriIndices[nTriStartIndex] / 3;
				}

				ConvertTriangleVector(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, cTrianglesVector);
				//cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
				nTriCount++;
			}
			nTriStartIndex += (nPointsCount + 1);
		}
	}

	return nTriCount;
}

// 4-2-6. Triangle Stripe One Normal Data 변환
UINT C3dImport::ConvertTessFaceDataTriangleStripeOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap, MeshTrianglesVector & cTrianglesVector)
{
	A3DUns32 nFacePointIndex[3];
	A3DUns32 nFaceNormalIndex[3];

	UINT nTriCount = 0;

	//size = size & ~kA3DTessFaceDataWireIsNotDrawn;
	bool bNormalSingleFlag;
	A3DUns32 numstripes = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex++];
	for(A3DUns32 stripe = 0; stripe < numstripes; ++stripe)
	{
		if(cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex] & kA3DTessFaceDataNormalSingle) {
			bNormalSingleFlag = true;
		}
		else {
			bNormalSingleFlag = false;
		}

		A3DUns32 numpoints = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex] & ~kA3DTessFaceDataNormalSingle;
		++nTriSizeIndex;
		A3DUns32 tpos2 = nTriStartIndex;
		if(!bNormalSingleFlag)
		{
			for(A3DUns32 i = 0; i < numpoints - 2; ++i)
			{
				if(!(i % 2))
				{
					nFacePointIndex[0] = pnTriIndices[tpos2 + 1] / 3;
					nFaceNormalIndex[0] = pnTriIndices[tpos2] / 3;
					tpos2 += 2;
					nFacePointIndex[1] = pnTriIndices[tpos2 + 1] / 3;
					nFaceNormalIndex[1] = pnTriIndices[tpos2] / 3;
					tpos2 += 2;
					nFacePointIndex[2] = pnTriIndices[tpos2 + 1] / 3;
					nFaceNormalIndex[2] = pnTriIndices[tpos2] / 3;
					tpos2 -= 2;
				}
				else
				{
					nFacePointIndex[2] = pnTriIndices[tpos2 + 1] / 3;
					nFaceNormalIndex[2] = pnTriIndices[tpos2] / 3;
					tpos2 += 2;
					nFacePointIndex[1] = pnTriIndices[tpos2 + 1] / 3;
					nFaceNormalIndex[1] = pnTriIndices[tpos2] / 3;
					tpos2 += 2;
					nFacePointIndex[0] = pnTriIndices[tpos2 + 1] / 3;
					nFaceNormalIndex[0] = pnTriIndices[tpos2] / 3;
					tpos2 -= 2;
				}

				ConvertTriangleVector(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, cTrianglesVector);
				//cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
				nTriCount++;
			}

			nTriStartIndex += numpoints * 2;
		}
		else
		{
			tpos2++;
			for(A3DUns32 i = 0; i < numpoints - 2; ++i)
			{
				if(!(i % 2))
				{
					nFacePointIndex[0] = pnTriIndices[tpos2] / 3;
					nFaceNormalIndex[0] = pnTriIndices[nTriStartIndex] / 3;
					tpos2++;
					nFacePointIndex[1] = pnTriIndices[tpos2] / 3;
					nFaceNormalIndex[1] = pnTriIndices[nTriStartIndex] / 3;
					tpos2++;
					nFacePointIndex[2] = pnTriIndices[tpos2] / 3;
					nFaceNormalIndex[2] = pnTriIndices[nTriStartIndex] / 3;
					tpos2--;
				}
				else
				{
					nFacePointIndex[2] = pnTriIndices[tpos2] / 3;
					nFaceNormalIndex[2] = pnTriIndices[nTriStartIndex] / 3;
					tpos2++;
					nFacePointIndex[1] = pnTriIndices[tpos2] / 3;
					nFaceNormalIndex[1] = pnTriIndices[nTriStartIndex] / 3;
					tpos2++;
					nFacePointIndex[0] = pnTriIndices[tpos2] / 3;
					nFaceNormalIndex[0] = pnTriIndices[nTriStartIndex] / 3;
					tpos2--;
				}

				ConvertTriangleVector(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, cTrianglesVector);
				//cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
				nTriCount++;
			}

			nTriStartIndex += (numpoints + 1);
		}
	}

	return nTriCount;
}

// 4-2-7. Tess3d에 들어있는 Wireframe Data 변환
bool C3dImport::CreateMbGridLoop(double * pcPoints, A3DTess3DData & cTess3dData, A3DTessFaceData & cTessFaceData,
	MeshIndexMap & maPointIndexMap, MbGrid * pcGrid)
{
	int nPointIndex = 0;
	int nStartWireIndex = cTessFaceData.m_uiStartWire;

	for(A3DUns32 nIndex = 0; nIndex < cTessFaceData.m_uiSizesWiresSize; ++nIndex)
	{
		A3DUns32 nSize = cTessFaceData.m_puiSizesWires[nIndex] & ~(kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);
		A3DUns32 nFlags = cTessFaceData.m_puiSizesWires[nIndex] & (kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);

		// 그려지지 않음
		if(nFlags & kA3DTessFaceDataWireIsNotDrawn) {
			nPointIndex += nSize;
			continue;
		}

		MbGridLoop * pcGridLoop = new MbGridLoop();

		for(A3DUns32 nWireIndex = 0; nWireIndex < nSize; nWireIndex++) {
			A3DUns32 nWirePntIdx = cTess3dData.m_puiWireIndexes[nStartWireIndex + nPointIndex++] / 3;
			A3DUns32 nPntIndex;
			if(true == maPointIndexMap.Lookup(nWirePntIdx, nPntIndex)) {
				pcGridLoop->Add(nPntIndex);
			}
		}

		pcGrid->AddGridLoop(*pcGridLoop);
	}

	return true;
}

//--- Utility -----

// 4-3-1. Normal Index를 조정하는 함수
// Vertex는 중복이 되고 Normal을 개개별로 들어온다 ( vertex index에서 찾도록 검사 )
void C3dImport::MatchVertexNormal(MeshIndexMap & maNormalIndexMap, A3DUns32 nPointIndex, A3DUns32 & nNormalIndex)
{
	A3DUns32 nFindNormalIndex;
	if(true == maNormalIndexMap.Lookup(nPointIndex, nFindNormalIndex)) {
		nNormalIndex = nFindNormalIndex;
	}
	else {
		maNormalIndexMap.SetAt(nPointIndex, nNormalIndex);
	}
}

// 4-3-2. Point Index Map과 Normal Index Map, Point, Normal Index를 이용해서 MbTrianle Vector를 구성한다.
// 주어진 Triangle Vector에 생성된 Vector를 추가시킴.
bool C3dImport::ConvertTriangleVector(MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap,
	A3DUns32 * pnFacePointIndex, A3DUns32 * pnFaceNormalIndex, MeshTrianglesVector & cTrianglesVector)
{
	// Triangle을 구성할 Point Index
	A3DUns32 nPointIndex[3];

	for(int nIndex = 0; nIndex < 3; nIndex++) {
		// 사전에 저장된 Face Point Index를 찾는다. 찾은 경우 새롭게 생성되어지고 있는 Point Index를 이용해서
		// MbTriangle을 구성한다.
		if(false == maPointIndexMap.Lookup(pnFacePointIndex[nIndex], nPointIndex[nIndex])) {
			A3DUns32 nCount = (A3DUns32) maPointIndexMap.GetCount();
			maPointIndexMap.SetAt(pnFacePointIndex[nIndex], nCount);
			nPointIndex[nIndex] = nCount;
		}
	}

	// Point Index를 바탕으로 Normal Index를 구성한다.
	for(int nIndex = 0; nIndex < 3; nIndex++) {
		A3DUns32 nNormalIndex;
		if(false == maNormalIndexMap.Lookup(nPointIndex[nIndex], nNormalIndex)) {
			maNormalIndexMap.SetAt(nPointIndex[nIndex], pnFaceNormalIndex[nIndex]);
		}
	}

	MbTriangle cTriangle(nPointIndex[0], nPointIndex[1], nPointIndex[2], true);
	cTrianglesVector.push_back(cTriangle);

	return true;
}

// 4-3-3. 주어진 Map과 Triangle을 이용해서 Grid를 생성함.
bool C3dImport::CreateMbGrid(double * pcPoints, double * pcNormals, double dSclae, MeshIndexMap & maPointIndexMap, MeshIndexMap & maNormalIndexMap,
	MeshTrianglesVector & cTrianglesVector, MbGrid * pcGrid)
{
	size_t nPoitnCount = maPointIndexMap.GetCount();
	size_t nNormalCount = maNormalIndexMap.GetCount();

	if(nPoitnCount != nNormalCount) {
		ASSERT(false);
	}

	A3DUns32 * pnPointIndex = new A3DUns32[nPoitnCount];

	A3DUns32 nOriginIndex, nGridIndex;
	POSITION pcPosition = maPointIndexMap.GetStartPosition();
	while(nullptr != pcPosition)
	{
		maPointIndexMap.GetNextAssoc(pcPosition, nOriginIndex, nGridIndex);
		if(nGridIndex >= nPoitnCount) {
			ASSERT(false);
		}

		pnPointIndex[nGridIndex] = nOriginIndex;
	}

	A3DUns32 * pnNormalIndex = new A3DUns32[nNormalCount];

	pcPosition = maNormalIndexMap.GetStartPosition();
	while(nullptr != pcPosition)
	{
		maNormalIndexMap.GetNextAssoc(pcPosition, nGridIndex, nOriginIndex);
		if(nGridIndex >= nNormalCount) {
			ASSERT(false);
		}

		pnNormalIndex[nGridIndex] = nOriginIndex;
	}

	for(size_t nIndex = 0; nIndex < nPoitnCount; nIndex++) {
		A3DUns32 nPoIdx = pnPointIndex[nIndex];
		MbFloatPoint3D cPoint((float) pcPoints[nPoIdx * 3] * dSclae, (float) pcPoints[nPoIdx * 3 + 1] * dSclae, (float) pcPoints[nPoIdx * 3 + 2] * dSclae);

		A3DUns32 nNorIdx = pnNormalIndex[nIndex];
		MbFloatVector3D cNormal((float) pcNormals[nNorIdx * 3], (float) pcNormals[nNorIdx * 3 + 1], (float) pcNormals[nNorIdx * 3 + 2]);
		pcGrid->AddPoint(cPoint, cNormal);
	}

	pcGrid->AddTriangles(cTrianglesVector);

	REMOVE_ARRAY(pnPointIndex);
	REMOVE_ARRAY(pnNormalIndex);

	return true;
}

// == 현재 사용되지 않는 함수들 ========================================================================

// 3-12-1 주어진 Curve Edge, Vertex, Loop Sense Flag을 이용해서 Oreient Edge의 방향을 찾아내는 함수
bool C3dImport::GetOrientEdgeSense(MbCurveEdge * pcCurveEdge, MbVertex * pcStartVertex, MbVertex * pcEndVertex, bool bOrientationWithLoopFlag)
{
	const MbVertex & cCurveEdgeBegVertex = pcCurveEdge->GetBegVertex();
	const MbVertex & cCurveEdgeEndVertex = pcCurveEdge->GetEndVertex();

	const MbCartPoint3D & cCurveEdgeBegVertexPoint = cCurveEdgeBegVertex.GetCartPoint();
	const MbCartPoint3D & cCurveEdgeEndVertexPoint = cCurveEdgeEndVertex.GetCartPoint();

	if(false == bOrientationWithLoopFlag) {
		MbVertex * pcTemp = pcStartVertex;
		pcStartVertex = pcEndVertex;
		pcEndVertex = pcTemp;
	}

	const MbCartPoint3D & cStartVertexPoint = pcStartVertex->GetCartPoint();
	const MbCartPoint3D & cEndVertexPoint = pcEndVertex->GetCartPoint();

	if(cCurveEdgeBegVertexPoint.DistanceToPoint(cStartVertexPoint) < cCurveEdgeEndVertexPoint.DistanceToPoint(cStartVertexPoint)) {
		return true;
	}

	return false;
}

bool C3dImport::CheckNearnessPoints(const MbSurface & surface1, const MbCurve & pCurve1,
	const MbSurface & surface2, const MbCurve & pCurve2,
	double metricAcc, bool checkLimits, double kIrr0)
{
	bool isOk = true;

	const double t1Min = pCurve1.GetTMin();
	const double t1Max = pCurve1.GetTMax();

	const double t2Min = pCurve2.GetTMin();
	const double t2Max = pCurve2.GetTMax();

	double t1, t2;
	MbCartPoint uv1, uv2;
	MbCartPoint3D pnt1, pnt2;

	if(checkLimits) {
		isOk = false;

		t1 = t1Min;
		t2 = t2Min;
		pCurve1._PointOn(t1, uv1);
		pCurve2._PointOn(t2, uv2);
		surface1._PointOn(uv1, pnt1);
		surface2._PointOn(uv2, pnt2);
		if(c3d::EqualPoints(pnt1, pnt2, metricAcc)) {
			t1 = t1Max;
			t2 = t2Max;
			pCurve1._PointOn(t1, uv1);
			pCurve2._PointOn(t2, uv2);
			surface1._PointOn(uv1, pnt1);
			surface2._PointOn(uv2, pnt2);
			if(c3d::EqualPoints(pnt1, pnt2, metricAcc)) {
				isOk = true;
			}
		}
	}

	if(isOk) {
		isOk = false;

		double kIrr = M_E / 8.0;
		if(kIrr0 > LENGTH_EPSILON && kIrr0 < 1.0 - LENGTH_EPSILON)
			kIrr = kIrr0;

		double dtIrr1 = kIrr * (t1Max - t1Min);
		double dtIrr2 = kIrr * (t2Max - t2Min);

		t1 = t1Min + dtIrr1;
		t2 = t2Min + dtIrr2;
		pCurve1._PointOn(t1, uv1);
		pCurve2._PointOn(t2, uv2);
		surface1._PointOn(uv1.x, uv1.y, pnt1);
		surface2._PointOn(uv2.x, uv2.y, pnt2);
		if(c3d::EqualPoints(pnt1, pnt2, metricAcc)) {
			t1 = t1Max - dtIrr1;
			t2 = t2Max - dtIrr2;
			pCurve1._PointOn(t1, uv1);
			pCurve2._PointOn(t2, uv2);
			surface1._PointOn(uv1.x, uv1.y, pnt1);
			surface2._PointOn(uv2.x, uv2.y, pnt2);
			if(c3d::EqualPoints(pnt1, pnt2, metricAcc))
				isOk = true;
		}
	}

	return isOk;
}

// 주어진 두 점을 이용해서 Line Segment를 Base로하는 Oriented Edge를 생성.
// 주어진 Surface를 One, Two로 하는 Oriented Edge를 생성함. 
// 주어진 Oriented Edge는 전후 관계를 확인해서 연결되어 있는 Curve의 값을 사용해야 한다.
bool C3dImport::CreateLineSegmentOrientedEdge(std::list<const DmiOrientedEdge *> & lpcOrientedEdgeList,
	const DmiOrientedEdge * pcCurEdge, const DmiOrientedEdge * pcNextEdge, const MbSurface * pcBaseSurface, double dTolerance,
	const DmiOrientedEdge *& pcLineSegmentOrientedEdge)
{
	auto cEndIterator = std::prev(lpcOrientedEdgeList.end());

	auto pcCurEdgeIterator = std::find(lpcOrientedEdgeList.begin(), lpcOrientedEdgeList.end(), pcCurEdge);
	auto pcNextEdgeIterator = std::find(lpcOrientedEdgeList.begin(), lpcOrientedEdgeList.end(), pcNextEdge);

	// 각각의 Edge의 앞쪽 및 뒷쪽 Edge와 비교해서, 연결된 Curve를 이용해서 
	auto pcCurEdgePrevIterator = pcCurEdgeIterator;
	if(pcCurEdgePrevIterator == lpcOrientedEdgeList.begin()) {
		pcCurEdgePrevIterator = lpcOrientedEdgeList.end();
		pcCurEdgePrevIterator--;
	}
	else {
		pcCurEdgePrevIterator--;
	}

	auto pcNextEdgeNextIterator = pcNextEdgeIterator;
	if(pcNextEdgeNextIterator == cEndIterator) {
		pcNextEdgeNextIterator = lpcOrientedEdgeList.begin();
	}
	else {
		pcNextEdgeNextIterator++;
	}

	bool bConnectFlag = false;
	const MbCurve * pcCurve[2];
	pcCurve[0] = nullptr;
	pcCurve[1] = nullptr;
	// current edge의 연결 curve를 찾아온다. 
	// pcCurCurve에 각각 Cur, Next의 연결된 MbCurve를 찾아 온다.
	CheckConectivity(pcCurEdge, pcNextEdge, pcBaseSurface, dTolerance, bConnectFlag, pcCurve);

	bool bCurSense = pcCurEdge->GetOrientation();
	bool bNextSense = pcNextEdge->GetOrientation();

	MbCartPoint cStartPoint, cEndPoint;

	if(true == bCurSense) {
		pcCurve[0]->GetEndPoint(cStartPoint);
	}
	else {
		pcCurve[0]->GetStartPoint(cStartPoint);
	}

	if(true == bNextSense) {
		pcCurve[1]->GetStartPoint(cEndPoint);
	}
	else {
		pcCurve[1]->GetEndPoint(cEndPoint);
	}

	MbLineSegment * pcLine1 = new MbLineSegment(cStartPoint, cEndPoint);
	if(nullptr == pcLine1) {
		ASSERT(NULL);
		return false;
	}

	MbLineSegment * pcLine2 = new MbLineSegment(cStartPoint, cEndPoint);
	if(nullptr == pcLine2) {
		ASSERT(NULL);
		return false;
	}

	MbSurfaceIntersectionCurve * pcInterCurve = new MbSurfaceIntersectionCurve(*pcBaseSurface, *pcLine1, *pcBaseSurface, *pcLine2,
		MbeCurveBuildType::cbt_Ordinary, true, true);

	pcInterCurve->SetPoleGlueType();

	const MbVertex & cStartVertex = pcCurEdge->GetEndVertex();
	const MbVertex & cEndVertex = pcNextEdge->GetBegVertex();

	MbCurveEdge * pcCurveEdge = new MbCurveEdge(cStartVertex, cEndVertex, *pcInterCurve, true);

	pcLineSegmentOrientedEdge = new DmiOrientedEdge(*pcCurveEdge, true, true);

	return true;
}

// 주어진 Orient Edge에 포함되어 있는 Curve에서 Base Surface와 일치하는 Curve만 선택해서 Bounding Box를 계산함.
bool C3dImport::CalculateBoundBox(const MbOrientedEdge * pcEdge, const MbSurface * pcBaseSurface, MbRect & cRect)
{
	const MbCurveEdge & cCurveEdge = pcEdge->GetCurveEdge();

	const MbSurfaceIntersectionCurve & cIntersectionCurve = cCurveEdge.GetIntersectionCurve();

	// Seam Edge의 경우 Curve의 Base Surface가 같음.
	if(pcBaseSurface == cIntersectionCurve.GetSurfaceOne()) {
		MbRect cCurveRect;
		cIntersectionCurve.GetCurveOneCurve().CalculateGabarit(cCurveRect);
		cRect.Union(cRect, cCurveRect);
	}

	if(pcBaseSurface == cIntersectionCurve.GetSurfaceTwo()) {
		MbRect cCurveRect;
		cIntersectionCurve.GetCurveTwoCurve().CalculateGabarit(cCurveRect);
		cRect.Union(cRect, cCurveRect);
	}

	return true;
}

// ----- Vertex 관련 함수 -----
bool C3dImport::ConvertTopoVertex(A3DTopoVertex * pcTopoVertex, VertexSPtr & pcVertex)
{
	if(nullptr != pcTopoVertex) {
		RETURN_FALSE;
	}

	A3DEEntityType eType = kA3DTypeUnknown;
	CHECK_A3D_RETURN(A3DEntityGetType(pcTopoVertex, &eType));

	bool bStatus = false;
	switch(eType)
	{
		case kA3DTypeTopoUniqueVertex:
			bStatus = ConvertTopoUniqueVertex(pcTopoVertex, pcVertex);
			break;

		case kA3DTypeTopoMultipleVertex:
			bStatus = ConvertTopoMultipleVertex(pcTopoVertex, pcVertex);
			break;

		default:
			ASSERT(false);
			break;
	}

	return bStatus;
}

bool C3dImport::ConvertTopoVertex(ConvVertex * pcConvVertex, MbVertex *& pcVertex)
{
	if(nullptr != pcConvVertex) {
		RETURN_FALSE;
	}

	A3DEEntityType eType = pcConvVertex->GetVertexType();

	bool bStatus = false;
	switch(eType)
	{
		case kA3DTypeTopoUniqueVertex:
			bStatus = ConvertTopoUniqueVertex(pcConvVertex, pcVertex);
			break;

		case kA3DTypeTopoMultipleVertex:
			bStatus = ConvertTopoMultipleVertex(pcConvVertex, pcVertex);
			break;

		default:
			ASSERT(false);
			break;
	}

	return bStatus;
}

bool C3dImport::ConvertTopoUniqueVertex(A3DTopoVertex * pcTopoVertex, VertexSPtr & pcVertex)
{
	if(nullptr != pcTopoVertex) {
		RETURN_FALSE;
	}

	A3DTopoUniqueVertexData cTopoUniqueVertexData;
	A3D_INITIALIZE_DATA(A3DTopoUniqueVertexData, cTopoUniqueVertexData);
	A3DTopoUniqueVertexGet(pcTopoVertex, &cTopoUniqueVertexData);

	MbCartPoint3D cCartPoint3d;
	cCartPoint3d.x = cTopoUniqueVertexData.m_sPoint.m_dX * m_dContextScale;
	cCartPoint3d.y = cTopoUniqueVertexData.m_sPoint.m_dY * m_dContextScale;
	cCartPoint3d.z = cTopoUniqueVertexData.m_sPoint.m_dZ * m_dContextScale;

	pcVertex = new MbVertex(cCartPoint3d);
	if(nullptr == pcVertex) {
		return false;
	}

	return true;
}

bool C3dImport::ConvertTopoUniqueVertex(ConvVertex * pcConvVertex, MbVertex *& pcVertex)
{
	if(nullptr != pcConvVertex) {
		RETURN_FALSE;
	}

	A3DTopoUniqueVertexData & cTopoUniqueVertexData = *((A3DTopoUniqueVertexData *) pcConvVertex->GetVertexData());

	MbCartPoint3D cCartPoint3d;
	cCartPoint3d.x = cTopoUniqueVertexData.m_sPoint.m_dX * m_dContextScale;
	cCartPoint3d.y = cTopoUniqueVertexData.m_sPoint.m_dY * m_dContextScale;
	cCartPoint3d.z = cTopoUniqueVertexData.m_sPoint.m_dZ * m_dContextScale;

	pcVertex = new MbVertex(cCartPoint3d);
	if(nullptr == pcVertex) {
		return false;
	}

	return true;
}

bool C3dImport::ConvertTopoMultipleVertex(A3DTopoVertex * pcTopoVertex, VertexSPtr & pcVertex)
{
	if(nullptr != pcTopoVertex) {
		RETURN_FALSE;
	}

	A3DTopoMultipleVertexData cMultipleVertexData;
	A3D_INITIALIZE_DATA(A3DTopoMultipleVertexData, cMultipleVertexData);
	A3DTopoMultipleVertexGet(pcTopoVertex, &cMultipleVertexData);

	ASSERT(0 != cMultipleVertexData.m_uiSize);

	// 	MbCartPoint3D * pcCartPoint3d = new MbCartPoint3D();
	// 	pcCartPoint3d->x = cMultipleVertexData.m_pPts[0].m_dX * m_dTopoContextScale;
	// 	pcCartPoint3d->y = cMultipleVertexData.m_pPts[0].m_dY * m_dTopoContextScale;
	// 	pcCartPoint3d->z = cMultipleVertexData.m_pPts[0].m_dZ * m_dTopoContextScale;

	MbCartPoint3D cCartPoint3d;
	cCartPoint3d.x = cMultipleVertexData.m_pPts[0].m_dX * m_dContextScale;
	cCartPoint3d.y = cMultipleVertexData.m_pPts[0].m_dY * m_dContextScale;
	cCartPoint3d.z = cMultipleVertexData.m_pPts[0].m_dZ * m_dContextScale;

	//pcVertex = new MbVertex(*pcCartPoint3d);
	pcVertex = new MbVertex(cCartPoint3d);
	if(nullptr == pcVertex) {
		return false;
	}

	return true;
}

bool C3dImport::ConvertTopoMultipleVertex(ConvVertex * pcConvVertex, MbVertex *& pcVertex)
{
	if(nullptr != pcConvVertex) {
		RETURN_FALSE;
	}

	A3DTopoMultipleVertexData & cMultipleVertexData = *((A3DTopoMultipleVertexData *) pcConvVertex->GetVertexData());

	ASSERT(0 != cMultipleVertexData.m_uiSize);

	MbCartPoint3D cCartPoint3d;
	cCartPoint3d.x = cMultipleVertexData.m_pPts[0].m_dX * m_dContextScale;
	cCartPoint3d.y = cMultipleVertexData.m_pPts[0].m_dY * m_dContextScale;
	cCartPoint3d.z = cMultipleVertexData.m_pPts[0].m_dZ * m_dContextScale;

	pcVertex = new MbVertex(cCartPoint3d);
	if(nullptr == pcVertex) {
		return false;
	}

	return true;
}

// ----- PMI 관련 함수 -----
bool C3dImport::ConvertAnnotationEntity(A3DMkpAnnotationEntity * pcAnnotationEntity, A3DMiscCascadedAttributes * pcParentAttr)
{
	if(nullptr == pcAnnotationEntity) {
		return false;
	}

	A3DEEntityType eType = kA3DTypeUnknown;
	CHECK_A3D_RETURN(A3DEntityGetType(pcAnnotationEntity, &eType));

	switch(eType)
	{
		case kA3DTypeMkpAnnotationItem:
			//return ConvertAnnotationItem((A3DMkpAnnotationItem *) pcAnnotationEntity, info, pcParentAttr);
			break;

		case kA3DTypeMkpAnnotationSet:
			//return ConvertAnnotationSet((A3DMkpAnnotationSet *) pcAnnotationEntity, info, pcParentAttr);
			break;

		case kA3DTypeMkpAnnotationReference:
			//return ConvertAnnotationReference((A3DMkpAnnotationReference *) pcAnnotationEntity, info, pcParentAttr);
			break;
	}

	return false;
}

// == Attribute 관련 함수 ============================================================================

// 1. Parent에서 받은(계단식으로) Attribute를 이용해서, Attribute를 생성
bool C3dImport::CreateAndPushCascadedAttributes(const A3DRootBaseWithGraphics * pcBase, const A3DMiscCascadedAttributes * pcParentAttr,
	A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData)
{
	if(A3D_SUCCESS != A3DMiscCascadedAttributesCreate(pcAttrs)) {
		return false;
	}

	if(A3D_SUCCESS != A3DMiscCascadedAttributesPush(*pcAttrs, pcBase, pcParentAttr)) {
		return false;
	}

	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, (*pcAttributesData));
	if(A3D_SUCCESS != A3DMiscCascadedAttributesGet(*pcAttrs, pcAttributesData)) {
		return false;
	}

	return true;
}

// 2. Tess Face용 Attribute 생성
A3DStatus C3dImport::CreateAndPushCascadedAttributesTessFace(const A3DRiRepresentationItem * pcRepItem, const A3DTessBase * pcTessBase,
	const A3DTessFaceData * pcTessFaceData, A3DUns32 nFaceIndex, const A3DMiscCascadedAttributes * pcParentAttribute,
	A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData)
{
	A3DStatus nStatus = A3DMiscCascadedAttributesCreate(pcAttrs);
	nStatus = A3DMiscCascadedAttributesPushTessFace(*pcAttrs, pcRepItem, pcTessBase, pcTessFaceData, nFaceIndex, pcParentAttribute);

	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, (*pcAttributesData));

	nStatus = A3DMiscCascadedAttributesGet(*pcAttrs, pcAttributesData);

	return nStatus;
}

// 3. 주어진 Attributes에서 Color 값을 추출
bool C3dImport::GetAttributeColor(A3DMiscCascadedAttributesData & cAttrsData, COLORREF & nColor)
{
	A3DGraphRgbColorData cGraphRgbColorData;
	A3D_INITIALIZE_DATA(A3DGraphRgbColorData, cGraphRgbColorData);
	if(A3D_SUCCESS != A3DGlobalGetGraphRgbColorData(cAttrsData.m_sStyle.m_uiRgbColorIndex, &cGraphRgbColorData)) {
		return false;
	}

	nColor = RGB(255.0 * cGraphRgbColorData.m_dRed, 255.0 * cGraphRgbColorData.m_dGreen, 255.0 * cGraphRgbColorData.m_dBlue);

	return true;
}

// 4. 주어진 Attributes에서 투명도 값을 추출
bool C3dImport::GetAttributeTransparency(A3DMiscCascadedAttributesData & cAttrsData, float & fTransparency)
{
	bool bTransparencyDefined = (1 == cAttrsData.m_sStyle.m_bIsTransparencyDefined) ? true : false;
	fTransparency = cAttrsData.m_sStyle.m_ucTransparency / 255.0f;

	return bTransparencyDefined;
}

// 5. 주어진 Attributes에서 Line Pattern 값을 추출
bool C3dImport::GetAttributeLinePattern(A3DMiscCascadedAttributesData & cAttrsData, int & nLinePattern)
{
	// Line Pattern 설정
	A3DUns32 nLinePatternIndex = cAttrsData.m_sStyle.m_uiLinePatternIndex;
	A3DGraphLinePatternData cLinePatternData;
	A3D_INITIALIZE_DATA(A3DGraphLinePatternData, cLinePatternData);
	if(A3D_SUCCESS != A3DGlobalGetGraphLinePatternData(nLinePatternIndex, &cLinePatternData)) {
		return false;
	}

	bool bStatus = false;
	switch(cLinePatternData.m_uiNumberOfLengths)
	{
		case 2:
		{
			double d1 = cLinePatternData.m_pdLengths[0];
			double d2 = cLinePatternData.m_pdLengths[1];
			if(0.0 != d2)
			{
				if(fabs(d1 / d2 - 1) < 1e-3) {
					nLinePattern = 3; // HPS::LinePattern::Default::Dotted, HC_Set_Line_Pattern("...");	// 짧은 점선
					bStatus = true;
				}
				else {
					nLinePattern = 2; // HPS::LinePattern::Default::Dashed, HC_Set_Line_Pattern("- -");	// 긴 점선
					bStatus = true;
				}
			}
		}
		break;

		case 4:
			nLinePattern = 1; // HPS::LinePattern::Default::DashDot, HC_Set_Line_Pattern("-.");	// 일점쇄선
			bStatus = true;
			break;

		default:
		{
			if(cLinePatternData.m_uiNumberOfLengths > 4) {
				nLinePattern = 4; // HPS::LinePattern::Default::Dash2Dot // HC_Set_Line_Pattern("-..");	// 이점쇄선
				bStatus = true;
			}
		}
		break;
	}

	return bStatus;
}


// == Utility 함수 ==================================================================================

// U-1. 주어진 CoEdge의 Surface를 찾아오는 함수 (A3D 함수 이용)
bool C3dImport::GetSurfBaseFromTopoCoEdge(const A3DTopoCoEdge * psCoEdge, const A3DSurfBase *& pcSurfBase)
{
	const A3DTopoFace * pcTopoFace = nullptr;
	if(false == GetTopoFaceFromTopoCoEdge(psCoEdge, pcTopoFace)) {
		return false;
	}

	if(false == GetSurfBaseFromTopoFace(pcTopoFace, pcSurfBase)) {
		return false;
	}

	return true;
}

// U-2. 주어진 CoEdge의 Face를 찾아오는 함수 (A3D 함수 이용)
bool C3dImport::GetTopoFaceFromTopoCoEdge(const A3DTopoCoEdge * psCoEdge, const A3DTopoFace *& pcTopoFace)
{
	if(nullptr == m_pcCurrentTopoBrepData) {
		return false;
	}

	A3DTopoLoop * pcTopoLoop = nullptr;
	A3DStatus eResult = A3DTopoCoedgeGetLoop(psCoEdge, &pcTopoLoop);
	if(A3D_SUCCESS != eResult) {
		return false;
	}

	eResult = A3DTopoLoopGetFace(m_pcCurrentTopoBrepData, pcTopoLoop, &pcTopoFace);
	if(A3D_SUCCESS != eResult) {
		return false;
	}

	if(nullptr == pcTopoFace) {
		return false;
	}

	return true;
}

// U-3. 주어진 Face에서 Surface를 찾아오는 함수
bool C3dImport::GetSurfBaseFromTopoFace(const A3DTopoFace * pcTopoFace, const A3DSurfBase *& pcSurfBase)
{
	A3DTopoFaceData cTopoFaceData;
	A3D_INITIALIZE_DATA(A3DTopoFaceData, cTopoFaceData);
	A3DStatus eResult = A3DTopoFaceGet(pcTopoFace, &cTopoFaceData);

	if(A3D_SUCCESS != eResult) {
		SetLastErrorMessage(L"ConvertTopoCoEdge - TopoFaceGet Error", eResult);
		return false;
	}

	pcSurfBase = cTopoFaceData.m_pSurface;

	// 메모리 해제
	A3DTopoFaceGet(nullptr, &cTopoFaceData);

	return true;
}

// U-4. MbSurface를 가져오는 함수, 저장되어 있는 Surface를 검색하고, 없는 경우 생성해서 돌려 준다. 생성후에는 Surface Map에 저장한다.
bool C3dImport::GetMbSurface(const A3DSurfBase * pcSurfBase, c3d::SurfaceSPtr & pcSurface)
{
	if(true == m_mpcSurfBaseMap.Lookup((DWORD_PTR) pcSurfBase, pcSurface)) {
		return true;
	}

	if(false == ConvSurfaceBase::ConvertSurfBase(pcSurfBase, m_dContextScale, m_dContextScale, pcSurface)) {
		return false;
	}

	m_mpcSurfBaseMap.SetAt((DWORD_PTR) pcSurfBase, pcSurface);

	return true;
}

// U-6-1. MbSurface를 Map에서 찾아오는 함수
bool C3dImport::FindMbSurface(const A3DSurfBase * pcSurfBase, SurfaceSPtr & pcSurface)
{
	if(true == m_mpcSurfBaseMap.Lookup((DWORD_PTR) pcSurfBase, pcSurface)) {
		return true;
	}

	return true;
}

bool C3dImport::FindMbSurface(SurfBaseMap & mpcSurfBaseMap, const A3DSurfBase * pcSurfBase, SurfaceSPtr & pcSurface)
{
	if(true == mpcSurfBaseMap.Lookup((DWORD_PTR) pcSurfBase, pcSurface)) {
		return true;
	}

	return true;
}

// U-6-2. MbSurface를 Map에 추가하는 함수
bool C3dImport::AddMbSurfaceToMap(const A3DSurfBase * pcSurfBase, SurfaceSPtr & pcSurface)
{
	m_mpcSurfBaseMap.SetAt((DWORD_PTR) pcSurfBase, pcSurface);
	return true;
}

bool C3dImport::FindTopoVertex(A3DTopoVertex * pcTopoVertex, c3d::VertexSPtr & pcVertex)
{
	if(true == m_mpcVertexMap.Lookup((DWORD_PTR) pcTopoVertex, pcVertex)) {
		return true;
	}

	return false;
}

bool C3dImport::AddTopoVertexToMap(A3DTopoVertex * pcTopoVertex, c3d::VertexSPtr & pcVertex)
{
	m_mpcVertexMap.SetAt((DWORD_PTR) pcTopoVertex, pcVertex);

	return true;
}

// == C3D 관련 Utility 함수 =========================================================================

// 1. Item의 이름을 설정
bool C3dImport::SetItemName(MbItem * pcItem, CString & strName)
{
	string_t strId = L"name";
	string_t strAssyName = strName.GetBuffer();
	SPtr<MbStringAttribute> strNameAttr(new MbStringAttribute(strId, true, strAssyName));
	pcItem->AddAttribute(strNameAttr);

	return true;
}

// 2. 3DX Location을 C3D Matrix로 변환해서 돌려줌
bool C3dImport::GetMatrix(A3DMiscTransformation * pcLocation, MbMatrix3D & cMatrix)
{
	if(nullptr == pcLocation) {
		return false;
	}

	A3DEEntityType eType;
	A3DEntityGetType(pcLocation, &eType);

	if(eType == kA3DTypeMiscCartesianTransformation)
	{
		A3DMiscCartesianTransformationData cTransformationData;
		A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, cTransformationData);

		A3DStatus nStatus = A3DMiscCartesianTransformationGet(pcLocation, &cTransformationData);
		if(A3D_SUCCESS != nStatus) {
			ASSERT(false);
			return false;
		}

		double m_dTopoContextScaleX = cTransformationData.m_sScale.m_dX;
		double m_dTopoContextScaleY = cTransformationData.m_sScale.m_dY;
		double m_dTopoContextScaleZ = cTransformationData.m_sScale.m_dZ;

		MbCartPoint3D cOrigin = Dmi3dx::GetMbCartPoint3D(cTransformationData.m_sOrigin);
		cOrigin.Scale(m_dTopoContextScaleX, m_dTopoContextScaleY, m_dTopoContextScaleZ);
		MbVector3D cAxisX = Dmi3dx::GetMbVector3D(cTransformationData.m_sXVector);
		MbVector3D cAxisY = Dmi3dx::GetMbVector3D(cTransformationData.m_sYVector);
		MbVector3D cAxisZ;

		if(cTransformationData.m_ucBehaviour & kA3DTransformationMirror) {
			cAxisZ = cAxisY | cAxisX;
		}
		else {
			cAxisZ = cAxisX | cAxisY;
		}

		cMatrix.SetOrigin() = cOrigin;
		cMatrix.SetAxisX() = cAxisX;
		cMatrix.SetAxisY() = cAxisY;
		cMatrix.SetAxisZ() = cAxisZ;

		A3DMiscCartesianTransformationGet(nullptr, &cTransformationData);
	}
	else if(eType == kA3DTypeMiscGeneralTransformation)
	{
		A3DMiscGeneralTransformationData cTransformationData;
		A3D_INITIALIZE_DATA(A3DMiscGeneralTransformationData, cTransformationData);

		A3DStatus nStatus = A3DMiscGeneralTransformationGet(pcLocation, &cTransformationData);
		if(A3D_SUCCESS != nStatus) {
			ASSERT(false);
			return false;
		}

		cMatrix.SetAxisX().Init(cTransformationData.m_adCoeff[0], cTransformationData.m_adCoeff[1], cTransformationData.m_adCoeff[2]);
		cMatrix.SetAxisY().Init(cTransformationData.m_adCoeff[4], cTransformationData.m_adCoeff[5], cTransformationData.m_adCoeff[6]);
		cMatrix.SetAxisZ().Init(cTransformationData.m_adCoeff[8], cTransformationData.m_adCoeff[9], cTransformationData.m_adCoeff[10]);
		cMatrix.SetOrigin().Init(cTransformationData.m_adCoeff[12], cTransformationData.m_adCoeff[13], cTransformationData.m_adCoeff[14]);

		// Memory Free
		A3DMiscGeneralTransformationGet(nullptr, &cTransformationData);
	}
	else
	{
		ASSERT(false);
		return false;
	}

	return true;
}

// 4. MbItem Attribute 관련 함수

// 4-1. String Attribuet 설정 함수. (사전에 값이 있으면 변경됨)
bool C3dImport::SetItemStringAttribute(MbItem * pcItem, string_t strId, string_t strAttribute)
{
	SPtr<MbStringAttribute> cAttr(new MbStringAttribute(strId, true, strAttribute));
	pcItem->AddAttribute(cAttr);

	return true;
}

bool C3dImport::SetItemIntAttribute(MbItem * pcItem, string_t strId, int nAttribute)
{
	SPtr<MbIntAttribute> cAttr(new MbIntAttribute(strId, true, nAttribute));
	pcItem->AddAttribute(cAttr);

	return true;
}

bool C3dImport::GetVector3d(const A3DVector3dData & cVector3dData, MbVector3D & cVector)
{
	cVector.x = cVector3dData.m_dX;
	cVector.y = cVector3dData.m_dY;
	cVector.z = cVector3dData.m_dZ;

	return true;
}

bool C3dImport::WriteSolid(const TCHAR * pathName, MbSolid & solid)
{
	bool isGoodWrite = false;

	solid.AddRef();
	MbModel * geomModel = new MbModel();
	geomModel->AddItem(solid, 0);

	membuf memBuf;           // Создаем membuf
	std::unique_ptr<writer> rc = writer::CreateMemWriter(memBuf, 0);
	isGoodWrite = rc->good();

	if(isGoodWrite) {
		::WriteModelItems(*rc.get(), *geomModel);
		memBuf.setMaxRegCount(rc->GetMaxRegisteredCount()); // \ru Максимальное количество регистрируемых объектов. \en The maximum number of registered facilities. 

		isGoodWrite = ::writeiobuftodisk(pathName, memBuf); // Пишем его на диск
	}
	::DeleteItem(geomModel);
	solid.DecRef();

	//	}
	return isGoodWrite;
}

//== Log 관련 함수 ===================================================================================
void C3dImport::CreateLog(int nId, const WCHAR * pchFilePathName)
{
	LogManager::SetCurrentId(nId);

	LogManager::SetFilePathName(nId, pchFilePathName);
	LogManager::SetCreateFile(nId, true);
	LogManager::SetWriteLog(nId, true);
	LogManager::ResetTabIndex(nId);

	LogManager::SetWriteTimeLog(nId, true);
	Log(nId, L"Create Log");
	LogManager::SetWriteTimeLog(nId, false);
}

void C3dImport::Log(int nId, LPCWSTR chMessage, ...)
{
#ifdef USED_LOG_MANAGER
	va_list cArgList;
	va_start(cArgList, chMessage);

	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::Log(nId, strBuffer);
#endif
}

void C3dImport::LogIncreaseTabIndex(int nId)
{
#ifdef USED_LOG_MANAGER
	LogManager::IncreaseTabIndex(nId);
#endif
}

void C3dImport::LogDecreaseTabIndex(int nId)
{
#ifdef USED_LOG_MANAGER
	LogManager::DecreaseTabIndex(nId);
#endif
}

CString C3dImport::LogHexStr(DWORD_PTR nValue)
{
#ifdef USED_LOG_MANAGER
	return LogManager::HexStr(nValue);
#endif
	return L"";
}

CString C3dImport::LogBoolStr(bool bValue)
{
#ifdef USED_LOG_MANAGER
	return LogManager::BoolStr(bValue);
#endif
	return L"";
}

// == Old Version ==================================================================================
bool C3dImport::ConvertTopoConnex(A3DTopoConnex * pcTopoConnex, A3DMiscCascadedAttributes * pcParentAttr,
	ShellSPtr & pcFaceShell)
{
	// Topo Connex에서는 Name이 나오지 않음.
	Log(2, L"ConvertTopoConnex: %s", LogHexStr((DWORD_PTR) pcTopoConnex));

	LogIncreaseTabIndex(2);

	if(nullptr != pcTopoConnex) {
		RETURN_FALSE;
	}

	A3DTopoConnexData cTopoConnexData;
	A3D_INITIALIZE_DATA(A3DTopoConnexData, cTopoConnexData);
	A3DStatus eStatus = A3DTopoConnexGet(pcTopoConnex, &cTopoConnexData);
	if(A3D_SUCCESS != eStatus) {
		SetLastErrorMessage(L"ParseRiBrepModel - TopoConnexGet Error", eStatus);
		return false;
	}

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcTopoConnex, pcParentAttr, &pcAttrs, &cAttrsData);

	if(A3D_FALSE == cAttrsData.m_bShow) {
		A3DTopoConnexGet(nullptr, &cTopoConnexData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return false;
	}

	for(A3DUns32 nIndex = 0; nIndex < cTopoConnexData.m_uiShellSize; nIndex++) {
		if(true == ConvertMbFaceShell(cTopoConnexData.m_ppShells[nIndex], pcAttrs, pcFaceShell)) {
		}
	}

	A3DTopoConnexGet(nullptr, &cTopoConnexData);
	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	LogDecreaseTabIndex(2);

	return true;
}

bool C3dImport::ConvertMbFaceShell(A3DTopoShell * pcTopoShell, A3DMiscCascadedAttributes * pcParentAttr,
	SolidsSPtrVector & vpSolidVector)
{
	if(nullptr == pcTopoShell) {
		return false;
	}

#ifdef _DEBUG
	// 	A3dTracer cA3dTracer(m_dTopoContextScale);
	// 	cA3dTracer.CreateLog(L"D:\\Temp\\A3dXInfo.log");
	// 	cA3dTracer.A3DTopoShellLog(pcTopoShell);
#endif
/*

	A3DTopoShellData cTopoShellData;
	A3D_INITIALIZE_DATA(A3DTopoShellData, cTopoShellData);
	A3DStatus nResult = A3DTopoShellGet(pcTopoShell, &cTopoShellData);
	if(A3D_SUCCESS != nResult) {
		SetLastErrorMessage(L"ConvertTopoShell - TopoShellGet Error", nResult);
		return false;
	}

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcTopoShell, pcParentAttr, &pcAttrs, &cAttrsData);

	if(A3D_FALSE == cAttrsData.m_bShow) {
		return false;
	}

	ShellSPtr pcFaceShell(new MbFaceShell());

	for(A3DUns32 nIndex = 0; nIndex < cTopoShellData.m_uiFaceSize; nIndex++)
		//for(A3DUns32 nIndex = 0; nIndex <= 3; nIndex++)
	//A3DUns32 nIndex = 13;
	{
		FaceSPtr pcFace;
		// Face의 Sense 결정
		bool bOrientationWithShell = (1 == cTopoShellData.m_pucOrientationWithShell[nIndex]) ? true : false;

		//if(true == ConvertMbFace(cTopoShellData.m_ppFaces[nIndex], bOrientationWithShell, pcAttrs, cAttrsData, pcFace)) {
		if(true == ConvertMbFace(nIndex, cTopoShellData.m_ppFaces[nIndex], bOrientationWithShell, pcAttrs, cAttrsData, pcFace)) {
			if(nullptr != pcFace) {
				pcFaceShell->AddFace(*pcFace);
			}
		}
		else {
			ASSERT(false);
		}
	}

	SolidSPtr pcSolid;
	pcSolid = new MbSolid(pcFaceShell, nullptr);
	if(nullptr == pcSolid) {
		ASSERT(NULL);
		return false;
	}

	COLORREF cColor;
	if(true == GetAttributeColor(cAttrsData, cColor)) {
		pcSolid->SetColor(cColor);
	}

	float fTransparency = 1.0f;
	if(true == GetAttributeTransparency(cAttrsData, fTransparency)) {
		float a, d, sp, sh, t, e;
		pcSolid->GetVisual(a, d, sp, sh, t, e);
		pcSolid->SetVisual(a, d, sp, sh, fTransparency, e);
	}

	vpSolidVector.push_back(pcSolid);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
*/

	return true;
}

bool C3dImport::ConvertMbLoop(ConvLoop * pcConvLoop, c3d::SurfaceSPtr & pcSurface, bool bOrientationWithShell, bool bFirstLoopFlag)
{
	if(nullptr != pcConvLoop) {
		RETURN_FALSE;
	}

	// TopoLoop Data 수집
	A3DTopoLoopData & cTopoLoopData = pcConvLoop->GetTopoLoopData();

	// Loop Sense 설정
	bool bOrientationWithSurfaceFlag = (1 == cTopoLoopData.m_ucOrientationWithSurface) ? true : false;

	ConvCoEdgeVector & cCoEdgeVector = pcConvLoop->GetConvCoEdgeVector();

	//MbeMultithreadedMode mode = Math::MultithreadedMode();

	//std::vector<std::future<bool>> futures;

	for_each(cCoEdgeVector.begin(), cCoEdgeVector.end(), [&] (ConvCoEdge * pcConvCoEdge) {
		//parallel_for_each(cCoEdgeVector.begin(), cCoEdgeVector.end(), [&] (ConvCoEdge * pcConvCoEdge) {
		//parallel_for(cCoEdgeVector.begin(), cCoEdgeVector.end(), [&] (ConvCoEdge * pcConvCoEdge) {
	/*

			pcConvCoEdge->m_mpcConvSurfaceMap = &m_mpcConvSurfaceMap;
			pcConvCoEdge->m_mpcSurfBaseMap = &m_mpcSurfBaseMap;
			pcConvCoEdge->m_mpcConvCoEdgeMap = &m_mpcConvCoEdgeMap;
			pcConvCoEdge->m_mpcMbVertexMap = &m_mpcMbVertexMap;

			pcConvCoEdge->m_pcSurface = pcSurface;

			ThreadConvertMbOrientedEdge(*pcConvCoEdge, m_dContextScale, m_pcModel);
	*/
		});

	/*	for(ConvCoEdge * pcConvCoEdge : cCoEdgeVector) {
			//ConvCoEdge * pcConvCoEdge = cCoEdgeVector[nIndex];
			pcConvCoEdge->m_mpcConvSurfaceMap = &m_mpcConvSurfaceMap;
			pcConvCoEdge->m_mpcSurfBaseMap = &m_mpcSurfBaseMap;
			pcConvCoEdge->m_mpcConvCoEdgeMap = &m_mpcConvCoEdgeMap;
			pcConvCoEdge->m_mpcMbVertexMap = &m_mpcMbVertexMap;

			pcConvCoEdge->m_pcSurface = pcSurface;

			ThreadConvertMbOrientedEdge(*pcConvCoEdge, m_dContextScale, m_pcModel);

	// 		m_pcThreadPoolpool->EnqueueJob([&] () { ThreadConvertMbOrientedEdge(*pcConvCoEdge, m_dTopoContextScale); });
	// 		m_pcThreadPoolpool->EnqueueJob(ThreadConvertMbOrientedEdge(*pcConvCoEdge, m_dTopoContextScale));
	// 		ThreadConvertMbOrientedEdge(*pcConvCoEdge, m_dTopoContextScale);
	// 		futures.emplace_back(m_pcThreadPoolpool->EnqueueJob(ThreadConvertMbOrientedEdge(*pcConvCoEdge, m_dTopoContextScale));
		}
	*/
	/*
		for_each(cCoEdgeVector.begin(), cCoEdgeVector.end(), [&] (ConvCoEdge * pcConvCoEdge) {
		//for_each(cCoEdgeVector.begin(), cCoEdgeVector.end(), [&] (ConvCoEdge * pcConvCoEdge) {
			ConvertMbOrientedEdge(pcConvCoEdge, pcSurface);
		});
	*/

	/*
		std::list<const DmiOrientedEdge *> lpcOrientedEdgeList;
		for(ConvCoEdge * pcConvCoEdge : cCoEdgeVector) {
			if(nullptr != pcConvCoEdge->GetOrientedEdge()) {
				lpcOrientedEdgeList.push_back(pcConvCoEdge->GetOrientedEdge());
			}
		};

		// 생성된 OrientEdge 크기를 확인
		if(0 == lpcOrientedEdgeList.size()) {
			return false;
		}


		// Loop가 Open되어 있는지 여부를 확인해서 Open되어 있는 경우 Close되도록 Edge를 추가한다.
		// Cone Surface에 대해서만 확인한다. (Cone은 Pole까지 Surface가 있는 경우 Curve가 Open됨)
		MbeSpaceType eSpaceType = pcSurface->IsA();

		if(st_ConeSurface == eSpaceType) {
			CheckOrientedEdgeOpenAndHealing(lpcOrientedEdgeList, pcSurface, 1.0e-6);
		}
		else {
			// ConeSurface가 아닌 경우에서 총 2개의 Edge Curve가 모드 line인 경우 Loop를 만들지 않는다.
			// 이것은 첫번째 Loop에 대해서만 적용하도록 한다.
			if(true == bFirstLoopFlag && 2 == lpcOrientedEdgeList.size()) {
				if(true == IsOrientedEdgeAllPlaneLine(lpcOrientedEdgeList, pcSurface)) {
					return false;
				}
			}
		}
	*/

	/*
		if(true == bFirstLoopFlag) {
			if(true == IsOpenOrientedEdgeList(lpcOrientedEdgeList, pcSurface, 1.0e-2)) {
				return false;
			}
		}
	*/

	/*
		MbLoop * pcLoop = new MbLoop();
		if(nullptr == pcLoop) {
			ASSERT(false);
			return false;
		}

		for(const MbOrientedEdge * pcOrientedEdge : lpcOrientedEdgeList) {
			pcLoop->AddEdge(*pcOrientedEdge);
		}

		if(bOrientationWithShell != bOrientationWithSurfaceFlag) {
			pcLoop->PartialReverse();
		}

		pcConvLoop->SetMbLoop(pcLoop);
	*/

	return true;
}

bool C3dImport::ConvertMbOrientedEdge(ConvCoEdge * pcConvCoEdge, SurfaceSPtr & pcSurface)
{
	if(nullptr != pcConvCoEdge) {
		RETURN_FALSE;
	}

	// 1. TopoCodeEdge의 Data값을 얻어온다.
	A3DTopoCoEdgeData & cTopoCoEdgeData = pcConvCoEdge->GetTopoCoEdgeData();

	bool bOrientationWithLoopFlag = (1 == cTopoCoEdgeData.m_ucOrientationWithLoop) ? true : false;
	bool bOrientationUVWithLoop = (1 == cTopoCoEdgeData.m_ucOrientationUVWithLoop) ? true : false;

	// 이웃 CoEdge를 이용해서 이웃 Orient Edge를 찾아와서 거기에 들어 있는 CurveEdge를 이용해서, 
	// Orient Edge를 생성한다.
	/*A3DTopoCoEdge * pcNeighborCoEdge = cTopoCoEdgeData.m_pNeighbor;
	if(nullptr != pcNeighborCoEdge) {
		auto cIterator = m_mpcConvCoEdgeMap.find(pcNeighborCoEdge);
		if(m_mpcConvCoEdgeMap.end() != cIterator) {
			MbOrientedEdge * pcNeighborOrientedEdge = (MbOrientedEdge *) cIterator->second;
			MbCurveEdge & cNeighborCurveEdge = pcNeighborOrientedEdge->GetCurveEdge();
			MbOrientedEdge * pcOrientedEdge = new MbOrientedEdge(cNeighborCurveEdge, bOrientationWithLoopFlag);
			pcConvCoEdge->SetOrientedEdge(pcOrientedEdge);
			return true;
		}
	}
*/

	ConvEdge * pcConvEdge = pcConvCoEdge->GetConvEdge();
	if(nullptr == pcConvEdge) {
		ASSERT(false);
		return false;
	}

	MbOrientedEdge * pcOrientedEdge = nullptr;
	A3DTopoEdge * pcTopoEdge = cTopoCoEdgeData.m_pEdge;
	MbCurveEdge * pcCurveEdge = nullptr;
	/*

		// 2. 사전에 생성된 Curve Edge가 있는 경우 조건을 판단해서 Orietn Edge를 생성해서 돌려 보내준다.
		if(true == FindMbCurveEdge(pcTopoEdge, pcCurveEdge)) {
			pcOrientedEdge = new MbOrientedEdge(*pcCurveEdge, bOrientationWithLoopFlag);
			pcConvCoEdge->SetOrientedEdge(pcOrientedEdge);
			return true;
		}
	*/

	// 3. Neighbor Surface 정보를 찾아온다. (pcNeighborTopoCoEdge를 이용해서 검색을 실시한다.)
	ConvSurface * pcNeighborConvSurface = pcConvCoEdge->GetNeighborSurface();
	SurfaceSPtr pcNeighborSurface;
	if(false == GetMbSurface(pcNeighborConvSurface, pcNeighborSurface)) {
		//ASSERT(false);
	}

	// ----- 4. MbCurveEdge Create -----
	// 4-1. Plane Curve 생성
	PlaneCurveSPtr pcPlaneCurve;
	if(false == pcConvCoEdge->GetConvCurve()->ConvertPlaneCurve(pcSurface, m_dContextScale, pcPlaneCurve)) {
		return false;
	}

	// 4-2. Neighbor Plane Curve 생성
	PlaneCurveSPtr pcNeighborPlaneCurve;
	if(nullptr != &pcNeighborSurface->GetSurface()) {
		if(false == pcConvCoEdge->GetNeighborCurve()->ConvertPlaneCurve(pcNeighborSurface, m_dContextScale, pcNeighborPlaneCurve)) {
			//return false;
		}
	}

	bool bFlag1 = true;
	bool bFlag2 = false;

	// 이웃 CoEdge에서 찾아온 Curve 정보를 이용해서 Intersection Curve를 생성
	MbSurfaceIntersectionCurve * pcInterCurve = nullptr;
	if(nullptr != pcNeighborSurface && nullptr != pcNeighborPlaneCurve) { // 이웃 Face가 있는 경우 처리
		pcInterCurve = new MbSurfaceIntersectionCurve(*pcSurface, *pcPlaneCurve,
			*pcNeighborSurface, *pcNeighborPlaneCurve, MbeCurveBuildType::cbt_Ordinary, bFlag1, bFlag2);
	}
	else {
		//  Convert Ri에서 시작된 Face들은 Face Set이거나 Solid 구성이 아니기 때문에, 이웃 CorEdge가 없다.
		pcInterCurve = new MbSurfaceIntersectionCurve(*pcSurface, *pcPlaneCurve,
			*pcSurface, *pcPlaneCurve, MbeCurveBuildType::cbt_Ordinary, bFlag1, bFlag2);
	}

	if(bOrientationUVWithLoop != bOrientationWithLoopFlag) {
		pcInterCurve->Inverse();
	}

	// 5. Vertex 정보를 검색하고, 저장된 값이 없으면 생성한다.
	ConvVertex * pcStartConvVertex = pcConvEdge->GetStartConvVertex();
	ConvVertex * pcEndConvVertex = pcConvEdge->GetEndConvVertex();

	A3DTopoVertex * pcStartTopoVertex = pcStartConvVertex->GetTopoVertex();
	A3DTopoVertex * pcEndTopoVertex = pcEndConvVertex->GetTopoVertex();

	VertexSPtr pcStartVertex, pcEndVertex;

	// 5-1. Start Vertex
	if(false == FindTopoVertex(pcStartTopoVertex, pcStartVertex)) {
		if(true == ConvertTopoVertex(pcStartConvVertex, pcStartVertex)) {
			AddTopoVertexToMap(pcStartTopoVertex, pcStartVertex);
		}
	}

	// 5-2. End Vertex
	if(false == FindTopoVertex(pcEndTopoVertex, pcEndVertex)) {
		if(true == ConvertTopoVertex(pcEndConvVertex, pcEndVertex)) {
			AddTopoVertexToMap(pcEndTopoVertex, pcEndVertex);
		}
	}

	pcCurveEdge = new MbCurveEdge(*pcStartVertex, *pcEndVertex, *pcInterCurve, true);

	pcOrientedEdge = new MbOrientedEdge(*pcCurveEdge, bOrientationWithLoopFlag);
	if(nullptr == pcOrientedEdge) {
		return false;
	}

	//m_mpcConvCoEdgeMap.insert(std::make_pair(pcConvCoEdge->GetTopoCoEdge(), pcOrientedEdge));

	pcConvCoEdge->SetOrientedEdge(pcOrientedEdge);

	return true;
}

bool C3dImport::ThreadConvertMbOrientedEdge(ConvCoEdge & cConvCoEdge, double dTopoContextScale, MbModel * pcModel)
{
	//ASSERT_RETURN_FALSE(nullptr != cConvCoEdge);
/*

	// 1. TopoCodeEdge의 Data값을 얻어온다.
	A3DTopoCoEdgeData & cTopoCoEdgeData = cConvCoEdge.GetTopoCoEdgeData();

	bool bOrientationWithLoopFlag = (1 == cTopoCoEdgeData.m_ucOrientationWithLoop) ? true : false;
	bool bOrientationUVWithLoop = (1 == cTopoCoEdgeData.m_ucOrientationUVWithLoop) ? true : false;

// 	Log(2, L"A3DTopoCoEdge: %s, %s", LogHexStr((DWORD_PTR) cConvCoEdge.GetTopoCoEdge()),
// 		LogHexStr((DWORD_PTR) cTopoCoEdgeData.m_pNeighbor));

	// 1. 이웃 CoEdge를 이용해서 이웃 Orient Edge를 찾아와서 거기에 들어 있는 CurveEdge를 이용해서, Orient Edge를 생성한다.
	A3DTopoCoEdge * pcNeighborTopoCoEdge = cTopoCoEdgeData.m_pNeighbor;
	if(nullptr != pcNeighborTopoCoEdge) {
		// 이웃 TopoCoEdge를 이용해서 ConvCoEdge를 검색한다.
		auto cIterator = cConvCoEdge.m_mpcConvCoEdgeMap->find(pcNeighborTopoCoEdge);
		if(cConvCoEdge.m_mpcConvCoEdgeMap->end() != cIterator) {
			ConvCoEdge * pcNeighborConvCoEdge = (ConvCoEdge *) cIterator->second;
			// ConvCoEdge에서 TopoCoEdgeData를 찾아온다.
			A3DTopoCoEdgeData & cNeighborTopoCoEdgeData = pcNeighborConvCoEdge->GetTopoCoEdgeData();

			// 이웃 ConvCoEdge의 이웃 TopoCoEdge가 변환중인 TopoCoEdge와 같은 경우에는 CurveEdge를 이용한다.
			// 다중 3D Curve가 생성되는 경우가 있기 때문에 확인해야 함.
			if(cNeighborTopoCoEdgeData.m_pNeighbor == cConvCoEdge.GetTopoCoEdge()) {
				if(nullptr != pcNeighborConvCoEdge->GetOrientedEdge()) {
					MbCurveEdge & cNeighborCurveEdge = pcNeighborConvCoEdge->GetOrientedEdge()->GetCurveEdge();
					MbOrientedEdge * pcOrientedEdge = new MbOrientedEdge(cNeighborCurveEdge, bOrientationWithLoopFlag);
					cConvCoEdge.SetOrientedEdge(pcOrientedEdge);
					return true;
				}
			}
		}
	}

	ConvEdge * pcConvEdge = cConvCoEdge.GetConvEdge();
	if(nullptr == pcConvEdge) {
		ASSERT(false);
		return false;
	}

	MbOrientedEdge * pcOrientedEdge = nullptr;
	A3DTopoEdge * pcTopoEdge = cTopoCoEdgeData.m_pEdge;
	EdgeSPtr pcCurveEdge;

	// 2. Neighbor Surface 정보를 찾아온다. (pcNeighborTopoCoEdge를 이용해서 검색을 실시한다.)
	ConvSurface * pcNeighborConvSurface = cConvCoEdge.GetNeighborSurface();
	SurfaceSPtr pcNeighborSurface;
	if(false == GetMbSurface(*cConvCoEdge.m_mpcSurfBaseMap, pcNeighborConvSurface, dTopoContextScale, pcNeighborSurface, pcModel)) {
		//ASSERT(false);
	}

	// ----- 3. MbCurveEdge Create -----
	// 3-1. Plane Curve 생성
	//PlaneCurveSPtr pcPlaneCurve;
	PlaneCurveSPtr pcPlaneCurve;
	if(false == cConvCoEdge.GetConvCurve()->ConvertPlaneCurve(cConvCoEdge.m_pcSurface, dTopoContextScale, pcPlaneCurve)) {
		return false;
	}

	// 3-2. Neighbor Plane Curve 생성
	PlaneCurveSPtr pcNeighborPlaneCurve;
	if(nullptr != pcNeighborSurface) {
		if(false == cConvCoEdge.GetNeighborCurve()->ConvertPlaneCurve(pcNeighborSurface, dTopoContextScale, pcNeighborPlaneCurve)) {
			ASSERT(false);
		}
	}

	bool bFlag1 = true;
	bool bFlag2 = true;

	// 이웃 CoEdge에서 찾아온 Curve 정보를 이용해서 Intersection Curve를 생성
	IntersectionCurveSPtr pcInterCurve;
	if(nullptr != pcNeighborSurface && nullptr != pcNeighborPlaneCurve) { // 이웃 Face가 있는 경우 처리
		pcInterCurve = new MbSurfaceIntersectionCurve(*cConvCoEdge.m_pcSurface, *pcPlaneCurve,
			*pcNeighborSurface, *pcNeighborPlaneCurve, MbeCurveBuildType::cbt_Ordinary, bFlag1, bFlag2);
	}
	else {
		//  Convert Ri에서 시작된 Face들은 Face Set이거나 Solid 구성이 아니기 때문에, 이웃 CorEdge가 없다.
		pcInterCurve = new MbSurfaceIntersectionCurve(*cConvCoEdge.m_pcSurface, *pcPlaneCurve,
			*cConvCoEdge.m_pcSurface, *pcPlaneCurve, MbeCurveBuildType::cbt_Ordinary, bFlag1, bFlag2);
	}

	if(bOrientationUVWithLoop != bOrientationWithLoopFlag) {
		pcInterCurve->Inverse();
	}

	// 4. Vertex 정보를 검색하고, 저장된 값이 없으면 생성한다.
	ConvVertex * pcStartConvVertex = pcConvEdge->GetStartConvVertex();
	ConvVertex * pcEndConvVertex = pcConvEdge->GetEndConvVertex();


	if(nullptr != pcStartConvVertex && nullptr != pcEndConvVertex) {
		A3DTopoVertex * pcStartTopoVertex = pcStartConvVertex->GetTopoVertex();
		A3DTopoVertex * pcEndTopoVertex = pcEndConvVertex->GetTopoVertex();

		VertexSPtr pcStartVertex;
		VertexSPtr pcEndVertex;

		// 5-1. Start Vertex
		if(false == FindTopoVertex(*cConvCoEdge.m_mpcMbVertexMap, pcStartTopoVertex, pcStartVertex)) {
			if(true == pcStartConvVertex->ConvertTopoVertex(dTopoContextScale, pcStartVertex)) {
				AddTopoVertexToMap(*cConvCoEdge.m_mpcMbVertexMap, pcStartTopoVertex, pcStartVertex);
			}
		}

		// 5-2. End Vertex
		if(false == FindTopoVertex(*cConvCoEdge.m_mpcMbVertexMap, pcEndTopoVertex, pcEndVertex)) {
			if(true == pcEndConvVertex->ConvertTopoVertex(dTopoContextScale, pcEndVertex)) {
				AddTopoVertexToMap(*cConvCoEdge.m_mpcMbVertexMap, pcEndTopoVertex, pcEndVertex);
			}
		}

		pcCurveEdge = new MbCurveEdge(*pcStartVertex, *pcEndVertex, *pcInterCurve, true);
	}
	else {
		pcCurveEdge = new MbCurveEdge(*pcInterCurve, true);
	}

	pcOrientedEdge = new MbOrientedEdge(*pcCurveEdge, bOrientationWithLoopFlag);
	if(nullptr == pcOrientedEdge) {
		return false;
	}

	cConvCoEdge.m_mpcConvCoEdgeMap->insert(std::make_pair(cConvCoEdge.GetTopoCoEdge(), &cConvCoEdge));

	cConvCoEdge.SetOrientedEdge(pcOrientedEdge);
*/
	return true;
}

bool C3dImport::ConvertMbFace_V1(A3DUns32 nIndex, A3DTopoFace * pcTopoFace, bool bOrientationWithShell, A3DMiscCascadedAttributes * pcParentAttr,
	A3DMiscCascadedAttributesData & cParentAttributesData, c3d::FaceSPtr & pcFace)
{
	// ConvFace를 생성해서, Thread 작업을 준비한다.
	ConvFace cConvFace(pcTopoFace, m_dContextScale, m_pcCurrentTopoBrepData, m_mpcConvSurfaceMap);
	if(false == cConvFace.IsInit()) {
		Log(2, L"Error - cConvFace.IsInit()");
		return false;
	}

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcTopoFace, pcParentAttr, &pcAttrs, &cAttrsData);

	if(A3D_FALSE == cAttrsData.m_bShow) {
		ASSERT(false);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return false;
	}

	// 1. Surface를 변환. MbLoop를 생성하기 위해서, Surface를 미리 만들어야 함.
	ConvSurface * pcConvSurface = cConvFace.GetConvSurface();
	if(nullptr == pcConvSurface) {
		ASSERT(false);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return false;
	}

	SurfaceSPtr pcSurface;
	bool bSwapUVFlag = false;
	if(false == GetMbSurface(pcConvSurface, pcSurface)) {
		ASSERT(false);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		SetLastErrorMessage(L"ConvertTopoFace - GetMbSurface Error");
		return false;
	}

	// #import_debug : Base Surface Drawing
// 	SInstanceSPtr cSpaceInstance(new MbSpaceInstance(*pcSurface));
// 	m_pcModel->AddItem(*cSpaceInstance);

	// 2. Loop Data를 수집
	bool bFirstLoopFlag = true;
	LoopsSPtrVector vpcLoopVector;
	for(ConvLoop * pcConvLoop : cConvFace.GetLoopVector()) {
		if(true == ConvertMbLoop(pcConvLoop, pcSurface, bOrientationWithShell, bFirstLoopFlag)) {
			vpcLoopVector.push_back(pcConvLoop->GetMbLoop());
		}
		else {
			return false;
		}

		bFirstLoopFlag = false;
	}

	if(true == vpcLoopVector.empty()) {
		Log(2, L"Error - vpcLoopVector.empty()");
		ASSERT(false);
		return false;
	}

	MbSurface * pcOriginSurface = pcSurface;

	A3DTopoFaceData & cTopoFaceData = cConvFace.GetTopoFaceData();

	SurfaceSPtr pcBoundedSurface;
	if(true == CreateBoundedSurface(pcSurface, vpcLoopVector[0], bOrientationWithShell, true, pcBoundedSurface)) {
		pcSurface = pcBoundedSurface;
	}

	/*
		// 3. 최외각 Loop를 이용해서, Surface의 Boundary를 Curve가 넘어가는지 여부를 확인하고 그런경우 Boundary Surface를 생성해서 사용하도록 한다.
		switch(pcSurface->IsA())
		{
			// 3-1. Periodic Surface만 적용하도록 한다.
			case st_CylinderSurface:
			case st_ConeSurface:
			case st_SphereSurface:
			{
				double dVMin1 = pcSurface->GetVMin();
				double dVMax1 = pcSurface->GetVMax();

				SurfaceSPtr pcBoundedSurface;

				if(TRUE == cTopoFaceData.m_bHasTrimDomain) {
					if(true == CreateBoundedSurface(pcSurface, cTopoFaceData.m_sSurfaceDomain, pcBoundedSurface)) {
						pcSurface = pcBoundedSurface;
					}
				}
				else if(true == CreateBoundedSurface(pcSurface, vpcLoopVector[0], bOrientationWithShell, pcBoundedSurface)) {
					double dVMin2 = pcSurface->GetVMin();
					double dVMax2 = pcSurface->GetVMax();

					pcSurface = pcBoundedSurface;
				}

				double dVMin3 = pcSurface->GetVMin();
				double dVMax3 = pcSurface->GetVMax();

				int i = 0;
			}
			break;

			// 3-2. Plane에서도 Face가 제대로 구성되지 않는 문제가 발생함.
			//      Torus는 Majon Minor Radius의 관계 때문에, 주어진 Domain을 사용하면 안되는 경우가 있음.
			case st_CylinderSurface:
			case st_ConeSurface:
			case st_SphereSurface:
			case st_TorusSurface:
			case st_Plane:
			{
				SurfaceSPtr pcBoundedSurface;
				if(true == CreateBoundedSurface(pcSurface, vpcLoopVector[0], bOrientationWithShell, pcBoundedSurface)) {
					pcSurface = pcBoundedSurface;
				}
			}
			break;

			default:
			{
				SurfaceSPtr pcBoundedSurface;
				if(true == CreateBoundedSurface(pcSurface, vpcLoopVector[0], bOrientationWithShell, pcBoundedSurface)) {
					pcSurface = pcBoundedSurface;
				}
			}
			break;
		}
		*/

		// 4. 생성된 Surface를 이용해서 Face를 생성한다.
	switch(pcOriginSurface->IsA())
	{
		// 3-1. Periodic Surface만 적용하도록 한다.
		case st_CylinderSurface:
		case st_ConeSurface:
		case st_SphereSurface:
		case st_TorusSurface:
		{
			const DmiSurface * pcRtSurface = dynamic_cast<const DmiSurface *>(pcOriginSurface);
			if(0.0 > pcRtSurface->GetVCoeff()) {
				bOrientationWithShell = !bOrientationWithShell;
			}
			else if(st_TorusSurface == pcOriginSurface->IsA())
			{
				// V 경계면을 넘어가는 UV 커브가 있는 경우 Major Radius를 Minus로 설정한다.
				if(0.0 > ((MbTorusSurface *) pcOriginSurface)->GetMajorRadius()) {
					bOrientationWithShell = !bOrientationWithShell;
				}
			}

			pcFace = new MbFace(*pcSurface, bOrientationWithShell);
		}
		break;

		default:
			pcFace = new MbFace(*pcSurface, bOrientationWithShell);
			break;
	}

	COLORREF cParentColor;
	bool bParentColorDefinedFlag = false;
	if(true == GetAttributeColor(cParentAttributesData, cParentColor)) {
		bParentColorDefinedFlag = true;
	}

	COLORREF cColor;
	if(true == GetAttributeColor(cAttrsData, cColor)) {
		bool bColoredFlag = true;
		if(true == bParentColorDefinedFlag) {
			if(cParentColor == cColor) {
				bColoredFlag = false;
			}
		}

		if(true == bColoredFlag) {
			pcFace->SetColor(cColor);
		}
	}

	float fParentTransparency = 1.0f;
	bool bParentTransparencyDefineFlag = false;
	if(true == GetAttributeTransparency(cParentAttributesData, fParentTransparency)) {
		bParentTransparencyDefineFlag = true;
	}

	float fTransparency = 1.0f;
	if(true == GetAttributeTransparency(cAttrsData, fTransparency)) {
		bool bTransparencyDefineFlag = true;
		if(true == bParentTransparencyDefineFlag) {
			if(fParentTransparency == fTransparency) {
				bTransparencyDefineFlag = false;
			}
		}

		if(true == bTransparencyDefineFlag) {
			float a, d, sp, sh, t, e;
			pcFace->GetVisual(a, d, sp, sh, t, e);
			pcFace->SetVisual(a, d, sp, sh, fTransparency, e);
		}
	}

	if(nullptr == pcFace) {
		SetLastErrorMessage(L"ConvertSurfPlane - MbFace nullptr");
		return false;
	}

	// 5. Loop를 Face에 추가.
	for(MbLoop * pcLoopElement : vpcLoopVector) {
		pcFace->AddLoop(*pcLoopElement);
	}

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return true;
}

// 3-11-3. OrientedEdge List의 Open 여부를 확인한다.
bool C3dImport::IsOpenOrientedEdgeList(std::list<const MbOrientedEdge *> & lpcOrientedEdgeList, const MbSurface * pcBaseSurface, double dTolerance)
{
	/*
		if(2 == lpcOrientedEdgeList.size()) {
		}

		auto cIterator = lpcOrientedEdgeList.begin();
		auto cCurIterator = cIterator;
		auto cNextIterator = cIterator;
		auto cEndIterator = std::prev(lpcOrientedEdgeList.end());

		while(cIterator != lpcOrientedEdgeList.end()) {
			const MbOrientedEdge * pcCurOrientedEdge = *cIterator;
			const MbOrientedEdge * pcNextOrientedEdge = nullptr;

			cCurIterator = cIterator;
			++cIterator;
			cNextIterator = cIterator;

			// 마지막 요소라면 처음 요소와 비교한다.
			if(cCurIterator == cEndIterator) {
				pcNextOrientedEdge = *lpcOrientedEdgeList.begin();
			}
			else {
				pcNextOrientedEdge = *cIterator;
			}

			bool bConnectFlag = false;
			if(true == CheckConectivity(pcCurOrientedEdge, pcNextOrientedEdge, pcBaseSurface, dTolerance, bConnectFlag)) {
				if(false == bConnectFlag) {
					return true;
				}
			}
		}*/

	return false;
}

bool C3dImport::CreateLineSegmentOrientedEdge_V1(std::list<const DmiOrientedEdge *> & lpcOrientedEdgeList,
	const DmiOrientedEdge * pcCurEdge, const DmiOrientedEdge * pcNextEdge, const MbSurface * pcBaseSurface, double dTolerance,
	const DmiOrientedEdge *& pcLineSegmentOrientedEdge)
{
	auto cEndIterator = std::prev(lpcOrientedEdgeList.end());

	auto pcCurEdgeIterator = std::find(lpcOrientedEdgeList.begin(), lpcOrientedEdgeList.end(), pcCurEdge);
	auto pcNextEdgeIterator = std::find(lpcOrientedEdgeList.begin(), lpcOrientedEdgeList.end(), pcNextEdge);

	// 각각의 Edge의 앞쪽 및 뒷쪽 Edge와 비교해서, 연결된 Curve를 이용해서 
	auto pcCurEdgePrevIterator = pcCurEdgeIterator;
	if(pcCurEdgePrevIterator == lpcOrientedEdgeList.begin()) {
		pcCurEdgePrevIterator = lpcOrientedEdgeList.end();
		pcCurEdgePrevIterator--;
	}
	else {
		pcCurEdgePrevIterator--;
	}

	auto pcNextEdgeNextIterator = pcNextEdgeIterator;
	if(pcNextEdgeNextIterator == cEndIterator) {
		pcNextEdgeNextIterator = lpcOrientedEdgeList.begin();
	}
	else {
		pcNextEdgeNextIterator++;
	}

	bool bConnectFlag = false;
	const MbCurve * pcCurCurve[2];
	pcCurCurve[0] = nullptr;
	pcCurCurve[1] = nullptr;
	// current edge의 연결 curve를 찾아온다. 
	// pcCurCurve에 각각 Cur, Next의 연결된 MbCurve를 찾아 온다.
	CheckConectivity(*pcCurEdgePrevIterator, *pcCurEdgeIterator, pcBaseSurface, dTolerance, bConnectFlag, pcCurCurve);

	const MbCurve * pcNextCurve[2];
	pcNextCurve[0] = nullptr;
	pcNextCurve[1] = nullptr;
	// next edge의 연결 curve를 찾아온다. 
	CheckConectivity(*pcNextEdgeIterator, *pcNextEdgeNextIterator, pcBaseSurface, dTolerance, bConnectFlag, pcNextCurve);

	// CurEdgePrev와 CurEdge가 연결된 Curve에서 pcCurCurve[1]가 CureEdge의 curve임.
	// NextEdgeNext와 NextEdge가 연결된 Curve에서 pcNextCurve[0]가 NextEdge의 curve임.
	if(nullptr == pcCurCurve[1] || nullptr == pcNextCurve[0]) {
		return false;
	}

	MbePlaneType eType1 = pcCurCurve[1]->IsA();
	MbePlaneType eType2 = pcNextCurve[0]->IsA();

	bool bCurSense = pcCurEdge->GetOrientation();
	bool bNextSense = pcNextEdge->GetOrientation();

	MbCartPoint cStartPoint, cEndPoint;

	if(true == bCurSense) {
		pcCurCurve[1]->GetEndPoint(cStartPoint);
	}
	else {
		pcCurCurve[1]->GetStartPoint(cStartPoint);
	}

	if(true == bNextSense) {
		pcNextCurve[0]->GetStartPoint(cEndPoint);
	}
	else {
		pcNextCurve[0]->GetEndPoint(cEndPoint);
	}

	MbLineSegment * pcLine1 = new MbLineSegment(cStartPoint, cEndPoint);
	if(nullptr == pcLine1) {
		ASSERT(NULL);
		return false;
	}

	MbLineSegment * pcLine2 = new MbLineSegment(cStartPoint, cEndPoint);
	if(nullptr == pcLine2) {
		ASSERT(NULL);
		return false;
	}

	MbSurfaceIntersectionCurve * pcInterCurve = new MbSurfaceIntersectionCurve(*pcBaseSurface, *pcLine1, *pcBaseSurface, *pcLine2,
		MbeCurveBuildType::cbt_Ordinary, true, false);

	const MbVertex & cStartVertex = pcCurEdge->GetEndVertex();
	const MbVertex & cEndVertex = pcNextEdge->GetBegVertex();

	MbCurveEdge * pcCurveEdge = new MbCurveEdge(cStartVertex, cEndVertex, *pcInterCurve, true);

	pcLineSegmentOrientedEdge = new DmiOrientedEdge(*pcCurveEdge, true, true);

	return true;
}
