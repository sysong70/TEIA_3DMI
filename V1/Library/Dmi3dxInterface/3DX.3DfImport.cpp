#include "stdafx.h"

#include "3DX.3DfImport.h"

#include "3DFSignal.Interface.h"

#include "LogManager.h"

#include <mb_matrix3d.h>

#include <3DF/3DF.MaterialMapping.h>
#include <3DF/3DF.Portfolio.h>
#include <3DF/3DF.Style.h>
#include <3DF/3DF.Shell.h>
#include <3DF/3DF.Line.h>

#include <chrono>
#include <utility>


#ifdef _DEBUG
//#	define USED_LOG_MANAGER
#endif

#define A3D_DRAW_WITH_INDICES

using namespace std::chrono;

#define CHECK_A3D_RETURN(FunctionCall)\
{\
	const A3DStatus iRet__ = FunctionCall;\
	if (iRet__ != A3D_SUCCESS)\
	{\
		if (A3DMiscGetErrorMsg)\
			fprintf(stderr, #FunctionCall " returned error %d = %s\n", iRet__ , A3DMiscGetErrorMsg(iRet__) ); \
		else\
			fprintf(stderr, #FunctionCall " returned error %d\n", iRet__ ); \
		return iRet__;\
	}\
}

USING_3DF_NAMESPACE
USING_3DX_NAMESPACE

_3DfImport::_3DfImport(_3DXSignal::Interface * pc3dxInterface) :
	ImportBase(pc3dxInterface)
{
}

_3DfImport::~_3DfImport()
{
}

bool _3DfImport::FileImport(CString strFilePathName, _3DF::SegmentKey & cModelSegment, CString & strErrorMessage)
{
	if(false == InitializeA3DLibrary(strErrorMessage)) {
		return false;
	}

	m_pcModelSegment = &cModelSegment;

	system_clock::time_point cTime1 = system_clock::now();
	long long tick = cTime1.time_since_epoch().count();

	m_strCadFileName = strFilePathName.Right(strFilePathName.GetLength() - strFilePathName.ReverseFind('\\') - 1);

	// ===== _3DfImport 옵션을 설정 =====
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

	//----- Model 관련 Include 선언 -----
	SegmentKey cModelInclude = cModelSegment.ModelInclude();

	m_cPartsIncludeSegment = cModelInclude.Subsegment(L"parts");
	m_cPoccsIncludeSegment = cModelInclude.Subsegment(L"poccs");
	m_cRisIncludeSegment = cModelInclude.Subsegment(L"ris");

	// Import 관련 Option 설정
	ImportOption sImportOption;

	// 처음부터 
	sImportOption.cParentSegment = cModelSegment;

	LoadMatrixIdentity();

	cModelSegment.SetVisibility(L"lines=on");
	
	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetLineColor(RGBAColor(0, 0, 0));
	cModelSegment.SetMaterialMapping(cMaterialMapping);

	bool bStatus = DrawModel(pcAsmModelFile, cModelSegment);

	A3DAsmModelFileDelete(pcAsmModelFile);

	system_clock::time_point cTime2 = system_clock::now();

	return bStatus;
}

// == 3DX 설정 관련 함수 ==============================================================================
bool _3DfImport::SetDefaultParamsLoadData(A3DRWParamsLoadData & cParamsLoadData)
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
	//cParamsLoadData.m_sMultiEntries

	// 특정 CAD Format 관련 변수
	// cParamsLoadData.m_sSpecifics

	//
	// cParamsLoadData.m_sIncremental

	return true;
}
//== 1. 3DF 변환 관련 함수 ============================================================================
bool _3DfImport::DrawModel(const A3DAsmModelFile * pcAsmModelFile, _3DF::SegmentKey & cModelSegment)
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
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesCreate(&pcAttrs));

	for(A3DUns32 nIndex = 0; nIndex < cModelFileData.m_uiPOccurrencesSize; ++nIndex) {
		DrawProductOccurrence(cModelFileData.m_ppPOccurrences[nIndex], cModelSegment, pcAttrs);
	}

	A3DAsmModelFileGet(nullptr, &cModelFileData);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	
	A3DGlobalGet(nullptr, &m_cGlobalData);

	LogDecreaseTabIndex(2);

	return true;
}

// == 2. Product Occurrences 관련 함수 ===============================================================

// 2-1. Product Occurrence 처리
A3DStatus _3DfImport::DrawProductOccurrence(A3DAsmProductOccurrence * pcOccurrence, _3DF::SegmentKey & cParentSegment, 
	A3DMiscCascadedAttributes * pcParentAttr) // , AssemblySPtr & pcParentAssy
{
	if(nullptr == pcOccurrence) {
		return A3D_ERROR;
	}

	// Segment를 생성하고 생성된 Segment를 Parent Segment에 Include한다.
	SegmentKey cSegment = m_cPoccsIncludeSegment.Subsegment(L"pocc%d", m_nIncrementalId++);
	cParentSegment.IncludeSegment(cSegment);

	// Attribute 생성
	// Parent에서 받은(계단식으로) Attribute를 이용해서, Attribute를 생성
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcOccurrence, pcParentAttr, &pcAttrs, &cAttrsData));

	MaterialMappingKit cMaterialMapping;
	A3DInt32 iUVCoordinatesIndex = -1;
	A3DUns8 ucTextureDimension = 2;
	DrawStyle(&cAttrsData.m_sStyle, &iUVCoordinatesIndex, &ucTextureDimension, cMaterialMapping);

	if(cAttrsData.m_bShow && !cAttrsData.m_bRemoved && true == IsShow(pcOccurrence))
	{
		A3DAsmProductOccurrenceData cData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcOccurrence, &cData));

		if(cData.m_ucBehaviour != 1)
		{
			A3DRootBaseData cRootBaseData;
			A3D_INITIALIZE_DATA(A3DRootBaseData, cRootBaseData);
			CHECK_A3D_RETURN(A3DRootBaseGet(pcOccurrence, &cRootBaseData));

			if(cRootBaseData.m_uiSize > 0)
			{
				for(A3DUns32 ui = 0; ui < cRootBaseData.m_uiSize; ui++)
				{
					A3DMiscAttributeData sMiscAttrData;
					A3D_INITIALIZE_DATA(A3DMiscAttributeData, sMiscAttrData);
					CHECK_A3D_RETURN(A3DMiscAttributeGet(cRootBaseData.m_ppAttributes[ui], &sMiscAttrData));
					CHECK_A3D_RETURN(A3DMiscAttributeGet(nullptr, &sMiscAttrData));
				}
			}

			MatrixKit cMatrix;
			if(A3D_SUCCESS == ProductOccurrenceGetLocation(&cData, cMatrix)) {
				cSegment.SetModellingMatrix(cMatrix);
			}

			A3DAsmPartDefinition * pcPart = nullptr;
			CHECK_A3D_RETURN(ProductOccurrenceGetPart(&cData, &pcPart));
			if(nullptr != pcPart)
			{
				CHECK_A3D_RETURN(DrawPartDefinition(pcPart, cSegment, pcAttrs));
			}

			A3DPointerArray apcChildArray;
			PointerArrayInitialize(&apcChildArray);
			CHECK_A3D_RETURN(ProductOccurrenceGetChild(&cData, &apcChildArray));

			A3DUns32 nSize = apcChildArray.m_uiSize;
			for(A3DUns32 nIndex = 0; nIndex < nSize; nIndex++) {
				CHECK_A3D_RETURN(DrawProductOccurrence(apcChildArray.m_ppPointers[nIndex], cSegment, pcAttrs));
			}

			PointerArrayTerminate(&apcChildArray);

			if(true == m_bDrawMarkups)
			{
				A3DPointerArray cMarkups, cViews;
				PointerArrayInitialize(&cMarkups);
				PointerArrayInitialize(&cViews);
				ProductOccurrenceGetMarkups(&cData, &cMarkups);
				ProductOccurrenceGetViews(&cData, &cViews);

				nSize = cViews.m_uiSize;
				for(A3DUns32 nIndex = 0; nIndex < nSize; nIndex++) {
					CHECK_A3D_RETURN(DrawMarkupView(cViews.m_ppPointers[nIndex], cSegment, pcAttrs));
				}
					

				nSize = cMarkups.m_uiSize;
				for(UINT nIndex = 0; nIndex < nSize; nIndex++) {
					CHECK_A3D_RETURN(DrawMarkup(cMarkups.m_ppPointers[nIndex], cSegment, cMaterialMapping, pcAttrs));
				}

				PointerArrayTerminate(&cMarkups);
				PointerArrayTerminate(&cViews);
			}

			CHECK_A3D_RETURN(A3DRootBaseGet(nullptr, &cRootBaseData));
		}

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttrs));

/*
	// Product Occurrence Data 추출
	A3DAsmProductOccurrenceData cPOData;
	A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cPOData);
	A3DStatus eStatus = A3DAsmProductOccurrenceGet(pcOccurrence, &cPOData);
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

	CString strPoName;
	GetName(pcOccurrence, strPoName);

	// 위치 정보 Matrix를 가져온다.
	MbMatrix3D cMatrix;
	GetProductOccurrenceTransformMatrix(&cPOData, cMatrix);

	AssemblySPtr pcAssy(new MbAssembly());
	SetItemName(pcAssy, strPoName);

	// Parent Assy가 nullptr인 경우 최상단 Node라고 생각한다.
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
		GetProductOccurrenceFilePath(pcOccurrence, strFilePath, strOriginalFilePath);
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
*/

	return A3D_SUCCESS;
}

// 2-1-1. Assembly Product Occurence의 위치를 가져오는 함수 / ProductOccurrenceGetLocation
A3DStatus _3DfImport::ProductOccurrenceGetLocation(A3DAsmProductOccurrenceData const * pcPoData, MatrixKit & cTransMatrix)
{
	if(nullptr == pcPoData) {
		return A3D_ERROR;
	}

	A3DMiscTransformation * pcLocation = nullptr;

	bool bFlag = false;

	if(nullptr != pcPoData->m_pLocation)
	{
		pcLocation = pcPoData->m_pLocation;
		GetMatrix(pcLocation, cTransMatrix);
		return A3D_SUCCESS;
	}
	
	if(nullptr != pcPoData->m_pPrototype)
	{
		A3DAsmProductOccurrenceData cProductPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cProductPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPoData->m_pPrototype, &cProductPrototypeData));

		ProductOccurrenceGetLocation(&cProductPrototypeData, cTransMatrix);

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData))

		return A3D_SUCCESS;
	}

	if(0 == pcPoData->m_uiPOccurrencesSize)
	{
		A3DAsmProductOccurrence * pExternal = nullptr;
		CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(pcPoData, &pExternal));

		if(nullptr != pExternal)
		{
			A3DAsmProductOccurrenceData sExternalDataData;
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sExternalDataData);
			CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pExternal, &sExternalDataData));

			ProductOccurrenceGetLocation(&sExternalDataData, cTransMatrix);

			CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &sExternalDataData));

			return A3D_SUCCESS;
		}
	}

	Log(2, L"GetProductOccurrenceTransformMatrix");

	return A3D_ERROR;
}

