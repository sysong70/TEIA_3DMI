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

#include <3DF/3DF.Visibility.h>
#include <3DF/3DF.MarkerAttribute.h>

#include <chrono>
#include <utility>

#ifdef _DEBUG
//#	define new DEBUG_NEW
#	define USED_LOG_MANAGER
#endif
//#	define USED_LOG_MANAGER

#define A3D_DRAW_WITH_INDICES

using namespace std::chrono;

#define CHECK_A3D_RETURN(FunctionCall)\
{\
	const A3DStatus iRet__ = FunctionCall;\
	if (iRet__ != A3D_SUCCESS)\
	{\
		assert(false);\
		if (A3DMiscGetErrorMsg)\
			fprintf(stderr, #FunctionCall " returned error %d = %s\n", iRet__ , A3DMiscGetErrorMsg(iRet__) ); \
		else\
			fprintf(stderr, #FunctionCall " returned error %d\n", iRet__ ); \
		return iRet__;\
	}\
}

#define PI 3.1415926535897932384626433832795028841971693993751

USING_3DF_NAMESPACE
USING_3DX_NAMESPACE

_3DfImport::_3DfImport(_3DXSignal::Interface * pc3dxInterface) :
	ImportBase(pc3dxInterface)
{
}

_3DfImport::~_3DfImport()
{
	m_vcMaterialMappingStyleVector.clear();
}

bool _3DfImport::FileImport(CString strFilePathName, _3DF::SegmentKey & cModelSegment, CString & strErrorMessage)
{
	if(false == InitializeA3DLibrary(strErrorMessage)) {
		return false;
	}

	m_nIncrementalId = 0;

	m_pcModelSegment = &cModelSegment;

	system_clock::time_point cTime1 = system_clock::now();
	long long tick = cTime1.time_since_epoch().count();

	m_strCadFileName = strFilePathName.Right(strFilePathName.GetLength() - strFilePathName.ReverseFind('\\') - 1);

	// ===== _3DfImport 옵션을 설정 =====
	A3DRWParamsLoadData cParamsLoadData;
	A3D_INITIALIZE_DATA(A3DRWParamsLoadData, cParamsLoadData);

	SetDefaultParamsLoadData(cParamsLoadData);

	// #option : ReadGeomTessMode
	//cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomOnly;
	//cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomAndTess;
	cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadTessOnly;

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
	m_cPmiIncludeSegment = cModelInclude.Subsegment(L"pmi");

	// Import 관련 Option 설정
	ImportOption sImportOption;

	// 처음부터 
	sImportOption.cParentSegment = cModelSegment;

	LoadMatrixIdentity();

	cModelSegment.SetVisibility(L"lines=on");
	
	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetColor(RGBAColor(0, 0, 0), Material::Color::Type::Diffuse);
	cModelSegment.SetMaterialMapping(L"lines", cMaterialMapping);
	cModelSegment.GetMarkerAttributeControl().SetSize(0.1f);

	bool bStatus = ParseModelFile(pcAsmModelFile, cModelSegment);

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
// 	cParamsLoadData.m_sPmi.m_bAlwaysSubstituteFont = true;
//  	cParamsLoadData.m_sPmi.m_pcSubstitutionFont = new A3DUTF8Char[sizeof("Myriad CAD")];
//  	printf(cParamsLoadData.m_sPmi.m_pcSubstitutionFont, "%s", "Myriad CAD");

	// Tessellation 변수
	cParamsLoadData.m_sTessellation.m_eTessellationLevelOfDetail = kA3DTessLODMedium;
	//cParamsLoadData.m_sTessellation.m_bDoNotComputeNormalsInAccurateTessellation = true;	// Compute Normal

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
bool _3DfImport::ParseModelFile(const A3DAsmModelFile * pcAsmModelFile, _3DF::SegmentKey & cModelSegment)
{
	// #Import_Log : ExcuteFunction.log
#ifdef USED_LOG_MANAGER
	CreateLog(2, L"d:\\Temp\\ExcuteFunction.log");
	//m_cA3dTracer.CreateLog(L"D:\\Temp\\A3dXInfo.log");
	//CreateLog(1, L"d:\\Temp\\AssyStruct.log");
#endif

	Log(2, L"ConvertAsmModelFile: %s", LogHexStr((DWORD_PTR) pcAsmModelFile));

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

	if(cModelFileData.m_eModellerType == kA3DModellerIFC) {
		m_fNormalAngleCosine = static_cast<float>(cos(30 * PI / 180));
		// #Coding_require
		//HC_Set_Handedness("right");
	}
	else {
		m_fNormalAngleCosine = static_cast<float>(cos(PI / 180));
	}
		
	// ===== Model Scale 처리 부분 =====
	// Matrix 안에 있는 Scale 값을 사용해야 한다 ( info.dModelScale은 더 이상 사용하지 않는다) [12/14/2016 이인호]
	// Model Scale을 사용해야 한다 ( POccurrence Scale을 맞지 않는다 ) [6/26/2017 이인호]
	// cImportInfo.dModelScale = cModelFileData.m_bUnitFromCAD ? cModelFileData.m_dUnit : 1.0;
	// cImportInfo.eModellerType = cModelFileData.m_eModellerType;

	// Texture 관련 사항 정의
	PopulateTextures();

	double dModelScale = cModelFileData.m_bUnitFromCAD ? cModelFileData.m_dUnit : 1.0;

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

	_3DF::MaterialMappingKit cMaterial;
	cMaterial.SetColor(RGBAColor(0.752941f, 0.752941f, 0.752941f));
	cModelSegment.SetMaterialMapping(L"faces", cMaterial);

	// 최초의 Attribute 생성
	A3DMiscCascadedAttributes * pcAttrs = nullptr;
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesCreate(&pcAttrs));

	A3DUns32 nSize = cModelFileData.m_uiPOccurrencesSize;
	for(A3DUns32 nIndex = 0; nIndex < nSize; ++nIndex) {
		ParseProductOccurrence(cModelFileData.m_ppPOccurrences[nIndex], pcAttrs, dModelScale, cModelSegment);
	}

	A3DAsmModelFileGet(nullptr, &cModelFileData);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	
	A3DGlobalGet(nullptr, &m_cGlobalData);

	Log(2, L"ConvertAsmModelFile End");

	return true;
}

// == 2. Product Occurrences 관련 함수 ===============================================================

// 2-1. Product Occurrence 처리
A3DStatus _3DfImport::ParseProductOccurrence(A3DAsmProductOccurrence * pcOccurrence, A3DMiscCascadedAttributes * pcParentAttr, double dModelScale, 
	_3DF::SegmentKey & cParentSegment)
{
	if(nullptr == pcOccurrence) {
		return A3D_ERROR;
	}

	LogIncreaseTabIndex(2);

	Log(2, L"DrawProductOccurrence: pocc%d", m_nIncrementalId);

	// Segment를 생성하고 생성된 Segment를 Parent Segment에 Include한다.
	CString strSegmentName;
	strSegmentName.Format(L"pocc%d", m_nIncrementalId++);
	_3DF::SegmentKey cSegment = m_cPoccsIncludeSegment.Subsegment(strSegmentName);
	cParentSegment.IncludeSegment(cSegment);

	// Attribute 생성
	// Parent에서 받은(계단식으로) Attribute를 이용해서, Attribute를 생성
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcOccurrence, pcParentAttr, &pcAttrs, &cAttrsData));

/*
	MaterialMappingKit cMaterialMapping;
	if(A3D_SUCCESS == DrawStyle(cAttrsData, cMaterialMapping)) {
		if(true == cMaterialMapping.IsAllocate()) {
			LogIncreaseTabIndex(2);
			SegmentKey cStyleSegment;
			if(true == FindMaterialMapping(cMaterialMapping, cStyleSegment)) {
				SetStyle(cSegment, cStyleSegment);
				Log(2, L"SetStyle");
			}
			else {
				SetStyleMaterialMapping(cMaterialMapping, cSegment);
				Log(2, L"SetStyleMaterialMapping");
			}
			LogDecreaseTabIndex(2);
		}
	}
*/

	if(cAttrsData.m_bShow && !cAttrsData.m_bRemoved && A3D_SUCCESS == IsShow(pcOccurrence))
	{
		A3DAsmProductOccurrenceData cData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcOccurrence, &cData));

		if(cData.m_ucBehaviour != 1)
		{
/*
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
*/
			//cSegment.ForcedOpen();

			_3DF::MatrixKit cMatrix;
			if(A3D_SUCCESS == ProductOccurrenceGetLocation(&cData, cMatrix)) {
				if(false == cMatrix.IsIdentity()) {
					cSegment.SetModellingMatrix(cMatrix);
				}
			}

			A3DAsmPartDefinition * pcPart = nullptr;
			CHECK_A3D_RETURN(ProductOccurrenceGetPart(&cData, &pcPart));
			if(nullptr != pcPart)
			{
				CHECK_A3D_RETURN(ParsePart(pcPart, pcAttrs, dModelScale, cSegment));
			}

			A3DPointerArray apcChildArray;
			PointerArrayInitialize(&apcChildArray);
			CHECK_A3D_RETURN(ProductOccurrenceGetChild(&cData, &apcChildArray));

			A3DUns32 nSize = apcChildArray.m_uiSize;

			for(A3DUns32 nIndex = 0; nIndex < nSize; nIndex++) {
				CHECK_A3D_RETURN(ParseProductOccurrence(apcChildArray.m_ppPointers[nIndex], pcAttrs, dModelScale, cSegment));
			}

			PointerArrayTerminate(&apcChildArray);

			//m_bDrawMarkups = false;

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
					CHECK_A3D_RETURN(DrawMarkup(cMarkups.m_ppPointers[nIndex], cSegment, pcAttrs));
				}

				PointerArrayTerminate(&cMarkups);
				PointerArrayTerminate(&cViews);
			}

			//cSegment.ForcedClose();

			//CHECK_A3D_RETURN(A3DRootBaseGet(nullptr, &cRootBaseData));
		}

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttrs));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrsData));

	//cSegment.ForcedClose();

	LogDecreaseTabIndex(2);
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

	if(nullptr != pcPoData->m_pLocation) {
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
			assert(false);
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

//== Parse 관련 함수 =================================================================================

// 1. Parse Part Definition
A3DStatus _3DfImport::ParsePart(const A3DAsmPartDefinition * pcPart, const A3DMiscCascadedAttributes * pcParentAttr, double dModelScale, _3DF::SegmentKey & cParentSegment)
{
	LogIncreaseTabIndex(2);
	
	// 기존에 생성된 Part를 찾은 경우 Include로 Parent에 추가 시킨다.

	HC_KEY nSegmentKey = INVALID_KEY;
	if(true == m_mPartsMap.Lookup((DWORD_PTR) pcPart, nSegmentKey)) {
		_3DF::SegmentKey cSegment(nSegmentKey);
		cParentSegment.IncludeSegment(cSegment);
		Log(2, L"ParsePart Map: %s", cSegment.Name());

		LogDecreaseTabIndex(2);

		return A3D_SUCCESS;
	}

	Log(2, L"ParsePart: part%d", m_nIncrementalId);

	// Segment를 생성하고 생성된 Segment를 Parent Segment에 Include한다.
	SegmentKey cSegment = m_cPartsIncludeSegment.Subsegment(L"part%d", m_nIncrementalId++);
	cParentSegment.IncludeSegment(cSegment);
	m_mPartsMap.SetAt((DWORD_PTR) pcPart, cSegment.KeyValue());

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcPart, pcParentAttr, &pcAttr, &cAttrData));

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved && A3D_SUCCESS == IsShow(pcPart))
	{
		A3DAsmPartDefinitionData sData;
		A3D_INITIALIZE_DATA(A3DAsmPartDefinitionData, sData);
		CHECK_A3D_RETURN(A3DAsmPartDefinitionGet(pcPart, &sData));

		if(0 < sData.m_uiRepItemsSize)
		{
			cSegment.ForcedOpen();

			for(A3DUns32 nIndex = 0; nIndex < sData.m_uiRepItemsSize; nIndex++) {
				CHECK_A3D_RETURN(ParseRiRepresentationItem(sData.m_ppRepItems[nIndex], cSegment, pcAttr));
			}

			cSegment.ForcedClose();

			//parseAnnotations(sData.m_ppAnnotations, sData.m_uiAnnotationsSize);
		}

		CHECK_A3D_RETURN(A3DAsmPartDefinitionGet(nullptr, &sData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 2. Draw Representation Item
A3DStatus _3DfImport::ParseRiRepresentationItem(const A3DRiRepresentationItem * pcRepItem, _3DF::SegmentKey & cParentSegment,
	const A3DMiscCascadedAttributes * pcParentAttr)
{
	LogIncreaseTabIndex(2);

	if(m_nIncrementalId == 3485) {
		int i = 0;
	}

	Log(2, L"DrawRiRepresentationItem: ri%d", m_nIncrementalId);

	// Segment를 생성하고 생성된 Segment를 Parent Segment에 Include한다.

	// #3DF_Debug
// 	if(2001 != m_nIncrementalId) {
// 		m_nIncrementalId++;
// 		LogDecreaseTabIndex(2);
// 		return A3D_SUCCESS;
// 	}
/*
	if(2274 == m_nIncrementalId) {
		m_nIncrementalId = m_nIncrementalId;

		parseAttributes(pcRepItem);
	}

	parseAttributes(pcRepItem);
*/
	//g_strRiName.Format(L"ri%d", m_nIncrementalId);

// 	if(10 < m_nIncrementalId) {
// 		m_nIncrementalId++;
// 		return A3D_SUCCESS;
// //		m_nIncrementalId = m_nIncrementalId;
// 	}

	SegmentKey cSegment = m_cRisIncludeSegment.Subsegment(L"ri%d", m_nIncrementalId++);
	cParentSegment.IncludeSegment(cSegment);
	//cSegment.ForcedOpen();

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcRepItem, pcParentAttr, &pcAttr, &cAttrData));

	MaterialMappingKit cMaterialMapping;
	A3DInt32 iUVCoordinatesIndex = -1;
	A3DUns8 ucTextureDimension = 2;

	cSegment.ForcedOpen();

/*
	SegmentKey cStyleSegment;
	if(true == FindFaceMaterialMapping(cAttrData, cStyleSegment)) {
		SetStyle(cSegment, cStyleSegment);
	}
	else {
		if(A3D_SUCCESS == GetMaterialMapping(cAttrData, cMaterialMapping)) {
			if(true == cMaterialMapping.IsAllocate()) {
				SetFaceMaterialMapping(cAttrData, cMaterialMapping, cSegment);
			}
		}
	}
*/

	A3DStatus eStatus = A3D_SUCCESS;

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved && A3D_SUCCESS == IsShow(pcRepItem))
	{
		A3DEEntityType eType = kA3DTypeUnknown;
		CHECK_A3D_RETURN(A3DEntityGetType(pcRepItem, &eType));

		A3DRiRepresentationItemData cRepItemData;
		A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, cRepItemData);
		CHECK_A3D_RETURN(A3DRiRepresentationItemGet(pcRepItem, &cRepItemData));

		const A3DRiCoordinateSystem * pcCoordSys = nullptr;

		if(cAttrData.m_pCoordinateSystem != nullptr) {
			pcCoordSys = cAttrData.m_pCoordinateSystem;
		}
		else if(cRepItemData.m_pCoordinateSystem != nullptr) {
			pcCoordSys = cRepItemData.m_pCoordinateSystem;
		}

		//cSegment.ForcedOpen();

		if(pcCoordSys != nullptr) {
			A3DRiCoordinateSystemData sCSysData;
			A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sCSysData);
			CHECK_A3D_RETURN(A3DRiCoordinateSystemGet(pcCoordSys, &sCSysData));

			_3DF::MatrixKit cMatrix;
			GetMatrix(sCSysData.m_pTransformation, cMatrix);
			cSegment.SetModellingMatrix(cMatrix);

			CHECK_A3D_RETURN(A3DRiCoordinateSystemGet(nullptr, &sCSysData));
		}

		switch(eType)
		{
			case kA3DTypeRiSet:
				eStatus = DrawSet((A3DRiSet *) pcRepItem, cSegment, pcAttr);
			break;

			case kA3DTypeRiBrepModel:
				eStatus = DrawRiBrepModel(pcRepItem, cRepItemData, cSegment, pcAttr, cAttrData);
			break;

			case kA3DTypeRiPolyBrepModel:
				eStatus = DrawRiPolyBrepModel(pcRepItem, cRepItemData, cSegment, pcAttr, cAttrData);
			break;

			case kA3DTypeRiCurve:
			case kA3DTypeRiPolyWire:
				eStatus = DrawRiPolyWire(pcRepItem, cRepItemData, cSegment, pcAttr, cAttrData);
			break;

			case kA3DTypeRiPointSet:
				eStatus = DrawRiPointSet(pcRepItem, cSegment, pcAttr);
			break;

			default:
				assert(false);
			break;
		}

		A3DRiRepresentationItemGet(nullptr, &cRepItemData);
	}

	cSegment.ForcedClose();

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	LogDecreaseTabIndex(2);

	return eStatus;
}