// 2-1-1. Product Occurence의 위치를 가져오는 함수
A3DStatus _3DfImport::ProductOccurrenceGetLocation(const A3DAsmProductOccurrenceData * pcPOccData,
	A3DMiscCartesianTransformation ** ppcLocation)
{
	if(nullptr == pcPOccData) {
		return A3D_ERROR;
	}

	*ppcLocation = nullptr;

	if(nullptr != pcPOccData->m_pLocation)
	{
		*ppcLocation = pcPOccData->m_pLocation;
		return A3D_SUCCESS;
	}

	A3DAsmProductOccurrence * pcProductPrototype = pcPOccData->m_pPrototype;

	while(nullptr != pcProductPrototype)
	{
		A3DAsmProductOccurrenceData cProductPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cProductPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcProductPrototype, &cProductPrototypeData));

		if(nullptr != cProductPrototypeData.m_pLocation)
		{
			CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData));
			*ppcLocation = cProductPrototypeData.m_pLocation;
			return A3D_SUCCESS;
		}
		else {
			pcProductPrototype = cProductPrototypeData.m_pPrototype;
		}
			
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData));
	}

	if(0 == pcPOccData->m_uiPOccurrencesSize)
	{
		A3DAsmProductOccurrence * pExternal = nullptr;
		CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(pcPOccData, &pExternal));
		if(nullptr != pExternal)
		{
			A3DAsmProductOccurrenceData sExternalDataData;
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sExternalDataData);
			CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pExternal, &sExternalDataData));
			A3DAsmPartDefinition * pLoc;
			CHECK_A3D_RETURN(ProductOccurrenceGetLocation(&sExternalDataData, &pLoc));
			*ppcLocation = pLoc;
			CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &sExternalDataData));

			return A3D_SUCCESS;
		}
	}

	return A3D_SUCCESS;
}

// 2-1-2. Product Occurence의 External Data 위치를 가져오는 함수
A3DStatus _3DfImport::ProductOccurrenceGetExternalData(const A3DAsmProductOccurrenceData * pcPOccData,
	A3DAsmProductOccurrence ** ppcExternalData)
{
	if(nullptr == pcPOccData) {
		return A3D_ERROR;
	}

	if(pcPOccData->m_pExternalData == nullptr && pcPOccData->m_pPrototype != nullptr)
	{
		A3DAsmProductOccurrenceData cProductPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cProductPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPOccData->m_pPrototype, &cProductPrototypeData));

		CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(&cProductPrototypeData, ppcExternalData));

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData));
	}
	else
		*ppcExternalData = pcPOccData->m_pExternalData;

	return A3D_SUCCESS;
}

// 2-1-3. Product Occurrence의 Markups을 가져옴
A3DStatus _3DfImport::ProductOccurrenceGetMarkups(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray)
{
	if(pcPOccData == nullptr || pcArray == nullptr) {
		return A3D_ERROR;
	}

	if(pcPOccData->m_uiAnnotationsSize == 0 && pcPOccData->m_pPrototype != nullptr) {
		A3DAsmProductOccurrenceData sPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPOccData->m_pPrototype, &sPrototypeData));

		CHECK_A3D_RETURN(ProductOccurrenceGetMarkups(&sPrototypeData, pcArray));

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &sPrototypeData));
	}
	else {
		UINT nIndex, uiSize = pcPOccData->m_uiAnnotationsSize;
		for(nIndex = 0; nIndex < uiSize; nIndex++) {
			CHECK_A3D_RETURN(AnnotationGetMarkups(pcPOccData->m_ppAnnotations[nIndex], pcArray));
		}
	}

	return A3D_SUCCESS;
}

// 2-1-3. Product Occurrence의 View를 가져옴
A3DStatus _3DfImport::ProductOccurrenceGetViews(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray)
{
	if(pcPOccData == nullptr || pcArray == nullptr)
		return A3D_ERROR;

	if(pcPOccData->m_uiViewsSize == 0 && pcPOccData->m_pPrototype != nullptr) {
		A3DAsmProductOccurrenceData sPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPOccData->m_pPrototype, &sPrototypeData));

		CHECK_A3D_RETURN(ProductOccurrenceGetViews(&sPrototypeData, pcArray));

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &sPrototypeData));
	}
	else {
		PointerArrayAddArray(pcArray, pcPOccData->m_ppViews, pcPOccData->m_uiViewsSize);
	}

	return A3D_SUCCESS;
}

// 2-2. Product Occurrence의 Child를 가져오는 함수
A3DStatus _3DfImport::ProductOccurrenceGetChild(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray)
{
	if(pcPOccData == nullptr || pcArray == nullptr)
		return A3D_ERROR;

	A3DStatus iRet = A3D_SUCCESS;

	CHECK_A3D_RETURN(ProductOccurrenceGetOccurrences(pcPOccData, pcArray));

	A3DAsmProductOccurrence * pExternal = nullptr;
	CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(pcPOccData, &pExternal));
	if(pExternal != nullptr)
	{
		A3DAsmProductOccurrenceData sExternalDataData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sExternalDataData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pExternal, &sExternalDataData));

		if(pcArray->m_uiSize == 0)
		{
			CHECK_A3D_RETURN(ProductOccurrenceGetChild(&sExternalDataData, pcArray));
		}
		else {
			PointerArrayAdd(pcArray, pExternal);
		}

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &sExternalDataData));
	}

	return iRet;
}

// 2-2-1. Product Occurrence의 Occurrence를 가져오는 함수
A3DStatus _3DfImport::ProductOccurrenceGetOccurrences(const A3DAsmProductOccurrenceData * pcPOccData,
	A3DPointerArray * pcArray)
{
	if(pcPOccData == nullptr || pcArray == nullptr)
		return A3D_ERROR;

	A3DStatus iRet = A3D_SUCCESS;

	if(pcPOccData->m_uiPOccurrencesSize == 0 && pcPOccData->m_pPrototype != nullptr)
	{
		A3DAsmProductOccurrenceData cPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPOccData->m_pPrototype, &cPrototypeData));

		CHECK_A3D_RETURN(ProductOccurrenceGetOccurrences(&cPrototypeData, pcArray));

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cPrototypeData));
	}
	else
	{
		PointerArrayAddArray(pcArray, pcPOccData->m_ppPOccurrences, pcPOccData->m_uiPOccurrencesSize);
	}

	return iRet;

}
// 2-3. Annotation Markups을 가져옴
A3DStatus _3DfImport::AnnotationGetMarkups(const A3DMkpAnnotationEntity * pcAnnotation, A3DPointerArray * pcArray)
{
	A3DStatus iRet = A3D_SUCCESS;
	A3DEEntityType eType = kA3DTypeUnknown;

	CHECK_A3D_RETURN(A3DEntityGetType(pcAnnotation, &eType));

	switch(eType)
	{
		case kA3DTypeMkpAnnotationSet:
			CHECK_A3D_RETURN(AnnotationSetGetMarkups(pcAnnotation, pcArray));
			break;

		case kA3DTypeMkpAnnotationReference:
			CHECK_A3D_RETURN(AnnotationReferenceGetMarkups(pcAnnotation, pcArray));
			break;

		case kA3DTypeMkpAnnotationItem:
			CHECK_A3D_RETURN(AnnotationItemGetMarkups(pcAnnotation, pcArray));
			break;

		default:
			break;
	}

	return iRet;
}

// 2-3-1. Annotation Set Markups을 가져옴
A3DStatus _3DfImport::AnnotationSetGetMarkups(const A3DMkpAnnotationSet * pcAnnotationSet, A3DPointerArray * pcArray)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DMkpAnnotationSetData sAnnotationSetData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationSetData, sAnnotationSetData);
	CHECK_A3D_RETURN(A3DMkpAnnotationSetGet(pcAnnotationSet, &sAnnotationSetData));

	for(A3DUns32 ui = 0; ui < sAnnotationSetData.m_uiAnnotationsSize; ui++)
	{
		CHECK_A3D_RETURN(AnnotationGetMarkups(sAnnotationSetData.m_ppAnnotations[ui], pcArray));
	}

	CHECK_A3D_RETURN(A3DMkpAnnotationSetGet(nullptr, &sAnnotationSetData));

	return iRet;
}

// 2-3-2. (미처리) Annotation Reference Markups을 가져옴
A3DStatus _3DfImport::AnnotationReferenceGetMarkups(const A3DMkpAnnotationItem * /*pcAnnotationItem*/, A3DPointerArray * /*pcArray*/)
{
	assert(false);
	return A3D_SUCCESS;
}

// 2-3-3. Annotation Item Markups을 가져옴
A3DStatus _3DfImport::AnnotationItemGetMarkups(const A3DMkpAnnotationItem * pcAnnotationItem, A3DPointerArray * pcArray)
{
	if(pcAnnotationItem == nullptr || pcArray == nullptr)
		return A3D_ERROR;

	A3DStatus iRet = A3D_SUCCESS;

	A3DMkpAnnotationItemData sAnnotationItemData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationItemData, sAnnotationItemData);
	CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(pcAnnotationItem, &sAnnotationItemData));

	PointerArrayAddUnique(pcArray, sAnnotationItemData.m_pMarkup);

	CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(nullptr, &sAnnotationItemData));

	return iRet;
}

//== 3. Part 관련 함수 ===============================================================================

// 3-1. 현재 product의 part를 가져오는 함수 
A3DStatus _3DfImport::ProductOccurrenceGetPart(const A3DAsmProductOccurrenceData * pcPOccData, A3DAsmPartDefinition ** ppcPart)
{
	if(nullptr == pcPOccData) {
		return A3D_ERROR;
	}

	*ppcPart = nullptr;

	if(nullptr != pcPOccData->m_pPart)
	{
		*ppcPart = pcPOccData->m_pPart;
		return A3D_SUCCESS;
	}

	A3DAsmProductOccurrence * pcProductPrototype = pcPOccData->m_pPrototype;
	while(nullptr != pcProductPrototype)
	{
		A3DAsmProductOccurrenceData cProductPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cProductPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcProductPrototype, &cProductPrototypeData));

		if(cProductPrototypeData.m_pPart != nullptr)
		{
			CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData));
			*ppcPart = cProductPrototypeData.m_pPart;
			return A3D_SUCCESS;
		}
		else {
			pcProductPrototype = cProductPrototypeData.m_pPrototype;
		}
			
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData));
	}

	if(pcPOccData->m_uiPOccurrencesSize == 0)
	{
		A3DAsmProductOccurrence * pcExternal = nullptr;
		CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(pcPOccData, &pcExternal));
		if(pcExternal != nullptr)
		{
			A3DAsmProductOccurrenceData sExternalDataData;
			A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sExternalDataData);
			CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcExternal, &sExternalDataData));
			A3DAsmPartDefinition * pPartDefinition;
			CHECK_A3D_RETURN(ProductOccurrenceGetPart(&sExternalDataData, &pPartDefinition));
			*ppcPart = pPartDefinition;

			CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &sExternalDataData));

			return A3D_SUCCESS;
		}
	}

	return A3D_SUCCESS;
}