// 2-2. Draw Set
A3DStatus _3DfImport::DrawSet(const A3DRiSet * pSet, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pSet, pcParentAttr, &pcAttr, &cAttrData));

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved && A3D_SUCCESS == IsShow(pSet))
	{
		A3DRiSetData sData;
		A3D_INITIALIZE_DATA(A3DRiSetData, sData);
		CHECK_A3D_RETURN(A3DRiSetGet(pSet, &sData));

		A3DUns32 ui;
		for(ui = 0; ui < sData.m_uiRepItemsSize; ui++)
		{
			CHECK_A3D_RETURN(ParseRiRepresentationItem(sData.m_ppRepItems[ui], cParentSegment, pcAttr));
		}

		CHECK_A3D_RETURN(A3DRiSetGet(nullptr, &sData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	return A3D_SUCCESS;
}

// 2-1. Draw Ri Brep Model
A3DStatus _3DfImport::DrawRiBrepModel(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData,
	_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData)
{
	LogIncreaseTabIndex(2);

	Log(2, L"DrawRiBrepModel: %s", LogHexStr((DWORD_PTR) pcRepItem));

	A3DRootBaseData cRootBaseData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, cRootBaseData);
	CHECK_A3D_RETURN(A3DRootBaseGet(pcRepItem, &cRootBaseData));
	m_pchRepresentationItemName = cRootBaseData.m_pcName;

	if(cRepItemData.m_pTessBase != nullptr) {
		CHECK_A3D_RETURN(DrawTessBase(cRepItemData.m_pTessBase, pcRepItem, cSegment, pcAttr));
	}
	else {
		A3DRWParamsTessellationData sTesselationData;
		A3D_INITIALIZE_DATA(A3DRWParamsTessellationData, sTesselationData);
		sTesselationData.m_eTessellationLevelOfDetail = kA3DTessLODMedium;
		CHECK_A3D_RETURN(A3DRiRepresentationItemComputeTessellation((A3DRiRepresentationItem *)pcRepItem, &sTesselationData));

		if(cRepItemData.m_pTessBase != nullptr) {
			CHECK_A3D_RETURN(DrawTessBase(cRepItemData.m_pTessBase, pcRepItem, cSegment, pcAttr));
		}
	}

	m_pchRepresentationItemName = nullptr;
	CHECK_A3D_RETURN(A3DRootBaseGet(nullptr, &cRootBaseData));

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 2-3. Draw Poly Brep Model
A3DStatus _3DfImport::DrawRiPolyBrepModel(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, 
	_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData)
{
	LogIncreaseTabIndex(2);

	Log(2, L"DrawRiPolyBrepModel: %s", LogHexStr((DWORD_PTR) pcRepItem));

	A3DStatus nStatus = A3D_SUCCESS;
	if(nullptr != cRepItemData.m_pTessBase) {
		nStatus = DrawTessBase(cRepItemData.m_pTessBase, pcRepItem, cSegment, pcAttr);
	}

	LogDecreaseTabIndex(2);

	return nStatus;
}

// 2-3. Draw Poly Wire
A3DStatus _3DfImport::DrawRiPolyWire(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, 
	_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData)
{
	A3DStatus nStatus = A3D_SUCCESS;
	if(nullptr != cRepItemData.m_pTessBase) {
		nStatus = DrawTessBase(cRepItemData.m_pTessBase, pcRepItem, cSegment, pcAttr);
	}

	return nStatus;
}

A3DStatus _3DfImport::DrawRiPointSet(const A3DRiRepresentationItem * pcRepItem, _3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	SetMarkerStyle(pcRepItem, cSegment, pcParentAttr);

	A3DRiPointSetData sData;
	A3D_INITIALIZE_DATA(A3DRiPointSetData, sData);

	A3DStatus nStatus = A3DRiPointSetGet((A3DRiPointSet *) pcRepItem, &sData);
	if(A3D_SUCCESS == nStatus) {
		cSegment.GetVisibilityControl().SetMarkers(true);

		for(unsigned int i = 0; i < sData.m_uiSize; ++i) {
			cSegment.InsertMarker(sData.m_pPts[i].m_dX, sData.m_pPts[i].m_dY, sData.m_pPts[i].m_dZ);
		}

		A3DRiPointSetGet(nullptr, &sData);
	}

	return nStatus;
}


// 4. Draw Ri Curve
A3DStatus _3DfImport::DrawRiCurve(A3DRiCurve * pcInputRiCurve, _3DF::SegmentKey & cParentSegment, A3DMiscCascadedAttributes * pcParentAttr)
{
	CString strName;
	GetName(pcInputRiCurve, strName);

	Log(2, L"DrawRiCurve: %s", strName);

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcInputRiCurve, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DRiCurveData cRiCurveData;
	A3D_INITIALIZE_DATA(A3DRiCurveData, cRiCurveData);
	A3DStatus nStatus = A3DRiCurveGet(pcInputRiCurve, &cRiCurveData);
	if(A3D_SUCCESS != nStatus) {
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return A3D_ERROR;
	}

	if(nullptr == cRiCurveData.m_pBody) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return A3D_ERROR;
	}

	A3DTopoSingleWireBodyData cSingleWireBodyData;
	A3D_INITIALIZE_DATA(A3DTopoSingleWireBodyData, cSingleWireBodyData);
	nStatus = A3DTopoSingleWireBodyGet(cRiCurveData.m_pBody, &cSingleWireBodyData);
	if(A3D_SUCCESS != nStatus) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return A3D_ERROR;
	}

	if(nullptr == cSingleWireBodyData.m_pWireEdge) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DTopoSingleWireBodyGet(nullptr, &cSingleWireBodyData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return A3D_ERROR;
	}

	// Scale을 구하기 위해서 Context Data에서 값을 가져온다.
	// 기본적으로 RiCurve에는 스케일이 들어있지 않음.
	//ConvertTopoContextScale(cRiCurveData.m_pBody, m_dContextScale);

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

/*
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
*/

	LogDecreaseTabIndex(2);

	A3DRiCurveGet(nullptr, &cRiCurveData);
	A3DTopoSingleWireBodyGet(nullptr, &cSingleWireBodyData);
	A3DTopoWireEdgeGet(nullptr, &cWireEdgeData);
	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return A3D_SUCCESS;
}

// 5. Draw Markup 관련 View
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
			//CHECK_A3D_RETURN(DrawAnnotations(cViewData.m_ppAnnotations[ui], cParentSegment, pcAttr));
		}

		CHECK_A3D_RETURN(A3DMkpViewGet(nullptr, &cViewData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	return iRet;
}
// 6. 복수의 Annotation을 그리는 함수
A3DStatus _3DfImport::DrawAnnotations(const A3DMkpAnnotationEntity ** pcAnnotation, A3DUns32 nAnnotationsSize, _3DF::SegmentKey & cParentSegment)
{
	if(0 == nAnnotationsSize) {
		return A3D_SUCCESS;
	}

	A3DMiscCascadedAttributes * pcParentAttr = nullptr;
	A3DMiscCascadedAttributesCreate(&pcParentAttr);

	for(unsigned int i = 0; i < nAnnotationsSize; i++)
	{
		DrawAnnotation(pcAnnotation[i], pcParentAttr, cParentSegment);
	}

	return A3D_SUCCESS;
}

// 6-1. Annotation을 그리는 함수
A3DStatus _3DfImport::DrawAnnotation(const A3DMkpAnnotationEntity * pcAnnotation, A3DMiscCascadedAttributes * pcParentAttr, _3DF::SegmentKey & cParentSegment)
{
	A3DEEntityType eType;
	A3DEntityGetType(pcAnnotation, &eType);

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData sAttrData;
	CreateAndPushCascadedAttributes(pcAnnotation, pcParentAttr, &pcAttrs, &sAttrData);

	switch(eType)
	{
		case kA3DTypeMkpAnnotationItem:
		{
			A3DMkpAnnotationItemData sData;
			A3D_INITIALIZE_DATA(A3DMkpAnnotationItemData, sData);
			A3DMkpAnnotationItemGet(pcAnnotation, &sData);

			A3DMiscCascadedAttributes * pcMarkupAttr;
			A3DMiscCascadedAttributesData sMarkupAttribData;
			CreateAndPushCascadedAttributes(sData.m_pMarkup, pcAttrs, &pcMarkupAttr, &sMarkupAttribData);

			//DrawMarkup(sData.m_pMarkup, &sMarkupAttribData, &tester);

/*
			HC_KEY tester = 0;
			int res = vhash_lookup_item(m_annotationhash, (void *) sData.m_pMarkup, (void **) &tester);

			if(res == VHASH_STATUS_SUCCESS)
			{
				HC_Open_Segment_By_Key(tester); {
					if(sMarkupAttribData.m_bShow)
						HC_Set_User_Options("default_visibility=on");
				}HC_Close_Segment();
			}
			else
			{
				m_AnnotationCounter++;
				traverseMarkup(sData.m_pMarkup, &sMarkupAttribData, &tester);
				vhash_insert_item(m_annotationhash, (void *) sData.m_pMarkup, (void *) tester);
			}
*/

			A3DMiscCascadedAttributesDelete(pcMarkupAttr);
		}
		break;
		
		case kA3DTypeMkpAnnotationSet:
		{
			A3DMkpAnnotationSetData sData;
			A3D_INITIALIZE_DATA(A3DMkpAnnotationSetData, sData);

			A3DMkpAnnotationSetGet(pcAnnotation, &sData);
			for(A3DUns32 i = 0; i < sData.m_uiAnnotationsSize; ++i)
			{
				//parseAnnotation(sData.m_ppAnnotations[i], pcAttrs);
			}

			A3DMkpAnnotationSetGet(NULL, &sData);
		}
		break;

		case kA3DTypeMkpAnnotationReference:

		default:
		break;
	}

	A3DMiscCascadedAttributesDelete(pcAttrs);

	return A3D_SUCCESS;
}

// 6-1. Draw Annotation Set
A3DStatus _3DfImport::DrawAnnotationSet(const A3DMkpAnnotationSet * pcAnnotationSet, _3DF::SegmentKey & cParentSegment, 
	const A3DMiscCascadedAttributes * pcParentAttr)
{
/*
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
			CHECK_A3D_RETURN(DrawAnnotations(sAnnotationSetData.m_ppAnnotations[ui], cParentSegment, pcAttr));
		}

		CHECK_A3D_RETURN(A3DMkpAnnotationSetGet(nullptr, &sAnnotationSetData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));
*/

	return A3D_SUCCESS;
}

// 6-2. (미완성) Draw Annotation Reference
A3DStatus _3DfImport::DrawAnnotationReference(const A3DMkpAnnotationItem * /*pcAnnotationItem*/, const A3DMiscCascadedAttributes * /*pcParentAttr*/)
{
	assert(false);
	return A3D_SUCCESS;
}

// 6-3. Draw Annotation Item
A3DStatus _3DfImport::DrawAnnotationItem(const A3DMkpAnnotationItem * pcAnnotationItem, _3DF::SegmentKey & cParentSegment, 
	const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcAnnotationItem, pcParentAttr, &pcAttr, &cAttrData));

	MaterialMappingKit cMaterialMapping;
	GetMaterialMapping(cAttrData, cMaterialMapping);

	if(cAttrData.m_bShow && !cAttrData.m_bRemoved)
	{
		A3DMkpAnnotationItemData sAnnotationItemData;
		A3D_INITIALIZE_DATA(A3DMkpAnnotationItemData, sAnnotationItemData);
		CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(pcAnnotationItem, &sAnnotationItemData));

		CHECK_A3D_RETURN(DrawMarkup(sAnnotationItemData.m_pMarkup, cParentSegment, pcAttr));

		CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(nullptr, &sAnnotationItemData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	return iRet;
}