//== Draw 관련 함수 ==================================================================================

// 1. Draw Part Definition
A3DStatus _3DfImport::DrawPartDefinition(const A3DAsmPartDefinition * pcPart, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	// 기존에 생성된 Part를 찾은 경우 Include로 Parent에 추가 시킨다.
	HC_KEY nSegmentKey = INVALID_KEY;
	if(true == m_mPartsMap.Lookup((DWORD_PTR) pcPart, nSegmentKey)) {
		SegmentKey cSegment(nSegmentKey);
		cParentSegment.IncludeSegment(cSegment);
		return A3D_SUCCESS;
	}

	// Segment를 생성하고 생성된 Segment를 Parent Segment에 Include한다.
	SegmentKey cSegment = m_cPartsIncludeSegment.Subsegment(L"part%d", m_nIncrementalId++);
	cParentSegment.IncludeSegment(cSegment);
	m_mPartsMap.SetAt((DWORD_PTR) pcPart, cSegment.KeyValue());

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcPart, pcParentAttr, &pcAttr, &cAttrData));

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved && true == IsShow(pcPart))
	{
		A3DAsmPartDefinitionData sData;
		A3D_INITIALIZE_DATA(A3DAsmPartDefinitionData, sData);
		CHECK_A3D_RETURN(A3DAsmPartDefinitionGet(pcPart, &sData));

		// 의미없는 함수
		//CHECK_A3D_RETURN(stDrawBoundingBox(&sData.m_sBoundingBox));

		for(A3DUns32 nIndex = 0; nIndex < sData.m_uiRepItemsSize; nIndex++) {
			CHECK_A3D_RETURN(DrawRepresentationItem(sData.m_ppRepItems[nIndex], cSegment, pcAttr));
		}

		// #Require_coding
/*
		if(stbDrawMarkups)
		{
			A3DPointerArray sMarkups;
			PointerArrayInitialize(&sMarkups);
			// #Require_coding
			// stPartDefinitionGetMarkups(&sData, &sMarkups);
			A3DUns32 ui;

			for(ui = 0; ui < sData.m_uiViewsSize; ui++) {
				// #Require_coding
				//CHECK_A3D_RETURN(stDrawView(sData.m_ppViews[ui], pcAttr));
			}

			A3DUns32 uiSize = sMarkups.m_uiSize;
			for(ui = 0; ui < uiSize; ui++)
				CHECK_A3D_RETURN(DrawMarkup(sMarkups.m_ppPointers[ui], pcAttr));

			PointerArrayTerminate(&sMarkups);
		}
*/

		CHECK_A3D_RETURN(A3DAsmPartDefinitionGet(nullptr, &sData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));

	return A3D_SUCCESS;
}

// 1-1. Draw Representation Item
A3DStatus _3DfImport::DrawRepresentationItem(const A3DRiRepresentationItem * pcRepItem, _3DF::SegmentKey & cParentSegment,
	const A3DMiscCascadedAttributes * pcParentAttr)
{
	// Segment를 생성하고 생성된 Segment를 Parent Segment에 Include한다.
	SegmentKey cSegment = m_cRisIncludeSegment.Subsegment(L"ri%d", m_nIncrementalId++);
	cParentSegment.IncludeSegment(cSegment);

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcRepItem, pcParentAttr, &pcAttr, &cAttrData));

	MaterialMappingKit cMaterialMapping;
	A3DInt32 iUVCoordinatesIndex = -1;
	A3DUns8 ucTextureDimension = 2;
	DrawStyle(&cAttrData.m_sStyle, &iUVCoordinatesIndex, &ucTextureDimension, cMaterialMapping);

	if(true == cMaterialMapping.IsAllocate()) {
		SegmentKey cStyleSegment;
		if(true == FindMaterialMapping(cMaterialMapping, cStyleSegment)) {
			SetStyle(cSegment, cStyleSegment);
		}
		else {
			SetStyleMaterialMapping(cMaterialMapping, cSegment);
		}
	}

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved && true == IsShow(pcRepItem))
	{
		A3DEEntityType eType = kA3DTypeUnknown;
		CHECK_A3D_RETURN(A3DEntityGetType(pcRepItem, &eType));

		if(eType == kA3DTypeRiSet)
		{
			CHECK_A3D_RETURN(DrawSet((A3DRiSet *) pcRepItem, cSegment, pcAttr));
		}
		else
		{
			//uiTotalRepItems++;

			A3DRootBaseData cRootBaseData;
			A3D_INITIALIZE_DATA(A3DRootBaseData, cRootBaseData);
			CHECK_A3D_RETURN(A3DRootBaseGet(pcRepItem, &cRootBaseData));
			m_pchRepresentationItemName = cRootBaseData.m_pcName;

			if(0 < cRootBaseData.m_uiSize)
			{
				for(A3DUns32 nIndex = 0; nIndex < cRootBaseData.m_uiSize; nIndex++)
				{
					A3DMiscAttributeData cMiscAttrData;
					A3D_INITIALIZE_DATA(A3DMiscAttributeData, cMiscAttrData);
					CHECK_A3D_RETURN(A3DMiscAttributeGet(cRootBaseData.m_ppAttributes[nIndex], &cMiscAttrData));
					CHECK_A3D_RETURN(A3DMiscAttributeGet(nullptr, &cMiscAttrData));
				}
			}

			A3DRiRepresentationItemData cData;
			A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, cData);
			CHECK_A3D_RETURN(A3DRiRepresentationItemGet(pcRepItem, &cData));

			const A3DRiCoordinateSystem * pcCoordSys = nullptr;

			if(cAttrData.m_pCoordinateSystem != nullptr) {
				pcCoordSys = cAttrData.m_pCoordinateSystem;
			}
			else if(cData.m_pCoordinateSystem != nullptr) {
				pcCoordSys = cData.m_pCoordinateSystem;
			}

			if(pcCoordSys != nullptr) {
				PushMatrix();

				A3DRiCoordinateSystemData sCSysData;
				A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sCSysData);
				CHECK_A3D_RETURN(A3DRiCoordinateSystemGet(cData.m_pCoordinateSystem, &sCSysData));

				CHECK_A3D_RETURN(DrawTransformation(sCSysData.m_pTransformation));

				CHECK_A3D_RETURN(A3DRiCoordinateSystemGet(nullptr, &sCSysData));
			}

			if(cData.m_pTessBase != nullptr) {
				CHECK_A3D_RETURN(DrawTessBase(cData.m_pTessBase, pcRepItem, cSegment, cMaterialMapping, pcAttr));
			}
			else {
				A3DRWParamsTessellationData sTesselationData;
				A3D_INITIALIZE_DATA(A3DRWParamsTessellationData, sTesselationData);
				sTesselationData.m_eTessellationLevelOfDetail = kA3DTessLODMedium;
				CHECK_A3D_RETURN(A3DRiRepresentationItemComputeTessellation((A3DRiRepresentationItem *) pcRepItem, &sTesselationData));

				A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, cData);
				CHECK_A3D_RETURN(A3DRiRepresentationItemGet(pcRepItem, &cData));
				if(cData.m_pTessBase != nullptr) {
					CHECK_A3D_RETURN(DrawTessBase(cData.m_pTessBase, pcRepItem, cSegment, cMaterialMapping, pcAttr));
				}
			}

			if(pcCoordSys != nullptr) {
				PopMatrix();
			}

			CHECK_A3D_RETURN(A3DRiRepresentationItemGet(nullptr, &cData));

			m_pchRepresentationItemName = nullptr;
			CHECK_A3D_RETURN(A3DRootBaseGet(nullptr, &cRootBaseData));
		}
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));

	return A3D_SUCCESS;
}

// 2. Draw Set
A3DStatus _3DfImport::DrawSet(const A3DRiSet * pSet, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pSet, pcParentAttr, &pcAttr, &cAttrData));

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved && IsShow(pSet) == A3D_SUCCESS)
	{
		A3DRiSetData sData;
		A3D_INITIALIZE_DATA(A3DRiSetData, sData);
		CHECK_A3D_RETURN(A3DRiSetGet(pSet, &sData));

		A3DUns32 ui;
		for(ui = 0; ui < sData.m_uiRepItemsSize; ui++)
		{
			CHECK_A3D_RETURN(DrawRepresentationItem(sData.m_ppRepItems[ui], cParentSegment, pcAttr));
		}

		CHECK_A3D_RETURN(A3DRiSetGet(nullptr, &sData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));

	return A3D_SUCCESS;
}

// 3. Draw Markup 관련 View
A3DStatus _3DfImport::DrawMarkupView(const A3DMkpView * pcView, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcView, pcParentAttr, &pcAttr, &cAttrData));

	if(/*cAttrData.m_bShow && */!cAttrData.m_bRemoved) // TODO m_bShow
	{
		A3DMkpViewData cViewData;
		A3D_INITIALIZE_DATA(A3DMkpViewData, cViewData);
		CHECK_A3D_RETURN(A3DMkpViewGet(pcView, &cViewData));

		for(A3DUns32 ui = 0; ui < cViewData.m_uiAnnotationsSize; ui++)
		{
			CHECK_A3D_RETURN(DrawAnnotation(cViewData.m_ppAnnotations[ui], cParentSegment, pcAttr));
		}

		CHECK_A3D_RETURN(A3DMkpViewGet(nullptr, &cViewData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));

	return iRet;
}
// 5. Draw Annotation
A3DStatus _3DfImport::DrawAnnotation(const A3DMkpAnnotationEntity * pcAnnotation, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DEEntityType eType = kA3DTypeUnknown;

	CHECK_A3D_RETURN(A3DEntityGetType(pcAnnotation, &eType));

	switch(eType)
	{
		case kA3DTypeMkpAnnotationSet:
			CHECK_A3D_RETURN(DrawAnnotationSet(pcAnnotation, cParentSegment, pcParentAttr));
			break;

		case kA3DTypeMkpAnnotationReference:
			CHECK_A3D_RETURN(DrawAnnotationReference(pcAnnotation, pcParentAttr));
			break;

		case kA3DTypeMkpAnnotationItem:
			CHECK_A3D_RETURN(DrawAnnotationItem(pcAnnotation, cParentSegment, pcParentAttr));
			break;
		default:
			break;
	}

	return A3D_SUCCESS;
}