// 7. Draw Markup
A3DStatus _3DfImport::DrawMarkup(const A3DMkpMarkup * pcMarkup, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr)
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
			CHECK_A3D_RETURN(DrawTessBase((A3DTessBase *) sMarkupData.m_pTessellation, nullptr, cParentSegment, pcAttr));
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
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	return A3D_SUCCESS;
}
// 8. Draw Tessellation Base
A3DStatus _3DfImport::DrawTessBase(A3DTessBase * pcTessBase, const A3DRiRepresentationItem * pcRepItem, _3DF::SegmentKey & cParentSegment, 
	const A3DMiscCascadedAttributes * pcParentAttr)
{
	if(pcTessBase == nullptr) {
		return A3D_ERROR;
	}

	A3DEEntityType eEntityType;
	CHECK_A3D_RETURN(A3DEntityGetType(pcTessBase, &eEntityType));

	A3DTessBaseData sTessBaseData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, sTessBaseData);
	CHECK_A3D_RETURN(A3DTessBaseGet(pcTessBase, &sTessBaseData));

	A3DStatus eStatus = A3D_SUCCESS;

	switch(eEntityType)
	{
		case kA3DTypeTess3D:
			eStatus = DrawTess3D((A3DTess3D *) pcTessBase, &sTessBaseData, pcRepItem, pcParentAttr, cParentSegment);
		break;

		case kA3DTypeTess3DWire:
			eStatus = DrawTess3DWire((A3DTess3DWire *) pcTessBase, &sTessBaseData, pcRepItem, pcParentAttr, cParentSegment);
		break;

/*
		case kA3DTypeTessMarkup:
			//eStatus = DrawTessMarkup((A3DTessMarkup *) pcTessBase, pcRepItem, pcParentAttr);
		break;
*/

		default:
			assert(false);
		break;
	}

	A3DTessBaseGet(nullptr, &sTessBaseData);

	return eStatus;
}


// 8-1. Draw Tess3D
A3DStatus _3DfImport::DrawTess3D(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
	const A3DMiscCascadedAttributes * pcParentAttr, _3DF::SegmentKey & cParentSegment)
{
	LogIncreaseTabIndex(2);

	double dUnitScale = 1.0;	// Tessellation은 1:1 비율로 들어온다 

	_3DF::SegmentKey cCurrnetSegment = cParentSegment;

	A3DTess3DData cTess3dData;
	A3D_INITIALIZE_DATA(A3DTess3DData, cTess3dData);
	CHECK_A3D_RETURN(A3DTess3DGet(pcTess3D, &cTess3dData));

	A3DUns32 nFacesCount = cTess3dData.m_uiFaceTessSize;

	if(0 == nFacesCount) {
		A3DTess3DGet(nullptr, &cTess3dData);
		return A3D_ERROR;
	}

	A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[0];


/*
	A3DMiscCascadedAttributes * pcAttribute;
	A3DMiscCascadedAttributesData sFistAttrData;
	CreateAndPushCascadedAttributesTessFace(pcRepItem, pcTess3D, &cTessFaceData, 0, pcParentAttr, &pcAttribute, &sFistAttrData);

	//Log(2, L"pcAttribute: %s, sFistAttrData: %s,, m_uiRgbColorIndex: %d", LogHexStr((DWORD_PTR) pcAttribute), LogHexStr((DWORD_PTR) &sFistAttrData), sFistAttrData.m_sStyle.m_uiRgbColorIndex);

	SetFaceStyle(cParentSegment, sFistAttrData);

	A3DMiscCascadedAttributesDelete(pcAttribute);
	A3DMiscCascadedAttributesGet(nullptr, &sFistAttrData);*/

	const double cBigValue = 1.0e+12;
	bool bStrange = false;

	double dScale = 1.0;// cImportInfo.dModelScale * cImportInfo.dTessellationScale;

	A3DUns32 * pnTriangleIndices = cTess3dData.m_puiTriangulatedIndexes;

	UINT nTriangleFaceCount = 0;

	// ----- Point 활당 -----
	A3DUns32 nPointCount = pcTessBaseData->m_uiCoordSize / 3;

	ConvertFaceInfo cConFaceInfo;

	cConFaceInfo.pnInIndices = cTess3dData.m_puiTriangulatedIndexes;

	cConFaceInfo.aInPoints.resize(nPointCount);
	for(A3DUns32 nIndex = 0; nIndex < nPointCount; nIndex++) {
		cConFaceInfo.aInPoints[nIndex].Set(
			pcTessBaseData->m_pdCoords[nIndex * 3],
			pcTessBaseData->m_pdCoords[nIndex * 3 + 1],
			pcTessBaseData->m_pdCoords[nIndex * 3 + 2]);
	}

	// ----- Normal Vector 활당 -----
	A3DUns32 nNormalCount = cTess3dData.m_uiNormalSize / 3;
	cConFaceInfo.aInNormals.resize(nNormalCount);
	for(A3DUns32 nIndex = 0; nIndex < nNormalCount; nIndex++) {
		cConFaceInfo.aInNormals[nIndex].Set(
			cTess3dData.m_pdNormals[nIndex * 3],
			cTess3dData.m_pdNormals[nIndex * 3 + 1],
			cTess3dData.m_pdNormals[nIndex * 3 + 2]);
	}

	// ----- Texture Parameter 활당 -----
/*
	A3DUns32 nTextureCoordCount = cTess3dData.m_uiTextureCoordSize;
	cConFaceInfo.aInParams.resize(nTextureCoordCount);
	for(A3DUns32 nIndex = 0; nIndex < nTextureCoordCount; nIndex++) {
		cConFaceInfo.aInParams[nIndex] = static_cast<float>(cTess3dData.m_pdTextureCoords[nIndex]);
	}
*/
	// Attribute를 계산해서 처리하는 부분, 전체 Attribute를 찾아서 가장 많은 종류의 Index를 찾아서 처리한다.
	A3DMiscCascadedAttributesData * psAttrData = new A3DMiscCascadedAttributesData[nFacesCount];
	A3DMiscCascadedAttributes * pcAttribute;

	struct StyleDefine
	{
		DWORD nCount = 0;
		A3DUns32 nRgbColorIndex = 0;
		A3DUns32 nFirstFaceIndex = 0;
	};

	CAtlMap<A3DUns32, StyleDefine> mStyleDefineMap;

	A3DUns32 nMaxRgbColorIndex = 0;
	A3DUns32 nMaxFaceIndex = 0;
	DWORD nMaxCount = 0;

	for(A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++) {
		A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[nFaceIndex];
		CreateAndPushCascadedAttributesTessFace(pcRepItem, pcTess3D, &cTessFaceData, nFaceIndex, pcParentAttr, &pcAttribute, &psAttrData[nFaceIndex]);

		A3DUns32 nRgbColorIndex = psAttrData[nFaceIndex].m_sStyle.m_uiRgbColorIndex;

		StyleDefine sStyleDefine;
		// 찾은 경우 Count 증가
		if(true == mStyleDefineMap.Lookup(nRgbColorIndex, sStyleDefine)) {
			sStyleDefine.nCount++;
			mStyleDefineMap.SetAt(nRgbColorIndex, sStyleDefine);
		}
		else { // 새로 삽입하는 경우 Count는 1, 나중에 사용하기 위해서 필요한 값들을 저장해 놓는다.
			sStyleDefine.nCount = 1;
			sStyleDefine.nRgbColorIndex = nRgbColorIndex;
			sStyleDefine.nFirstFaceIndex = nFaceIndex;
			mStyleDefineMap.SetAt(nRgbColorIndex, sStyleDefine);
		}

		// Max Count Style을 찾아내서 대표 Style로 지정할 준비를 한다.
		if(nMaxCount < sStyleDefine.nCount) {
			nMaxRgbColorIndex = sStyleDefine.nRgbColorIndex;
			nMaxCount = sStyleDefine.nCount;
			nMaxFaceIndex = sStyleDefine.nFirstFaceIndex;
		}

		A3DMiscCascadedAttributesDelete(pcAttribute);
	}

	// Max Key는 삭제한다.
	mStyleDefineMap.RemoveKey(nMaxRgbColorIndex);

	CAtlMap<A3DUns32, HC_KEY> mFaceSegmentStyleMap;

	// Max Color Index 값을 Parent Segment에 적용시켜 대표 Color Style로 지정한다.
	SetFaceStyle(cParentSegment, psAttrData[nMaxFaceIndex]);
	mFaceSegmentStyleMap.SetAt(nMaxRgbColorIndex, cParentSegment.KeyValue());
	Log(2, L"max_style_face_%d, count: %d", nMaxRgbColorIndex, nMaxCount);

	// Max Color Index를 제외한 나머지 Color Index들은 각각의 Style Face Segment를 생성해서 집어 넣도록 한다.
	POSITION pcPos = mStyleDefineMap.GetStartPosition();
	while(nullptr != pcPos)
	{
		StyleDefine sStyleDefine = mStyleDefineMap.GetNextValue(pcPos);

		_3DF::SegmentKey cSubSegment = cParentSegment.Subsegment(L"style_face_%d", sStyleDefine.nRgbColorIndex).KeyValue();
		SetFaceStyle(cSubSegment, psAttrData[sStyleDefine.nFirstFaceIndex]);
		mFaceSegmentStyleMap.SetAt(sStyleDefine.nRgbColorIndex, cSubSegment.KeyValue());
	}

	mStyleDefineMap.RemoveAll();

	LogIncreaseTabIndex(2);

	for(A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++)
	{
		A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[nFaceIndex];
		cConFaceInfo.pcInTessFaceData = &cTessFaceData;

		A3DUns32 nRgbColorIndex = psAttrData[nFaceIndex].m_sStyle.m_uiRgbColorIndex;

		bool bForceOpenFlag = false;
		if(nMaxRgbColorIndex != nRgbColorIndex)
		{
			HC_KEY nKey;
			if(true == mFaceSegmentStyleMap.Lookup(nRgbColorIndex, nKey)) {
				_3DF::SegmentKey cSubSegment(nKey);
				cCurrnetSegment = cSubSegment;
				bForceOpenFlag = true;
				cCurrnetSegment.ForcedOpen();

				Log(2, L"style_face_%d", nRgbColorIndex);
			}
		}

		cConFaceInfo.nOutTriSizeIndex = 0;	// 한 Triangle Type당 하나씩
		cConFaceInfo.nOutTriStartIndex = cTessFaceData.m_uiStartTriangulated;
		cConFaceInfo.nOutTriColorIndex = 0;
		cConFaceInfo.fInNormalCosine = m_fNormalAngleCosine;

		cConFaceInfo.mOutIndexMap.clear();
		cConFaceInfo.aOutVertexRefs.clear();
		cConFaceInfo.aOutFacePoints.clear();
		cConFaceInfo.aOutFaceList.clear();
		cConFaceInfo.aOutFaceVertexNormals.clear();
		cConFaceInfo.aOutFaceVertexParams.clear();
		cConFaceInfo.aOutFaceVertexColors.clear();

		// fill out the RGBA vertex color array (if necessary)
/*
		cConFaceInfo.aInColors.reserve(cTessFaceData.m_uiRGBAVerticesSize / (cTessFaceData.m_bIsRGBA ? 4 : 3));
		for(A3DUns32 nIndex = 0; nIndex < cTessFaceData.m_uiRGBAVerticesSize; / * increment inside loop * /)
		{
			RGBAColor cRgbaColor(cTessFaceData.m_pucRGBAVertices[nIndex + 0] / 255.0f,
				cTessFaceData.m_pucRGBAVertices[nIndex + 1] / 255.0f,
				cTessFaceData.m_pucRGBAVertices[nIndex + 2] / 255.0f);
			nIndex += 3;
			if(cTessFaceData.m_bIsRGBA) {
				cRgbaColor.alpha = cTessFaceData.m_pucRGBAVertices[nIndex++] / 255.0f;
			}

			cConFaceInfo.aInColors.push_back(cRgbaColor);
		}
*/


		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle) {
			nTriangleFaceCount += ConvertTessFaceDataTriangle(cConFaceInfo);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleFan(cConFaceInfo);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleStripe(cConFaceInfo);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleOneNormal(cConFaceInfo);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleFanOneNormal(cConFaceInfo);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleStripeOneNormal(cConFaceInfo);
		}

		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured) {
			nTriangleFaceCount += ConveTessFaceDataTriangleTextured(cConFaceInfo);
		}

		if(false == cConFaceInfo.aOutFacePoints.empty()) {

			_3DF::ShellKit cShellKit;
			cShellKit.SetPoints(cConFaceInfo.aOutFacePoints);
			cShellKit.SetNormals(cConFaceInfo.aOutFaceVertexNormals);
			cShellKit.SetFacelist(cConFaceInfo.aOutFaceList);
			cShellKit.SetParameters(cConFaceInfo.aOutFaceVertexParams);
			cShellKit.SetColors(cConFaceInfo.aOutFaceVertexColors);

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
				acWirePoints[k] = cConFaceInfo.aInPoints[cTess3dData.m_puiWireIndexes[nStartWireIndex + index++] / 3];
			}

			cCurrnetSegment.InsertLine(acWirePoints.size(), acWirePoints.data());
		}

		if(true == bForceOpenFlag) {
			cCurrnetSegment.ForcedClose();
		}

		cCurrnetSegment = cParentSegment;
	}

	LogDecreaseTabIndex(2);

	Log(2, L"DrawTess3D: %s, %d, Style Count: %d", LogHexStr((DWORD_PTR) pcTess3D), nTriangleFaceCount, m_mFaceMaterialMappingStyleMap.size());

	REMOVE_ARRAY(psAttrData);
// 
// 	if(cTess3DData.m_bMustRecalculateNormals)
// 		cTess3DData.m_pdNormals = nullptr;

	CHECK_A3D_RETURN(A3DTess3DGet(nullptr, &cTess3dData));

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 8-1-1. Triangle Face Data 변환
UINT _3DfImport::ConvertTessFaceDataTriangle(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceListIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32  nInVertexParamSize = 0;

	A3DUns32 nTriangleCount = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangle: %d", nTriangleCount);

	for(A3DUns32 nTriIndex = 0; nTriIndex < nTriangleCount; nTriIndex++)
	{
		A3DUns32 nNormalIndex = cInFaceInfo.nOutTriStartIndex++;

		nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceListIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceListIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceListIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		AddTriangle(cInFaceInfo, nFaceListIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
	}

	return nTriangleCount;
}

UINT _3DfImport::DrawTessFaceDataTriangle(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriangleIndices, A3DUns32 & nTriangleSizeIndex, A3DUns32 & nTriangleStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray)
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

		ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray, anNormalIndexArray);
	}

	return nTriangleCount;
}

// 8-1-2. Triangle Fan Data 변환
UINT _3DfImport::ConvertTessFaceDataTriangleFan(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceListIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32  nInVertexParamSize = 0;

	A3DUns32 nTriFanCount = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleFan: %d", nTriFanCount);

	for(A3DUns32 nFanIndex = 0; nFanIndex < nTriFanCount; nFanIndex++)
	{
		A3DUns32 nTriFanIndex = cInFaceInfo.nOutTriStartIndex + 2;
		A3DUns32 nTriColorIndex = cInFaceInfo.nOutTriColorIndex;
		A3DUns32 nTriFanPointSize = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex];
		bool bSingleNormalFlag = false;								// flag indicating if there are uniform vertex normals per triangle fan

/*
		// determine if we have one normal per vertex or just one per triangle fan
		if(cInFaceInfo.pcInTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal
			|| cInFaceInfo.pcInTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured)
		{
			if(cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex] & kA3DTessFaceDataNormalSingle)
				bSingleNormalFlag = true;

			nTriFanPointSize &= ~kA3DTessFaceDataNormalSingle;
		}
*/

		cInFaceInfo.nOutTriSizeIndex++;

		//A3DUns32 first_color_index = nTriColorIndex;
// 		if(cInFaceInfo.aInColors.size() > 0)
// 			nTriColorIndex++;

		for(A3DUns32 tri = 0; tri < nTriFanPointSize - 2; tri++)
		{
			nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
			nFaceListIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;

			nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nTriFanIndex++] / 3;
			nFaceListIndices[1] = cInFaceInfo.pnInIndices[nTriFanIndex++] / 3;

			nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex] / 3;
			nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex + 1];
			nFaceListIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex + 1] / 3;

			AddTriangle(cInFaceInfo, nFaceListIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
		}

// 		if(bSingleNormalFlag)
// 			nOutTriStartIndex += (1 + tc_size) * nTriFanPointSize + 1;
// 		else
		cInFaceInfo.nOutTriStartIndex += 2 * nTriFanPointSize;

// 		if(cInFaceInfo.aInColors.size() > 0)
// 			out_tri_color_index += nTriFanPointSize;
	}

	return nTriFanCount;
}

UINT _3DfImport::DrawTessFaceDataTriangleFan(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray)
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

			ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray, anNormalIndexArray);
			nTriFaceCount++;
		}

		nTriStartIndex += nPointCount * 2;
	}

	return nTriFaceCount;
}

// 8-1-3. Triangle Stripe Data 변환
UINT _3DfImport::ConvertTessFaceDataTriangleStripe(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceListIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array

	A3DUns32 nStripesCount = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];
	A3DUns32 nTriFaceCount = 0;

	// Log(2, L"ConvertTessFaceDataTriangleStripe: %d", nStripesCount);

	bool hasVertexColor = false; // INT2bool(cTessFaceData.m_uiRGBAVerticesSize);
	if(0 < cInFaceInfo.pcInTessFaceData->m_uiRGBAVerticesSize) {
		hasVertexColor = true;
	}

	for(A3DUns32 nStripeIndex = 0; nStripeIndex < nStripesCount; nStripeIndex++)
	{
		A3DUns32 nTriStripIndex = cInFaceInfo.nOutTriStartIndex;
		A3DUns32 nPointCount = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex];
		if(0 == nPointCount) {
			cInFaceInfo.nOutTriSizeIndex++;
			assert(false);
			continue;
		}

		cInFaceInfo.nOutTriSizeIndex++;

		for(A3DUns32 tri = 0; tri < nPointCount - 2; tri++)
		{
			if((tri % 2) == 0)
			{
				nFaceListIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFaceListIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFaceListIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex -= 2;
			}
			else
			{
				nFaceListIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFaceListIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFaceListIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex -= 2;
			}

			A3DUns32  nInVertexParamSize = 0; //(tc_size == 0 ? 0 : 2),
			AddTriangle(cInFaceInfo, nFaceListIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);

			//ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacelistIndices, nVertexNormalIndices, anFacelistArray, anNormalIndexArray);

			nTriFaceCount++;
		}

// 		if(bSingleNormalFlag)
// 			nOutTriStartIndex += (1) * nTsPoints + 1;
// 		else

		cInFaceInfo.nOutTriStartIndex += 2 * nPointCount;
	}

	return nTriFaceCount;
}

UINT _3DfImport::DrawTessFaceDataTriangleStripe(A3DTessFaceData & cTessFaceData, _3DF::PointArray & acInPoints, _3DF::VectorArray acInNormals, A3DUns32 * pnInIndices,
	A3DUns32 & nOutTriSizeIndex, A3DUns32 & nOutTriStartIndex, A3DUns32 & nTriColorIndex, IndexHash & mOutIndexMap, TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap,
	_3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray)
{
	int nFaceListIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array

	// HPoint face4[3];

	A3DUns32 nStripesCount = cTessFaceData.m_puiSizesTriangulated[nOutTriSizeIndex++];
	A3DUns32 nTriFaceCount = 0;

	bool hasVertexColor = false; // INT2bool(cTessFaceData.m_uiRGBAVerticesSize);
	if(0 < cTessFaceData.m_uiRGBAVerticesSize) {
		hasVertexColor = true;
	}

	//for(A3DUns32 nStripeIndex = 0; nStripeIndex < nStripesCount; nStripeIndex++)
	for(A3DUns32 nStripeIndex = 0; nStripeIndex < 4; nStripeIndex++)
	{
		A3DUns32 nTsPoints = cTessFaceData.m_puiSizesTriangulated[nOutTriSizeIndex];
		if(0 == nTsPoints) {
			nOutTriSizeIndex++;
			continue;
		}

		int nColorIndex = nTriColorIndex;
		int nTriStripIndex = nOutTriStartIndex;

		bool bSingleNormalFlag = false;								// flag indicating if there are uniform vertex normals per tri-nStripIndex

		// determine if we have a normal per vertex or just one per triangle nStripIndex
		if(cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal
			|| cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
		{
			if(cTessFaceData.m_puiSizesTriangulated[nOutTriSizeIndex] & kA3DTessFaceDataNormalSingle)
			{
				bSingleNormalFlag = true;
			}
		}

		nOutTriSizeIndex++;

		if(true == bSingleNormalFlag) {
			nTriStripIndex++;
		}

		for(A3DUns32 tri = 0; tri < nTsPoints - 2; tri++)
		{
			if((tri % 2) == 0)
			{
				nFaceVertexNormalIndices[0] = pnInIndices[nTriStripIndex++] / 3;
				//nVertexNormalIndices[0] = pnInIndices[(bSingleNormalFlag ? nOutTriStartIndex : nTriStripIndex++)] / 3;
				nFaceListIndices[0] = pnInIndices[nTriStripIndex++] / 3;

				nFaceVertexNormalIndices[1] = pnInIndices[nTriStripIndex] / 3;
				//nVertexNormalIndices[1] = pnInIndices[(bSingleNormalFlag ? nOutTriStartIndex : nTriStripIndex + 0)] / 3;
				nFaceListIndices[1] = pnInIndices[nTriStripIndex + 1] / 3;
				//nFacelistIndices[1] = pnInIndices[nTriStripIndex + (bSingleNormalFlag ? 0 : 1) + tc_size] / 3;

				nFaceVertexNormalIndices[2] = pnInIndices[nTriStripIndex + 2] / 3;
				//nVertexNormalIndices[2] = pnInIndices[(bSingleNormalFlag ? nOutTriStartIndex : nTriStripIndex + 2 + tc_size)] / 3;
				nFaceListIndices[2] = pnInIndices[nTriStripIndex + 3] / 3;
				//nFacelistIndices[2] = pnInIndices[nTriStripIndex + (bSingleNormalFlag ? 1 : 3) + 2 * tc_size] / 3;
			}
			else
			{
				nFaceVertexNormalIndices[0] = pnInIndices[(bSingleNormalFlag ? nOutTriStartIndex : nTriStripIndex + 4)] / 3;
				nFaceListIndices[0] = pnInIndices[nTriStripIndex + (bSingleNormalFlag ? 2 : 5)] / 3;

				nFaceVertexNormalIndices[1] = pnInIndices[(bSingleNormalFlag ? nOutTriStartIndex : nTriStripIndex + 2)] / 3;
				nFaceListIndices[1] = pnInIndices[nTriStripIndex + (bSingleNormalFlag ? 1 : 3)] / 3;

				nFaceVertexNormalIndices[2] = pnInIndices[(bSingleNormalFlag ? nOutTriStartIndex : nTriStripIndex++)] / 3;
				nFaceListIndices[2] = pnInIndices[nTriStripIndex++] / 3;
			}
/*
			if(!(tri % 2))
			{
				nVertexNormalIndices[0] = pnInIndices[nTriStripIndex] / 3;
				nFacelistIndices[0] = pnInIndices[nTriStripIndex + 1] / 3;
				nTriStripIndex += 2;

				nVertexNormalIndices[1] = pnInIndices[nTriStripIndex] / 3;
				nFacelistIndices[1] = pnInIndices[nTriStripIndex + 1] / 3;
				nTriStripIndex += 2;

				nVertexNormalIndices[2] = pnInIndices[nTriStripIndex] / 3;
				nFacelistIndices[2] = pnInIndices[nTriStripIndex + 1] / 3;
				nTriStripIndex -= 2;

				if(hasVertexColor) {
					//AddVertexColor(face4, cTessFaceData.m_pucRGBAVertices, nColorIndex, false, true)
				}
			}
			else
			{
				nFacelistIndices[2] = pnInIndices[nTriStripIndex + 1] / 3;
				nVertexNormalIndices[2] = pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFacelistIndices[1] = pnInIndices[nTriStripIndex + 1] / 3;
				nVertexNormalIndices[1] = pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFacelistIndices[0] = pnInIndices[nTriStripIndex + 1] / 3;
				nVertexNormalIndices[0] = pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex -= 2;

				if(hasVertexColor) {
					//AddVertexColor(face4, cTessFaceData.m_pucRGBAVertices, nColorIndex, true, true);
				}
			}*/

/*
			// m_VColors가 nullptr 인 경우가 있다 -_-;
			if(hasVertexColor && cCreatedShell.m_VColors) {
				cCreatedShell.AddNextFaceWithDistinctNormalsAndColor(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, face4, 3);
			}
			else {
				cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
			}
*/

			FloatArray cInGParams;
			RGBAColorArray acInColors;
			A3DUns32  nInVertexParamSize = 0; //(tc_size == 0 ? 0 : 2),
			IntArray anIntVertexRefsArray;

// 			AddTriangle(acInPoints, acInNormals, cInGParams, acInColors, nFaceListIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices,
// 				nInVertexParamSize, m_fNormalAngleCosine, mOutIndexMap, anIntVertexRefsArray);


/*
				_3DF::PointArray & acOutFacePoints,		// points for the shell (representing a CAD face) to which we are adding this triangle
				_3DF::IntArray & anOutFaceList,			// facelist for shell (representing a CAD face) to which we are adding this triangle
				_3DF::VectorArray & acOutFaceVertexNormals,		// vertex normals for the shell (representing a CAD face) to which we are adding this triangle
				_3DF::FloatArray & afOutFaceVertexParams,		// optional vertex parameters for the shell (representing a CAD face) to which we are adding this triangle
				_3DF::RGBAColorArray & acOutFaceVertexColors)		// optional RGBA vertex colors for the shell (representing a CAD face) to which we are adding this triangle
*/

			//ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacelistIndices, nVertexNormalIndices, anFacelistArray, anNormalIndexArray);

			nTriFaceCount++;
		}

		if(bSingleNormalFlag)
			nOutTriStartIndex += (1) * nTsPoints + 1;
		else
			nOutTriStartIndex += (2) * nTsPoints;

// 		nOutTriStartIndex += nTsPoints * 2;
// 		nTriColorIndex += nTsPoints * 3;
	}

	return nTriFaceCount;
}

// 8-1-4. Triangle Stripe One Normal 변환
UINT _3DfImport::ConvertTessFaceDataTriangleOneNormal(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceListIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32 nInVertexParamSize = 0;

	A3DUns32 nTriangleCount = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleOneNormal: %d", nTriangleCount);

	for(A3DUns32 nTriIndex = 0; nTriIndex < nTriangleCount; nTriIndex++)
	{
		A3DUns32 nNormalIndex = cInFaceInfo.nOutTriStartIndex++;

		nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceListIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceListIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceListIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		AddTriangle(cInFaceInfo, nFaceListIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
	}

	return nTriangleCount;
}

UINT _3DfImport::DrawTessFaceDataTriangleOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray)
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

		ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray, anNormalIndexArray);
	}

	return nTriCount;
}