// 5-1. Draw Annotation Set
A3DStatus _3DfImport::DrawAnnotationSet(const A3DMkpAnnotationSet * pcAnnotationSet, _3DF::SegmentKey & cParentSegment, 
	const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcAnnotationSet, pcParentAttr, &pcAttr, &cAttrData));

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved)
	{
		A3DMkpAnnotationSetData sAnnotationSetData;
		A3D_INITIALIZE_DATA(A3DMkpAnnotationSetData, sAnnotationSetData);
		CHECK_A3D_RETURN(A3DMkpAnnotationSetGet(pcAnnotationSet, &sAnnotationSetData));

		A3DUns32 ui;
		for(ui = 0; ui < sAnnotationSetData.m_uiAnnotationsSize; ui++)
		{
			CHECK_A3D_RETURN(DrawAnnotation(sAnnotationSetData.m_ppAnnotations[ui], cParentSegment, pcAttr));
		}

		CHECK_A3D_RETURN(A3DMkpAnnotationSetGet(nullptr, &sAnnotationSetData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));

	return iRet;
}

// 5-2. (미완성) Draw Annotation Reference
A3DStatus _3DfImport::DrawAnnotationReference(const A3DMkpAnnotationItem * /*pcAnnotationItem*/, const A3DMiscCascadedAttributes * /*pcParentAttr*/)
{
	assert(false);
	return A3D_SUCCESS;
}

// 5-3. Draw Annotation Item
A3DStatus _3DfImport::DrawAnnotationItem(const A3DMkpAnnotationItem * pcAnnotationItem, _3DF::SegmentKey & cParentSegment, 
	const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcAnnotationItem, pcParentAttr, &pcAttr, &cAttrData));

	MaterialMappingKit cMaterialMapping;
	DrawStyle(&cAttrData.m_sStyle, cMaterialMapping);

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved)
	{
		A3DMkpAnnotationItemData sAnnotationItemData;
		A3D_INITIALIZE_DATA(A3DMkpAnnotationItemData, sAnnotationItemData);
		CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(pcAnnotationItem, &sAnnotationItemData));

		CHECK_A3D_RETURN(DrawMarkup(sAnnotationItemData.m_pMarkup, cParentSegment, cMaterialMapping, pcAttr));

		CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(nullptr, &sAnnotationItemData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));

	return iRet;
}

// 6. Draw Markup
A3DStatus _3DfImport::DrawMarkup(const A3DMkpMarkup * pcMarkup, _3DF::SegmentKey & cParentSegment, 
	MaterialMappingKit const & cParentMaterialMapping, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcMarkup, pcParentAttr, &pcAttr, &cAttrData));

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved)
	{
		A3DRootBaseData sRootBaseData;
		A3D_INITIALIZE_DATA(A3DRootBaseData, sRootBaseData);
		CHECK_A3D_RETURN(A3DRootBaseGet(pcMarkup, &sRootBaseData));

		A3DMkpMarkupData sMarkupData;
		A3D_INITIALIZE_DATA(A3DMkpMarkupData, sMarkupData);
		CHECK_A3D_RETURN(A3DMkpMarkupGet(pcMarkup, &sMarkupData));

		A3DEMarkupType eType;
		A3DEMarkupSubType eSubType;
		eType = sMarkupData.m_eType;
		eSubType = sMarkupData.m_eSubType;

		if(sMarkupData.m_pTessellation != nullptr)
		{
			CHECK_A3D_RETURN(DrawTessBase((A3DTessBase *) sMarkupData.m_pTessellation, nullptr, cParentSegment, cParentMaterialMapping, pcAttr));
		}

		A3DUns32 ui;
		for(ui = 0; ui < sMarkupData.m_uiLeadersSize; ui++)
		{
			// #Require_coding
			//CHECK_A3D_RETURN(DrawLeader(sMarkupData.m_ppLeaders[ui], pcAttr));
		}

		CHECK_A3D_RETURN(A3DMkpMarkupGet(nullptr, &sMarkupData));

		CHECK_A3D_RETURN(A3DRootBaseGet(nullptr, &sRootBaseData));

		//uiTotalMarkups++;
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));

	return A3D_SUCCESS;
}

// 7. Draw Tessellation Base
A3DStatus _3DfImport::DrawTessBase(A3DTessBase * pcTessBase, const A3DRiRepresentationItem * pcRepItem,
	_3DF::SegmentKey & cParentSegment, MaterialMappingKit const & cParentMaterialMapping, const A3DMiscCascadedAttributes * pcParentAttr)
{
	if(pcTessBase == nullptr) {
		return A3D_ERROR;
	}

	A3DEEntityType eEntityType;
	CHECK_A3D_RETURN(A3DEntityGetType(pcTessBase, &eEntityType));

	A3DStatus eStatus = A3D_SUCCESS;

	switch(eEntityType)
	{
		case kA3DTypeTess3D:
		{
			eStatus = DrawTess3D((A3DTess3D *) pcTessBase, pcRepItem, cParentSegment, cParentMaterialMapping, pcParentAttr);
			break;
		}
		case kA3DTypeTess3DWire:
		{
			//eStatus = DrawTess3DWire((A3DTess3DWire *) pcTessBase, pcRepItem, pcParentAttr);
			break;
		}
		case kA3DTypeTessMarkup:
		{
			//eStatus = DrawTessMarkup((A3DTessMarkup *) pcTessBase, pcRepItem, pcParentAttr);
			break;
		}
		default:
			break;
	}

	return eStatus;
}

// 7-1. Draw Tess3D
A3DStatus _3DfImport::DrawTess3D(const A3DTess3D * pcTess3D, const A3DRiRepresentationItem * pcRepItem,
	_3DF::SegmentKey & cParentSegment, MaterialMappingKit const & cParentMaterialMapping, const A3DMiscCascadedAttributes * pcParentAttr)
{
	double dUnitScale = 1.0;	// Tessellation은 1:1 비율로 들어온다 

	SegmentKey cCurrnetSegment = cParentSegment;

	A3DTessBaseData sTessBaseData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, sTessBaseData);
	CHECK_A3D_RETURN(A3DTessBaseGet(pcTess3D, &sTessBaseData));

	A3DTess3DData cTess3dData;
	A3D_INITIALIZE_DATA(A3DTess3DData, cTess3dData);
	CHECK_A3D_RETURN(A3DTess3DGet(pcTess3D, &cTess3dData));

	A3DUns32 nFacesCount = cTess3dData.m_uiFaceTessSize;

	if(0 == nFacesCount) {
		A3DTess3DGet(nullptr, &cTess3dData);
		return A3D_ERROR;
	}

	// Attribute of Cascade
	/*A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[0];
	A3DMiscCascadedAttributes * pcAttribute;
	A3DMiscCascadedAttributesData sAttrData;
	CreateAndPushCascadedAttributesTessFace(pcRepItem, pcTess3D, &cTessFaceData, 0, pcParentAttr, &pcAttribute, &sAttrData);
*/

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
/*

	_3DF::MaterialMappingKit cMaterialMapping;

	A3DInt32 iUVCoordinatesIndex = -1;
	A3DUns8 ucTextureDimension = 2;
	A3DStatus nStatus = DrawStyle(&sAttrData.m_sStyle, &iUVCoordinatesIndex, &ucTextureDimension, cMaterialMapping);

	// Parent Material과 다른 경우 한개의 Segment를 생성하고, Material Mapping을 적용한다.
	if(cMaterialMapping != cParentMaterialMapping) {
		if(true == cMaterialMapping.IsAllocate()) {

			cCurrnetSegment = cParentSegment.Subsegment();

			SegmentKey cStyleSegment;
			if(true == FindMaterialMapping(cMaterialMapping, cStyleSegment)) {
				SetStyle(cCurrnetSegment, cStyleSegment);
			}
			else {
				SetStyleMaterialMapping(cMaterialMapping, cCurrnetSegment);
			}
		}
	}
*/

	// ----- Point 활당 -----
	A3DUns32 nPointCount = sTessBaseData.m_uiCoordSize / 3;

/*
	DWORD nTick1 = GetTickCount();
	for(int nIndex = 0; nIndex < 10000; nIndex++) {
		_3DF::PointArray acPoints1(10000);
		//std::vector<_3DF::Point3D> acPoints2(10000);
	}
	DWORD nTick2 = GetTickCount();
	for(int nIndex = 0; nIndex < 10000; nIndex++) {
		std::vector<_3DF::Point> acPoints2(10000);
	}
	DWORD nTick3 = GetTickCount();
	for(int nIndex = 0; nIndex < 10000; nIndex++) {
		_3DF::Point * pcPoint1 = new _3DF::Point[10000];
// 		for(int nIndex1 = 0; nIndex1 < 10000; nIndex1++) {
// 			pcPoint1[nIndex1].Set(nIndex1, nIndex1, nIndex1);
// 		}
		delete[] pcPoint1;
	}
	DWORD nTick4 = GetTickCount();
	for(int nIndex = 0; nIndex < 10000; nIndex++) {
		_3DF::Point3D * pcPoint1 = new _3DF::Point3D[10000];
// 		for(int nIndex1 = 0; nIndex1 < 10000; nIndex1++) {
// 			pcPoint1[nIndex1].Set(nIndex1, nIndex1, nIndex1);
// 		}
		delete[] pcPoint1;
	}
	DWORD nTick5 = GetTickCount();

	DWORD nSub1 = nTick2 - nTick1;
	DWORD nSub2 = nTick3 - nTick2;
	DWORD nSub3 = nTick4 - nTick3;
	DWORD nSub4 = nTick5 - nTick4;
*/

	_3DF::PointArray acPoints(nPointCount);
	for(A3DUns32 nIndex = 0; nIndex < nPointCount; nIndex++) {
		acPoints[nIndex].Set(
			sTessBaseData.m_pdCoords[nIndex * 3],
			sTessBaseData.m_pdCoords[nIndex * 3 + 1],
			sTessBaseData.m_pdCoords[nIndex * 3 + 2]);
	}

	// ----- Normal Vector 활당 -----
	A3DUns32 nNormalCount = cTess3dData.m_uiNormalSize / 3;
	_3DF::VectorArray acNormals(nNormalCount);
	for(A3DUns32 nIndex = 0; nIndex < nNormalCount; nIndex++) {
		acNormals[nIndex].Set(
			cTess3dData.m_pdNormals[nIndex * 3],
			cTess3dData.m_pdNormals[nIndex * 3 + 1],
			cTess3dData.m_pdNormals[nIndex * 3 + 2]);
	}