// 8-1-5. Triangle Fan One Normal Data 변환
UINT _3DfImport::ConvertTessFaceDataTriangleFanOneNormal(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceListIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32 nInVertexParamSize = 0;

	A3DUns32 nTriFanCount = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];
	
	// Log(2, L"ConvertTessFaceDataTriangleFanOneNormal: %d", nTriFanCount);

	for(A3DUns32 fan = 0; fan < nTriFanCount; fan++)
	{
		A3DUns32	nTriFanIndex = cInFaceInfo.nOutTriStartIndex + 2;
		A3DUns32	nTriColorIndex = cInFaceInfo.nOutTriColorIndex;
		A3DUns32	nTriFanPointSize = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex];
		bool		bSingleNormalFlag = false;								// flag indicating if there are uniform vertex normals per triangle fan

		// determine if we have one normal per vertex or just one per triangle fan
		if(cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex] & kA3DTessFaceDataNormalSingle) {
			bSingleNormalFlag = true;
			nTriFanPointSize &= ~kA3DTessFaceDataNormalSingle;
		}

		cInFaceInfo.nOutTriSizeIndex++;

		A3DUns32 nFirstColorIndex = nTriColorIndex;
		if(0 < cInFaceInfo.aInColors.size()) {
			nTriColorIndex++;
		}

		for(A3DUns32 tri = 0; tri < nTriFanPointSize - 2; tri++)
		{
			nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
			nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1];
			nFaceListIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;
			if(cInFaceInfo.aInColors.size() > 0) {
				nFaceVertexColorIndices[0] = nFirstColorIndex;
			}

			nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[(bSingleNormalFlag ? cInFaceInfo.nOutTriStartIndex : nTriFanIndex++)] / 3;
			nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[nTriFanIndex];
			nFaceListIndices[1] = cInFaceInfo.pnInIndices[nTriFanIndex++] / 3;
			if(cInFaceInfo.aInColors.size() > 0)
				nFaceVertexColorIndices[1] = nTriColorIndex++;

			nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[(bSingleNormalFlag ? cInFaceInfo.nOutTriStartIndex : nTriFanIndex)] / 3;
			nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex + (bSingleNormalFlag ? 0 : 1)];
			nFaceListIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex + (bSingleNormalFlag ? 0 : 1)] / 3;
			if(cInFaceInfo.aInColors.size() > 0)
				nFaceVertexColorIndices[2] = nTriColorIndex;

			AddTriangle(cInFaceInfo, nFaceListIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
		}

		if(true == bSingleNormalFlag) {
			cInFaceInfo.nOutTriStartIndex += nTriFanPointSize + 1;
		}
		else {
			cInFaceInfo.nOutTriStartIndex += 2 * nTriFanPointSize;
		}

		if(cInFaceInfo.aInColors.size() > 0) {
			cInFaceInfo.nOutTriColorIndex += nTriFanPointSize;
		}
	}

	return nTriFanCount;
}

UINT _3DfImport::DrawTessFaceDataTriangleFanOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray)
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

				ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray, anNormalIndexArray);
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

				ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray, anNormalIndexArray);
				//cCreatedShell.AddNextFaceWithDistinctNormals(pcPoints, pcNormals, nFacePointIndex, nFaceNormalIndex, 3);
				nTriCount++;
			}
			nTriStartIndex += (nPointsCount + 1);
		}
	}

	return nTriCount;
}

// 8-1-6. Triangle Stripe One Normal Data 변환
UINT _3DfImport::ConvertTessFaceDataTriangleStripeOneNormal(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceListIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32 nInVertexParamSize = 0;

	A3DUns32 nStripesCount = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleStripeOneNormal: %d", nStripesCount);

	for(A3DUns32 nStripIndex = 0; nStripIndex < nStripesCount; nStripIndex++)
	{
		A3DUns32 nTriStripIndex = cInFaceInfo.nOutTriStartIndex;
		A3DUns32 nTriColorIndex = cInFaceInfo.nOutTriColorIndex;
		A3DUns32 nTriStripPointSize = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex];
		bool bSingleNormal = false;								// flag indicating if there are uniform vertex normals per tri-nStripIndex

		// determine if we have a normal per vertex or just one per triangle nStripIndex
		if(cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex] & kA3DTessFaceDataNormalSingle) {
			bSingleNormal = true;
			nTriStripPointSize &= ~kA3DTessFaceDataNormalSingle;
		}

		cInFaceInfo.nOutTriSizeIndex++;

		if(true == bSingleNormal) {
			nTriStripIndex++;
		}

		for(A3DUns32 nTriIndex = 0; nTriIndex < nTriStripPointSize - 2; nTriIndex++)
		{
			if((nTriIndex % 2) == 0)
			{
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex++)] / 3;
				nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex];
				nFaceListIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex++] / 3;
				if(cInFaceInfo.aInColors.size() > 0) {
					nFaceVertexColorIndices[0] = nTriColorIndex++;
				}

				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex + 0)] / 3;
				nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 0 : 1)];
				nFaceListIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 0 : 1)] / 3;
				if(cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[1] = nTriColorIndex;

				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex + 2)] / 3;
				nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 1 : 3)];
				nFaceListIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 1 : 3)] / 3;
				if(cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[2] = nTriColorIndex + 1;
			}
			else
			{
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex + 4)] / 3;
				nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 2 : 5)];
				nFaceListIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 2 : 5)] / 3;
				if(cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[0] = nTriColorIndex + 2;

				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex + 2)] / 3;
				nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 1 : 3)];
				nFaceListIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 1 : 3)] / 3;
				if(cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[1] = nTriColorIndex + 1;

				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex++)] / 3;
				nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex];
				nFaceListIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex++] / 3;
				if(cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[2] = nTriColorIndex++;
			}

			AddTriangle(cInFaceInfo, nFaceListIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
		}

		if(bSingleNormal)
			cInFaceInfo.nOutTriStartIndex += nTriStripPointSize + 1;
		else
			cInFaceInfo.nOutTriStartIndex += 2 * nTriStripPointSize;

		if(0 < cInFaceInfo.aInColors.size() > 0) {
			cInFaceInfo.nOutTriColorIndex += nTriStripPointSize;
		}
	}

	return nStripesCount;
}

UINT _3DfImport::ConveTessFaceDataTriangleTextured(ConvertFaceInfo & cInFaceInfo)
{
	A3DUns32 nTextureCoordSize = 0;
	if(cInFaceInfo.pcInTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured) {
		nTextureCoordSize = cInFaceInfo.pcInTessFaceData->m_uiTextureCoordIndexesSize;
	}

	int nFaceListIndices[3]; // facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3]; // vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3]; // vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3]; // vertex color indices into the RGBA color array
	A3DUns32 nInVertexParamSize = 0;

	A3DUns32 nTriangleCount = cInFaceInfo.pcInTessFaceData->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	for(A3DUns32 tri = 0; tri < nTriangleCount; tri++)
	{
		A3DUns32 nNormalIndex = cInFaceInfo.nOutTriStartIndex++;

		nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += nTextureCoordSize;
		nFaceListIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		if(cInFaceInfo.aInColors.size() > 0) {
			nFaceVertexColorIndices[0] = cInFaceInfo.nOutTriColorIndex++;
		}

		nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += nTextureCoordSize;
		nFaceListIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		if(cInFaceInfo.aInColors.size() > 0) {
			nFaceVertexColorIndices[1] = cInFaceInfo.nOutTriColorIndex++;
		}

		nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += nTextureCoordSize;
		nFaceListIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		if(cInFaceInfo.aInColors.size() > 0) {
			nFaceVertexColorIndices[2] = cInFaceInfo.nOutTriColorIndex++;
		}

		AddTriangle(cInFaceInfo, nFaceListIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
	}

	return nTriangleCount;
}

UINT _3DfImport::DrawTessFaceDataTriangleTextured(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
	TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray)
{
	A3DUns32 nTriangleCount = cTessFaceData.m_puiSizesTriangulated[nTriSizeIndex++];
	A3DUns32 nTextureCoordIndexSize = cTessFaceData.m_uiTextureCoordIndexesSize;

	A3DUns32 nFacePointIndex[3];
	A3DUns32 nFaceNormalIndex[3];

	TessIndexMap maLocalNormalIndexMap;

	for(A3DUns32 nIndex = 0; nIndex < nTriangleCount; ++nIndex)
	{
		A3DUns32 nNormalIndex = nTriStartIndex++;

		nFaceNormalIndex[0] = pnTriIndices[nNormalIndex] / 3;
		nTriStartIndex += nTextureCoordIndexSize;
		nFacePointIndex[0] = pnTriIndices[nTriStartIndex++] / 3;

		MatchVertexNormal(maLocalNormalIndexMap, nFacePointIndex[0], nFaceNormalIndex[0]);

		nFaceNormalIndex[1] = pnTriIndices[nTriStartIndex++] / 3;
		nTriStartIndex += nTextureCoordIndexSize;
		nFacePointIndex[1] = pnTriIndices[nTriStartIndex++] / 3;

		MatchVertexNormal(maLocalNormalIndexMap, nFacePointIndex[1], nFaceNormalIndex[1]);

		nFaceNormalIndex[2] = pnTriIndices[nTriStartIndex++] / 3;
		nTriStartIndex += nTextureCoordIndexSize;
		nFacePointIndex[2] = pnTriIndices[nTriStartIndex++] / 3;

		MatchVertexNormal(maLocalNormalIndexMap, nFacePointIndex[2], nFaceNormalIndex[2]);

		ConvertFaceList(maPointIndexMap, maNormalIndexMap, nFacePointIndex, nFaceNormalIndex, anFacelistArray, anNormalIndexArray);
	}

	return nTriangleCount;
}

// 8-2-1. Tess3D Wire 처리
A3DStatus _3DfImport::DrawTess3DWire(const A3DTess3DWire * pTess3DWire, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
	const A3DMiscCascadedAttributes * pcParentAttr, _3DF::SegmentKey & cParentSegment)
{
	A3DEEntityType eType;
	CHECK_A3D_RETURN(A3DEntityGetType(pcRepItem, &eType));

	switch (eType)
	{
		case kA3DTypeRiPolyWire:
			return DrawPolyWires(pTess3DWire, pcTessBaseData, pcRepItem, pcParentAttr, cParentSegment);
		break;

/*
		case kA3DTypeRiCurve:
			assert(false);
			return A3D_ERROR;
		break;
*/
	}
	

	A3DStatus nStatus = A3D_SUCCESS;

	SetLineStyle(pTess3DWire, cParentSegment, pcParentAttr);

	const A3DDouble * pdCoord = &pcTessBaseData->m_pdCoords[0];

	A3DUns32 nPointCount = pcTessBaseData->m_uiCoordSize / 3;

	_3DF::PointArray acWirePoints(nPointCount);

	for(A3DUns32 nIndex = 0; nIndex < nPointCount; nIndex++)
	{
		acWirePoints[nIndex].x = pcTessBaseData->m_pdCoords[(nIndex * 3)];
		acWirePoints[nIndex].y = pcTessBaseData->m_pdCoords[(nIndex * 3) + 1];
		acWirePoints[nIndex].z = pcTessBaseData->m_pdCoords[(nIndex * 3) + 2];
	}

	cParentSegment.InsertLine(acWirePoints.size(), acWirePoints.data());

	return nStatus;
}

A3DStatus _3DfImport::DrawPolyWires(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
	const A3DMiscCascadedAttributes * pcParentAttr, _3DF::SegmentKey & cSegment)
{
	A3DTess3DWireData sWireData;
	A3D_INITIALIZE_DATA(A3DTess3DWireData, sWireData);
	CHECK_A3D_RETURN(A3DTess3DWireGet(pcTess3D, &sWireData));

	cSegment.GetVisibilityControl().SetLines(true);

	if(nullptr == sWireData.m_puiSizesWires) {
		A3DUns32 nPointCount = pcTessBaseData->m_uiCoordSize / 3;
		_3DF::PointArray aPoints(nPointCount);

		A3DUns32 nPointIndex = 0;
		for(A3DUns32 nIndex = 0; nIndex < nPointCount; nIndex++) {
			nPointIndex = nIndex * 3;
			aPoints[nIndex].x = pcTessBaseData->m_pdCoords[nPointIndex];
			aPoints[nIndex].y = pcTessBaseData->m_pdCoords[nPointIndex + 1];
			aPoints[nIndex].z = pcTessBaseData->m_pdCoords[nPointIndex + 2];
		}

		cSegment.InsertLine(aPoints.size(), aPoints.data());
	}
	else 
	{
		A3DUns32 nIndex = 0;
		A3DUns32 nInfoIndex = 0;
		A3DUns32 nPointIndex = 0;
		bool bClosed = false;
		Point cPoint;

		_3DF::PointArray aPoints;

		while(nIndex < sWireData.m_uiSizesWiresSize)
		{
			nInfoIndex = sWireData.m_puiSizesWires[nIndex];

			if(nInfoIndex & kA3DTess3DWireDataIsClosing) {
				bClosed = true;
				nInfoIndex -= kA3DTess3DWireDataIsClosing;
			}

			if(!(nInfoIndex & kA3DTess3DWireDataIsContinuous)) //Continuious
			{
				if(0 < aPoints.size())
				{
					cSegment.InsertLine(aPoints.size(), aPoints.data());
					aPoints.resize(0);
				}
			}
			else {
				nInfoIndex -= kA3DTess3DWireDataIsContinuous;
			}

			for(A3DUns32 i = 0; i < nInfoIndex; i++)
			{
				nPointIndex = sWireData.m_puiSizesWires[nIndex + 1 + i];
				cPoint.x = pcTessBaseData->m_pdCoords[nPointIndex];
				cPoint.y = pcTessBaseData->m_pdCoords[nPointIndex + 1];
				cPoint.z = pcTessBaseData->m_pdCoords[nPointIndex + 2];
				aPoints.push_back(cPoint);
			}

			if(bClosed)
			{
				nPointIndex = sWireData.m_puiSizesWires[nIndex + 1];
				cPoint.x = pcTessBaseData->m_pdCoords[nPointIndex];
				cPoint.y = pcTessBaseData->m_pdCoords[nPointIndex + 1];
				cPoint.z = pcTessBaseData->m_pdCoords[nPointIndex + 2];
				aPoints.push_back(cPoint);

				cSegment.InsertLine(aPoints.size(), aPoints.data());
				aPoints.resize(0);
			}

			nIndex += nInfoIndex + 1;
		}
	}

	SetLineStyle(cSegment, pcParentAttr);

	return A3D_SUCCESS;
}

// 8-2-3. Facet 갯수를 가져옴
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

// 8-2-1-1. Facet 갯수를 가져옴
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
	A3DUns32 * pnFacePointIndex, A3DUns32 * pnFaceNormalIndex, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anVertexNoramlIndexArray)
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

	// Triangle을 구성할 Normal Index
	A3DUns32 nNormalIndex[3];

	for(int nIndex = 0; nIndex < 3; nIndex++) {
		// 사전에 저장된 Face Point Index를 찾는다. 찾은 경우 새롭게 생성되어지고 있는 Point Index를 이용해서
		// Face Index를 구성한다. 여기서 생성되는 Face Index는 개별 Face의 Index이기 때문에 0부터 시작하도록 구성한다.
		if(false == maNormalIndexMap.Lookup(pnFaceNormalIndex[nIndex], nNormalIndex[nIndex])) {
			A3DUns32 nCount = (A3DUns32) maNormalIndexMap.GetCount();
			maPointIndexMap.SetAt(pnFaceNormalIndex[nIndex], nCount);
			nNormalIndex[nIndex] = nCount;
		}
	}

	anFacelistArray.push_back(3);
	anFacelistArray.push_back(nPointIndex[0]);
	anFacelistArray.push_back(nPointIndex[1]);
	anFacelistArray.push_back(nPointIndex[2]);

	anVertexNoramlIndexArray.push_back(3);
	anVertexNoramlIndexArray.push_back(nNormalIndex[0]);
	anVertexNoramlIndexArray.push_back(nNormalIndex[1]);
	anVertexNoramlIndexArray.push_back(nNormalIndex[2]);

	return true;
}