/*

	COLORREF cColor;
	if(true == GetAttributeColor(sAttrData, cColor)) {
		pcMesh->SetColor(cColor);
	}

	float fTransparency = 1.0f;
	if(true == GetAttributeTransparency(sAttrData, fTransparency)) {
		float a, d, sp, sh, t, e;
		pcMesh->GetVisual(a, d, sp, sh, t, e);
		pcMesh->SetVisual(a, d, sp, sh, fTransparency, e);
	}
*/

	for(A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; ++nFaceIndex)
	{
		A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[nFaceIndex];

		A3DMiscCascadedAttributes * pcAttribute;
		A3DMiscCascadedAttributesData sAttrData;
		CreateAndPushCascadedAttributesTessFace(pcRepItem, pcTess3D, &cTessFaceData, 0, pcParentAttr, &pcAttribute, &sAttrData);

		MaterialMappingKit cMaterialMapping;
		DrawStyle(&sAttrData.m_sStyle, cMaterialMapping);

		// Parent Material과 다른 경우 한개의 Segment를 생성하고, Material Mapping을 적용한다.
		if(cMaterialMapping != cParentMaterialMapping) {
			if(true == cMaterialMapping.IsAllocate()) {

				cCurrnetSegment = cParentSegment.Subsegment(L"face_%d", nFaceIndex);

				SegmentKey cStyleSegment;
				if(true == FindMaterialMapping(cMaterialMapping, cStyleSegment)) {
					SetStyle(cCurrnetSegment, cStyleSegment);
				}
				else {
					SetStyleMaterialMapping(cMaterialMapping, cCurrnetSegment);
				}
			}
		}

		_3DF::IntArray anFacelistArray;
		_3DF::IntArray anTristripsArray;

		A3DUns32 nTriSizeIndex = 0;	// 한 Triangle Type당 하나씩
		A3DUns32 nTriStartIndex = cTessFaceData.m_uiStartTriangulated;
		A3DUns32 nTriColorIndex = 0;

		// 3DX에서 돌려주는 Face Index는 Tess3d 전체의 Index를 돌려주는데, Grid를 구성하기 위해서는 부분적인 Point들의 Index가 필요함.
		// 별도의 순서를 갖는 Face Index를 구성하기 위해서 Map을 사용한다.
		TessIndexMap maPointIndexMap;
		TessIndexMap maNormalIndexMap;

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
		{
			nTriangleFaceCount += DrawTessFaceDataTriangle(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, anFacelistArray);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
		{
			nTriangleFaceCount += DrawTessFaceDataTriangleFan(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, anFacelistArray);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
		{
			nTriangleFaceCount += DrawTessFaceDataTriangleStripe(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				nTriColorIndex, maPointIndexMap, maNormalIndexMap, anFacelistArray);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
		{
			nTriangleFaceCount += DrawTessFaceDataTriangleOneNormal(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, anFacelistArray);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
		{
			nTriangleFaceCount += DrawTessFaceDataTriangleFanOneNormal(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, anFacelistArray);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
		{
			nTriangleFaceCount += DrawTessFaceDataTriangleStripeOneNormal(cTessFaceData, pnTriangleIndices, nTriSizeIndex, nTriStartIndex,
				maPointIndexMap, maNormalIndexMap, anFacelistArray);
		}

		if(false == anFacelistArray.empty()) {

			size_t nPoitnCount = maPointIndexMap.GetCount();
			size_t nNormalCount = maNormalIndexMap.GetCount();

			if(nPoitnCount != nNormalCount) {
				ASSERT(false);
			}

			PointArray cShellPoints(nPoitnCount);
			VectorArray cShellNormals(nPoitnCount);

			A3DUns32 nOriginIndex, nGridIndex;
			POSITION pcPosition = maPointIndexMap.GetStartPosition();
			while(nullptr != pcPosition)
			{
				maPointIndexMap.GetNextAssoc(pcPosition, nOriginIndex, nGridIndex);
				if(nGridIndex >= nPoitnCount) {
					ASSERT(false);
				}

				cShellPoints[nGridIndex] = acPoints[nOriginIndex];
			}

			pcPosition = maNormalIndexMap.GetStartPosition();
			while(nullptr != pcPosition)
			{
				maNormalIndexMap.GetNextAssoc(pcPosition, nGridIndex, nOriginIndex);
				if(nGridIndex >= nNormalCount) {
					ASSERT(false);
				}

				cShellNormals[nGridIndex] = acNormals[nOriginIndex];
			}

			_3DF::ShellKit cShellKit;
			cShellKit.SetPoints(cShellPoints);
			cShellKit.SetNormals(cShellNormals);
			cShellKit.SetFacelist(anFacelistArray);
			cShellKit.SetTristrips(anTristripsArray);

			ShellKey cShell = cCurrnetSegment.InsertShell(cShellKit);
		}

		//---- Draw Edge Line -----

		A3DUns32 nStartWireIndex = cTessFaceData.m_uiStartWire;
		int index = 0;
		for(A3DUns32 j = 0; j < cTessFaceData.m_uiSizesWiresSize; ++j)
		{
			A3DUns32 size = cTessFaceData.m_puiSizesWires[j] & ~(kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);
			A3DUns32 flags = cTessFaceData.m_puiSizesWires[j] & (kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);

			if(flags & kA3DTessFaceDataWireIsNotDrawn) {
				index += size;
				continue;
			}

			_3DF::PointArray acWirePoints(size);

			for(A3DUns32 k = 0; k < size; ++k) {
				acWirePoints[k] = acPoints[cTess3dData.m_puiWireIndexes[nStartWireIndex + index++] / 3];
			}

			cCurrnetSegment.InsertLine(acWirePoints.size(), acWirePoints.data());
		}

/*


		A3DUns32 size = cTess3dData.m_puiSizesWires[j] & ~(kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);
		A3DUns32 flags = cTess3dData.m_puiSizesWires[j] & (kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);

*/


		cCurrnetSegment = cParentSegment;
	}


// 
// 	if(cTess3DData.m_bMustRecalculateNormals)
// 		cTess3DData.m_pdNormals = nullptr;

// 	CHECK_A3D_RETURN(A3DTessBaseGet(nullptr, &sTessBaseData));
// 	CHECK_A3D_RETURN(A3DTess3DGet(nullptr, &cTess3dData));

	return A3D_SUCCESS;
}

// 7-1-1. Triangle Face Data 변환
UINT _3DfImport::DrawTessFaceDataTriangle(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriangleIndices, A3DUns32 & nTriangleSizeIndex, A3DUns32 & nTriangleStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray)
{
	A3DUns32 nTriangleCount = cTessFaceData.m_puiSizesTriangulated[nTriangleSizeIndex++];

	A3DUns32 nFacePointIndex[3];
	A3DUns32 nFaceNormalIndex[3];

	TessIndexMap maLocalNormalIndexMap;

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

		ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray);
	}

	return nTriangleCount;
}

// 7-1-2. Triangle Fan Data 변환
UINT _3DfImport::DrawTessFaceDataTriangleFan(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray)
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

			ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray);
			nTriFaceCount++;
		}

		nTriStartIndex += nPointCount * 2;
	}

	return nTriFaceCount;
}

// 7-1-3. Triangle Stripe Data 변환
UINT _3DfImport::DrawTessFaceDataTriangleStripe(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	A3DUns32 & nTriColorIndex, TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray)
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
			ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray);

			nTriFaceCount++;
		}

		nTriStartIndex += nPointCount * 2;
		nTriColorIndex += nPointCount * 3;
	}

	return nTriFaceCount;
}

// 4-2-4. Triangle Stripe One Normal 변환
UINT _3DfImport::DrawTessFaceDataTriangleOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray)
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

		ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray);
	}

	return nTriCount;
}

// 4-2-5. Triangle Fan One Normal Data 변환
UINT _3DfImport::DrawTessFaceDataTriangleFanOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray)
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

				ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray);
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

				ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray);
				//cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
				nTriCount++;
			}
			nTriStartIndex += (nPointsCount + 1);
		}
	}

	return nTriCount;
}

// 4-2-6. Triangle Stripe One Normal Data 변환
UINT _3DfImport::DrawTessFaceDataTriangleStripeOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray)
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

				ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray);
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

				ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray);
				//cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
				nTriCount++;
			}

			nTriStartIndex += (numpoints + 1);
		}
	}

	return nTriCount;
}

// 7-2-1. Facet 갯수를 가져옴
A3DUns32 _3DfImport::Tess3DDataGetNumberOfFacets(const A3DTess3DData * pcTess3DData)
{
	A3DUns32 ui;
	A3DUns32 uiNbFacets = 0;
	for(ui = 0; ui < pcTess3DData->m_uiFaceTessSize; ui++)
	{
		uiNbFacets += TessFaceDataGetNumberOfFacets(&pcTess3DData->m_psFaceTessData[ui]);
	}

	return uiNbFacets;
}