void _3DfImport::AddTriangle(ConvertFaceInfo & cInFaceInfo,
	int const pnInFaceListIndices[3],			// facelist indices into the "global" point array for this triangle
	int const pnInFaceVertexNromalIndices[3],	// vertex normal indices into the "global" normal array for this triangle
	int const pnInFaceVertexParamIndices[3],	// optional vertex parameter indices into the "global" parameter array for this triangle
	int const pnInFaceVertexColorIndices[3],	// optional vertex color indices into the RGBA color array for this triangle
	A3DUns32  nInVertexParamSize)				// vertex parameter size (0 if no parameters)
{
	if(pnInFaceListIndices[0] == pnInFaceListIndices[1] || pnInFaceListIndices[1] == pnInFaceListIndices[2] || pnInFaceListIndices[0] == pnInFaceListIndices[2]) {
		return;
	}

	// Face List 숫자 추가
	cInFaceInfo.aOutFaceList.push_back(3);

	_3DF::Vector cNormal;
	bool bZeroLengthFlag = false;

	double dLength0 = cInFaceInfo.aInNormals[pnInFaceVertexNromalIndices[0]].LengthSquared();
	double dLength1 = cInFaceInfo.aInNormals[pnInFaceVertexNromalIndices[1]].LengthSquared();
	double dLength2 = cInFaceInfo.aInNormals[pnInFaceVertexNromalIndices[2]].LengthSquared();
	
	if(0.01 > dLength0 || 0.01 > dLength1 || 0.01 > dLength2) {
		bZeroLengthFlag = true;
		_3DF::Vector cX = cInFaceInfo.aInPoints[pnInFaceListIndices[1]] - cInFaceInfo.aInPoints[pnInFaceListIndices[0]];
		_3DF::Vector cY = cInFaceInfo.aInPoints[pnInFaceListIndices[2]] - cInFaceInfo.aInPoints[pnInFaceListIndices[0]];

		cNormal = cX.Cross(cY);
		cNormal.Normalize();
	}

	for(int nVertexIndex = 0; nVertexIndex < 3; nVertexIndex++)
	{
		// Two cases:
		// (1) Either we haven't seen this vertex before (or we have but our normal, if present, or vertex parameter, if present, 
		//     or vertex color, if present, is different), in which case we need to add this point, possibly normal, possibly vertex parameter
		//     and possibly vertex color to our "local" arrays and note the mapping between the "global" index and the new "local" index; OR
		// (2) We have seen this vertex before and it has the same normals if present (and vertex parameter if present, and vertex color if present)
		//     as a previous encounter, in which case we can reuse the "local" index.

		auto cOutIndexIterator = cInFaceInfo.mOutIndexMap[pnInFaceListIndices[nVertexIndex]].cbegin();

		while(true)
		{
			// Index Map에서 FaceListIndex를 발견하지 못한 경우 처리
			if(cOutIndexIterator == cInFaceInfo.mOutIndexMap[pnInFaceListIndices[nVertexIndex]].cend())
			{
				cInFaceInfo.aOutFacePoints.push_back(cInFaceInfo.aInPoints[pnInFaceListIndices[nVertexIndex]]);
				cInFaceInfo.aOutFaceList.push_back(static_cast<int>(cInFaceInfo.aOutFacePoints.size() - 1));

				if(cInFaceInfo.aInNormals.size() > 0) {
					if(false == bZeroLengthFlag) {
						cInFaceInfo.aOutFaceVertexNormals.push_back(cInFaceInfo.aInNormals[pnInFaceVertexNromalIndices[nVertexIndex]]);
					}
					else {
						cInFaceInfo.aOutFaceVertexNormals.push_back(cNormal);
					}
				}

				if(nInVertexParamSize > 0) {
					for(A3DUns32 k = 0; k < nInVertexParamSize; k++) {
						cInFaceInfo.aOutFaceVertexParams.push_back(cInFaceInfo.aInParams[pnInFaceVertexParamIndices[nVertexIndex] + k]);
					}
				}

				if(cInFaceInfo.aInColors.size() > 0) {
					cInFaceInfo.aOutFaceVertexColors.push_back(cInFaceInfo.aInColors[pnInFaceVertexColorIndices[nVertexIndex]]);
				}

				cInFaceInfo.mOutIndexMap[pnInFaceListIndices[nVertexIndex]].push_back(static_cast<int>(cInFaceInfo.aOutFacePoints.size() - 1));
				cInFaceInfo.aOutVertexRefs.push_back(1);
				
				break;
			}
			// Index Map에서 FaceListIndex를 발견한 경우 처리
			else if(false == bZeroLengthFlag)
			{
				if(cInFaceInfo.aInNormals.empty() ||
					cInFaceInfo.aOutFaceVertexNormals[*cOutIndexIterator].Dot(cInFaceInfo.aInNormals[pnInFaceVertexNromalIndices[nVertexIndex]]) >= cInFaceInfo.fInNormalCosine)
				{
					bool bMatchingFlag = true;
					for(A3DUns32 k = 0; k < nInVertexParamSize; k++) {
						if(cInFaceInfo.aOutFaceVertexParams[*cOutIndexIterator + k] != cInFaceInfo.aInParams[pnInFaceVertexParamIndices[nVertexIndex] + k])
						{
							bMatchingFlag = false;
							break;
						}
					}

					if(cInFaceInfo.aInColors.size() > 0) {
						if(cInFaceInfo.aOutFaceVertexColors[*cOutIndexIterator] != cInFaceInfo.aInColors[pnInFaceVertexColorIndices[nVertexIndex]]) {
							bMatchingFlag = false;
						}
					}

					if(true == bMatchingFlag)
					{
						// 찾은 Index를 이용해서 Face List를 추가한다.
						cInFaceInfo.aOutFaceList.push_back(*cOutIndexIterator);
						cInFaceInfo.aOutVertexRefs[*cOutIndexIterator] += 1;

						if(!cInFaceInfo.aInNormals.empty())
						{
							int ref_count = cInFaceInfo.aOutVertexRefs[*cOutIndexIterator];

							cInFaceInfo.aOutFaceVertexNormals[*cOutIndexIterator] = (ref_count - 1) / (float) ref_count * cInFaceInfo.aOutFaceVertexNormals[*cOutIndexIterator] +
								cInFaceInfo.aInNormals[pnInFaceVertexNromalIndices[nVertexIndex]] / (float) ref_count;

							cInFaceInfo.aOutFaceVertexNormals[*cOutIndexIterator].Normalize();
						}
						break;
					}
				} // if(cInFaceInfo.aInNormals.empty() ||
			} // else if(false == bZeroLengthFlag)

			cOutIndexIterator++;
		}
	}


}
// 8. Face Draw Style 정의
A3DStatus _3DfImport::SetFaceStyle(const A3DRootBaseWithGraphics * pcBase, _3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcBase, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DStatus eStatus = SetFaceStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus _3DfImport::SetFaceStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributesData cAttrsData;
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, cAttrsData);
	A3DMiscCascadedAttributesGet(pcParentAttr, &cAttrsData);

	A3DStatus eStatus = SetFaceStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus _3DfImport::SetFaceStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData)
{
	SegmentKey cStyleSegment;
	if(true == FindFaceMaterialMapping(cAttrsData, cStyleSegment)) {
		SetStyle(cSegment, cStyleSegment);
	}
	else {
		_3DF::MaterialMappingKit cMaterialMapping;
		if(A3D_SUCCESS == GetMaterialMapping(cAttrsData, cMaterialMapping)) {
			SetFaceMaterialMapping(cAttrsData, cMaterialMapping, cSegment);
		}
	}

	return A3D_SUCCESS;
}

A3DStatus _3DfImport::SetLineStyle(const A3DRootBaseWithGraphics * pcBase, _3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcBase, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DStatus eStatus = SetLineStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus _3DfImport::SetLineStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributesData cAttrsData;
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, cAttrsData);
	A3DMiscCascadedAttributesGet(pcParentAttr, &cAttrsData);

	A3DStatus eStatus = SetLineStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus _3DfImport::SetLineStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData)
{
	SegmentKey cStyleSegment;
	if(true == FindLineMaterialMapping(cAttrsData, cStyleSegment)) {
		SetStyle(cSegment, cStyleSegment);
	}
	else {
		_3DF::MaterialMappingKit cMaterialMapping;
		if(A3D_SUCCESS == GetMaterialMapping(cAttrsData, cMaterialMapping)) {
			SetLineMaterialMapping(cAttrsData, cMaterialMapping, cSegment);
		}
	}

	return A3D_SUCCESS;
}

A3DStatus _3DfImport::SetMarkerStyle(const A3DRootBaseWithGraphics * pcBase, _3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcBase, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DStatus eStatus = SetMarkerStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus _3DfImport::SetMarkerStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributesData cAttrsData;
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, cAttrsData);
	A3DMiscCascadedAttributesGet(pcParentAttr, &cAttrsData);

	A3DStatus eStatus = SetMarkerStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus _3DfImport::SetMarkerStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData)
{
	SegmentKey cStyleSegment;
	if(true == FindMarkerMaterialMapping(cAttrsData, cStyleSegment)) {
		SetStyle(cSegment, cStyleSegment);
	}
	else {
		_3DF::MaterialMappingKit cMaterialMapping;
		if(A3D_SUCCESS == GetMaterialMapping(cAttrsData, cMaterialMapping)) {
			SetMarkerMaterialMapping(cAttrsData, cMaterialMapping, cSegment);
		}
	}

	return A3D_SUCCESS;
}