// 7-2-1-1. Facet 갯수를 가져옴
A3DUns32 _3DfImport::TessFaceDataGetNumberOfFacets(const A3DTessFaceData * pcTessFaceData)
{
	A3DUns32 iNbFacet = 0;
	A3DUns32 iNumberIndex = 0;
	A3DUns32 iNbEntities = 0;
	A3DUns32 iStartId;
	A3DUns32 iId;

#define SIZE_TRIANGLE_ENTITY(index) ((index < pcTessFaceData->m_uiSizesTriangulatedSize ? \
	pcTessFaceData->m_puiSizesTriangulated[index] & kA3DTessFaceDataNormalMask : 0))

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
	{
		iNbFacet += SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iNumberIndex++;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for(iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for(iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
	{
		iNbFacet += SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iNumberIndex++;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for(iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for(iId = 0; iId < iNbEntities; iId++)
		{
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;
		}
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured)
	{
		iNbFacet += SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iNumberIndex++;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanTextured)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for(iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeTextured)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for(iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormalTextured)
	{
		iNbFacet += SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iNumberIndex++;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for(iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if(pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for(iId = 0; iId < iNbEntities; iId++)
		{
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;
		}
	}

#undef SIZE_TRIANGLE_ENTITY

	return iNbFacet;
}

// ----- Utility -----

// 7-10-1. Normal Index를 조정하는 함수
// Vertex는 중복이 되고 Normal Index는 각각 별도로 들어온다 (vertex index에서 찾도록 검사)
void _3DfImport::MatchVertexNormal(TessIndexMap & maNormalIndexMap, A3DUns32 nVertexIndex, A3DUns32 & nNormalIndex)
{
	A3DUns32 nFindNormalIndex;
	if(true == maNormalIndexMap.Lookup(nVertexIndex, nFindNormalIndex)) {
		nNormalIndex = nFindNormalIndex;
	}
	else {
		maNormalIndexMap.SetAt(nVertexIndex, nNormalIndex);
	}
}

// 7-10-2. Point Index Map과 Normal Index Map, Point, Normal Index를 이용해서 MbTrianle Vector를 구성한다.
// 주어진 Triangle Vector에 생성된 Vector를 추가시킴.
bool _3DfImport::ConvertFaceList(TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap,
	A3DUns32 * pnFacePointIndex, A3DUns32 * pnFaceNormalIndex, _3DF::IntArray & anFacelistArray)
{
	// Triangle을 구성할 Point Index
	A3DUns32 nPointIndex[3];

	for(int nIndex = 0; nIndex < 3; nIndex++) {
		// 사전에 저장된 Face Point Index를 찾는다. 찾은 경우 새롭게 생성되어지고 있는 Point Index를 이용해서
		// Face Index를 구성한다. 여기서 생성되는 Face Index는 개별 Face의 Index이기 때문에 0부터 시작하도록 구성한다.
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

	anFacelistArray.push_back(3);
	anFacelistArray.push_back(nPointIndex[0]);
	anFacelistArray.push_back(nPointIndex[1]);
	anFacelistArray.push_back(nPointIndex[2]);

	return true;
}

// 8. Face Draw Style 정의
A3DStatus _3DfImport::DrawStyle(const A3DGraphStyleData * pcStyleData, A3DInt32 * pnUVCoordinatesIndex,
	A3DUns8 * pucTextureDimension, _3DF::MaterialMappingKit & cMaterialKit)
{
	if(pcStyleData == nullptr)
		return A3D_ERROR;

	A3DStatus nRetStatus = A3D_SUCCESS;

	if(A3D_TRUE == pcStyleData->m_bMaterial)
	{
		A3DBool bMaterialIsTexture = false;

		A3DDouble adColor[4] = { 0.0, 0.0, 0.0, 1.0 };

		// check if this material is a texture
		CHECK_A3D_RETURN(A3DGlobalIsMaterialTexture(pcStyleData->m_uiRgbColorIndex, &bMaterialIsTexture));

		if(A3D_TRUE == bMaterialIsTexture)
		{
			A3DGraphTextureApplicationData sTextureAppplicationData;
			A3D_INITIALIZE_DATA(A3DGraphTextureApplicationData, sTextureAppplicationData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphTextureApplicationData(pcStyleData->m_uiRgbColorIndex,
				&sTextureAppplicationData));

			*pnUVCoordinatesIndex = sTextureAppplicationData.m_iUVCoordinatesIndex;

			A3DGraphTextureDefinitionData sTextureDefinitionData;
			A3D_INITIALIZE_DATA(A3DGraphTextureDefinitionData, sTextureDefinitionData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphTextureDefinitionData(sTextureAppplicationData.m_uiTextureDefinitionIndex,
				&sTextureDefinitionData));

			*pucTextureDimension = sTextureDefinitionData.m_ucTextureDimension;

			_3DF::RGBAColor cDiffuseColor(sTextureDefinitionData.m_dRed, sTextureDefinitionData.m_dGreen,
				sTextureDefinitionData.m_dBlue, sTextureDefinitionData.m_dAlpha);

			//cMaterialKit.SetDiffuseColor(cDiffuseColor);

			//A3D_DRAW_CALL3(Material, kA3DDrawMaterialDiffuse, adColor, 4);

			nRetStatus = A3DGlobalGetGraphTextureDefinitionData(A3D_DEFAULT_TEXTURE_DEFINITION_INDEX, &sTextureDefinitionData);

			nRetStatus = A3DGlobalGetGraphTextureApplicationData(A3D_DEFAULT_MATERIAL_INDEX, &sTextureAppplicationData);
		}
		else
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);

			A3DGraphMaterialData sMaterialData;
			A3D_INITIALIZE_DATA(A3DGraphMaterialData, sMaterialData);

			CHECK_A3D_RETURN(A3DGlobalGetGraphMaterialData(pcStyleData->m_uiRgbColorIndex, &sMaterialData));
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiDiffuse, &sRgbColorData));
			_3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dDiffuseAlpha);
			cMaterialKit.SetFaceColor(cDiffuseColor, _3DF::Material::Color::Channel::DiffuseColor);

/*
			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiAmbient, &sRgbColorData));
			_3DF::RGBAColor cAmbientColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dAmbientAlpha);
			cMaterialKit.SetFaceColor(cAmbientColor, _3DF::Material::Color::Channel::);
*/

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiEmissive, &sRgbColorData));
			_3DF::RGBAColor cEmissiveColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dEmissiveAlpha);
			cMaterialKit.SetFaceColor(cEmissiveColor, _3DF::Material::Color::Channel::Emission);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiSpecular, &sRgbColorData));
			_3DF::RGBAColor cSpecularColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dSpecularAlpha);
			cMaterialKit.SetFaceColor(cSpecularColor, _3DF::Material::Color::Channel::Specular);

			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);

			nRetStatus = A3DGlobalGetGraphMaterialData(A3D_DEFAULT_MATERIAL_INDEX, &sMaterialData);
			cMaterialKit.SetFaceGloss(sMaterialData.m_dShininess);
		}
	}
	else
	{
		// m_uiRgbColorIndex : A global index to either an `A3DGraphMaterialData` or an `A3DGraphRgbColorData` according to the value of `m_bMaterial`.
		if(A3D_DEFAULT_COLOR_INDEX != pcStyleData->m_uiRgbColorIndex) {
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(pcStyleData->m_uiRgbColorIndex, &sRgbColorData));

			_3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);
			cMaterialKit.SetFaceColor(cDiffuseColor);

			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
		}
	}

	return nRetStatus;
}

// 8-1. 일반 DrawStyle 정의 
A3DStatus _3DfImport::DrawStyle(const A3DGraphStyleData * pcStyleData, _3DF::MaterialMappingKit & cMaterialKit)
{
	if(pcStyleData == nullptr) {
		return A3D_ERROR;
	}

	A3DStatus nRetStatus = A3D_SUCCESS;

	if(A3D_TRUE == pcStyleData->m_bMaterial)
	{
		A3DBool bMaterialIsTexture = false;

		// check if this material is a texture
		CHECK_A3D_RETURN(A3DGlobalIsMaterialTexture(pcStyleData->m_uiRgbColorIndex, &bMaterialIsTexture));

		if(A3D_TRUE == bMaterialIsTexture)
		{
			assert(false);
/*
			A3DGraphTextureApplicationData sTextureAppplicationData;
			A3D_INITIALIZE_DATA(A3DGraphTextureApplicationData, sTextureAppplicationData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphTextureApplicationData(pcStyleData->m_uiRgbColorIndex,
				&sTextureAppplicationData));

			*pnUVCoordinatesIndex = sTextureAppplicationData.m_iUVCoordinatesIndex;

			A3DGraphTextureDefinitionData sTextureDefinitionData;
			A3D_INITIALIZE_DATA(A3DGraphTextureDefinitionData, sTextureDefinitionData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphTextureDefinitionData(sTextureAppplicationData.m_uiTextureDefinitionIndex,
				&sTextureDefinitionData));

			*pucTextureDimension = sTextureDefinitionData.m_ucTextureDimension;

			_3DF::RGBAColor cDiffuseColor(sTextureDefinitionData.m_dRed, sTextureDefinitionData.m_dGreen,
				sTextureDefinitionData.m_dBlue, sTextureDefinitionData.m_dAlpha);

			//cMaterialKit.SetDiffuseColor(cDiffuseColor);

			//A3D_DRAW_CALL3(Material, kA3DDrawMaterialDiffuse, adColor, 4);

			nRetStatus = A3DGlobalGetGraphTextureDefinitionData(A3D_DEFAULT_TEXTURE_DEFINITION_INDEX, &sTextureDefinitionData);

			nRetStatus = A3DGlobalGetGraphTextureApplicationData(A3D_DEFAULT_MATERIAL_INDEX, &sTextureAppplicationData);
*/
		}
		else
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);

			A3DGraphMaterialData sMaterialData;
			A3D_INITIALIZE_DATA(A3DGraphMaterialData, sMaterialData);

			CHECK_A3D_RETURN(A3DGlobalGetGraphMaterialData(pcStyleData->m_uiRgbColorIndex, &sMaterialData));
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiDiffuse, &sRgbColorData));
			_3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dDiffuseAlpha);
			cMaterialKit.SetFaceColor(cDiffuseColor, _3DF::Material::Color::Channel::DiffuseColor);

/*
			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiAmbient, &sRgbColorData));
			_3DF::RGBAColor cAmbientColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dAmbientAlpha);
			cMaterialKit.SetFaceColor(cAmbientColor, _3DF::Material::Color::Channel::);
*/

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiEmissive, &sRgbColorData));
			_3DF::RGBAColor cEmissiveColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dEmissiveAlpha);
			cMaterialKit.SetFaceColor(cEmissiveColor, _3DF::Material::Color::Channel::Emission);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiSpecular, &sRgbColorData));
			_3DF::RGBAColor cSpecularColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dSpecularAlpha);
			cMaterialKit.SetFaceColor(cSpecularColor, _3DF::Material::Color::Channel::Specular);

			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);

			nRetStatus = A3DGlobalGetGraphMaterialData(A3D_DEFAULT_MATERIAL_INDEX, &sMaterialData);
			cMaterialKit.SetFaceGloss(sMaterialData.m_dShininess);
		}
	}
	else 
	{
		// m_uiRgbColorIndex : A global index to either an `A3DGraphMaterialData` or an `A3DGraphRgbColorData` according to the value of `m_bMaterial`.
		if(A3D_DEFAULT_COLOR_INDEX != pcStyleData->m_uiRgbColorIndex)
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(pcStyleData->m_uiRgbColorIndex, &sRgbColorData));

			_3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);
			cMaterialKit.SetFaceColor(cDiffuseColor);

			A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);

			return A3D_SUCCESS;
		}
	}

	return A3D_ERROR;
}

// 20. Draw용 Transforamtion을 계산하는 부분
A3DStatus _3DfImport::DrawTransformation(const A3DMiscTransformation * pcTransformation)
{
	if(nullptr == pcTransformation) {
		return A3D_SUCCESS;
	}

	A3DEEntityType eType = kA3DTypeUnknown;
	A3DEntityGetType(pcTransformation, &eType);

	switch(eType)
	{
		case kA3DTypeMiscCartesianTransformation:
		{
			A3DMiscCartesianTransformationData cData;
			A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, cData);

			CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(pcTransformation, &cData));

			double adMatrix[16];
			double dMirror = (cData.m_ucBehaviour & kA3DTransformationMirror) ? -1. : 1.;

			A3DVector3dData sZVector;
			memset(adMatrix, 0, 16 * sizeof(double));
			VectorCross(&cData.m_sXVector, &cData.m_sYVector, &sZVector);

			adMatrix[12] = cData.m_sOrigin.m_dX;
			adMatrix[13] = cData.m_sOrigin.m_dY;
			adMatrix[14] = cData.m_sOrigin.m_dZ;

			adMatrix[0] = cData.m_sXVector.m_dX * cData.m_sScale.m_dX;
			adMatrix[1] = cData.m_sXVector.m_dY * cData.m_sScale.m_dX;
			adMatrix[2] = cData.m_sXVector.m_dZ * cData.m_sScale.m_dX;

			adMatrix[4] = cData.m_sYVector.m_dX * cData.m_sScale.m_dY;
			adMatrix[5] = cData.m_sYVector.m_dY * cData.m_sScale.m_dY;
			adMatrix[6] = cData.m_sYVector.m_dZ * cData.m_sScale.m_dY;

			adMatrix[8] = dMirror * sZVector.m_dX * cData.m_sScale.m_dZ;
			adMatrix[9] = dMirror * sZVector.m_dY * cData.m_sScale.m_dZ;
			adMatrix[10] = dMirror * sZVector.m_dZ * cData.m_sScale.m_dZ;

			adMatrix[15] = 1.;

			MultMatrix(adMatrix);

			CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(nullptr, &cData));
		}
		break;

		case kA3DTypeMiscGeneralTransformation:
		{
			A3DMiscGeneralTransformationData cData;
			A3D_INITIALIZE_DATA(A3DMiscGeneralTransformationData, cData);

			CHECK_A3D_RETURN(A3DMiscGeneralTransformationGet(pcTransformation, &cData));

			MultMatrix(cData.m_adCoeff);

			CHECK_A3D_RETURN(A3DMiscGeneralTransformationGet(nullptr, &cData));
		}
		break;

		default:
		break;
	}

	return A3D_SUCCESS;
}

// == Attribute 관련 함수 ============================================================================

// 1. Parent에서 받은(계단식으로) Attribute를 이용해서, Attribute를 생성
A3DStatus _3DfImport::CreateAndPushCascadedAttributes(const A3DRootBaseWithGraphics * pcBase, const A3DMiscCascadedAttributes * pcParentAttr,
	A3DMiscCascadedAttributes ** ppcAttr, A3DMiscCascadedAttributesData * pcAttrData)
{
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesCreate(ppcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesPush(*ppcAttr, pcBase, pcParentAttr));

	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, (*pcAttrData));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(*ppcAttr, pcAttrData));

	return A3D_SUCCESS;
}

// 2. Tess Face용 Attribute 생성
A3DStatus _3DfImport::CreateAndPushCascadedAttributesTessFace(const A3DRiRepresentationItem * pcRepItem, const A3DTessBase * pcTessBase,
	const A3DTessFaceData * pcTessFaceData, A3DUns32 nFaceIndex, const A3DMiscCascadedAttributes * pcParentAttribute,
	A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData)
{
	A3DStatus nStatus = A3DMiscCascadedAttributesCreate(pcAttrs);
	nStatus = A3DMiscCascadedAttributesPushTessFace(*pcAttrs, pcRepItem, pcTessBase, pcTessFaceData, nFaceIndex, pcParentAttribute);

	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, (*pcAttributesData));

	nStatus = A3DMiscCascadedAttributesGet(*pcAttrs, pcAttributesData);

	return nStatus;
}

// 3. Face Attribute 생성
A3DStatus _3DfImport::CreateAndPushCascadedAttributesFace(const A3DRiRepresentationItem * pcRepItem, const A3DTessBase * pcTessBase,
	const A3DTessFaceData * pcTessFaceData, A3DUns32 uiFaceIndex, const A3DMiscCascadedAttributes * pcParentAttr,
	A3DMiscCascadedAttributes ** ppcAttr, A3DMiscCascadedAttributesData * pcAttrData)
{
	A3DStatus iRet = A3D_SUCCESS;

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesCreate(ppcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesPushTessFace(*ppcAttr, pcRepItem, pcTessBase, pcTessFaceData, uiFaceIndex,
		pcParentAttr));

	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, (*pcAttrData));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(*ppcAttr, pcAttrData));

	return iRet;
}


// 4. Show 상태를 확인
bool _3DfImport::IsShow(const A3DRootBaseWithGraphics * pGraphics)
{
	if(pGraphics == nullptr) {
		return false;
	}

	A3DStatus iRet = A3D_SUCCESS;

	A3DRootBaseWithGraphicsData sRootData;
	A3D_INITIALIZE_DATA(A3DRootBaseWithGraphicsData, sRootData);
	CHECK_A3D_RETURN(A3DRootBaseWithGraphicsGet(pGraphics, &sRootData));

	A3DGraphicsData sGraphicsData;
	A3D_INITIALIZE_DATA(A3DGraphicsData, sGraphicsData);
	CHECK_A3D_RETURN(A3DGraphicsGet(sRootData.m_pGraphics, &sGraphicsData));

	A3DBool bShow;
	if(sRootData.m_pGraphics)
		bShow = (sGraphicsData.m_usBehaviour & kA3DGraphicsShow) != 0;
	else
		bShow = A3D_TRUE;
	CHECK_A3D_RETURN(A3DGraphicsGet(nullptr, &sGraphicsData));
	CHECK_A3D_RETURN(A3DRootBaseWithGraphicsGet(nullptr, &sRootData));

	if(A3D_TRUE == bShow) {
		return true;
	}

	return false;
}

// 5. 주어진 Material Mapping을 이용해서 
bool _3DfImport::SetStyleMaterialMapping(MaterialMappingKit const & cInKit, SegmentKey & cSegment)
{
	SegmentKey cStyleSegment = m_pcModelSegment->StylesInclude().Subsegment(L"material_%d", m_nShellMaterialId++);
	cStyleSegment.SetMaterialMapping(cInKit);

	StyleControl cStyleControl = cSegment.GetStyleControl();
	StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

	if(INVALID_KEY == cStyle.KeyValue()) {
		return false;
	}

	MaterialMappingStyleKit cMaterialMappingStyle;
	cMaterialMappingStyle.cMaterialMappingKit = cInKit;
	cMaterialMappingStyle.cStyleSegment = cStyleSegment;

	m_vcMaterialMappingStyleVector.push_back(cMaterialMappingStyle);

	return true;
}

// 5-1. 주어진 Material Mapping을 이용해서 
bool _3DfImport::SetStyle(SegmentKey & cSegment, SegmentKey & cStyleSegment)
{
	StyleControl cStyleControl = cSegment.GetStyleControl();
	StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

	if(INVALID_KEY == cStyle.KeyValue()) {
		return false;
	}

	return true;
}

// 6. 미리 저장되어 있는 Material Mapping Kit을 찾아오는 함수
bool _3DfImport::FindMaterialMapping(MaterialMappingKit const & cInKit, SegmentKey & cOutStyleSegment)
{
	for(auto & cMaterialStyle : m_vcMaterialMappingStyleVector) {
		if(cMaterialStyle.cMaterialMappingKit == cInKit) {
			cOutStyleSegment = cMaterialStyle.cStyleSegment;
			return true;
		}
	}

	return false;
}

// == C3D 관련 Utility 함수 =========================================================================

// 1. 3DX Location을 C3D Matrix로 변환해서 돌려줌
A3DStatus _3DfImport::GetMatrix(A3DMiscTransformation * pcLocation, MbMatrix3D & cMatrix)
{
	if(nullptr == pcLocation) {
		return A3D_ERROR;
	}

	A3DEEntityType eType;
	A3DEntityGetType(pcLocation, &eType);

	if(eType == kA3DTypeMiscCartesianTransformation)
	{
		A3DMiscCartesianTransformationData cTransformationData;
		A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, cTransformationData);

		CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(pcLocation, &cTransformationData));

		double m_dTopoContextScaleX = cTransformationData.m_sScale.m_dX;
		double m_dTopoContextScaleY = cTransformationData.m_sScale.m_dY;
		double m_dTopoContextScaleZ = cTransformationData.m_sScale.m_dZ;

		MbCartPoint3D cOrigin = GetMbCartPoint3D(cTransformationData.m_sOrigin);
		cOrigin.Scale(m_dTopoContextScaleX, m_dTopoContextScaleY, m_dTopoContextScaleZ);
		MbVector3D cAxisX = GetMbVector3D(cTransformationData.m_sXVector);
		MbVector3D cAxisY = GetMbVector3D(cTransformationData.m_sYVector);
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

		CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(nullptr, &cTransformationData));
	}
	else if(eType == kA3DTypeMiscGeneralTransformation)
	{
		A3DMiscGeneralTransformationData cTransformationData;
		A3D_INITIALIZE_DATA(A3DMiscGeneralTransformationData, cTransformationData);

		CHECK_A3D_RETURN(A3DMiscGeneralTransformationGet(pcLocation, &cTransformationData));

		cMatrix.SetAxisX().Init(cTransformationData.m_adCoeff[0], cTransformationData.m_adCoeff[1], cTransformationData.m_adCoeff[2]);
		cMatrix.SetAxisY().Init(cTransformationData.m_adCoeff[4], cTransformationData.m_adCoeff[5], cTransformationData.m_adCoeff[6]);
		cMatrix.SetAxisZ().Init(cTransformationData.m_adCoeff[8], cTransformationData.m_adCoeff[9], cTransformationData.m_adCoeff[10]);
		cMatrix.SetOrigin().Init(cTransformationData.m_adCoeff[12], cTransformationData.m_adCoeff[13], cTransformationData.m_adCoeff[14]);

		// Memory Free
		CHECK_A3D_RETURN(A3DMiscGeneralTransformationGet(nullptr, &cTransformationData));
	}
	else
	{
		ASSERT(false);
		return A3D_ERROR;
	}

	return A3D_SUCCESS;
}

A3DStatus _3DfImport::GetMatrix(A3DMiscTransformation * pcLocation, MatrixKit & cOutMatrix)
{
	MbMatrix3D cMatrix;
	CHECK_A3D_RETURN(GetMatrix(pcLocation, cMatrix));

	// X-Axis
	cOutMatrix.m_fData[0] = cMatrix.GetAxisX().x;
	cOutMatrix.m_fData[1] = cMatrix.GetAxisX().y;
	cOutMatrix.m_fData[2] = cMatrix.GetAxisX().z;

	// Y-Axis
	cOutMatrix.m_fData[4] = cMatrix.GetAxisY().x;
	cOutMatrix.m_fData[5] = cMatrix.GetAxisY().y;
	cOutMatrix.m_fData[6] = cMatrix.GetAxisY().z;

	// Z-Axis
	cOutMatrix.m_fData[8] = cMatrix.GetAxisZ().x;
	cOutMatrix.m_fData[9] = cMatrix.GetAxisZ().y;
	cOutMatrix.m_fData[10] = cMatrix.GetAxisZ().z;

	// Origin
	cOutMatrix.m_fData[12] = cMatrix.GetOrigin().x;
	cOutMatrix.m_fData[13] = cMatrix.GetOrigin().y;
	cOutMatrix.m_fData[14] = cMatrix.GetOrigin().z;

	return A3D_SUCCESS;
}