A3DStatus _3DfImport::GetMaterialMapping(const A3DMiscCascadedAttributesData & cAttrsData, A3DInt32 * pnUVCoordinatesIndex,
	A3DUns8 * pucTextureDimension, _3DF::MaterialMappingKit & cMaterialKit)
{
	const A3DGraphStyleData * pcStyleData = &cAttrsData.m_sStyle;

	if(nullptr == pcStyleData) {
		return A3D_ERROR;
	}

	LogIncreaseTabIndex(2);

	A3DStatus nRetStatus = A3D_SUCCESS;

	bool bTransparencyDefined = (1 == cAttrsData.m_sStyle.m_bIsTransparencyDefined) ? true : false;
	float fTransparency = cAttrsData.m_sStyle.m_ucTransparency / 255.0f;

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

// 			nRetStatus = A3DGlobalGetGraphTextureDefinitionData(A3D_DEFAULT_TEXTURE_DEFINITION_INDEX, &sTextureDefinitionData);
// 			nRetStatus = A3DGlobalGetGraphTextureApplicationData(A3D_DEFAULT_MATERIAL_INDEX, &sTextureAppplicationData);
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
			if(true == bTransparencyDefined) { cDiffuseColor.alpha = fTransparency; }
			cMaterialKit.SetColor(cDiffuseColor, _3DF::Material::Color::Type::Diffuse);

			//Log(2, L"DrawStyle: DiffuseColor R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);
/*
			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiAmbient, &sRgbColorData));
			_3DF::RGBAColor cAmbientColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dAmbientAlpha);
			cMaterialKit.SetFaceColor(cAmbientColor, _3DF::Material::Color::Channel::);
*/

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiEmissive, &sRgbColorData));
			_3DF::RGBAColor cEmissiveColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dEmissiveAlpha);
			if(true == bTransparencyDefined) { cEmissiveColor.alpha = fTransparency; }
			cMaterialKit.SetColor(cEmissiveColor, _3DF::Material::Color::Type::Emission);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiSpecular, &sRgbColorData));
			_3DF::RGBAColor cSpecularColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dSpecularAlpha);
			if(true == bTransparencyDefined) { cSpecularColor.alpha = fTransparency; }
			cMaterialKit.SetColor(cSpecularColor, _3DF::Material::Color::Type::Specular);

// 			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
// 			nRetStatus = A3DGlobalGetGraphMaterialData(A3D_DEFAULT_MATERIAL_INDEX, &sMaterialData);

			cMaterialKit.SetGloss(sMaterialData.m_dShininess);
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
			cMaterialKit.SetColor(cDiffuseColor);

			//Log(2, L"DrawStyle: R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
		}
	}

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 8-1. 일반 DrawStyle 정의 
A3DStatus _3DfImport::GetMaterialMapping(const A3DMiscCascadedAttributesData & cAttrsData, _3DF::MaterialMappingKit & cMaterialKit)
{
	const A3DGraphStyleData * pcStyleData = &cAttrsData.m_sStyle;

	if(pcStyleData == nullptr) {
		return A3D_ERROR;
	}

	LogIncreaseTabIndex(2);

	bool bTransparencyDefined = (1 == cAttrsData.m_sStyle.m_bIsTransparencyDefined) ? true : false;
	float fTransparency = cAttrsData.m_sStyle.m_ucTransparency / 255.0f;

	A3DStatus nRetStatus = A3D_SUCCESS;

	if(A3D_TRUE == pcStyleData->m_bMaterial)
	{
		A3DGraphMaterialData materialdata;
		A3D_INITIALIZE_DATA(A3DGraphMaterialData, materialdata);

		nRetStatus = A3DGlobalGetGraphMaterialData(pcStyleData->m_uiRgbColorIndex, &materialdata);
		A3DBool bMaterialIsTexture = A3D_FALSE;
		A3DGlobalIsMaterialTexture(pcStyleData->m_uiRgbColorIndex, &bMaterialIsTexture);

		if(A3D_TRUE == bMaterialIsTexture)
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);

			nRetStatus = A3DGlobalGetGraphRgbColorData(materialdata.m_uiDiffuse, &sRgbColorData);

			A3DGraphTextureApplicationData sTextureAppData;
			A3D_INITIALIZE_DATA(A3DGraphTextureApplicationData, sTextureAppData);

			A3DGlobalGetGraphTextureApplicationData(pcStyleData->m_uiRgbColorIndex, &sTextureAppData);

			A3DGraphTextureDefinitionData sTextureData;
			A3D_INITIALIZE_DATA(A3DGraphTextureDefinitionData, sTextureData);

			A3DGlobalGetGraphTextureDefinitionData(sTextureAppData.m_uiTextureDefinitionIndex, &sTextureData);

			_3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);
			cMaterialKit.SetColor(cDiffuseColor, _3DF::Material::Color::Type::Diffuse);

			CString strTexture;
			if(sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				strTexture.Format(L"environment = texture_%d, mirror = (r = 0.5 g = 0.5 b = 0.5))", sTextureAppData.m_uiTextureDefinitionIndex);
			}
			else {
				strTexture.Format(L"texture_%d", sTextureAppData.m_uiTextureDefinitionIndex);
			}

			cMaterialKit.SetTexture(strTexture);
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
			if(true == bTransparencyDefined) {  cDiffuseColor.alpha = fTransparency; }
			cMaterialKit.SetColor(cDiffuseColor, _3DF::Material::Color::Type::Diffuse);

			//Log(2, L"DrawStyle: DiffuseColor R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);
/*
			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiAmbient, &sRgbColorData));
			_3DF::RGBAColor cAmbientColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dAmbientAlpha);
			cMaterialKit.SetFaceColor(cAmbientColor, _3DF::Material::Color::Channel::);
*/

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiEmissive, &sRgbColorData));
			_3DF::RGBAColor cEmissiveColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dEmissiveAlpha);
			if(true == bTransparencyDefined) { cEmissiveColor.alpha = fTransparency; }
			cMaterialKit.SetColor(cEmissiveColor, _3DF::Material::Color::Type::Emission);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiSpecular, &sRgbColorData));
			_3DF::RGBAColor cSpecularColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dSpecularAlpha);
			if(true == bTransparencyDefined) { cSpecularColor.alpha = fTransparency; }
			cMaterialKit.SetColor(cSpecularColor, _3DF::Material::Color::Type::Specular);

// 			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
// 			nRetStatus = A3DGlobalGetGraphMaterialData(A3D_DEFAULT_MATERIAL_INDEX, &sMaterialData);
			cMaterialKit.SetGloss(sMaterialData.m_dShininess);
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
			if(true == bTransparencyDefined) { cDiffuseColor.alpha = fTransparency; }
			cMaterialKit.SetColor(cDiffuseColor);

			//Log(2, L"DrawStyle: R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);

			//A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
		}
	}

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
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
			assert(false);
		break;
	}

	return A3D_SUCCESS;
}

//== Texture 관련 함수 ===============================================================================

void _3DfImport::PopulateTextures()
{
/*
	InitializeMagick(".");

	HC_Set_Visibility("image = off");

	A3DGlobal * pGlobal = NULL;
	A3DGlobalGetPointer(&pGlobal);

	A3DGlobalData globalData;
	A3D_INITIALIZE_DATA(A3DGlobalData, globalData);

	A3DGlobalGet(pGlobal, &globalData);

	/ * This while loop is loading all the images. * /
	VArray<unsigned char> pixels;

	for(unsigned int ui = 0; ui < globalData.m_uiPicturesSize; ui++)
	{
		Query::GraphPicture graphPicture(ui);
		if(!graphPicture.IsValid())
			continue;

		A3DGraphPictureData const & data = graphPicture.data;
		bool	size_ok = (data.m_uiPixelWidth != 0 && data.m_uiPixelHeight != 0);

		if(size_ok && data.m_eFormat == kA3DPictureBitmapRgbaByte)
		{
			InvertImage(data.m_pucBinaryData, data.m_uiPixelWidth, data.m_uiPixelHeight, true);
			HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", ui), data.m_uiPixelWidth, data.m_uiPixelHeight, data.m_pucBinaryData);
		}
		else if(size_ok && data.m_eFormat == kA3DPictureBitmapRgbByte)
		{
			InvertImage(data.m_pucBinaryData, data.m_uiPixelWidth, data.m_uiPixelHeight, false);
			HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgb, name = image %u, local = on", ui), data.m_uiPixelWidth, data.m_uiPixelHeight, data.m_pucBinaryData);
		}
		else
		{
			ExceptionInfo exception;
			GetExceptionInfo(&exception);
			ImageInfo * image_info = CloneImageInfo((ImageInfo *) NULL);
			Image * image = BlobToImage(image_info, (void *) data.m_pucBinaryData, data.m_uiSize, &exception);
			if(image != NULL)
			{
				unsigned long width = image->magick_columns;
				unsigned long height = image->magick_rows;
				pixels.EnsureSize(width * height * 4);

				ExportImagePixels(image, 0, 0, width, height, "RGBA", CharPixel, &pixels[0], &exception);
				DestroyImage(image);
				DestroyImageInfo(image_info);
				DestroyExceptionInfo(&exception);

				HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", ui), width, height, &pixels[0]);
			}
		}
	}

	for(A3DUns32 ui = 0; ui < globalData.m_uiTextureDefinitionsSize; ui++)
	{
		Query::GraphTextureDefinition textureDefinition(ui);
		if(!textureDefinition.IsValid())
			continue;

		A3DGraphTextureDefinitionData const & data = textureDefinition.data;

		H_FORMAT_TEXT texture_options("source = image %u", data.m_uiPictureIndex);

		if(data.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection)
		{
			texture_options.Append(", parameterization source = reflection vector");
		}
		else
		{
			texture_options.Append(", parameterization source = uv");
		}

		texture_options.Append(data.m_eTextureFunction == kA3DTextureFunctionModulate ? ", modulate" : ", no modulate");

		char const * tiling = nullptr;
		switch(data.m_eTextureWrappingModeS)
		{
			case kA3DTextureWrappingModeMirroredRepeat:
			{
				tiling = "mirror";
			}	break;

			case kA3DTextureWrappingModeClamp:
			case kA3DTextureWrappingModeClampToBorder:
			case kA3DTextureWrappingModeClampToEdge:
			{
				tiling = "drop";
			}	break;

			case kA3DTextureWrappingModeUnknown:
			case kA3DTextureWrappingModeRepeat:
			default:
			{
				tiling = "repeat";
			}
		}
		texture_options.Append(", tiling = %s", tiling);

		if(data.m_pTextureTransfo != nullptr)
		{
			A3DGraphTextureTransformationData transformationData;
			A3D_INITIALIZE_DATA(A3DGraphTextureTransformationData, transformationData);
			A3DGraphTextureTransformationGet(data.m_pTextureTransfo, &transformationData);
			float textureMatrix[16] = { 0 };
			for(int i = 0; i < 16; ++i)
				textureMatrix[i] = static_cast<float>(transformationData.m_dMatrix[i]);
			HC_Compute_Matrix_Inverse(textureMatrix, textureMatrix);

			char textureTransformSegment[4096] = "";
			HC_Open_Segment("/include library/texture_transformations");
			{
				HC_Open_Segment("");
				{
					HC_Show_Segment(HC_Create_Segment("."), textureTransformSegment);
					HC_Set_Texture_Matrix(textureMatrix);
				}
				HC_Close_Segment();
			}
			HC_Close_Segment();

			texture_options.Append(", transform = %s", textureTransformSegment);
		}

		texture_options.Append(", decal, interpolation filter = on, no decimation filter, no downsampling, parameter offset = 0");
		HC_Define_Local_Texture(H_FORMAT_TEXT("texture_%u", ui), texture_options);
	}
	DestroyMagick();
*/
}

//== Attribute 관련 함수 =============================================================================

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

// 4. Show 상태를 확인
A3DStatus _3DfImport::IsShow(const A3DRootBaseWithGraphics * pGraphics)
{
	if(pGraphics == nullptr) {
		return A3D_ERROR;
	}

	A3DStatus iRet = A3D_SUCCESS;

	A3DRootBaseWithGraphicsData sRootData;
	A3D_INITIALIZE_DATA(A3DRootBaseWithGraphicsData, sRootData);
	CHECK_A3D_RETURN(A3DRootBaseWithGraphicsGet(pGraphics, &sRootData));

	A3DGraphicsData sGraphicsData;
	A3D_INITIALIZE_DATA(A3DGraphicsData, sGraphicsData);
	CHECK_A3D_RETURN(A3DGraphicsGet(sRootData.m_pGraphics, &sGraphicsData));

	A3DBool bShow;
	if(sRootData.m_pGraphics) {
		bShow = (sGraphicsData.m_usBehaviour & kA3DGraphicsShow) != 0;
	}
	else {
		bShow = A3D_TRUE;
	}

	CHECK_A3D_RETURN(A3DGraphicsGet(nullptr, &sGraphicsData));
	CHECK_A3D_RETURN(A3DRootBaseWithGraphicsGet(nullptr, &sRootData));

	if(A3D_TRUE == bShow) {
		return A3D_SUCCESS;
	}

	return A3D_ERROR;
}

// 5. 주어진 Material Mapping을 이용해서 
bool _3DfImport::SetFaceMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cSegment)
{
	_3DF::SegmentKey cStyleSegment = m_pcModelSegment->StylesInclude().Subsegment(L"face_mat_%d", cAttrData.m_sStyle.m_uiRgbColorIndex);
	cStyleSegment.SetMaterialMapping(L"faces", cInKit);

	_3DF::StyleControl cStyleControl = cSegment.GetStyleControl();
	_3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

// 	if(INVALID_KEY == cStyle.KeyValue()) {
// 		return false;
// 	}

	m_mFaceMaterialMappingStyleMap.insert(std::make_pair(cAttrData.m_sStyle.m_uiRgbColorIndex, cStyleSegment));

	return true;
}