//== 계산 관련 함수 ===================================================================================

void _3DfImport::MatrixMatrixMult(double m[16], const double o[16])
{
	memcpy(m_dMatrix, m, 16 * sizeof(double));
	m[0] = m_dMatrix[0] * o[0] + m_dMatrix[4] * o[1] + m_dMatrix[8] * o[2] + m_dMatrix[12] * o[3];
	m[1] = m_dMatrix[1] * o[0] + m_dMatrix[5] * o[1] + m_dMatrix[9] * o[2] + m_dMatrix[13] * o[3];
	m[2] = m_dMatrix[2] * o[0] + m_dMatrix[6] * o[1] + m_dMatrix[10] * o[2] + m_dMatrix[14] * o[3];
	m[3] = m_dMatrix[3] * o[0] + m_dMatrix[7] * o[1] + m_dMatrix[11] * o[2] + m_dMatrix[15] * o[3];

	m[4] = m_dMatrix[0] * o[4] + m_dMatrix[4] * o[5] + m_dMatrix[8] * o[6] + m_dMatrix[12] * o[7];
	m[5] = m_dMatrix[1] * o[4] + m_dMatrix[5] * o[5] + m_dMatrix[9] * o[6] + m_dMatrix[13] * o[7];
	m[6] = m_dMatrix[2] * o[4] + m_dMatrix[6] * o[5] + m_dMatrix[10] * o[6] + m_dMatrix[14] * o[7];
	m[7] = m_dMatrix[3] * o[4] + m_dMatrix[7] * o[5] + m_dMatrix[11] * o[6] + m_dMatrix[15] * o[7];

	m[8] = m_dMatrix[0] * o[8] + m_dMatrix[4] * o[9] + m_dMatrix[8] * o[10] + m_dMatrix[12] * o[11];
	m[9] = m_dMatrix[1] * o[8] + m_dMatrix[5] * o[9] + m_dMatrix[9] * o[10] + m_dMatrix[13] * o[11];
	m[10] = m_dMatrix[2] * o[8] + m_dMatrix[6] * o[9] + m_dMatrix[10] * o[10] + m_dMatrix[14] * o[11];
	m[11] = m_dMatrix[3] * o[8] + m_dMatrix[7] * o[9] + m_dMatrix[11] * o[10] + m_dMatrix[15] * o[11];

	m[12] = m_dMatrix[0] * o[12] + m_dMatrix[4] * o[13] + m_dMatrix[8] * o[14] + m_dMatrix[12] * o[15];
	m[13] = m_dMatrix[1] * o[12] + m_dMatrix[5] * o[13] + m_dMatrix[9] * o[14] + m_dMatrix[13] * o[15];
	m[14] = m_dMatrix[2] * o[12] + m_dMatrix[6] * o[13] + m_dMatrix[10] * o[14] + m_dMatrix[14] * o[15];
	m[15] = m_dMatrix[3] * o[12] + m_dMatrix[7] * o[13] + m_dMatrix[11] * o[14] + m_dMatrix[15] * o[15];
}

void _3DfImport::LoadMatrixIdentity()
{
	memset(m_dMatrixStack[m_nMatrixPos], 0, 16 * sizeof(double));
	m_dMatrixStack[m_nMatrixPos][0] = 1.0;
	m_dMatrixStack[m_nMatrixPos][5] = 1.0;
	m_dMatrixStack[m_nMatrixPos][10] = 1.0;
	m_dMatrixStack[m_nMatrixPos][15] = 1.0;
}

void _3DfImport::MultMatrix(const double m[16])
{
	MatrixMatrixMult(m_dMatrixStack[m_nMatrixPos], m);
}

void _3DfImport::PushMatrix()
{
	if(m_nMatrixPos < 7)
	{
		m_nMatrixPos++;
		memcpy(m_dMatrixStack[m_nMatrixPos], m_dMatrixStack[m_nMatrixPos - 1], 16 * sizeof(double));
	}
}

void _3DfImport::PopMatrix()
{
	if(m_nMatrixPos > 0)
	{
		m_nMatrixPos--;
	}
}

void _3DfImport::GetMatrix(double m[16])
{
	memcpy(m, m_dMatrixStack[m_nMatrixPos], 16 * sizeof(double));
}

void _3DfImport::VectorCross(const A3DVector3dData * X, const A3DVector3dData * Y, A3DVector3dData * Z)
{
	Z->m_dX = X->m_dY * Y->m_dZ - X->m_dZ * Y->m_dY;
	Z->m_dY = X->m_dZ * Y->m_dX - X->m_dX * Y->m_dZ;
	Z->m_dZ = X->m_dX * Y->m_dY - X->m_dY * Y->m_dX;
}

void _3DfImport::VectorMatrixMult(A3DDouble dX, A3DDouble dY, A3DDouble dZ, double adMatrix[16],
	A3DDouble * pdResX, A3DDouble * pdResY, A3DDouble * pdResZ)
{
	*pdResX = adMatrix[0] * dX + adMatrix[4] * dY + adMatrix[8] * dZ + adMatrix[12];
	*pdResY = adMatrix[1] * dX + adMatrix[5] * dY + adMatrix[9] * dZ + adMatrix[13];
	*pdResZ = adMatrix[2] * dX + adMatrix[6] * dY + adMatrix[10] * dZ + adMatrix[14];
}

void _3DfImport::AllocVector3dArray(A3DVector3dData ** ppacArray, A3DUns32 uiSize)
{
	A3DUns32 ui;
	*ppacArray = (A3DVector3dData *) malloc(uiSize * sizeof(A3DVector3dData));
	for(ui = 0; ui < uiSize; ui++)
		A3D_INITIALIZE_DATA(A3DVector3dData, ((*ppacArray)[ui]));
}

void _3DfImport::AllocNormalsAndPoints(A3DVector3dData ** ppasNormals, A3DUns32 uiNormalSize, A3DVector3dData ** ppasPoints,
	A3DUns32 uiPointSize)
{
	AllocVector3dArray(ppasNormals, uiNormalSize);
	AllocVector3dArray(ppasPoints, uiPointSize);
}

void _3DfImport::BoundingBoxAddPoint(A3DBoundingBoxData * e, double x, double y, double z)
{
	if(e == nullptr)
		return;

	if(x < e->m_sMin.m_dX) e->m_sMin.m_dX = x;
	if(y < e->m_sMin.m_dY) e->m_sMin.m_dY = y;
	if(z < e->m_sMin.m_dZ) e->m_sMin.m_dZ = z;
	if(x > e->m_sMax.m_dX) e->m_sMax.m_dX = x;
	if(y > e->m_sMax.m_dY) e->m_sMax.m_dY = y;
	if(z > e->m_sMax.m_dZ) e->m_sMax.m_dZ = z;
}

//== 변수 Pointer 관리 함수 ===========================================================================
void _3DfImport::PointerArrayInitialize(A3DPointerArray * pcArray)
{
	if(pcArray == nullptr)
		return;

	pcArray->m_uiAllocated = 0;
	pcArray->m_uiSize = 0;
	pcArray->m_ppPointers = nullptr;
}

void _3DfImport::PointerArrayFree(A3DPointerArray * pcArray)
{
	pcArray->m_uiAllocated = 0;
	pcArray->m_uiSize = 0;
	free(pcArray->m_ppPointers);
}

void _3DfImport::PointerArrayTerminate(A3DPointerArray * pcArray)
{
	PointerArrayFree(pcArray);
}

void * _3DfImport::MiscRealloc(void * p, A3DUns32 uiOldSize, A3DUns32 uiNewSize)
{
	void * newp = malloc(uiNewSize);
	memcpy(newp, p, uiOldSize);
	free(p);

	return newp;
}

UINT _3DfImport::PointerArrayAdd(A3DPointerArray * pcArray, void * pcPointer)
{
	if(pcArray == nullptr)
		return 0;

	if(pcArray->m_uiAllocated == 0)
	{
		pcArray->m_uiAllocated = 2;
		pcArray->m_ppPointers = (void **) malloc(pcArray->m_uiAllocated * sizeof(void *));
	}

	if(pcArray->m_uiSize == pcArray->m_uiAllocated)
	{
		pcArray->m_uiAllocated *= 2;
		pcArray->m_ppPointers = (void **) MiscRealloc(pcArray->m_ppPointers, pcArray->m_uiSize * sizeof(void *),
			pcArray->m_uiAllocated * sizeof(void *));
	}

	pcArray->m_ppPointers[pcArray->m_uiSize] = pcPointer;
	pcArray->m_uiSize++;

	return pcArray->m_uiSize;
}

int _3DfImport::PointerArrayFind(A3DPointerArray * pcArray, void * pcPointer)
{
	for(UINT ui = 0; ui < pcArray->m_uiSize; ui++)
	{
		if(pcArray->m_ppPointers[ui] == pcPointer)
			return ui;
	}

	return -1;
}

UINT _3DfImport::PointerArrayAddUnique(A3DPointerArray * pcArray, void * pcPointer)
{
	if(pcArray == nullptr) {
		return 0;
	}

	if(PointerArrayFind(pcArray, pcPointer) != -1)
		return pcArray->m_uiSize;

	return PointerArrayAdd(pcArray, pcPointer);
}

UINT _3DfImport::PointerArrayAddArray(A3DPointerArray * pcArray, void ** const ppcPointers, UINT uiSize)
{
	for(UINT ui = 0; ui < uiSize; ui++) {
		PointerArrayAdd(pcArray, ppcPointers[ui]);
	}
		
	return pcArray->m_uiSize;
}

//== Log 관련 함수 ===================================================================================
void _3DfImport::CreateLog(int nId, const WCHAR * pchFilePathName)
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

void _3DfImport::Log(int nId, LPCWSTR chMessage, ...)
{
#ifdef USED_LOG_MANAGER
	va_list cArgList;
	va_start(cArgList, chMessage);

	CString strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	LogManager::GetInstance()->WriteLog(nId, strBuffer);
#endif
}

void _3DfImport::LogIncreaseTabIndex(int nId)
{
#ifdef USED_LOG_MANAGER
	LogManager::IncreaseTabIndex(nId);
#endif
}

void _3DfImport::LogDecreaseTabIndex(int nId)
{
#ifdef USED_LOG_MANAGER
	LogManager::DecreaseTabIndex(nId);
#endif
}

CString _3DfImport::LogHexStr(DWORD_PTR nValue)
{
#ifdef USED_LOG_MANAGER
	return LogManager::HexStr(nValue);
#endif
	return L"";
}

CString _3DfImport::LogBoolStr(bool bValue)
{
#ifdef USED_LOG_MANAGER
	return LogManager::BoolStr(bValue);
#endif
	return L"";
}