bool _3DfImport::SetLineMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cSegment)
{
	_3DF::SegmentKey cStyleSegment = m_pcModelSegment->StylesInclude().Subsegment(L"line_mat_%d", cAttrData.m_sStyle.m_uiRgbColorIndex);
	cStyleSegment.SetMaterialMapping(L"lines", cInKit);

	_3DF::StyleControl cStyleControl = cSegment.GetStyleControl();
	_3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

// 	if(INVALID_KEY == cStyle.KeyValue()) {
// 		return false;
// 	}

	m_mLineMaterialMappingStyleMap.insert(std::make_pair(cAttrData.m_sStyle.m_uiRgbColorIndex, cStyleSegment));

	return true;
}

bool _3DfImport::SetMarkerMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cSegment)
{
	_3DF::SegmentKey cStyleSegment = m_pcModelSegment->StylesInclude().Subsegment(L"marker_mat_%d", cAttrData.m_sStyle.m_uiRgbColorIndex);
	cStyleSegment.SetMaterialMapping(L"markers", cInKit);

	_3DF::StyleControl cStyleControl = cSegment.GetStyleControl();
	_3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

// 	if(INVALID_KEY == cStyle.KeyValue()) {
// 		return false;
// 	}

	m_mMarkerMaterialMappingStyleMap.insert(std::make_pair(cAttrData.m_sStyle.m_uiRgbColorIndex, cStyleSegment));

	return true;
}


// 5-1. 주어진 Material Mapping을 이용해서 
bool _3DfImport::SetStyle(_3DF::SegmentKey & cSegment, _3DF::SegmentKey & cStyleSegment)
{
	_3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

	if(INVALID_KEY == cStyle.KeyValue()) {
		return false;
	}

	return true;
}

// 6. 미리 저장되어 있는 Material Mapping Kit을 찾아오는 함수
bool _3DfImport::FindFaceMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, SegmentKey & cOutStyleSegment)
{
	auto pcIterator = m_mFaceMaterialMappingStyleMap.find(cAttrData.m_sStyle.m_uiRgbColorIndex);

	if(pcIterator == m_mFaceMaterialMappingStyleMap.end()) {
		return false;
	}

	cOutStyleSegment = pcIterator->second;

	return true;
}

bool _3DfImport::FindLineMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, SegmentKey & cOutStyleSegment)
{
	auto pcIterator = m_mLineMaterialMappingStyleMap.find(cAttrData.m_sStyle.m_uiRgbColorIndex);

	if(pcIterator == m_mLineMaterialMappingStyleMap.end()) {
		return false;
	}

	cOutStyleSegment = pcIterator->second;

	return true;
}

bool _3DfImport::FindMarkerMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, SegmentKey & cOutStyleSegment)
{
	auto pcIterator = m_mMarkerMaterialMappingStyleMap.find(cAttrData.m_sStyle.m_uiRgbColorIndex);

	if(pcIterator == m_mMarkerMaterialMappingStyleMap.end()) {
		return false;
	}

	cOutStyleSegment = pcIterator->second;

	return true;
}

bool _3DfImport::FindMaterialMapping(CString strGeometry, _3DF::MaterialMappingKit const & cInKit, SegmentKey & cOutStyleSegment)
{
	for(auto & cMaterialStyle : m_vcMaterialMappingStyleVector) {
		if(0 == cMaterialStyle.strGeometry.CompareNoCase(strGeometry)) {
			if(cMaterialStyle.cMaterialMappingKit == cInKit) {
				cOutStyleSegment = cMaterialStyle.cStyleSegment;
				return true;
			}
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
		A3DMiscCartesianTransformationData sData;
		A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, sData);

		CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(pcLocation, &sData));

		MbCartPoint3D cOrigin = GetMbCartPoint3D(sData.m_sOrigin);
		MbVector3D cAxisX = GetMbVector3D(sData.m_sXVector);
		MbVector3D cAxisY = GetMbVector3D(sData.m_sYVector);
		MbVector3D cAxisZ;

		if(sData.m_ucBehaviour & kA3DTransformationMirror) {
			cAxisZ = cAxisY | cAxisX;
		}
		else {
			cAxisZ = cAxisX | cAxisY;
		}

		cMatrix.SetAxisX() = cAxisX;
		cMatrix.SetAxisY() = cAxisY;
		cMatrix.SetAxisZ() = cAxisZ;
		cMatrix.Scale(sData.m_sScale.m_dX, sData.m_sScale.m_dY, sData.m_sScale.m_dZ);

		cMatrix.SetOrigin() = cOrigin;

		CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(nullptr, &sData));
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

#define MVO_BUFFER_SIZE 4096

void  _3DfImport::parseAttributes(const A3DEntity * pEntity)
{
	A3DRootBaseData rootbaseData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, rootbaseData);

	A3DRootBaseGet(pEntity, &rootbaseData);

	std::vector<char> title;
	std::vector<char> collect;
	std::vector<char> processed;

	char scratch[MVO_BUFFER_SIZE];

	for(unsigned int i = 0; i < rootbaseData.m_uiSize; i++)
	{
		if(title.capacity() < MVO_BUFFER_SIZE)
			title.reserve(MVO_BUFFER_SIZE);
		title.resize(0);

		A3DMiscAttributeData attributeData;
		A3D_INITIALIZE_DATA(A3DMiscAttributeData, attributeData);

		A3DMiscAttributeGet(rootbaseData.m_ppAttributes[i], &attributeData);

		//		H_UTF16 utf16_title_attributes;
		if(attributeData.m_bTitleIsInt)
		{
			A3DUns32 uiVal;
			memcpy(&uiVal, attributeData.m_pcTitle, sizeof(A3DUns32));
			sprintf(scratch, "%u", uiVal);
			std::copy(scratch, scratch + strlen(scratch), std::back_inserter(title));
			//			utf16_title_attributes.append( H_FORMAT_TEXT("%d", (char*)(intptr_t)uiVal)); // Is this actually what the coder wanted?
		}
		else if(attributeData.m_pcTitle)
		{
			const char * s = attributeData.m_pcTitle;
			std::copy(s, s + strlen(s), std::back_inserter(title));
			//stvAppendA3DUTF8String( attributeData.m_pcTitle, utf16_title_attributes);
		}
		else {
			const char * no_title = "no Title";
			std::copy(no_title, no_title + strlen(no_title), std::back_inserter(title));
			//utf16_title_attributes.append(H_FORMAT_TEXT("no Title"));
		}

		if(attributeData.m_uiSize == 0)
		{
			const char * no_value = " = no value";
			std::copy(no_value, no_value + strlen(no_value), std::back_inserter(title));

// 			escape_user_option(title, processed);
// 			HC_Set_User_Options((const char *) processed.data());
			//utf16_title_attributes.append(H_FORMAT_TEXT("no value"));
		//	HC_Set_Unicode_Options((unsigned short*)utf16_title_attributes.encodedText());
			continue;
		}

		if(collect.capacity() < MVO_BUFFER_SIZE)
			collect.reserve(MVO_BUFFER_SIZE);

		for(unsigned int j = 0; j < attributeData.m_uiSize; j++)
		{
			collect.resize(0);

			std::copy(title.cbegin(), title.cend(), std::back_inserter(collect));

			//			H_UTF16 utf16_attributes = utf16_title_attributes;
			if(attributeData.m_asSingleAttributesData[j].m_bTitleIsInt)
			{
				A3DUns32 uiVal;
				memcpy(&uiVal, attributeData.m_asSingleAttributesData[j].m_pcTitle, sizeof(A3DUns32));
				sprintf(scratch, "/%u", uiVal);
				std::copy(scratch, scratch + strlen(scratch), std::back_inserter(collect));
				//utf16_attributes.append(H_FORMAT_TEXT("'/'%d", (char*)(intptr_t)uiVal)); // Is this actually what the coder wanted?
			}
			else if(attributeData.m_asSingleAttributesData[j].m_pcTitle)
			{
				collect.push_back('/');
				const char * s = attributeData.m_asSingleAttributesData[j].m_pcTitle;
				std::copy(s, s + strlen(s), std::back_inserter(collect));
				//				utf16_attributes.append("'/'");
				//				stvAppendA3DUTF8String( attributeData.m_pSingleAttributesData[j].m_pcTitle, utf16_attributes);
			}

			if(attributeData.m_asSingleAttributesData[j].m_pcData == NULL)
			{
// 				escape_user_option(collect, processed);
// 				HC_Set_User_Options((const char *) processed.data());
				//	HC_Set_Unicode_Options((unsigned short*)utf16_attributes.encodedText());
				continue;
			}

			collect.push_back(' ');
			collect.push_back('=');
			collect.push_back(' ');
			//utf16_attributes.append(H_FORMAT_TEXT(" = "));

			if(attributeData.m_asSingleAttributesData[j].m_eType == kA3DModellerAttributeTypeString)
			{
				const char * s = attributeData.m_asSingleAttributesData[j].m_pcData;
				std::copy(s, s + strlen(s), std::back_inserter(collect));
				//stvAppendA3DUTF8String( attributeData.m_pSingleAttributesData[j].m_pcData, utf16_attributes);
			}
			else if(attributeData.m_asSingleAttributesData[j].m_eType == kA3DModellerAttributeTypeInt)
			{
				A3DUns32 uiVal;
				memcpy(&uiVal, attributeData.m_asSingleAttributesData[j].m_pcData, sizeof(A3DUns32));
				sprintf(scratch, "%u", uiVal);
				std::copy(scratch, scratch + strlen(scratch), std::back_inserter(collect));
				//			utf16_attributes.append(H_FORMAT_TEXT("%d", uiVal));
			}
			else if(attributeData.m_asSingleAttributesData[j].m_eType == kA3DModellerAttributeTypeReal)
			{
				A3DDouble dVal;
				memcpy(&dVal, attributeData.m_asSingleAttributesData[j].m_pcData, sizeof(A3DDouble));
// 				H_FORMAT_TEXT	d_formated("%g", dVal);
// 				const char * s = d_formated.c_str();
// 				std::copy(s, s + strlen(s), std::back_inserter(collect));
				//				utf16_attributes.append(H_FORMAT_TEXT("%g", dVal));
			}
			else if(attributeData.m_asSingleAttributesData[j].m_eType == kA3DModellerAttributeTypeTime)
			{
				A3DUns32 uiVal;
				memcpy(&uiVal, attributeData.m_asSingleAttributesData[j].m_pcData, sizeof(A3DUns32));
#ifdef _MSC_VER			
				time_t time = uiVal;
				struct tm * tmTime = gmtime(&time);
				if(tmTime)
				{
					char * pcTime = asctime(tmTime);
					if(pcTime)
					{
						char * pc = strchr(pcTime, '\n');
						if(pc)
							*pc = '\0';

						std::copy(pc, pc + strlen(pc), std::back_inserter(collect));
						//utf16_title_attributes.append(pcTime);
					}
				}
#else
				{
					const char * time_error = "time reading error";
					std::copy(time_error, time_error + strlen(time_error), std::back_inserter(collect));
					//utf16_title_attributes.append(H_FORMAT_TEXT("time reading error"));
				}
#endif
			}
			else if(attributeData.m_asSingleAttributesData[j].m_eType == kA3DModellerAttributeTypeNull)
			{
				const char * null_value = "NULL";
				std::copy(null_value, null_value + strlen(null_value), std::back_inserter(collect));
				//utf16_title_attributes.append(H_FORMAT_TEXT("NULL"));
			}
			else {
				const char * unexpected_value = "unexpected value type";
				std::copy(unexpected_value, unexpected_value + strlen(unexpected_value), std::back_inserter(collect));
				//utf16_attributes.append(H_FORMAT_TEXT("unexpected value type"));
			}

// 			escape_user_option(collect, processed);
// 			HC_Set_User_Options((const char *) processed.data());

			// 			H_UTF8 utf8(utf16_attributes);
			// 			HC_Set_User_Options((char*)utf8.encodedText());
		}
		A3DMiscAttributeGet(NULL, &attributeData);
	}
	A3DRootBaseGet(NULL, &rootbaseData);

	// Getting the layer list : for smoke test N. 21598_HIO_Exchange_Layer_ifc_lost_info
	// Tung: Currently I restore the previously accepted behavior: 
	// We will set the user options to LayerList/<LayerName> = 2
	// However I think that the expected result LayerList/<LayerName> = 1,2 should be simple to implement
	A3DAsmLayer * pLayerList = NULL;
	A3DUns32 LayerNb = 0;
	A3DInt32 iRet = A3DAsmProductOccurrenceGetLayerList((A3DAsmProductOccurrence *) pEntity, &LayerNb, &pLayerList);
	if(iRet == A3D_SUCCESS && pLayerList)
	{
		for(A3DUns32 layerIdx = 0; layerIdx < LayerNb; layerIdx++)
		{
			collect.resize(0);
			const char * s = "__prc_reserved_attribute_layerslist/";
			std::copy(s, s + strlen(s), std::back_inserter(collect));
			const char * layerName = (pLayerList[layerIdx]).m_pcLayerName;
			if(layerName)
			{
				std::copy(layerName, layerName + strlen(layerName), std::back_inserter(collect));
				collect.push_back(' ');
				collect.push_back('=');
				collect.push_back(' ');
				sprintf(scratch, "%u", (pLayerList[layerIdx]).m_usLayer);
				std::copy(scratch, scratch + strlen(scratch), std::back_inserter(collect));

// 				escape_user_option(collect, processed);
// 				HC_Set_User_Options((const char *) processed.data());
			}
		}
	}
	//Clean the struct
	A3DAsmProductOccurrenceGetLayerList(NULL, &LayerNb, &pLayerList);
}