#include "stdafx.h"

#include "3DX.3DfImport.h"
#include "3DX.ExchangeWrapper.h"
//:Ken - 20240131
#include "3DX.FileOptions.h"

#include "LogManager.h"

#include <mb_matrix3d.h>

#include <3DF/Impl/SegmentImpl.h>

#include <3DF/Material.h>
#include <3DF/Portfolio.h>
#include <3DF/Style.h>
#include <3DF/Shell.h>
#include <3DF/Line.h>
#include <3DF/LineAttribute.h>
#include <3DF/Polygon.h>
#include <3DF/Bounding.h>
#include <3DF/Camera.h>

#include <3DF/Math.Matrix.h>

#include <3DF/Visibility.h>
#include <3DF/MarkerAttribute.h>
#include <3DF/Condition.h>

#include <3DF/3DF.Utility.h>

#include <Sprocket/Impl/CADModelImpl.h>
#include <Sprocket/3DF.MetaData.h>

#include <WStr.h>

#include <chrono>
#include <utility>
#include <memory>

#include <A3DSDKIncludes.h>

#include "Dmi3dx.h"

#include <Signal.h>
#include <Path.h>

#ifdef _DEBUG
#	define USED_LOG_MANAGER
#endif

//#	define USED_LOG_MANAGER

// #define A3D_DRAW_WITH_INDICES

#define DEFAULT_OFFSET {\
	pdData += *(pnStart + 1); \
	pnStart += (nCount + 1);\
}

#define MAKE_OFFSET(CountInt, CountFloats) {\
	pdData += CountFloats;\
	pnStart += (CountInt + 1);\
}

#define NOSHOW_CONDITION		"noshow"

using namespace std::chrono;

/*
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
*/

#define PI 3.1415926535897932384626433832795028841971693993751

USING_3DF_NAMESPACE
USING_3DX_NAMESPACE

TdfImport::TdfImport(_3DXSignal::Interface * pc3dxInterface) :
	ImportBase(pc3dxInterface)
{
}

TdfImport::~TdfImport()
{
	m_vcMaterialMappingStyleVector.clear();
}


bool TdfImport::FileImport(CString strFilePathName, H3DF::SegmentKey & cModelSegment, H3DF::CADModel & cInCADModel, Signal::Delivery & cInDelivery, CString & strErrorMessage)
{
	if (false == InitializeA3DLibrary(strErrorMessage)) {
		cInDelivery.progress.AddLog(Signal::Progress::Status::Fail, strErrorMessage);
		return false;
	}

	m_pcInDelivery = &cInDelivery;

	m_pcCADModel = &cInCADModel;

	m_nIncrementalId = 0;

	m_pcModelSegment = &cModelSegment;

	system_clock::time_point cTime1 = system_clock::now();
	//LONGLONG nTick1 = system_clock::now().time_since_epoch().count();

	m_strCadFileName = strFilePathName.Right(strFilePathName.GetLength() - strFilePathName.ReverseFind('\\') - 1);

	//:Ken - 20240131, sample test. remove later
#define KEN

#ifndef KEN
	// ===== TdfImport 옵션을 설정 =====
	A3DRWParamsLoadData cParamsLoadData;
	A3D_INITIALIZE_DATA(A3DRWParamsLoadData, cParamsLoadData);

	SetDefaultParamsLoadData(cParamsLoadData);

	// #option : ReadGeomTessMode
	//cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomOnly;
	//cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomAndTess;
	cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadTessOnly;
#else
	//:WARNING - do not initialize
	A3DRWParamsLoadData cParamsLoadData;
	TheFileOptions.Import.Get(strFilePathName, cParamsLoadData);
#endif

	// 이 부분을 True로 만들어야, 파일 내부에 있는 Assy File 정보와 일치하지 않는 경우도 같은 하부 Directory를 찾아서 로드한다.
	cParamsLoadData.m_sAssembly.m_bUseRootDirectory = true;

	// Report용 Callback 함수 설정
	SetCallbacksReport();

	A3DUTF8Char * pchUtf8Path = nullptr;
	if (false == CStringToUtf8(strFilePathName, pchUtf8Path)) {
		return false;
	}

	try
	{
		cParamsLoadData.m_sGeneral.m_bReadActiveFilter = false;
		A3DAsmModelFile * pcAsmModelFile = nullptr;
		A3DStatus eStatus = A3D_ERROR;
		eStatus = A3DAsmModelFileLoadFromFile(pchUtf8Path, &cParamsLoadData, &pcAsmModelFile);
		delete[] pchUtf8Path;

		if (A3D_SUCCESS != eStatus && A3D_LOAD_MULTI_MODELS_CADFILE != eStatus && A3D_LOAD_MISSING_COMPONENTS != eStatus)
		{
			CString strErrorMessage;
			Reset(strErrorMessage);

			CString strMessage;
			switch (eStatus)
			{
				case A3D_LOAD_FILE_TOO_RECENT:
					strMessage = L"The file is too recent for the current version of the software.";
					break;

				default:
					strMessage = L"Model File Load Error";
					break;
			}

			cInDelivery.progress.AddLog(Signal::Progress::Status::Fail, strMessage);
			return false;
		}
		else if (A3D_LOAD_CANNOT_LOAD_MODEL == eStatus) {
			CString strMessage;
			strMessage = L"Model File Load Error";
			cInDelivery.progress.AddLog(Signal::Progress::Status::Fail, strMessage);
			return false;
		}

		system_clock::time_point cTime2 = system_clock::now();
		auto cMilliSec1 = duration_cast<milliseconds>(cTime2 - cTime1);

		CString strMessage;
		strMessage.Format(L"Stage 1/3 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec1));
		cInDelivery.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

		cInDelivery.progress.AddLog(Signal::Progress::Status::Succeed, "Stage 2/3 : Creating Graphics Database");

		//----- Model 관련 Include 선언 -----
		SegmentKey cModelInclude = cModelSegment.Subsegment("model_include");

		SegmentKey cModels = cModelSegment.Subsegment("models");
		SegmentKey cMeasurements = cModelSegment.Subsegment("measurements");
		SegmentKey cMarkups = cModelSegment.Subsegment("markups");

		SegmentKey cIncludeSegment = cModelInclude.Subsegment("include");
		m_cModelIncludeKey = cIncludeSegment.Subsegment("model");
		m_cIncludeStyles = m_cModelIncludeKey.Subsegment("styles");

		m_cShowStyle = m_cIncludeStyles.Subsegment("show_style");
		m_cShowVertexStyle = m_cIncludeStyles.Subsegment("show_vertex_style");
		m_cNoShowStyle = m_cIncludeStyles.Subsegment("noshow_style");
		m_cNoShowVertexStyle = m_cIncludeStyles.Subsegment("noshow_vertex_style");

		m_cPoccsIncludeSegment = m_cModelIncludeKey.Subsegment("poccs");
		m_cPartsIncludeSegment = m_cModelIncludeKey.Subsegment("parts");
		m_cRisIncludeSegment = m_cModelIncludeKey.Subsegment("ris");
		m_cPmiIncludeSegment = m_cModelIncludeKey.Subsegment("pmi");

		m_cPmiIncludeSegment.SetVisibility(L"everything = off");
		m_cPmiIncludeSegment.SetHeuristics(L"exclude bounding");

		// #CADModel: CAD Model 초기화
		CString strFileTitle = Path::GetFileTitle(strFilePathName);

		H3DF::CADModelImpl * pcCdModelImpl = dynamic_cast<H3DF::CADModelImpl *>(cInCADModel.GetImpl());
		DEBUG_VALID(pcCdModelImpl);

		// CAD Model 기본이름 설정
		H3DF::ComponentImpl::SetData(cInCADModel, strFileTitle, cModelSegment.KeyValue(), INVALID_KEY);

		// Models Component 생성
		H3DF::Component * pcModelsComponent = AddComponent(cModels, L"Models", H3DF::Component::Type::ModelsComponent, cInCADModel);
		m_pcModelsComponent = pcModelsComponent;
		pcCdModelImpl->m_pcModels = pcModelsComponent;

		// Measurements Component 생성
		H3DF::Component * pcMeasurementsComponent = AddComponent(cMeasurements, L"Measurements", H3DF::Component::Type::MeasurementsComponent, cInCADModel);
		m_pcMeasurementsComponent = pcMeasurementsComponent;
		pcCdModelImpl->m_pcMeasurements = pcMeasurementsComponent;

		// Measurements Component 생성
		H3DF::Component * pcMarkupsComponent = AddComponent(cMarkups, L"Markups", H3DF::Component::Type::MarkupsComponent, cInCADModel);
		m_pcMarkups = pcMarkupsComponent;
		pcCdModelImpl->m_pcMarkups = pcMarkupsComponent;

		// Import 관련 Option 설정
		ImportOption sImportOption;

		// 처음부터 
		sImportOption.cParentSegment = cModels;

		LoadMatrixIdentity();

		MaterialMappingKit cMaterialMapping;
		cMaterialMapping.SetLineColor(RGBAColor(0, 0, 0));
		cMaterialMapping.SetEdgeColor(RGBAColor(0, 0, 0));
		cModels.SetMaterialMapping(cMaterialMapping);

		cModels.GetMarkerAttributeControl().SetSize(0.2f);

		bool bStatus = ParseModelFile(pcAsmModelFile, cModels);

		A3DAsmModelFileDelete(pcAsmModelFile);

		if (nullptr != m_pcPoints) {
			REMOVE_ARRAY(m_pcPoints);
		}

		if (nullptr != m_pcNormals) {
			REMOVE_ARRAY(m_pcNormals);
		}

		if (nullptr != m_pcTextureCoords) {
			REMOVE_ARRAY(m_pcTextureCoords);
		}

		system_clock::time_point cTime3 = system_clock::now();
		auto cMilliSec2 = duration_cast<milliseconds>(cTime3 - cTime2);

		strMessage.Format(L"Stage 2/3 : Complete [%s]", Utility::GetTimeSpanString(cMilliSec2));
		cInDelivery.progress.AddLog(Signal::Progress::Status::Succeed, strMessage);

		m_vcMaterialMappingStyleVector.clear();
		m_mMaterialMappingStyleMap.RemoveAll();
		m_mLineMaterialMappingStyleMap.clear();
		m_mMarkerMaterialMappingStyleMap.clear();

		return bStatus;
	}
	catch (...)
	{
		return false;
	}

	return false;
}

// == 3DX 설정 관련 함수 ==============================================================================
bool TdfImport::SetDefaultParamsLoadData(A3DRWParamsLoadData & cParamsLoadData)
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
	cParamsLoadData.m_sGeneral.m_eReadGeomTessMode = kA3DReadGeomOnly;// kA3DReadTessOnly;
	cParamsLoadData.m_sGeneral.m_eDefaultUnit = kA3DUnitUnknown;

	cParamsLoadData.m_sGeneral.m_bReadFeature = false; // Allows reading of model feature tree. version 10.2

	// PMI 변수
//	cParamsLoadData.m_sPmi.m_bAlwaysSubstituteFont = true;
//  cParamsLoadData.m_sPmi.m_pcSubstitutionFont = new A3DUTF8Char[sizeof("Myriad CAD")];
//	printf(cParamsLoadData.m_sPmi.m_pcSubstitutionFont, "%s", "Myriad CAD");

	// Tessellation 변수
	cParamsLoadData.m_sTessellation.m_eTessellationLevelOfDetail = kA3DTessLODMedium;
	//cParamsLoadData.m_sTessellation.m_bDoNotComputeNormalsInAccurateTessellation = true;	// Compute Normal

	// Assembly 변수
	cParamsLoadData.m_sAssembly.m_bUseRootDirectory = true;
	cParamsLoadData.m_sAssembly.m_bRootDirRecursive = true;

	// Multiple models 변수
	// cParamsLoadData.m_sMultiEntries

	// 특정 CAD Format 관련 변수
	// cParamsLoadData.m_sSpecifics

	//
	// cParamsLoadData.m_sIncremental

	return true;
}
//== 1. 3DF 변환 관련 함수 ============================================================================
bool TdfImport::ParseModelFile(const A3DAsmModelFile * pcAsmModelFile, H3DF::SegmentKey & cModelSegment)
{
	// #Import_Log : ExcuteFunction.log
#ifdef USED_LOG_MANAGER
	//LogManager::SetWriteTimeLog(2, true);

	CString strExecuteDirectory = LogManager::GetExecuteDirectory();
	LogManager::CreateFolder(strExecuteDirectory + L"\\Log");

	CString strLogFilePathName = strExecuteDirectory + L"\\Log\\3DX_ExcuteFunction.log";
	CreateLog(2, strLogFilePathName);

	//m_cA3dTracer.CreateLog(L"D:\\Temp\\A3dXInfo.log");
	//CreateLog(1, L"d:\\Temp\\AssyStruct.log");
#endif

	Log(2, L"ConvertAsmModelFile: %s, '%s'", LogHexStr((DWORD_PTR)pcAsmModelFile), m_strCadFileName);

	LogIncreaseTabIndex(2);

	// 	UINT nProcessCount = std::thread::hardware_concurrency();
	// 	m_pcThreadPoolpool = new ThreadPool(nProcessCount);

		// Model File Data를 가져오는 부분
	A3DAsmModelFileData cModelFileData;
	A3D_INITIALIZE_DATA(A3DAsmModelFileData, cModelFileData);
	A3DStatus nStatus = A3DAsmModelFileGet(pcAsmModelFile, &cModelFileData);
	if (A3D_SUCCESS != nStatus) {
		SetLastErrorMessage(L"Asm Model File Get Error", nStatus);
		return false;
	}

	if (cModelFileData.m_eModellerType == kA3DModellerIFC) {
		m_fNormalAngleCosine = static_cast<float>(cos(30 * PI / 180));
	}
	else {
		m_fNormalAngleCosine = static_cast<float>(cos(PI / 180));
	}

	// ===== Model Scale 처리 부분 =====
	// Matrix 안에 있는 Scale 값을 사용해야 한다 ( info.dModelScale은 더 이상 사용하지 않는다) [12/14/2016 이인호]
	// Model Scale을 사용해야 한다 ( POccurrence Scale을 맞지 않는다 ) [6/26/2017 이인호]
	// cImportInfo.dModelScale = cModelFileData.m_bUnitFromCAD ? cModelFileData.m_dUnit : 1.0;
	// cImportInfo.eModellerType = cModelFileData.m_eModellerType;

	double dModelScale = cModelFileData.m_bUnitFromCAD ? cModelFileData.m_dUnit : 1.0;

	m_bGlobalDataFlag = false;
	A3DGlobal * pcGlobal = nullptr;

	nStatus = A3DGlobalGetPointer(&pcGlobal);
	if (A3D_SUCCESS == nStatus) {
		A3D_INITIALIZE_DATA(A3DGlobalData, m_cGlobalData);
		nStatus = A3DGlobalGet(pcGlobal, &m_cGlobalData);
		if (A3D_SUCCESS == nStatus) {
			m_bGlobalDataFlag = true;
		}
	}

	H3DF::SegmentKey cTextureDefineSegment = cModelSegment.Subsegment("texture_define");
	// 	// Texture 관련 사항 정의
	PopulateTextures(cTextureDefineSegment);

	H3DF::MaterialMappingKit cMaterial;
	cMaterial.SetFaceColor(RGBAColor(0.752941f, 0.752941f, 0.752941f));
	cModelSegment.SetMaterialMapping(cMaterial);

	// 최초의 Attribute 생성
	A3DMiscCascadedAttributes * pcAttrs = nullptr;
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesCreate(&pcAttrs));

	A3DUns32 nSize = cModelFileData.m_uiPOccurrencesSize;
	for (A3DUns32 nIndex = 0; nIndex < nSize; ++nIndex) {
		ParseProductOccurrence(cModelFileData.m_ppPOccurrences[nIndex], pcAttrs, dModelScale, cModelSegment, *m_pcModelsComponent);
	}

	A3DAsmModelFileGet(nullptr, &cModelFileData);

	A3DMiscCascadedAttributesDelete(pcAttrs);

	A3DGlobalGet(nullptr, &m_cGlobalData);

	LogDecreaseTabIndex(2);

	Log(2, L"ConvertAsmModelFile End");

	return true;
}

// == 2. Product Occurrences 관련 함수 ===============================================================

// 2-1. Product Occurrence 처리
A3DStatus TdfImport::ParseProductOccurrence(A3DAsmProductOccurrence * pcPocc, A3DMiscCascadedAttributes * pcParentAttr, double dModelScale,
	H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp)
{
	if (nullptr == pcPocc) {
		return A3D_ERROR;
	}

	CString strPoName;
	GetName(pcPocc, strPoName);

	Log(2, L"ParseProductOccurrence: %s, pocc%d, '%s'", LogHexStr((DWORD_PTR)pcPocc), m_nIncrementalId, strPoName);

	LogIncreaseTabIndex(2);

	// Segment를 생성하고 생성된 Segment를 Parent Segment에 Include한다.
	CStringA strSegmentName;
	strSegmentName.Format("pocc%d", m_nIncrementalId++);
	H3DF::SegmentKey cSegment = m_cPoccsIncludeSegment.Subsegment(strSegmentName);
	IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

	// #CADModel: ProductOccurrence 추가
	H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, strPoName, H3DF::Component::Type::ExchangeProductOccurrence, cParentComp);

	//cSegment.Open(); // Segment를 Open하면 문제가 생김. 검토가 필요함.

	H3DF::UserData::SetSegmentName(cSegment, strPoName);

	// Attribute 생성
	// Parent에서 받은(계단식으로) Attribute를 이용해서, Attribute를 생성
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcPocc, pcParentAttr, &pcAttrs, &cAttrsData));

	if (cAttrsData.m_bShow && !cAttrsData.m_bRemoved && A3D_SUCCESS == IsShow(pcPocc))
	{
		A3DAsmProductOccurrenceData cPoccData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cPoccData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPocc, &cPoccData));

		A3DAsmProductOccurrenceData cPoccPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cPoccPrototypeData);

		if (nullptr != cPoccData.m_pPrototype) {
			A3DAsmProductOccurrenceGet(cPoccData.m_pPrototype, &cPoccPrototypeData);
		}

		if (0 < cPoccData.m_uiEntityReferenceSize || 0 < cPoccPrototypeData.m_uiEntityReferenceSize) {
			LogIncreaseTabIndex(2);
			Log(2, L"Pocc Reference Count: %d", cPoccData.m_uiEntityReferenceSize);

			LogIncreaseTabIndex(2);
			for (A3DUns32 nIndex = 0; nIndex < cPoccData.m_uiEntityReferenceSize; nIndex++) {
				A3DMiscEntityReference * pcEntityRef = cPoccData.m_ppEntityReferences[nIndex];

				A3DMiscEntityReferenceData cEntityRefData;
				A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, cEntityRefData);
				A3DMiscEntityReferenceGet(pcEntityRef, &cEntityRefData);

				A3DEEntityType eType;
				A3DEntityGetType(cEntityRefData.m_pEntity, &eType);

				m_mEntityReferenceMap.insert(std::make_pair(cEntityRefData.m_pEntity, pcEntityRef));

				Log(2, L"%d. Ref Data Entity: %s, %s", nIndex, LogHexStr((DWORD_PTR)cEntityRefData.m_pEntity), Dmi3dx::GetA3dEntityTypeString(eType));

				A3DMiscEntityReferenceGet(NULL, &cEntityRefData);
			}

			for (A3DUns32 nIndex = 0; nIndex < cPoccPrototypeData.m_uiEntityReferenceSize; nIndex++) {
				A3DMiscEntityReference * pcEntityRef = cPoccPrototypeData.m_ppEntityReferences[nIndex];

				A3DMiscEntityReferenceData cEntityRefData;
				A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, cEntityRefData);
				A3DMiscEntityReferenceGet(pcEntityRef, &cEntityRefData);

				A3DEEntityType eType;
				A3DEntityGetType(cEntityRefData.m_pEntity, &eType);

				m_mEntityReferenceMap.insert(std::make_pair(cEntityRefData.m_pEntity, pcEntityRef));

				Log(2, L"%d. Prototype Ref Data Entity: %s, %s", nIndex, LogHexStr((DWORD_PTR)cEntityRefData.m_pEntity), Dmi3dx::GetA3dEntityTypeString(eType));

				A3DMiscEntityReferenceGet(NULL, &cEntityRefData);
			}

			LogDecreaseTabIndex(2);

			LogDecreaseTabIndex(2);
		}

		if (cPoccData.m_ucBehaviour != 1)
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

			H3DF::MatrixKit cMatrix;
			if (A3D_SUCCESS == ProductOccurrenceGetLocation(&cPoccData, cMatrix)) {
				if (false == cMatrix.IsIdentity()) {
					cSegment.SetModellingMatrix(cMatrix);
				}
			}

			A3DAsmPartDefinition * pcPart = nullptr;
			CHECK_A3D_RETURN(ProductOccurrenceGetPart(&cPoccData, &pcPart));
			if (nullptr != pcPart) {

				A3DMiscEntityReference * pcEntityRef = nullptr;
				auto pcIterator = m_mEntityReferenceMap.find(pcPocc);
				if (pcIterator != m_mEntityReferenceMap.end()) {
					pcEntityRef = pcIterator->second;
				}

				CHECK_A3D_RETURN(ParsePart(pcPart, pcAttrs, dModelScale, pcEntityRef, cSegment, *pcComponent));
			}

			A3DPointerArray apcChildArray;
			PointerArrayInitialize(&apcChildArray);
			CHECK_A3D_RETURN(ProductOccurrenceGetChild(&cPoccData, &apcChildArray));

			A3DUns32 nSize = apcChildArray.m_uiSize;

			Log(2, L"ParseProductOccurrence Child Count: %d", nSize);

			for (A3DUns32 nIndex = 0; nIndex < nSize; nIndex++) {
				CHECK_A3D_RETURN(ParseProductOccurrence(apcChildArray.m_ppPointers[nIndex], pcAttrs, dModelScale, cSegment, *pcComponent));
			}

			PointerArrayTerminate(&apcChildArray);

			//m_bDrawMarkups = false;

			if (true == m_bDrawMarkups)
			{
				A3DPointerArray cMarkups, cViews;
				PointerArrayInitialize(&cMarkups);
				PointerArrayInitialize(&cViews);

				// Product Occurrence Markup 정보 수집
				// ProductOccurrenceGetMarkups(&cData, &cMarkups);
				// Product Occurrence View 정보 수집
				ProductOccurrenceGetViews(&cPoccData, &cViews);

				Log(2, L"ParseProductOccurrence View Size: %d", cViews.m_uiSize);

				nSize = cViews.m_uiSize;

				for (A3DUns32 nIndex = 0; nIndex < nSize; nIndex++) {
					CHECK_A3D_RETURN(ParseMarkupView(cViews.m_ppPointers[nIndex], pcAttrs, cSegment, *pcComponent));
				}

				Log(2, L"ParseProductOccurrence Annotations Size: %d", cPoccData.m_uiAnnotationsSize);
				ParseAnnotations(cPoccData.m_ppAnnotations, cPoccData.m_uiAnnotationsSize, cSegment, *pcComponent);

// 				nSize = cMarkups.m_uiSize;
// 				for(UINT nIndex = 0; nIndex < nSize; nIndex++) {
// 					ParseAnnotation(cMarkups.m_ppPointers[nIndex], pcAttrs, cSegment);
// 					//CHECK_A3D_RETURN(BuildMarkup(cMarkups.m_ppPointers[nIndex], pcAttrs, cSegment));
// 				}

				// PointerArrayTerminate(&cMarkups);
				PointerArrayTerminate(&cViews);
			}

			//CHECK_A3D_RETURN(A3DRootBaseGet(nullptr, &cRootBaseData));
		}

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cPoccData));
	}
	else {
		Log(2, L"Product Occurrence is not shown");
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttrs));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrsData));

	//cSegment.Close();

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 2-1-1. Assembly Product Occurence의 위치를 가져오는 함수 / ProductOccurrenceGetLocation
A3DStatus TdfImport::ProductOccurrenceGetLocation(A3DAsmProductOccurrenceData const * pcPoData, H3DF::MatrixKit & cTransMatrix)
{
	if (nullptr == pcPoData) {
		return A3D_ERROR;
	}

	A3DMiscTransformation * pcLocation = nullptr;

	bool bFlag = false;

	if (nullptr != pcPoData->m_pLocation) {
		pcLocation = pcPoData->m_pLocation;
		GetMatrix(pcLocation, cTransMatrix);
		return A3D_SUCCESS;
	}

	if (nullptr != pcPoData->m_pPrototype)
	{
		A3DAsmProductOccurrenceData cProductPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cProductPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPoData->m_pPrototype, &cProductPrototypeData));

		ProductOccurrenceGetLocation(&cProductPrototypeData, cTransMatrix);

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &cProductPrototypeData))

			return A3D_SUCCESS;
	}

	if (0 == pcPoData->m_uiPOccurrencesSize)
	{
		A3DAsmProductOccurrence * pExternal = nullptr;
		CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(pcPoData, &pExternal));

		if (nullptr != pExternal)
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
A3DStatus TdfImport::ProductOccurrenceGetLocation(const A3DAsmProductOccurrenceData * pcPOccData,
	A3DMiscCartesianTransformation ** ppcLocation)
{
	if (nullptr == pcPOccData) {
		return A3D_ERROR;
	}

	*ppcLocation = nullptr;

	if (nullptr != pcPOccData->m_pLocation)
	{
		*ppcLocation = pcPOccData->m_pLocation;
		return A3D_SUCCESS;
	}

	A3DAsmProductOccurrence * pcProductPrototype = pcPOccData->m_pPrototype;

	while (nullptr != pcProductPrototype)
	{
		A3DAsmProductOccurrenceData cProductPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cProductPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcProductPrototype, &cProductPrototypeData));

		if (nullptr != cProductPrototypeData.m_pLocation)
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

	if (0 == pcPOccData->m_uiPOccurrencesSize)
	{
		A3DAsmProductOccurrence * pExternal = nullptr;
		CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(pcPOccData, &pExternal));
		if (nullptr != pExternal)
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
A3DStatus TdfImport::ProductOccurrenceGetExternalData(const A3DAsmProductOccurrenceData * pcPOccData,
	A3DAsmProductOccurrence ** ppcExternalData)
{
	if (nullptr == pcPOccData) {
		return A3D_ERROR;
	}

	if (pcPOccData->m_pExternalData == nullptr && pcPOccData->m_pPrototype != nullptr)
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
A3DStatus TdfImport::ProductOccurrenceGetMarkups(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray)
{
	if (pcPOccData == nullptr || pcArray == nullptr) {
		return A3D_ERROR;
	}

	if (pcPOccData->m_uiAnnotationsSize == 0 && pcPOccData->m_pPrototype != nullptr) {
		A3DAsmProductOccurrenceData sPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPOccData->m_pPrototype, &sPrototypeData));

		CHECK_A3D_RETURN(ProductOccurrenceGetMarkups(&sPrototypeData, pcArray));

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &sPrototypeData));
	}
	else {
		UINT nIndex, uiSize = pcPOccData->m_uiAnnotationsSize;
		for (nIndex = 0; nIndex < uiSize; nIndex++) {
			CHECK_A3D_RETURN(AnnotationGetMarkups(pcPOccData->m_ppAnnotations[nIndex], pcArray));
		}
	}

	return A3D_SUCCESS;
}

// 2-1-3. Product Occurrence의 View를 가져옴
A3DStatus TdfImport::ProductOccurrenceGetViews(const A3DAsmProductOccurrenceData * pcPoccData, A3DPointerArray * pcArray)
{
	if (pcPoccData == nullptr || pcArray == nullptr)
		return A3D_ERROR;

	if (pcPoccData->m_uiViewsSize == 0 && pcPoccData->m_pPrototype != nullptr) {
		A3DAsmProductOccurrenceData sPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sPrototypeData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcPoccData->m_pPrototype, &sPrototypeData));

		CHECK_A3D_RETURN(ProductOccurrenceGetViews(&sPrototypeData, pcArray));

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(nullptr, &sPrototypeData));
	}
	else {
		PointerArrayAddArray(pcArray, pcPoccData->m_ppViews, pcPoccData->m_uiViewsSize);
	}

	return A3D_SUCCESS;
}

// 2-2. Product Occurrence의 Child를 가져오는 함수
A3DStatus TdfImport::ProductOccurrenceGetChild(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray)
{
	if (pcPOccData == nullptr || pcArray == nullptr)
		return A3D_ERROR;

	A3DStatus iRet = A3D_SUCCESS;

	CHECK_A3D_RETURN(ProductOccurrenceGetOccurrences(pcPOccData, pcArray));

	A3DAsmProductOccurrence * pExternal = nullptr;
	CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(pcPOccData, &pExternal));
	if (pExternal != nullptr)
	{
		A3DAsmProductOccurrenceData sExternalDataData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sExternalDataData);
		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pExternal, &sExternalDataData));

		if (pcArray->m_uiSize == 0)
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
A3DStatus TdfImport::ProductOccurrenceGetOccurrences(const A3DAsmProductOccurrenceData * pcPOccData,
	A3DPointerArray * pcArray)
{
	if (pcPOccData == nullptr || pcArray == nullptr)
		return A3D_ERROR;

	A3DStatus iRet = A3D_SUCCESS;

	if (pcPOccData->m_uiPOccurrencesSize == 0 && pcPOccData->m_pPrototype != nullptr)
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
A3DStatus TdfImport::AnnotationGetMarkups(const A3DMkpAnnotationEntity * pcAnnotation, A3DPointerArray * pcArray)
{
	A3DStatus iRet = A3D_SUCCESS;
	A3DEEntityType eType = kA3DTypeUnknown;

	CHECK_A3D_RETURN(A3DEntityGetType(pcAnnotation, &eType));

	switch (eType)
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
A3DStatus TdfImport::AnnotationSetGetMarkups(const A3DMkpAnnotationSet * pcAnnotationSet, A3DPointerArray * pcArray)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DMkpAnnotationSetData sAnnotationSetData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationSetData, sAnnotationSetData);
	CHECK_A3D_RETURN(A3DMkpAnnotationSetGet(pcAnnotationSet, &sAnnotationSetData));

	for (A3DUns32 ui = 0; ui < sAnnotationSetData.m_uiAnnotationsSize; ui++)
	{
		CHECK_A3D_RETURN(AnnotationGetMarkups(sAnnotationSetData.m_ppAnnotations[ui], pcArray));
	}

	CHECK_A3D_RETURN(A3DMkpAnnotationSetGet(nullptr, &sAnnotationSetData));

	return iRet;
}

// 2-3-2. Annotation Reference Markups을 가져옴
A3DStatus TdfImport::AnnotationReferenceGetMarkups(const A3DMkpAnnotationItem * pcAnnotationItem, A3DPointerArray * pcArray)
{
	if (pcAnnotationItem == nullptr || pcArray == nullptr) {
		return A3D_ERROR;
	}

	A3DStatus nRet = A3D_SUCCESS;

	A3DMkpAnnotationReferenceData sData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationReferenceData, sData);
	CHECK_A3D_RETURN(A3DMkpAnnotationReferenceGet(pcAnnotationItem, &sData));

	PointerArrayAddUnique(pcArray, sData.m_ppLinkedItems);

	CHECK_A3D_RETURN(A3DMkpAnnotationReferenceGet(nullptr, &sData));

	return nRet;
/*
	Log(2, "AnnotationReferenceGetMarkups: %s", LogHexStrA((DWORD_PTR)pcAnnotationItem));

	LogIncreaseTabIndex(2);

	A3DMkpAnnotationReferenceData sData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationReferenceData, sData);
	CHECK_A3D_RETURN(A3DMkpAnnotationReferenceGet(pcAnnotationItem, &sData));

	Log(2, "LinkedItemsSize: %d", sData.m_uiLinkedItemsSize);

	for (A3DUns32 nIndex = 0; nIndex < sData.m_uiLinkedItemsSize; nIndex++)
	{
		A3DMiscMarkupLinkedItem * pcLinkedItem = sData.m_ppLinkedItems[nIndex];

		A3DMiscMarkupLinkedItemData sLinkedItemData;
		A3D_INITIALIZE_DATA(A3DMiscMarkupLinkedItemData, sLinkedItemData);
		CHECK_A3D_RETURN(A3DMiscMarkupLinkedItemGet(pcLinkedItem, &sLinkedItemData));

		A3DMiscMarkupLinkedItemGet(nullptr, &sLinkedItemData);
	}

	A3DMkpAnnotationReferenceGet(nullptr, &sData);

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
*/
}

// 2-3-3. Annotation Item Markups을 가져옴
A3DStatus TdfImport::AnnotationItemGetMarkups(const A3DMkpAnnotationItem * pcAnnotationItem, A3DPointerArray * pcArray)
{
	if (pcAnnotationItem == nullptr || pcArray == nullptr) {
		return A3D_ERROR;
	}

	A3DStatus nRet = A3D_SUCCESS;

	A3DMkpAnnotationItemData sAnnotationItemData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationItemData, sAnnotationItemData);
	CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(pcAnnotationItem, &sAnnotationItemData));

	PointerArrayAddUnique(pcArray, sAnnotationItemData.m_pMarkup);

	CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(nullptr, &sAnnotationItemData));

	return nRet;
}

//== 3. Part 관련 함수 ===============================================================================

// 3-1. 현재 product의 part를 가져오는 함수 
A3DStatus TdfImport::ProductOccurrenceGetPart(const A3DAsmProductOccurrenceData * pcPOccData, A3DAsmPartDefinition ** ppcPart)
{
	if (nullptr == pcPOccData) {
		return A3D_ERROR;
	}

	*ppcPart = nullptr;

	if (nullptr != pcPOccData->m_pPart)
	{
		*ppcPart = pcPOccData->m_pPart;
		return A3D_SUCCESS;
	}

	A3DAsmProductOccurrence * pcProductPrototype = pcPOccData->m_pPrototype;
	while (nullptr != pcProductPrototype)
	{
		A3DAsmProductOccurrenceData cProductPrototypeData;
		A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, cProductPrototypeData);

		CHECK_A3D_RETURN(A3DAsmProductOccurrenceGet(pcProductPrototype, &cProductPrototypeData));

		if (cProductPrototypeData.m_pPart != nullptr)
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

	if (pcPOccData->m_uiPOccurrencesSize == 0)
	{
		A3DAsmProductOccurrence * pcExternal = nullptr;
		CHECK_A3D_RETURN(ProductOccurrenceGetExternalData(pcPOccData, &pcExternal));
		if (pcExternal != nullptr)
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
A3DStatus TdfImport::ParsePart(const A3DAsmPartDefinition * pcPart, const A3DMiscCascadedAttributes * pcParentAttr, double dModelScale, const A3DMiscEntityReference * pcEntityRef,
	H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp)
{
	Log(2, L"ParsePart: part%d", m_nIncrementalId);

	LogIncreaseTabIndex(2);

	// 기존에 생성된 Part를 찾은 경우 Include로 Parent에 추가 시킨다.
	HC_KEY nSegmentKey = INVALID_KEY;
	if (true == m_mPartsMap.Lookup((DWORD_PTR)pcPart, nSegmentKey))
	{
		H3DF::SegmentKey cSegment(nSegmentKey);
		H3DF::IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

		DWORD nIncludeCount = 0;

		if (true == H3DF::UserData::ShowIncludedCount(cSegment, nIncludeCount)) {
			nIncludeCount++;
		}
		else {
			// 저장된 Include Count가 없는 경우는 2로 설정한다. 처음 1번, 이제 다시 Include되었으므로 2번이 된다.
			nIncludeCount = 2;
		}

		H3DF::UserData::SetIncludedCount(cSegment, nIncludeCount);

		// #CADModel: ParsePart 추가. 값은 새롭게 생성해서 넣도록 한다.

		// 1. CADModel에 들어있는 Component맵에서 Key값을 이용해서 Component를 찾도록 한다.
		H3DF::CADModelImpl * pcCdModelImpl = dynamic_cast<H3DF::CADModelImpl *>(m_pcCADModel->GetImpl());
		DEBUG_VALID(pcCdModelImpl);

		H3DF::Component * pcFindComponent = nullptr;
		pcCdModelImpl->m_pmComponentMap->Lookup(nSegmentKey, pcFindComponent);

		// 2. 찾은 Component를 복사해서 새로운 Component를 생성한다. 이렇게 해야 tree에서 별도의 Component로 인식해서 UI와 연동해서 작업할 수 있음.
		//    하부의 Component들도 복사된다.
		H3DF::Component * pcComponent = new H3DF::Component(*pcFindComponent);

		// 새롭게 생성된 Component에서 Include Key를 변경한다.
		H3DF::ComponentImpl * pcComponentImpl = dynamic_cast<H3DF::ComponentImpl *>(pcComponent->GetImpl());
		pcComponentImpl->m_pcOwner = &cParentComp;
		pcComponentImpl->m_nIncludeKey = cInclude.KeyValue();

		if (nullptr != pcFindComponent) {
			H3DF::ComponentImpl::AddSubComponent(cParentComp, *pcComponent);
		}

		Log(2, L"ParsePart Map: %s", CString(cSegment.Name()));

		LogDecreaseTabIndex(2);

		return A3D_SUCCESS;
	}

	// Segment를 생성하고 생성된 Segment를 Parent Segment에 Include한다.
	SegmentKey cSegment = m_cPartsIncludeSegment.Subsegment("part%d", m_nIncrementalId++);
	H3DF::IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);
	m_mPartsMap.SetAt((DWORD_PTR)pcPart, cSegment.KeyValue());

	// #CADModel: ParsePart 추가. 값은 새롭게 생성해서 넣도록 한다.
	H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, L"", H3DF::Component::Type::ExchangePartDefinition, cParentComp);

	cSegment.Open();

	H3DF::UserData::SetComponentType(cSegment, (DWORD)H3DF::Component::Type::ExchangePartDefinition);

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcPart, pcParentAttr, &pcAttr, &cAttrData));

	if (cAttrData.m_bShow && !cAttrData.m_bRemoved && A3D_SUCCESS == IsShow(pcPart)) {
		// Show 상태에서는 별도 처리하지 않는다.
	}
	else {
		cSegment.GetStyleControl().PushSegment(m_cNoShowStyle);

		pcComponent->AddStatus(H3DF::Component::Status::Hide);
		pcComponent->AddStatus(H3DF::Component::Status::NoShow);
	}

	A3DAsmPartDefinitionData sPartDefData;
	A3D_INITIALIZE_DATA(A3DAsmPartDefinitionData, sPartDefData);
	CHECK_A3D_RETURN(A3DAsmPartDefinitionGet(pcPart, &sPartDefData));

	// 포함되어 있는 Annotation 정보를 가져온다.
#ifdef USED_LOG_MANAGER
	Log(2, L"ParsePart AnnotationsSize: %d", sPartDefData.m_uiAnnotationsSize);
#endif
	if (0 < sPartDefData.m_uiAnnotationsSize) {
		ParseAnnotations(sPartDefData.m_ppAnnotations, sPartDefData.m_uiAnnotationsSize, cSegment, cParentComp);
	}

	// 포함되어 있는 Markup View 정보를 가져온다.
#ifdef USED_LOG_MANAGER
	Log(2, L"ParsePart MarkupView Size: %d", sPartDefData.m_uiViewsSize);
#endif
	for (A3DUns32 nIndex = 0; nIndex < sPartDefData.m_uiViewsSize; nIndex++) {
		CHECK_A3D_RETURN(ParseMarkupView(sPartDefData.m_ppViews[nIndex], pcAttr, cSegment, cParentComp));
	}


#ifdef USED_LOG_MANAGER
	Log(2, L"ParsePart RepItems Size: %d", sPartDefData.m_uiRepItemsSize);
#endif
	for (A3DUns32 nIndex = 0; nIndex < sPartDefData.m_uiRepItemsSize; nIndex++) {
		CHECK_A3D_RETURN(ParseRiRepresentationItem(sPartDefData.m_ppRepItems[nIndex], pcAttr, pcEntityRef, cSegment, *pcComponent));
	}

	CHECK_A3D_RETURN(A3DAsmPartDefinitionGet(nullptr, &sPartDefData));

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	cSegment.Close();

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 2. Parse RI Representation Item
A3DStatus TdfImport::ParseRiRepresentationItem(const A3DRiRepresentationItem * pcInRepItem, const A3DMiscCascadedAttributes * pcParentAttr, const A3DMiscEntityReference * pcEntityRef,
	H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp)
{
	CString strRiName;
	GetName(pcInRepItem, strRiName);

	if (m_nIncrementalId == 956) {
		int i = 0;
	}

	CStringA strSegmentName;
	strSegmentName.Format("ri%d", m_nIncrementalId++);
	SegmentKey cSegment = m_cRisIncludeSegment.Subsegment(strSegmentName);
	IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

	A3DEEntityType eType = kA3DTypeUnknown;
	CHECK_A3D_RETURN(A3DEntityGetType(pcInRepItem, &eType));

	// #CADModel: ParseRiRepresentationItem 추가
	H3DF::Component::Type eComponentType = H3DF::Component::Type::ExchangeRepresentationItemMask;

	switch (eType)
	{
		case kA3DTypeRiSet:
			eComponentType = H3DF::Component::Type::ExchangeRISet;
			break;

		case kA3DTypeRiBrepModel: {
			// Solid, Surface 판정
			A3DRiBrepModelData cData;
			A3D_INITIALIZE_DATA(A3DRiBrepModelData, cData);
			A3DStatus nResult = A3DRiBrepModelGet(pcInRepItem, &cData);

			bool bSolidFlag = (A3D_TRUE == cData.m_bSolid) ? true : false;

			A3DRiBrepModelGet(nullptr, &cData);

			eComponentType = (true == bSolidFlag) ? H3DF::Component::Type::ExchangeRIBRepModelSolid : H3DF::Component::Type::ExchangeRIBRepModelSurface;
		} break;

		case kA3DTypeRiPolyBrepModel: {
			A3DRiPolyBrepModelData cData;
			A3D_INITIALIZE_DATA(A3DRiPolyBrepModelData, cData);
			A3DStatus nResult = A3DRiPolyBrepModelGet(pcInRepItem, &cData);

			bool bSolidFlag = (A3D_TRUE == cData.m_bIsClosed) ? true : false;

			A3DRiPolyBrepModelGet(nullptr, &cData);

			eComponentType = (true == bSolidFlag) ? H3DF::Component::Type::ExchangeRIPolyBRepModelSolid : H3DF::Component::Type::ExchangeRIPolyBRepModelSurface;
		} break;

		case kA3DTypeRiCurve:
			eComponentType = H3DF::Component::Type::ExchangeRICurve;
			break;

		case kA3DTypeRiPolyWire:
			eComponentType = H3DF::Component::Type::ExchangeRIPolyWire;
			break;

		case kA3DTypeRiPointSet:
			eComponentType = H3DF::Component::Type::ExchangeRIPointSet;
			break;

		default:
			assert(false);
			break;
	}

	H3DF::UserData::SetComponentType(cSegment, (DWORD)eComponentType);

	H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, strRiName, eComponentType, cParentComp);

	if (true == strRiName.IsEmpty()) {
		Log(2, "ParseRiRepresentationItem: %s, %s, Include: %d, Segment: %d", LogHexStrA((DWORD_PTR)pcInRepItem), strSegmentName, cInclude.KeyValue(), cSegment.KeyValue());
	}
	else {
		CStringA strText;
		strText = strRiName;
		Log(2, "ParseRiRepresentationItem: %s, %s, Include: %d, Segment: %d, '%s'", LogHexStrA((DWORD_PTR)pcInRepItem), strSegmentName, cInclude.KeyValue(), cSegment.KeyValue(), strText);
	}

	// Ri Rep에 설정된 이름을 확인해서 저장한다.
	// 저장된 이름이 없는 경우 각 요소의 특성값을 저장하도록 한다. (Surface, Curve, Point 등)

	// 외부에서 Entity References가 들어와도 Map에서 다시 검색해서 일치하는게 있는지 여부를 확인해서 적용한다.
	A3DMiscEntityReference * pcInEntityRef = nullptr;
	auto pcIterator = m_mEntityReferenceMap.find((A3DEntity *)pcInRepItem);
	if (pcIterator != m_mEntityReferenceMap.end()) {
		pcInEntityRef = pcIterator->second;
	}

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;

	if (nullptr != pcInEntityRef) {
		CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcInEntityRef, pcParentAttr, &pcAttr, &cAttrData));
	}
	else {
		CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcInRepItem, pcParentAttr, &pcAttr, &cAttrData));
	}

	MaterialMappingKit cMaterialMapping;
	A3DInt32 iUVCoordinatesIndex = -1;
	A3DUns8 ucTextureDimension = 2;

	cSegment.Open();

	A3DStatus eStatus = A3D_SUCCESS;

	// #3DX : Show / Noshw 처리
	if (cAttrData.m_bShow && !cAttrData.m_bRemoved && A3D_SUCCESS == IsShow(pcInRepItem)) {
		// Ri Point Set인 경우는 별도로 Show Vertex Style을 적용한다.
		if(kA3DTypeRiPointSet == eType) {
			cSegment.GetStyleControl().PushSegment(m_cShowVertexStyle);
		}
	}
	else {
		if (kA3DTypeRiPointSet == eType) {
			cSegment.GetStyleControl().PushSegment(m_cNoShowVertexStyle);
		}
		else {
			cSegment.GetStyleControl().PushSegment(m_cNoShowStyle);
		}

		pcComponent->AddStatus(H3DF::Component::Status::Hide);
		pcComponent->AddStatus(H3DF::Component::Status::NoShow);
	}

	A3DRiRepresentationItemData cRepItemData;
	A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, cRepItemData);
	CHECK_A3D_RETURN(A3DRiRepresentationItemGet(pcInRepItem, &cRepItemData));

	const A3DRiCoordinateSystem * pcCoordSys = nullptr;

	if (cAttrData.m_pCoordinateSystem != nullptr) {
		pcCoordSys = cAttrData.m_pCoordinateSystem;
	}
	else if (cRepItemData.m_pCoordinateSystem != nullptr) {
		pcCoordSys = cRepItemData.m_pCoordinateSystem;
	}


	if (pcCoordSys != nullptr) {
		A3DRiCoordinateSystemData sCSysData;
		A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sCSysData);
		CHECK_A3D_RETURN(A3DRiCoordinateSystemGet(pcCoordSys, &sCSysData));

		H3DF::MatrixKit cMatrix;
		GetMatrix(sCSysData.m_pTransformation, cMatrix);
		cSegment.SetModellingMatrix(cMatrix);

		Log(2, "SetModellingMatrix");

		CHECK_A3D_RETURN(A3DRiCoordinateSystemGet(nullptr, &sCSysData));
	}

	switch (eType)
	{
		case kA3DTypeRiSet:
			eStatus = ParseRiSet((A3DRiSet *)pcInRepItem, pcEntityRef, pcAttr, cSegment, *pcComponent);
			break;

		case kA3DTypeRiBrepModel:
			eStatus = ParseRiBrepModel(pcInRepItem, cRepItemData, pcEntityRef, pcAttr, cAttrData, cSegment);
			break;

		case kA3DTypeRiPolyBrepModel:
			eStatus = DrawRiPolyBrepModel(pcInRepItem, cRepItemData, pcEntityRef, pcAttr, cAttrData, cSegment);
			break;

		case kA3DTypeRiCurve:
		case kA3DTypeRiPolyWire:
			eStatus = DrawRiPolyWire(pcInRepItem, cRepItemData, pcEntityRef, pcAttr, cAttrData, cSegment);
			break;

		case kA3DTypeRiPointSet:
			eStatus = DrawRiPointSet(pcInRepItem, cSegment, pcAttr);
		break;

		default:
			assert(false);
			break;
	}

	A3DRiRepresentationItemGet(nullptr, &cRepItemData);

	if (false == strRiName.IsEmpty()) {
		H3DF::UserData::SetSegmentName(cSegment, strRiName);
	}

	cSegment.Close();

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	return eStatus;
}

// 2-2. Draw Set
A3DStatus TdfImport::ParseRiSet(const A3DRiSet * pSet, const A3DMiscEntityReference * pcEntityRef, const A3DMiscCascadedAttributes * pcParentAttr,
	H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp)
{
	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pSet, pcParentAttr, &pcAttr, &cAttrData));
	
	// #3DX : Show / Noshw 처리
	// if (cAttrData.m_bShow && !cAttrData.m_bRemoved && A3D_SUCCESS == IsShow(pSet))
	{
		A3DRiSetData sData;
		A3D_INITIALIZE_DATA(A3DRiSetData, sData);
		CHECK_A3D_RETURN(A3DRiSetGet(pSet, &sData));

		A3DUns32 ui;
		for (ui = 0; ui < sData.m_uiRepItemsSize; ui++) {
			CHECK_A3D_RETURN(ParseRiRepresentationItem(sData.m_ppRepItems[ui], pcAttr, pcEntityRef, cParentSegment, cParentComp));
		}

		CHECK_A3D_RETURN(A3DRiSetGet(nullptr, &sData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	return A3D_SUCCESS;
}

// 2-1. Draw Ri Brep Model (B-Rep Model 및 Tessellation Model도 함께 처리된다.)
A3DStatus TdfImport::ParseRiBrepModel(const A3DRiRepresentationItem * pcInRepItem, const A3DRiRepresentationItemData & cInRepItemData, const A3DMiscEntityReference * pcInEntityRef,
	const A3DMiscCascadedAttributes * pcInAttr, const A3DMiscCascadedAttributesData & cInAttrData,  H3DF::SegmentKey & cInSegment)
{
	LogIncreaseTabIndex(2);

	Log(2, L"ParseRiBrepModel: %s", LogHexStr((DWORD_PTR)pcInRepItem));

	A3DRootBaseData cRootBaseData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, cRootBaseData);
	CHECK_A3D_RETURN(A3DRootBaseGet(pcInRepItem, &cRootBaseData));
	m_pchRepresentationItemName = cRootBaseData.m_pcName;

	// 외부에서 Entity References가 들어와도 Map에서 다시 검색해서 일치하는게 있는지 여부를 확인해서 적용한다.
	auto pcIterator = m_mEntityReferenceMap.find((A3DEntity *)pcInRepItem);
	if (pcIterator != m_mEntityReferenceMap.end()) {
		pcInEntityRef = pcIterator->second;
	}

/*
	A3DRiBrepModelData cBrepModelData;
	A3D_INITIALIZE_DATA(A3DRiBrepModelData, cBrepModelData);
	A3DStatus nResult = A3DRiBrepModelGet(pcRepItem, &cBrepModelData);

	A3DRiBrepModelGet(nullptr, &cBrepModelData);
*/

/*
	// Scale을 구하기 위해서 Context Data에서 값을 가져온다.
	double m_dContextScale = 1.0;
	ParseTopoContextScale(cBrepModelData.m_pBrepData, m_dContextScale);
*/

	if (cInRepItemData.m_pTessBase != nullptr) {
		CHECK_A3D_RETURN(DrawTessBase(cInRepItemData.m_pTessBase, pcInRepItem, pcInEntityRef, pcInAttr, cInSegment));
	}
	else {
		// Tessellation이 없는 경우는 Tesselltion을 생성한다.
		A3DRiRepresentationItemData sData;
		A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, sData);

		A3DRWParamsTessellationData sTesselationData;
		A3D_INITIALIZE_DATA(A3DRWParamsTessellationData, sTesselationData);
		sTesselationData.m_eTessellationLevelOfDetail = kA3DTessLODMedium;
		CHECK_A3D_RETURN(A3DRiRepresentationItemComputeTessellation((A3DRiRepresentationItem *)pcInRepItem, &sTesselationData));
		A3DRiRepresentationItemGet((A3DRiRepresentationItem *)pcInRepItem, (A3DRiRepresentationItemData *)&cInRepItemData);

		if (cInRepItemData.m_pTessBase != nullptr) {
			CHECK_A3D_RETURN(DrawTessBase(cInRepItemData.m_pTessBase, pcInRepItem, pcInEntityRef, pcInAttr, cInSegment));
		}
	}

	m_pchRepresentationItemName = nullptr;
	CHECK_A3D_RETURN(A3DRootBaseGet(nullptr, &cRootBaseData));

	//	CHECK_A3D_RETURN(A3DRiBrepModelGet(nullptr, &cBrepModelData));

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 2-3. Draw Poly Brep Model
A3DStatus TdfImport::DrawRiPolyBrepModel(const A3DRiRepresentationItem * pcInRepItem, const A3DRiRepresentationItemData & cRepItemData, const A3DMiscEntityReference * pcInEntityRef,
	const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData, H3DF::SegmentKey & cSegment)
{
	LogIncreaseTabIndex(2);

	Log(2, L"DrawRiPolyBrepModel: %s", LogHexStr((DWORD_PTR)pcInRepItem));

	// 외부에서 Entity References가 들어와도 Map에서 다시 검색해서 일치하는게 있는지 여부를 확인해서 적용한다.
	auto pcIterator = m_mEntityReferenceMap.find((A3DEntity *)pcInRepItem);
	if (pcIterator != m_mEntityReferenceMap.end()) {
		pcInEntityRef = pcIterator->second;
	}

	A3DStatus nStatus = A3D_SUCCESS;
	if (nullptr != cRepItemData.m_pTessBase) {
		nStatus = DrawTessBase(cRepItemData.m_pTessBase, pcInRepItem, pcInEntityRef, pcAttr, cSegment);
	}

	LogDecreaseTabIndex(2);

	return nStatus;
}

// 2-3. Draw Poly Wire
A3DStatus TdfImport::DrawRiPolyWire(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, const A3DMiscEntityReference * pcInEntityRef,
	const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData, H3DF::SegmentKey & cSegment)
{
	A3DStatus nStatus = A3D_SUCCESS;
	if (nullptr != cRepItemData.m_pTessBase) {
		nStatus = DrawTessBase(cRepItemData.m_pTessBase, pcRepItem, pcInEntityRef, pcAttr, cSegment);
	}

	return nStatus;
}

A3DStatus TdfImport::DrawRiPointSet(const A3DRiRepresentationItem * pcRepItem, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	SetMarkerStyle(pcRepItem, cSegment, pcParentAttr);

	A3DRiPointSetData sData;
	A3D_INITIALIZE_DATA(A3DRiPointSetData, sData);

	A3DStatus nStatus = A3DRiPointSetGet((A3DRiPointSet *)pcRepItem, &sData);
	if (A3D_SUCCESS == nStatus) {
		// 앞쪽에서 Vertex Style을 이용해서 처리함.
		//cSegment.GetVisibilityControl().SetMarkers(true);
		//cSegment.GetVisibilityControl().SetVertices(true);

		PointArray cPointArray;
		for (unsigned int i = 0; i < sData.m_uiSize; ++i) {
			cPointArray.push_back(Point(sData.m_pPts[i].m_dX, sData.m_pPts[i].m_dY, sData.m_pPts[i].m_dZ));
		}

		H3DF::ShellKit cShellKit;
		cShellKit.SetPoints(cPointArray);

		cSegment.InsertShell(cShellKit);

		A3DRiPointSetGet(nullptr, &sData);
	}

	return nStatus;
}


// 4. Draw Ri Curve
A3DStatus TdfImport::DrawRiCurve(A3DRiCurve * pcInputRiCurve, H3DF::SegmentKey & cParentSegment, A3DMiscCascadedAttributes * pcParentAttr)
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
	if (A3D_SUCCESS != nStatus) {
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return A3D_ERROR;
	}

	if (nullptr == cRiCurveData.m_pBody) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return A3D_ERROR;
	}

	A3DTopoSingleWireBodyData cSingleWireBodyData;
	A3D_INITIALIZE_DATA(A3DTopoSingleWireBodyData, cSingleWireBodyData);
	nStatus = A3DTopoSingleWireBodyGet(cRiCurveData.m_pBody, &cSingleWireBodyData);
	if (A3D_SUCCESS != nStatus) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
		return A3D_ERROR;
	}

	if (nullptr == cSingleWireBodyData.m_pWireEdge) {
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
	if (A3D_SUCCESS != nStatus) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
		A3DTopoSingleWireBodyGet(nullptr, &cSingleWireBodyData);
		A3DMiscCascadedAttributesDelete(pcAttrs);
		A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);
	}

	if (nullptr == cWireEdgeData.m_p3dCurve) {
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
			if(false == strName.empty()) {
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

// 5. Markup View
A3DStatus TdfImport::ParseMarkupView(const A3DMkpView * pcView, const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp)
{
	A3DStatus iRet = A3D_SUCCESS;

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcView, pcParentAttr, &pcAttr, &cAttrData));

	if (/*cAttrData.m_bShow && */!cAttrData.m_bRemoved) // TODO m_bShow
	{
		A3DMkpViewData cViewData;
		A3D_INITIALIZE_DATA(A3DMkpViewData, cViewData);
		CHECK_A3D_RETURN(A3DMkpViewGet(pcView, &cViewData));

		CString strViewName;
		GetName(pcView, strViewName);

		
		H3DF::Component * pcViewGroupComponent = nullptr;

		if (false == cViewData.m_bIsAnnotationView) {
			pcViewGroupComponent = GetViewGroupComponent(cParentComp);
			if (nullptr == pcViewGroupComponent) {
				DEBUG_STOP;
				return A3D_ERROR;
			}
		}
		else {
			pcViewGroupComponent = GetAnnotationViewGroupComponent(cParentComp);
			if (nullptr == pcViewGroupComponent) {
				DEBUG_STOP;
				return A3D_ERROR;
			}
		}

		SegmentKey cViewGroupSegment(pcViewGroupComponent->GetSegmentKey());

		H3DF::SegmentKey cSegment = m_cPmiIncludeSegment.Subsegment("view%d", m_nMarkupId++);
		IncludeKey cInclude = cViewGroupSegment.IncludeSegment(cSegment);

		cSegment.GetVisibilityControl().SetEverything(false);

		// #CADModel: PMI 추가
		H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, strViewName, H3DF::Component::Type::ExchangeMkpView, *pcViewGroupComponent);

		if (nullptr != cViewData.m_pPlane) {
			A3DSurfPlaneData cData;
			A3D_INITIALIZE_DATA(A3DSurfPlaneData, cData);
			if (A3D_SUCCESS == A3DSurfPlaneGet(cViewData.m_pPlane, &cData)) {
				H3DF::MatrixKit * pcMatrix = new H3DF::MatrixKit();

				Point cOrigin;
				cOrigin.x = cData.m_sTrsf.m_sOrigin.m_dX;
				cOrigin.y = cData.m_sTrsf.m_sOrigin.m_dY;
				cOrigin.z = cData.m_sTrsf.m_sOrigin.m_dY;

				Vector cXAxis;
				cXAxis.x = cData.m_sTrsf.m_sXVector.m_dX;
				cXAxis.y = cData.m_sTrsf.m_sXVector.m_dY;
				cXAxis.z = cData.m_sTrsf.m_sXVector.m_dZ;

				Vector cYAxis;
				cYAxis.x = cData.m_sTrsf.m_sYVector.m_dX;
				cYAxis.y = cData.m_sTrsf.m_sYVector.m_dY;
				cYAxis.z = cData.m_sTrsf.m_sYVector.m_dZ;

				Vector cZAxis = cXAxis.Cross(cYAxis);

				pcMatrix->SetOrigin(cOrigin);
				pcMatrix->SetXAxis(cXAxis);
				pcMatrix->SetYAxis(cYAxis);
				pcMatrix->SetZAxis(cZAxis);

				H3DF::DwordPtrMetaData * pcMetaData = new H3DF::DwordPtrMetaData(H3DF::MetaDataIndex::Camera, (DWORD_PTR)pcMatrix);
				pcComponent->AddMetaData(pcMetaData);
			}

			//CString strJsonText = Dmi3dx::GetJsonString(cViewData.m_pPlane);
// 			H3DF::StringMetaData * pcMetaData = new H3DF::StringMetaData(H3DF::MetaDataIndex::PlaneString, strJsonText);
// 			pcComponent->AddMetadata(pcMetaData, true);

			H3DF::CameraKit * pcCamera = new H3DF::CameraKit();
			
/*
			pcCamera->SetTarget()

			Point cPosition = pcImpl->cPosition;
			Point cTarget = pcImpl->cTarget;
			Vector cViewNormal = cPosition - cTarget;
			cViewNormal.Normalize();

			Vector cYAxis = pcImpl->cUpVector;
			cYAxis.Normalize();

			Vector cXAxis = cYAxis.Cross(cViewNormal);

			cMatrix[0][0] = cXAxis.x;
			cMatrix[0][1] = cXAxis.y;
			cMatrix[0][2] = cXAxis.z;

			cMatrix[1][0] = cYAxis.x;
			cMatrix[1][1] = cYAxis.y;
			cMatrix[1][2] = cYAxis.z;

			cMatrix[2][0] = cViewNormal.x;
			cMatrix[2][1] = cViewNormal.y;
			cMatrix[2][2] = cViewNormal.z;

			cMatrix[3][0] = cTarget.x;
			cMatrix[3][1] = cTarget.y;
			cMatrix[3][2] = cTarget.z;
*/

		}

#ifdef USED_LOG_MANAGER
		LogMkpViewData((A3DMkpView *)pcView, cViewData);
		LogIncreaseTabIndex(2);
		Log(2, "Annotations Size: %d", cViewData.m_uiAnnotationsSize);
#endif

		for (A3DUns32 nIndex = 0; nIndex < cViewData.m_uiAnnotationsSize; nIndex++) {
			CHECK_A3D_RETURN(ParseAnnotation(cViewData.m_ppAnnotations[nIndex], pcAttr, cSegment, *pcComponent, true));
		}

#ifdef USED_LOG_MANAGER
		LogDecreaseTabIndex(2);
#endif

		CHECK_A3D_RETURN(A3DMkpViewGet(nullptr, &cViewData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	return iRet;
}

// 5-1. View를 Group으로 처리하기 위해서, Parent Component에서 View Group을 검색하고, 없으면 생성한다.
// Segment도 "grpview"로 생성한다.
H3DF::Component * TdfImport::GetViewGroupComponent(H3DF::Component & cInParentComp)
{
	H3DF::ComponentArray * paSubComponentArray = cInParentComp.GetSubComponents();

	if (nullptr != paSubComponentArray) {
		for (auto * pcComponent : *paSubComponentArray) {
			if (H3DF::Component::Type::ViewGroupComponent == pcComponent->GetType()) {
				return pcComponent;
			}
		}
	}

	// Parent Component에 PMI Group을 생성.
	SegmentKey cParentSegment(cInParentComp.GetSegmentKey());

	SegmentKey cSegment = m_cPmiIncludeSegment.Subsegment("grpview%d", m_nMarkupId++);
	IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

	H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, "View", H3DF::Component::Type::ViewGroupComponent, cInParentComp);
	return pcComponent;
}

// 5-2. View중에서 Annotation View를 Group으로 처리하기 위해서, Parent Component에서 Annotation View Group을 검색하고, 없으면 생성한다.
// Segment도 "anogrpview"로 생성한다.
H3DF::Component * TdfImport::GetAnnotationViewGroupComponent(H3DF::Component & cInParentComp)
{
	H3DF::Component * pcViewGroupComp = GetViewGroupComponent(cInParentComp);
	if (nullptr == pcViewGroupComp) {
		DEBUG_STOP;
		return nullptr;
	}

	H3DF::ComponentArray * paSubComponentArray = pcViewGroupComp->GetSubComponents();

	if (nullptr != paSubComponentArray) {
		for (auto * pcComponent : *paSubComponentArray) {
			if (H3DF::Component::Type::AnnotationViewGroupComponent == pcComponent->GetType()) {
				return pcComponent;
			}
		}
	}

	// Parent Component에 PMI Group을 생성.
	SegmentKey cParentSegment(pcViewGroupComp->GetSegmentKey());

	SegmentKey cSegment = m_cPmiIncludeSegment.Subsegment("anogrpview%d", m_nMarkupId++);
	IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

	H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, "Annotation View", H3DF::Component::Type::AnnotationViewGroupComponent, *pcViewGroupComp);
	return pcComponent;
}


// 6. 복수의 Annotation을 그리는 함수
A3DStatus TdfImport::ParseAnnotations(A3DMkpAnnotationEntity ** pcAnnotation, A3DUns32 nAnnotationsSize, H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp)
{
	if (0 == nAnnotationsSize) {
		return A3D_SUCCESS;
	}

	LogIncreaseTabIndex(2);

	A3DMiscCascadedAttributes * pcAttr = nullptr;
	A3DMiscCascadedAttributesCreate(&pcAttr);

	for (unsigned int i = 0; i < nAnnotationsSize; i++) {
		ParseAnnotation(pcAnnotation[i], pcAttr, cParentSegment, cParentComp, false);
	}

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 6-1. Annotation을 그리는 함수
A3DStatus TdfImport::ParseAnnotation(const A3DMkpAnnotationEntity * pcAnnotation, A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp, bool bAddChildToParentComp)
{
	A3DEEntityType eType;
	A3DEntityGetType(pcAnnotation, &eType);

	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrData;
	CreateAndPushCascadedAttributes(pcAnnotation, pcParentAttr, &pcAttrs, &cAttrData);

#ifdef USED_LOG_MANAGER
	Log(2, "ParseAnnotation: %s, %s", LogHexStrA((DWORD_PTR)pcAnnotation), Dmi3dx::GetA3dEntityTypeString(eType));
#endif

	LogIncreaseTabIndex(2);

	switch (eType)
	{
		case kA3DTypeMkpAnnotationItem:
		{
			A3DMkpAnnotationItemData sData;
			A3D_INITIALIZE_DATA(A3DMkpAnnotationItemData, sData);
			A3DMkpAnnotationItemGet(pcAnnotation, &sData);

			A3DMiscCascadedAttributes * pcMarkupAttr;
			A3DMiscCascadedAttributesData sMarkupAttribData;
			CreateAndPushCascadedAttributes(sData.m_pMarkup, pcAttrs, &pcMarkupAttr, &sMarkupAttribData);

			ParseMarkup(sData.m_pMarkup, &sMarkupAttribData, cParentSegment, cParentComp, bAddChildToParentComp);

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

		case kA3DTypeMkpAnnotationSet: {
			A3DMkpAnnotationSetData sData;
			A3D_INITIALIZE_DATA(A3DMkpAnnotationSetData, sData);
			CHECK_A3D_RETURN(A3DMkpAnnotationSetGet(pcAnnotation, &sData));

			for (A3DUns32 i = 0; i < sData.m_uiAnnotationsSize; ++i) {
				ParseAnnotation(sData.m_ppAnnotations[i], pcAttrs, cParentSegment, cParentComp, bAddChildToParentComp);
			}

			A3DMkpAnnotationSetGet(nullptr, &sData);
		}
		break;

		case kA3DTypeMkpAnnotationReference: {
			A3DMkpAnnotationReferenceData cData;
			A3D_INITIALIZE_DATA(A3DMkpAnnotationReferenceData, cData);
			CHECK_A3D_RETURN(A3DMkpAnnotationReferenceGet(pcAnnotation, &cData));

			Log(2, "Linked Items Size: %d", cData.m_uiLinkedItemsSize);

			for (A3DUns32 nIndex = 0; nIndex < cData.m_uiLinkedItemsSize; nIndex++) {
				ParseMarkupLinkedItem(cData.m_ppLinkedItems[nIndex], pcAttrs, cParentSegment, cParentComp);
			}

			CHECK_A3D_RETURN(A3DMkpAnnotationReferenceGet(nullptr, &cData));
		} break;

		default:
			break;
	}

	A3DMiscCascadedAttributesDelete(pcAttrs);

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}


// 6-1. Draw Annotation Set
A3DStatus TdfImport::DrawAnnotationSet(const A3DMkpAnnotationSet * pcAnnotationSet, H3DF::SegmentKey & cParentSegment,
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
A3DStatus TdfImport::DrawAnnotationReference(const A3DMkpAnnotationItem * /*pcAnnotationItem*/, const A3DMiscCascadedAttributes * /*pcParentAttr*/)
{
	assert(false);
	return A3D_SUCCESS;
}

// 6-3. Draw Annotation Item
A3DStatus TdfImport::ParseAnnotationItem(const A3DMkpAnnotationItem * pcAnnotationItem, const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cParentSegment)
{
	//A3DStatus iRet = A3D_SUCCESS;

	A3DMiscCascadedAttributes * pcAttr;
	A3DMiscCascadedAttributesData cAttrData;
	CHECK_A3D_RETURN(CreateAndPushCascadedAttributes(pcAnnotationItem, pcParentAttr, &pcAttr, &cAttrData));

	MaterialKit cMaterial;
	GetMaterial(cAttrData.m_sStyle, cMaterial);

	if (cAttrData.m_bShow && !cAttrData.m_bRemoved)
	{
		A3DMkpAnnotationItemData sAnnotationItemData;
		A3D_INITIALIZE_DATA(A3DMkpAnnotationItemData, sAnnotationItemData);
		CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(pcAnnotationItem, &sAnnotationItemData));

		//CHECK_A3D_RETURN(DrawMarkup(sAnnotationItemData.m_pMarkup, cParentSegment, pcAttr));

		CHECK_A3D_RETURN(A3DMkpAnnotationItemGet(nullptr, &sAnnotationItemData));
	}

	CHECK_A3D_RETURN(A3DMiscCascadedAttributesDelete(pcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(nullptr, &cAttrData));

	return A3D_SUCCESS;
}

// 6-4. Markup Linked Item을 처리하는 함수
A3DStatus TdfImport::ParseMarkupLinkedItem(A3DMiscMarkupLinkedItem * pcInLinkedItems, A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp)
{
	A3DMiscMarkupLinkedItemData cData;
	A3D_INITIALIZE_DATA(A3DMiscMarkupLinkedItemData, cData);
	CHECK_A3D_RETURN(A3DMiscMarkupLinkedItemGet(pcInLinkedItems, &cData));

	A3DMiscEntityReferenceData cRefData;
	A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, cRefData);
	A3DMiscEntityReferenceGet(pcInLinkedItems, &cRefData);

	LogMarkupLinkedItem(cData, cRefData, (DWORD_PTR)pcInLinkedItems);

	A3DMiscMarkupLinkedItemGet(nullptr, &cData);
	A3DMiscEntityReferenceGet(nullptr, &cRefData);

	return A3D_SUCCESS;
}

// 7. Markup Data를 전체적으로 가져오는 부분
A3DStatus TdfImport::ParseMarkup(const A3DMkpMarkup * pcMarkup, A3DMiscCascadedAttributesData * psAttribData, H3DF::SegmentKey & cParentSegment, H3DF::Component & cParentComp, bool bAddChildToParentComp)
{
	A3DMkpMarkupData sData;
	A3D_INITIALIZE_DATA(A3DMkpMarkupData, sData);

	CString strPmiName;
	GetName(pcMarkup, strPmiName);

	H3DF::Component * pcPmiGroupComponent = GetPmiGroupComponent(cParentComp);
	if (nullptr == pcPmiGroupComponent) {
		DEBUG_STOP;
		return A3D_ERROR;
	}

	// 이미 추가되어 있는 경우에는 Parent Component에 추가한다. Markup View에 추가하는 경우가 대부분이다.
	HC_KEY nSegmentKey = INVALID_KEY;
	if (true == m_mMarkupsMap.Lookup((DWORD_PTR)pcMarkup, nSegmentKey))
	{
		Log(2, L"LookUp ParseMarkup: %s, '%s'", LogHexStr((DWORD_PTR)pcMarkup), strPmiName);

		H3DF::SegmentKey cSegment(nSegmentKey);
		H3DF::IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

		// Markup View등에서 생성된 경우를 대응하기 위해서 Parent Component에 추가한다.
		if (true == bAddChildToParentComp) {
			H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, strPmiName, H3DF::Component::Type::ExchangePMI, cParentComp);
		}

		return A3D_SUCCESS;
	}

	Log(2, L"ParseMarkup: %s, '%s', Markup%d", LogHexStr((DWORD_PTR)pcMarkup),  strPmiName, m_nMarkupId);

	LogManager::IncreaseTabIndex(2);

	// PMI는 PMI Group에 추가하도록 한다.
	SegmentKey cPmiGroupSegment(pcPmiGroupComponent->GetSegmentKey());
	H3DF::SegmentKey cSegment = m_cPmiIncludeSegment.Subsegment("pmi%d", m_nMarkupId++);
	IncludeKey cInclude = cPmiGroupSegment.IncludeSegment(cSegment);
	m_mMarkupsMap.SetAt((DWORD_PTR)pcMarkup, cSegment.KeyValue());

	// #CADModel: PMI 추가
	H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, strPmiName, H3DF::Component::Type::ExchangePMI, *pcPmiGroupComponent);

	// Markup View등에서 생성된 경우를 대응하기 위해서 Parent Component에 추가한다.
	if (true == bAddChildToParentComp) {
		AddComponent(cSegment, cInclude, strPmiName, H3DF::Component::Type::ExchangePMI, cParentComp);
	}

	if (A3D_FALSE == psAttribData->m_bShow || A3D_TRUE == psAttribData->m_bRemoved) {
		cSegment.GetStyleControl().PushSegment(m_cNoShowStyle);

		pcComponent->AddStatus(H3DF::Component::Status::Hide);
		pcComponent->AddStatus(H3DF::Component::Status::NoShow);
	}

	A3DMkpMarkupGet(pcMarkup, &sData);

	H3DF::PMI::Entity * pcEntity = nullptr;

	switch (sData.m_eType)
	{
		case kA3DMarkupTypeDatum:
		{
			pcEntity = new H3DF::PMI::DatumEntity(cSegment);
			H3DF::PMI::DatumEntity * pcDatum = (PMI::DatumEntity *)pcEntity;

			switch (sData.m_eSubType)
			{
				case kA3DMarkupSubTypeDatumIdent:
					pcDatum->SetDatumType(PMI::Datum::Type::Identifier);
					break;

				case kA3DMarkupSubTypeDatumTarget:
					pcDatum->SetDatumType(PMI::Datum::Type::Target);
					break;

				default:
					pcDatum->SetDatumType(PMI::Datum::Type::Unknown);
			}
		}
		break;

		case kA3DMarkupTypeDimension:
		{
			pcEntity = new H3DF::PMI::DimensionEntity(cSegment);
			PMI::DimensionEntity * pcDimension = (PMI::DimensionEntity *)pcEntity;

			pcDimension->SetDimensionType(PMI::Dimension::Type::UnknownType);

			switch (sData.m_eSubType)
			{
				case kA3DMarkupSubTypeDimensionAngle:
					pcDimension->SetDimensionSubType(PMI::Dimension::SubType::AngleSubType);
					break;

				case kA3DMarkupSubTypeDimensionChamfer:
					pcDimension->SetDimensionSubType(PMI::Dimension::SubType::ChamferSubType);
					break;

				case kA3DMarkupSubTypeDimensionDiameter:
				case kA3DMarkupSubTypeDimensionDiameterCone:
				case kA3DMarkupSubTypeDimensionDiameterCylinder:
				case kA3DMarkupSubTypeDimensionDiameterEdge:
				case kA3DMarkupSubTypeDimensionDiameterTangent:
					pcDimension->SetDimensionSubType(PMI::Dimension::SubType::DiameterSubType);
					break;

				case kA3DMarkupSubTypeDimensionDistance:
				case kA3DMarkupSubTypeDimensionDistanceCumulate:
				case kA3DMarkupSubTypeDimensionDistanceOffset:
					pcDimension->SetDimensionSubType(PMI::Dimension::SubType::DistanceSubType);
					break;

				case kA3DMarkupSubTypeDimensionLength:
				case kA3DMarkupSubTypeDimensionLengthCircular:
				case kA3DMarkupSubTypeDimensionLengthCurvilinear:
					pcDimension->SetDimensionSubType(PMI::Dimension::SubType::LengthSubType);
					break;

				case kA3DMarkupSubTypeDimensionRadius:
				case kA3DMarkupSubTypeDimensionRadiusCylinder:
				case kA3DMarkupSubTypeDimensionRadiusEdge:
				case kA3DMarkupSubTypeDimensionRadiusTangent:
					pcDimension->SetDimensionSubType(PMI::Dimension::SubType::RadiusSubType);
					break;

				case kA3DMarkupSubTypeDimensionSlope:
					pcDimension->SetDimensionSubType(PMI::Dimension::SubType::SlopeSubType);
					break;

				default:
					pcDimension->SetDimensionSubType(PMI::Dimension::SubType::UnknownSubType);
					break;
			}
		}
		break;

		case kA3DMarkupTypeGdt:
		{
			// current data coming out of 3DX for FCFs makes it quite difficult to accurately determine components
			// of a FCF, so we just insert a generic entity (just text strings and polylines, no extra information)
			pcEntity = new PMI::GenericEntity(cSegment);
		}
		break;

		case kA3DMarkupTypeRoughness:
		{
			pcEntity = new PMI::RoughnessEntity(cSegment);

			PMI::RoughnessEntity * pcRoughness = (PMI::RoughnessEntity *)pcEntity;

			pcRoughness->SetObtentionType(PMI::Roughness::Obtention::Type::Unknown);
			pcRoughness->SetApplicabilityType(PMI::Roughness::Applicability::Type::Unknown);
			pcRoughness->SetModeType(PMI::Roughness::Mode::Type::Unknown);
		}
		break;

		case kA3DMarkupTypeText:
		{
			pcEntity = new PMI::NoteEntity(cSegment);
		}
		break;

		default:
		{
			pcEntity = new PMI::GenericEntity(cSegment);
		}
	}

	PolylineArray cLeaderLines;
	PolygonArray cLeaderSymbols;

	for (unsigned int i = 0; i < sData.m_uiLeadersSize; i++) {
		GetLeaderLinesAndSymbols(sData.m_ppLeaders[i], cLeaderLines, cLeaderSymbols);
	}

	if (0 < cLeaderLines.size()) {
		pcEntity->SetLeaderLines((unsigned int)cLeaderLines.size(), cLeaderLines.data());
	}

	if (0 < cLeaderSymbols.size()) {
		pcEntity->SetLeaderSymbols((unsigned int)cLeaderSymbols.size(), cLeaderSymbols.data());
	}

/*
	if (GetDLLVersion() >= 202)
	{
		for (unsigned int j = 0; j < sData.m_uiLinkedItemsSize; j++)
		{
			A3DMiscMarkupLinkedItemData sData2;
			A3D_INITIALIZE_DATA(A3DMiscMarkupLinkedItemData, sData2);

			A3DMiscMarkupLinkedItemGet(sData.m_ppLinkedItems[j], &sData2);

			if (sData2.m_pReference)
				treatReference(sData2.m_pReference, pMarkup);

			A3DMiscMarkupLinkedItemGet(NULL, &sData2);
		}
	}
*/

	A3DTessBaseData sBaseData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, sBaseData);

	A3DTessBaseGet(sData.m_pTessellation, &sBaseData);

	A3DMkpMarkupGet(nullptr, &sData);

	A3DTessMarkupData sMarkupData;
	A3D_INITIALIZE_DATA(A3DTessMarkupData, sMarkupData);

	A3DTessMarkupGet(sData.m_pTessellation, &sMarkupData);

	PolylineArray aPolyline;
	StringArray aStrings;
	PMI::TextAttributesArray aTextAttributes;
	PMI::Options cOptions;
	PolygonArray aPolygons;

	// Tesselation관련 정보를 수집. Segment에 정보를 추가하거나 하지 않는다.
	GetMarkupTesselation(&sBaseData, &sMarkupData, aPolyline, aPolygons, aStrings, aTextAttributes, &cOptions);

	A3DTessMarkupGet(nullptr, &sMarkupData);
	A3DTessBaseGet(nullptr, &sBaseData);

	if (false == aPolyline.empty())
	{
		PMI::Frame cFrame;
		cFrame.SetPolylines((unsigned int)aPolyline.size(), aPolyline.data());
		if (nullptr != pcEntity) {
			// Frame Data를 Segement에 추가
			pcEntity->SetFrame(cFrame);
		}
	}

	if (false == aPolygons.empty())
	{
		PMI::Drawing cDrawing;
		cDrawing.SetPolygons((unsigned int)aPolygons.size(), aPolygons.data());
		// Polygon Data를 Segement에 추가
		pcEntity->SetDrawing(cDrawing);
	}

	assert(aStrings.size() == aTextAttributes.size());
	unsigned int nCount = (unsigned int)aStrings.size();


	switch (pcEntity->GetType())
	{
		case PMI::Type::DatumType:
		{
			PMI::DatumEntity * pcDatum = (PMI::DatumEntity *)pcEntity;
			pcDatum->SetDisplayParallelToScreen(cOptions.IsDisplayParallelToScreen());
			if (0 < nCount) {
				pcDatum->SetLabels(nCount, aStrings.data(), aTextAttributes.data());
			}
		}
		break;

		case PMI::Type::DimensionType:
		{
			PMI::DimensionEntity * pcDimension = (PMI::DimensionEntity *)pcEntity;

			pcDimension->SetDisplayParallelToScreen(cOptions.IsDisplayParallelToScreen());
			if (0 < nCount) {
				pcDimension->SetStrings(nCount, aStrings.data(), aTextAttributes.data());
			}
		}
		break;

		case PMI::Type::GenericType:
		{
			PMI::GenericEntity * pcGeneric = (PMI::GenericEntity *)pcEntity;

			if (0 < nCount) {
				pcGeneric->SetStrings(nCount, aStrings.data(), aTextAttributes.data());
			}
		}
		break;

		case PMI::Type::NoteType:
		{
			PMI::NoteEntity * pcNote = (PMI::NoteEntity *)pcEntity;

			pcNote->SetDisplayParallelToScreen(cOptions.IsDisplayParallelToScreen());
			if (0 < nCount) {
				pcNote->SetStrings(nCount, aStrings.data(), aTextAttributes.data());
			}
		}
		break;

		case PMI::Type::RoughnessType:
		{
			PMI::RoughnessEntity * pcRoughness = (PMI::RoughnessEntity *)pcEntity;

			if (0 < nCount) {
				pcRoughness->SetFields(nCount, aStrings.data(), aTextAttributes.data());
			}

		} break;

		default:
			break;
	}

	//	m_pmi_entities.push_back(*pcEntity);


	if (nullptr != pcEntity) {
		delete pcEntity;
	}

	LogManager::DecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 7-1. Mark up Tesselation 처리
A3DStatus TdfImport::GetMarkupTesselation(const A3DTessBaseData * psTessBaseData, const A3DTessMarkupData * psTessMarkupData,
	PolylineArray & aOutPolylines, PolygonArray & aOutPolygones, StringArray & aOutStrings, PMI::TextAttributesArray & cOutTextAttributes,
	PMI::Options * pcOutPmiOptions)
{
	Log(2, "ParseMarkupTesselation: %s, %s", LogHexStr((DWORD_PTR)psTessBaseData), LogHexStr((DWORD_PTR)psTessMarkupData));

	if (psTessMarkupData->m_uiCodesSize == 0) {
		return A3D_ERROR;
	}

	unsigned int nCount = 0;

	const A3DDouble * pdData = psTessBaseData->m_pdCoords;
	const A3DUns32 * pnStart = &psTessMarkupData->m_puiCodes[0];
	const A3DUns32 * pnEndCodes = &psTessMarkupData->m_puiCodes[psTessMarkupData->m_uiCodesSize - 1];

	if (nullptr == pdData || nullptr == pnStart || nullptr == pnEndCodes) {
		return A3D_ERROR;
	}

	LogIncreaseTabIndex(2);

	A3DFontKeyData sFontKeyData;
	A3D_INITIALIZE_DATA(A3DFontKeyData, sFontKeyData);

	A3DGraphRgbColorData sRgbColorData;
	A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);

	A3DGraphVPicturePatternData sPicturePatternData;
	A3D_INITIALIZE_DATA(A3DGraphVPicturePatternData, sPicturePatternData);

	RGBColor cColor(1, 1, 1);
	PMI::TextAttributes cTextAttributes;
	// 	PMI::Options options;

	bool bFrameDrawMode = false;
	bool bFaceViewMode = false;
	// 	H_UTF8 pmi_utf8;
	Point cTextMove;
	float char_width = 0.;
	float char_height = 1.;
	H3DF::MatrixKit cMatrix;
	H3DF::MatrixKit cTransformMatrix;
	//FloatArray pline;
	CStringA strLinePattern;


	for (; pnStart < pnEndCodes; ++pnStart)
	{
		nCount = *pnStart & kA3DMarkupIntegerMask;

		if (*pnStart & kA3DMarkupIsExtraData)
		{
			switch (A3D_DECODE_EXTRA_DATA(*pnStart))
			{
				case A3D_DECODE_EXTRA_DATA(kA3DMarkupPatternMask): //0:  //pattern
					DEFAULT_OFFSET;
					break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupPictureMask): // 1:  //picture
					DEFAULT_OFFSET;
					break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupTrianglesMask): // 2:  //triangles
				{
					unsigned int triangleCount = *(pnStart + 1) / 9;
					for (unsigned int i = 0; i < triangleCount; ++i)
					{
						H3DF::Point cPoints[3];
						for (UINT j = 0; j < 3; ++j)
						{
							cPoints[j].x = static_cast<float>(pdData[9 * i + 3 * j + 0]);
							cPoints[j].y = static_cast<float>(pdData[9 * i + 3 * j + 1]);
							cPoints[j].z = static_cast<float>(pdData[9 * i + 3 * j + 2]);
						}

						if (false == cTransformMatrix.IsIdentity()) {
							for (UINT j = 0; j < 3; ++j) {
								cPoints[j] = cTransformMatrix.Transform(cPoints[j]);
							}
						}

						H3DF::PolygonKit cPolygon;
						cPolygon.SetPoints(3, cPoints);
						cPolygon.SetRGBColor(cColor);
						aOutPolygones.push_back(cPolygon);
					}

					DEFAULT_OFFSET;
				}
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupQuadsMask):
					DEFAULT_OFFSET;
					break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupFaceViewMask): // 6 :  // Camera Dependant -  Faceview
				{
					if (*(pnStart + 1) > 0) {
						bFaceViewMode = true;

						if (nullptr != pcOutPmiOptions) {
							pcOutPmiOptions->SetDisplayParallelToScreen();
						}

						H3DF::MatrixKit cMatrix;
						cMatrix[3][0] = static_cast<float>(pdData[0]);
						cMatrix[3][1] = static_cast<float>(pdData[1]);
						cMatrix[3][2] = static_cast<float>(pdData[2]);

						PMI::Orientation cOrientation;
						cOrientation.SetMatrix(cMatrix);

						cTextAttributes.SetOrientation(cOrientation);

						MAKE_OFFSET(0, 3);
					}
					else {
						bFaceViewMode = false;

						MAKE_OFFSET(0, 0);
					}
				}
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupFrameDrawMask): // 7 :  // CameraDependant  -  Frame draw
				{
					if (*(pnStart + 1) > 0)
					{
						bFrameDrawMode = true;

						if (nullptr != pcOutPmiOptions) {
							pcOutPmiOptions->SetDisplayParallelToScreen();
						}

						H3DF::MatrixKit cMatrix;
						cMatrix[3][0] = static_cast<float>(pdData[0]);
						cMatrix[3][1] = static_cast<float>(pdData[1]);
						cMatrix[3][2] = static_cast<float>(pdData[2]);

						PMI::Orientation cOrientation;
						cOrientation.SetMatrix(cMatrix);
						cTextAttributes.SetOrientation(cOrientation);
						MAKE_OFFSET(0, 3);
					}
					else
					{
						bFrameDrawMode = false;
						MAKE_OFFSET(0, 0);
					}
				}
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupFixedSizeMask): // 8 :  // CameraDependant  -  Fixedsize
				{
					if (*(pnStart + 1) > 0)
					{
						bool m_bFixedSize = true;
						double dViewDependantParameter[3];
						dViewDependantParameter[0] = *pdData;
						dViewDependantParameter[1] = *(pdData + 1);
						dViewDependantParameter[2] = /*bIsText ? 0 : */*(pdData + 2);
						UINT nFixedSizePatternIndex = *(pnStart + 2);
						MAKE_OFFSET(0, 3);
					}
					else {
						MAKE_OFFSET(0, 0);
					}
					// this defines the size of an object which has a view independent size
					DEFAULT_OFFSET;
				} 
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupSymbolMask): // 9 :  // Camera Dependant -  Symbol
				{
					A3DGlobalGetGraphVPicturePatternData(*(pnStart + 2), &sPicturePatternData);
					A3DTessBaseData sBaseData;
					A3D_INITIALIZE_DATA(A3DTessBaseData, sBaseData);
					A3DTessBaseGet(sPicturePatternData.m_pMarkupTess, &sBaseData);

					A3DTessMarkupData spMarkupData;
					A3D_INITIALIZE_DATA(A3DTessMarkupData, spMarkupData);

					A3DTessMarkupGet(sPicturePatternData.m_pMarkupTess, &spMarkupData);
					GetMarkupTesselation(&sBaseData, &spMarkupData, aOutPolylines, aOutPolygones, aOutStrings, cOutTextAttributes, pcOutPmiOptions);
					A3DTessBaseGet(NULL, &sBaseData);
					A3DTessMarkupGet(NULL, &spMarkupData);
					// this is an image which will be texture mapped to a 3d quad
					DEFAULT_OFFSET;
				}
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupCylinderMask): // 10 : cylinder
					DEFAULT_OFFSET;
					break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupColorMask): // 11 : color
				{
					A3DGlobalGetGraphRgbColorData(*(pnStart + 2), &sRgbColorData);
					cColor.Set(static_cast<float>(sRgbColorData.m_dRed),
						static_cast<float>(sRgbColorData.m_dGreen),
						static_cast<float>(sRgbColorData.m_dBlue));
					DEFAULT_OFFSET;
				}
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupLineStippleMask): // 12 : line stipple
				{
// 					if (0 < nCount) {
// 						strLinePattern = GetLinePattern(*(pnStartCodes + 2));
// 						H3DF::SegmentKey cStyleSegment = m_cIncludeStyles.Subsegment("line_mat_%d", cAttrData.m_sStyle.m_uiRgbColorIndex);
// 					}

					DEFAULT_OFFSET;
				}
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupFontMask): // 13 : font
				{
					sFontKeyData.m_iFontFamilyIndex = *(pnStart + 2);
					sFontKeyData.m_iFontStyleIndex = (*(pnStart + 3) & kA3DFontKeyStyle) >> 24;
					sFontKeyData.m_iFontSizeIndex = (*(pnStart + 3) & kA3DFontKeySize) >> 12;
					sFontKeyData.m_cAttributes = (A3DInt8)(*(pnStart + 3) & kA3DFontKeyAttrib);

					A3DFontData fontdata;
					A3D_INITIALIZE_DATA(A3DFontData, fontdata);

					A3DGlobalFontKeyGet(&sFontKeyData, &fontdata);

					cTextAttributes.SetFontSize(static_cast<float>(fontdata.m_uiSize));
					if (fontdata.m_pcFamilyName) {
						cTextAttributes.SetFontName(fontdata.m_pcFamilyName);
					}

					cTextAttributes.SetFormat(sFontKeyData.m_cAttributes);

					A3DGlobalFontKeyGet(nullptr, &fontdata);

					DEFAULT_OFFSET;
				}
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupTextMask): // 14:  text
				{
					double dTextboxHeight = pdData[1];

					A3DUns32 nTextIndex = pnStart[2];
					A3DUTF8Char * pcBuffer = nullptr;

					if (nTextIndex < psTessMarkupData->m_uiTextsSize) {
						pcBuffer = psTessMarkupData->m_ppcTexts[nTextIndex];
					}

					CString strText = WStr::ToUtf16(pcBuffer);
					aOutStrings.push_back(strText);

					cTextAttributes.SetRGBColor(cColor);
					cTextAttributes.SetFontSize(static_cast<float>(dTextboxHeight * char_height));
					if (true == bFrameDrawMode) {
						cTextAttributes.SetFontSizeUnits(PMI::Font::Size::Units::PixelUnits);
						cTextAttributes.SetInsertionPoint(cTextMove);
					}
					else if (true == bFaceViewMode) {
						cTextAttributes.SetFontSizeUnits(PMI::Font::Size::Units::WorldSpaceUnits);
						cTextAttributes.SetInsertionPoint(cTextMove);
					}
					else {
						cTextAttributes.SetFontSizeUnits(PMI::Font::Size::Units::WorldSpaceUnits);
					}

					cOutTextAttributes.push_back(cTextAttributes);

					DEFAULT_OFFSET;
				}
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupPointsMask): // 15 : points
					DEFAULT_OFFSET;
					break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupPolygonMask): // 16 : polylines
				{
					int kSize = *(pnStart + 1);

					Point * pcPoints = new Point[kSize];
					int pt_count = 0;
					int i = 0;
					while (i < kSize)
					{
						pcPoints[pt_count].x = static_cast<float>(pdData[i++]);
						pcPoints[pt_count].y = static_cast<float>(pdData[i++]);
						pcPoints[pt_count++].z = static_cast<float>(pdData[i++]);
					}

					if (false == cTransformMatrix.IsIdentity()) {
						for (int nIndex = 0; nIndex < pt_count; nIndex++) {
							pcPoints[nIndex] = cTransformMatrix.Transform(pcPoints[nIndex]);
						}
					}

					H3DF::PolygonKit cPmiPolygon;
					cPmiPolygon.SetPoints(pt_count, pcPoints);
					cPmiPolygon.SetRGBColor(cColor);
					aOutPolygones.push_back(cPmiPolygon);

					if (nullptr != pcPoints) {
						delete[] pcPoints;
					}
				}
				DEFAULT_OFFSET;
				break;

				case A3D_DECODE_EXTRA_DATA(kA3DMarkupLineWidthMask): // 17 : line width
					DEFAULT_OFFSET;
					break;

				default:
					DEFAULT_OFFSET;
			}
		}
		else if (*pnStart & kA3DMarkupIsMatrix)
		{
			if (*(pnStart + 1) > 0)
			{
				Vector cXAxis(static_cast<float>(pdData[0]), static_cast<float>(pdData[1]), static_cast<float>(pdData[2]));
				char_width = (float)cXAxis.Length();

				Vector cYAxis(static_cast<float>(pdData[4]), static_cast<float>(pdData[5]), static_cast<float>(pdData[6]));
				char_height = (float)cYAxis.Length();

				if (bFrameDrawMode || bFaceViewMode) {
					cTextAttributes.SetWidthScale(char_width / char_height);
					cTextMove.Set(
						static_cast<float>(pdData[12] / m_dCadModelUnit),
						static_cast<float>(pdData[13] / m_dCadModelUnit),
						static_cast<float>(pdData[14] / m_dCadModelUnit));
				}
				else {
					cXAxis.Normalize();
					cYAxis.Normalize();
					Vector z = cXAxis.Cross(cYAxis);

					cMatrix[0][0] = cXAxis.x;
					cMatrix[0][1] = cXAxis.y;
					cMatrix[0][2] = cXAxis.z;

					cMatrix[1][0] = cYAxis.x;
					cMatrix[1][1] = cYAxis.y;
					cMatrix[1][2] = cYAxis.z;

					cMatrix[2][0] = z.x;
					cMatrix[2][1] = z.y;
					cMatrix[2][2] = z.z;

					cMatrix[3][0] = static_cast<float>(pdData[12]);
					cMatrix[3][1] = static_cast<float>(pdData[13]);
					cMatrix[3][2] = static_cast<float>(pdData[14]);


					cTransformMatrix = cMatrix * cTransformMatrix;
					//H3DF::TestMatrix cm;
					//MatrixCal::ComputeMatrixProduct(cMatrix.data(), cTransformMatrix.data(), cTransformMatrix.data());

					PMI::Orientation orientation;
					orientation.SetMatrix(cTransformMatrix);
					cTextAttributes.SetOrientation(orientation);
				}

				MAKE_OFFSET(0, 16);
			}
			else
			{
				cMatrix.Invert();
				cTransformMatrix = cMatrix * cTransformMatrix;
				cMatrix.Reset();

// 				MatrixCal::InverseMatrix(cMatrix.data(), cMatrix.data());
// 				MatrixCal::ComputeMatrixProduct(cMatrix.data(), cTransformMatrix.data(), cTransformMatrix.data());
// 				cMatrix.SetIdentity();
				char_height = 1.;
				MAKE_OFFSET(0, 0);
			}
		}
		else // Polyling Data
		{
			if (true == bFrameDrawMode || true == bFaceViewMode) {
				DEFAULT_OFFSET;
				continue;
			}

			int kSize = *(pnStart + 1);
			//pline.SetCount(kSize);

			int i = 0, nPointCount = kSize / 3;
			Point * pcPoints = new Point[nPointCount];
			nPointCount = 0;

			while (i < kSize)
			{
				pcPoints[nPointCount].x = static_cast<float>(pdData[i++]);
				pcPoints[nPointCount].y = static_cast<float>(pdData[i++]);
				pcPoints[nPointCount++].z = static_cast<float>(pdData[i++]);
			}

			if (false == cTransformMatrix.IsIdentity()) {
				for (int nIndex = 0; nIndex < nPointCount; nIndex++) {
					pcPoints[nIndex] = cTransformMatrix.Transform(pcPoints[nIndex]);
				}
			}

			H3DF::Polyline cPmiPolyline;
			cPmiPolyline.SetPoints(nPointCount, pcPoints);
			cPmiPolyline.SetRGBColor(cColor);

// 			if (line_pattern.encodedText())
// 				cPmiPolyline.SetLinePattern(reinterpret_cast<char const *>(line_pattern.encodedText()));

			aOutPolylines.push_back(cPmiPolyline);

			DEFAULT_OFFSET;
		}
	}

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 7-2. Leader Lines 및 Symbol 처리
A3DStatus TdfImport::GetLeaderLinesAndSymbols(const A3DMkpLeader * pMarkup, PolylineArray & cOutLeaderLines, PolygonArray & cOutLeaderSymbols)
{
	A3DMkpLeaderData sData;
	A3D_INITIALIZE_DATA(A3DMkpLeaderData, sData);

	A3DMkpLeaderGet(pMarkup, &sData);

	A3DTessBaseData sBaseData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, sBaseData);

	A3DTessBaseGet(sData.m_pTessellation, &sBaseData);

	A3DTessMarkupData sMarkupData;
	A3D_INITIALIZE_DATA(A3DTessMarkupData, sMarkupData);

	A3DInt32 iErr = A3DTessMarkupGet(sData.m_pTessellation, &sMarkupData);
	if (!iErr && sData.m_pTessellation)
	{
		StringArray strings;
		PMI::TextAttributesArray text_attributes;

		GetMarkupTesselation(&sBaseData, &sMarkupData, cOutLeaderLines, cOutLeaderSymbols, strings, text_attributes);
	}

	A3DTessMarkupGet(nullptr, &sMarkupData);
	A3DTessBaseGet(nullptr, &sBaseData);
	A3DMkpLeaderGet(nullptr, &sData);

	return A3D_SUCCESS;
}

// 7-3. PMI를 Group으로 처리하기 위해서, Parent Component에서 PMI Group을 검색하고, 없으면 생성한다.
// Segment도 "grppmi"로 생성한다.
H3DF::Component * TdfImport::GetPmiGroupComponent(H3DF::Component & cParentComp)
{
	if (nullptr != cParentComp.GetSubComponents()) {
		for (auto * pcComponent : *cParentComp.GetSubComponents()) {
			if (H3DF::Component::Type::PMIGroupComponent == pcComponent->GetType()) {
				return pcComponent;
			}
		}
	}

	H3DF::Component * pcOwner = cParentComp.GetOwner();

	while (nullptr != pcOwner) {
		if (nullptr != pcOwner->GetSubComponents()) {
			for (auto * pcComponent : *pcOwner->GetSubComponents()) {
				if (H3DF::Component::Type::PMIGroupComponent == pcComponent->GetType()) {
					return pcComponent;
				}
			}
		}

		pcOwner = pcOwner->GetOwner();
	}

	
	// Parent Component에 PMI Group을 생성.
	SegmentKey cParentSegment(cParentComp.GetSegmentKey());

	SegmentKey cSegment = m_cPmiIncludeSegment.Subsegment("grppmi%d", m_nMarkupId++);
	IncludeKey cInclude = cParentSegment.IncludeSegment(cSegment);

	H3DF::Component * pcComponent = AddComponent(cSegment, cInclude, "PMI", H3DF::Component::Type::PMIGroupComponent, cParentComp);
	return pcComponent;
}

// 8. Draw Tessellation Base
A3DStatus TdfImport::DrawTessBase(A3DTessBase * pcInTessBase, const A3DRiRepresentationItem * pcInRepItem, const A3DMiscEntityReference * pcInEntityRef,
	const A3DMiscCascadedAttributes * pcInParentAttr, H3DF::SegmentKey & cInSegment)
{
	if (pcInTessBase == nullptr) {
		return A3D_ERROR;
	}

	A3DEEntityType eEntityType;
	CHECK_A3D_RETURN(A3DEntityGetType(pcInTessBase, &eEntityType));

	A3DTessBaseData sTessBaseData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, sTessBaseData);
	CHECK_A3D_RETURN(A3DTessBaseGet(pcInTessBase, &sTessBaseData));

	A3DStatus eStatus = A3D_SUCCESS;

	switch (eEntityType)
	{
		case kA3DTypeTess3D:
			//eStatus = DrawTess3D((A3DTess3D *) pcTessBase, &sTessBaseData, pcRepItem, pcParentAttr, cParentSegment);
			eStatus = DrawTess3DFaceRegion((A3DTess3D *)pcInTessBase, &sTessBaseData, pcInRepItem, pcInEntityRef, pcInParentAttr, cInSegment);
			break;

		case kA3DTypeTess3DWire:
			eStatus = DrawTess3DWire((A3DTess3DWire *)pcInTessBase, &sTessBaseData, pcInRepItem, pcInParentAttr, cInSegment);
			break;

		case kA3DTypeTessMarkup:
			eStatus = BuildMarkup((A3DTessMarkup *)pcInTessBase, &sTessBaseData, cInSegment);
			break;


		default:
			assert(false);
			break;
	}

	A3DTessBaseGet(nullptr, &sTessBaseData);

	return eStatus;
}


// 8-1. Draw Tess3D
A3DStatus TdfImport::DrawTess3D(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
	const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cParentSegment)
{
	LogIncreaseTabIndex(2);

	double dUnitScale = 1.0;	// Tessellation은 1:1 비율로 들어온다 

	H3DF::SegmentKey cCurrnetSegment = cParentSegment;

	A3DTess3DData cTess3dData;
	A3D_INITIALIZE_DATA(A3DTess3DData, cTess3dData);
	CHECK_A3D_RETURN(A3DTess3DGet(pcTess3D, &cTess3dData));

	A3DUns32 nFacesCount = cTess3dData.m_uiFaceTessSize;

	if (0 == nFacesCount) {
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

	A3DUns32 * pnTriIndices = cTess3dData.m_puiTriangulatedIndexes;

	UINT nTriangleFaceCount = 0;

	// ----- Point 활당 -----
	A3DUns32 nPointCount = pcTessBaseData->m_uiCoordSize / 3;

	ConvertFaceInfo cConFaceInfo;

	cConFaceInfo.pnInIndices = cTess3dData.m_puiTriangulatedIndexes;

	if (nPointCount > m_nMaxPointCount) {
		if (nullptr != m_pcPoints) {
			delete[] m_pcPoints;
		}

		m_nMaxPointCount = nPointCount * 2000;
		m_pcPoints = new H3DF::Point[m_nMaxPointCount];
	}

	m_nPointCount = nPointCount;

	for (A3DUns32 nIndex = 0; nIndex < nPointCount; nIndex++) {
		m_pcPoints[nIndex].Set(
			pcTessBaseData->m_pdCoords[nIndex * 3],
			pcTessBaseData->m_pdCoords[nIndex * 3 + 1],
			pcTessBaseData->m_pdCoords[nIndex * 3 + 2]);
	}

	// ----- Normal Vector 활당 -----
	A3DUns32 nNormalCount = cTess3dData.m_uiNormalSize / 3;

	if (nNormalCount > m_nMaxNormalCount) {
		if (nullptr != m_pcNormals) {
			delete[] m_pcNormals;
		}

		m_nMaxNormalCount = nNormalCount * 2000;
		m_pcNormals = new H3DF::Vector[m_nMaxNormalCount];
	}

	m_nNormalCount = nNormalCount;

	for (A3DUns32 nIndex = 0; nIndex < nNormalCount; nIndex++) {
		m_pcNormals[nIndex].Set(
			cTess3dData.m_pdNormals[nIndex * 3],
			cTess3dData.m_pdNormals[nIndex * 3 + 1],
			cTess3dData.m_pdNormals[nIndex * 3 + 2]);
	}

	// ----- Texture Parameter 활당 -----
	A3DUns32 nTextureCoordCount = cTess3dData.m_uiTextureCoordSize;
	cConFaceInfo.aInParams.resize(nTextureCoordCount);
	for (A3DUns32 nIndex = 0; nIndex < nTextureCoordCount; nIndex++) {
		cConFaceInfo.aInParams[nIndex] = static_cast<float>(cTess3dData.m_pdTextureCoords[nIndex]);
	}

	// Attribute를 계산해서 처리하는 부분, 전체 Attribute를 찾아서 가장 많은 종류의 Index를 찾아서 처리한다.
	A3DMiscCascadedAttributesData * psAttrData = new A3DMiscCascadedAttributesData[nFacesCount];

	struct StyleDefine
	{
		DWORD nCount = 0;
		A3DUns32 nRgbColorIndex = 0;
		A3DUns32 nFirstFaceIndex = 0; // nRgbColorIndex가 처음 나타나는 Face Index;
	};

	CAtlMap<A3DUns32, StyleDefine> mStyleDefineMap;

	A3DUns32 nMaxRgbColorIndex = 0;
	A3DUns32 nMaxFaceIndex = 0;
	DWORD nMaxCount = 0;


	//== Style 관련 정보 수집 ========================================================================

	A3DMiscCascadedAttributes * pcAttribute = nullptr;
	A3DStatus nStatus = A3DMiscCascadedAttributesCreate(&pcAttribute);

	// 각각 Face의 Color Index를 이용해서 Style map을 생성한다.
	for (A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++) {
		A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[nFaceIndex];

		CreateAndPushCascadedAttributesTessFace(pcRepItem, pcTess3D, &cTessFaceData, nFaceIndex, pcParentAttr, &pcAttribute, &psAttrData[nFaceIndex]);

		A3DUns32 nRgbColorIndex = psAttrData[nFaceIndex].m_sStyle.m_uiRgbColorIndex;

		StyleDefine sStyleDefine;
		// 찾은 경우 Count 증가
		if (true == mStyleDefineMap.Lookup(nRgbColorIndex, sStyleDefine)) {
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
		if (nMaxCount < sStyleDefine.nCount) {
			nMaxRgbColorIndex = sStyleDefine.nRgbColorIndex;
			nMaxCount = sStyleDefine.nCount;
			nMaxFaceIndex = sStyleDefine.nFirstFaceIndex;
		}
	}

	A3DMiscCascadedAttributesDelete(pcAttribute);

	// Max Key는 삭제한다.
	mStyleDefineMap.RemoveKey(nMaxRgbColorIndex);

	CAtlMap<A3DUns32, HC_KEY> mFaceSegmentStyleMap;

	// Max Color Index 값을 Parent Segment에 적용시켜 대표 Color Style로 지정한다.
	SetFaceStyle(cParentSegment, psAttrData[nMaxFaceIndex]);

	// Texture Mapping 설정
	//SetTextureMapping(cParentSegment, psAttrData[nMaxFaceIndex]);

	mFaceSegmentStyleMap.SetAt(nMaxRgbColorIndex, cParentSegment.KeyValue());
	Log(2, L"max_style_face_%d, count: %d", nMaxRgbColorIndex, nMaxCount);

	// Max Color Index를 제외한 나머지 Color Index들은 각각의 Style Face Segment를 생성해서 집어 넣도록 한다.
	// 각각 Face의 Color Index가 Max Index가 아닌 경우는 Color Index를 적용하기 위해서 별도의 Face Segment를 생성하고 Style을 적용시킨다.
	POSITION pcPos = mStyleDefineMap.GetStartPosition();
	while (nullptr != pcPos)
	{
		// 사전에 탐색해 놓은 Style Map 검색
		StyleDefine sStyleDefine = mStyleDefineMap.GetNextValue(pcPos);

		// Face Segment를 별도로 생성하고, Style을 적용시킨다. 이 Face에 Insert Shell을 하게 된다.
		H3DF::SegmentKey cSubSegment = cParentSegment.Subsegment("style_face_%d", sStyleDefine.nRgbColorIndex).KeyValue();
		SetFaceStyle(cSubSegment, psAttrData[sStyleDefine.nFirstFaceIndex]);

		mFaceSegmentStyleMap.SetAt(sStyleDefine.nRgbColorIndex, cSubSegment.KeyValue());
	}

	mStyleDefineMap.RemoveAll();

	LogIncreaseTabIndex(2);

	for (A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++)
	{
		A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[nFaceIndex];
		cConFaceInfo.pnIndices = &cTessFaceData;

		A3DUns32 nRgbColorIndex = psAttrData[nFaceIndex].m_sStyle.m_uiRgbColorIndex;

		bool bForceOpenFlag = false;
		if (nMaxRgbColorIndex != nRgbColorIndex)
		{
			HC_KEY nKey;
			if (true == mFaceSegmentStyleMap.Lookup(nRgbColorIndex, nKey)) {
				H3DF::SegmentKey cSubSegment(nKey);
				cCurrnetSegment = cSubSegment;
				bForceOpenFlag = true;
				cCurrnetSegment.Open();

				//----- Texture Mapping 설정 -----
				//SetTextureMapping(cCurrnetSegment, psAttrData[nFaceIndex]);

				// Log(2, L"style_face_%d", nRgbColorIndex);
			}
		}

		//== Draw Edge Line ========================================================================
		A3DUns32 nStartWireIndex = cTessFaceData.m_uiStartWire;
		int index = 0;
		for (A3DUns32 j = 0; j < cTessFaceData.m_uiSizesWiresSize; ++j)
		{
			A3DUns32 size = cTessFaceData.m_puiSizesWires[j] & ~(kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);
			A3DUns32 flags = cTessFaceData.m_puiSizesWires[j] & (kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);

			if (flags & kA3DTessFaceDataWireIsNotDrawn) {
				index += size;
				continue;
			}

			H3DF::PointArray acWirePoints;
			acWirePoints.resize(size);

			for (A3DUns32 k = 0; k < size; ++k) {
				acWirePoints[k] = m_pcPoints[cTess3dData.m_puiWireIndexes[nStartWireIndex + index++] / 3];
			}

			H3DF::LineKey cLineKey = cCurrnetSegment.InsertLine(acWirePoints.size(), acWirePoints.data());

			// 대용량 파일에서 메모리 소모가 심하므로 사용하지 않은다. 심한 경우 20%이상 메모리를 소모한다.
// 			if (true == bSolidSegmentFlag) {
// 				H3DF::UserData::SetTopologyType(cLineKey, (DWORD)TopologyType::Edge);
// 			}
		}

		cConFaceInfo.nOutTriSizeIndex = 0;	// 한 Triangle Type당 하나씩
		cConFaceInfo.nOutTriStartIndex = cTessFaceData.m_uiStartTriangulated;
		cConFaceInfo.nOutTriColorIndex = 0;
		cConFaceInfo.fInNormalCosine = m_fNormalAngleCosine;

		cConFaceInfo.mOutIndexMap.RemoveAll();
		cConFaceInfo.aOutVertexRefs.clear();
		cConFaceInfo.aOutFacePoints.clear();
		cConFaceInfo.aOutFaceList.clear();
		cConFaceInfo.aOutFaceVertexNormals.clear();
		cConFaceInfo.aOutFaceVertexParams.clear();
		cConFaceInfo.aOutFaceVertexColors.clear();

		// fill out the RGBA vertex color array (if necessary) 1
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

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle) {
			nTriangleFaceCount += ConvertTessFaceDataTriangle(cConFaceInfo);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleFan(cConFaceInfo);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleStripe(cConFaceInfo);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleOneNormal(cConFaceInfo);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleFanOneNormal(cConFaceInfo);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleStripeOneNormal(cConFaceInfo);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured) {
			nTriangleFaceCount += ConveTessFaceDataTriangleTextured(cConFaceInfo);
		}

		if (false == cConFaceInfo.aOutFacePoints.empty()) {
			H3DF::ShellKit cShellKit;
			cShellKit.SetPoints(cConFaceInfo.aOutFacePoints);
			cShellKit.SetNormals(cConFaceInfo.aOutFaceVertexNormals);
			cShellKit.SetFacelist(cConFaceInfo.aOutFaceList);
			cShellKit.SetParameters(cConFaceInfo.aOutFaceVertexParams);
			cShellKit.SetColors(cConFaceInfo.aOutFaceVertexColors);

			ShellKey cShell = cCurrnetSegment.InsertShell(cShellKit);
		}

		if (true == bForceOpenFlag) {
			cCurrnetSegment.Close();
		}

		cCurrnetSegment = cParentSegment;
	}

	mFaceSegmentStyleMap.RemoveAll();

	for (A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++) {
		A3DMiscCascadedAttributesGet(nullptr, &psAttrData[nFaceIndex]);
	}

	REMOVE_ARRAY(psAttrData);

	LogDecreaseTabIndex(2);

	Log(2, L"DrawTess3D [InsertShell]: %s, %d, Style Count: %d", LogHexStr((DWORD_PTR)pcTess3D), nTriangleFaceCount, m_mMaterialMappingStyleMap.GetCount());

	CHECK_A3D_RETURN(A3DTess3DGet(nullptr, &cTess3dData));

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

A3DStatus TdfImport::DrawTess3DFaceRegion(const A3DTess3D * pcInTess3D, const A3DTessBaseData * pcInTessBaseData, const A3DRiRepresentationItem * pcInRepItem, 
	const A3DMiscEntityReference * pcInEntityRef, const A3DMiscCascadedAttributes * pcInParentAttr, H3DF::SegmentKey & cInSegment)
{
	LogIncreaseTabIndex(2);

	double dUnitScale = 1.0;	// Tessellation은 1:1 비율로 들어온다 

	A3DTess3DData cTess3dData;
	A3D_INITIALIZE_DATA(A3DTess3DData, cTess3dData);
	CHECK_A3D_RETURN(A3DTess3DGet(pcInTess3D, &cTess3dData));

	A3DUns32 nFacesCount = cTess3dData.m_uiFaceTessSize;

	if (0 == nFacesCount) {
		A3DTess3DGet(nullptr, &cTess3dData);
		return A3D_ERROR;
	}

	A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[0];

	const double cBigValue = 1.0e+12;
	bool bStrange = false;

	double dScale = 1.0;// cImportInfo.dModelScale * cImportInfo.dTessellationScale;

	A3DUns32 * pnTriIndices = cTess3dData.m_puiTriangulatedIndexes;

	// ----- Point 활당 -----
	A3DUns32 nPointCount = pcInTessBaseData->m_uiCoordSize / 3;

	ConvertFaceInfo cConFaceInfo;

	cConFaceInfo.pnInIndices = cTess3dData.m_puiTriangulatedIndexes;

	if (nPointCount > m_nMaxPointCount) {
		if (nullptr != m_pcPoints) {
			delete[] m_pcPoints;
		}

		m_nMaxPointCount = nPointCount + 2000;
		m_pcPoints = new H3DF::Point[m_nMaxPointCount];
	}

	m_nPointCount = nPointCount;

	for (A3DUns32 nIndex = 0; nIndex < nPointCount; nIndex++) {
		m_pcPoints[nIndex].Set(
			pcInTessBaseData->m_pdCoords[nIndex * 3],
			pcInTessBaseData->m_pdCoords[nIndex * 3 + 1],
			pcInTessBaseData->m_pdCoords[nIndex * 3 + 2]);
	}

	// ----- Normal Vector 활당 -----
	A3DUns32 nNormalCount = cTess3dData.m_uiNormalSize / 3;

	if (nNormalCount > m_nMaxNormalCount) {
		if (nullptr != m_pcNormals) {
			delete[] m_pcNormals;
		}

		m_nMaxNormalCount = nNormalCount + 2000;
		m_pcNormals = new H3DF::Vector[m_nMaxNormalCount];
	}

	m_nNormalCount = nNormalCount;

	for (A3DUns32 nIndex = 0; nIndex < nNormalCount; nIndex++) {
		m_pcNormals[nIndex].Set(
			cTess3dData.m_pdNormals[nIndex * 3],
			cTess3dData.m_pdNormals[nIndex * 3 + 1],
			cTess3dData.m_pdNormals[nIndex * 3 + 2]);
	}

	// ----- Texture Parameter 활당 -----
	A3DUns32 nTextureCoordCount = cTess3dData.m_uiTextureCoordSize / 2;

	// Texture Buffer 활당
	if (nTextureCoordCount > m_nMaxTextureCoordCount) {
		if (nullptr != m_pcTextureCoords) {
			delete[] m_pcTextureCoords;
		}

		m_nMaxTextureCoordCount = nTextureCoordCount + 2000;
		m_pcTextureCoords = new H3DF::Point[m_nMaxTextureCoordCount];
	}

	m_nTextureCoordCount = nTextureCoordCount;

	for (A3DUns32 nIndex = 0; nIndex < nTextureCoordCount; nIndex++) {
		m_pcTextureCoords[nIndex].Set(
			cTess3dData.m_pdTextureCoords[nIndex * 2],
			cTess3dData.m_pdTextureCoords[nIndex * 2 + 1], 0.0f);
	}

	// Attribute를 계산해서 처리하는 부분, 전체 Attribute를 찾아서 가장 많은 종류의 Index를 찾아서 처리한다.
	struct StyleDefine
	{
		DWORD nCount = 0;
		A3DUns32 nRgbColorIndex = 0;
		A3DGraphStyleData cStyleData;
	};


	//== Style 관련 정보 수집 ========================================================================

	// A3DUns32값은 RGB Color Index 값이다.
	CAtlMap<A3DUns32, StyleDefine> mStyleDefineMap;
	A3DUns32 nMaxRgbColorIndex = 0;
	A3DUns32 nMaxFaceIndex = 0;
	DWORD nMaxCount = 0;

	// Region에 Style을 적용시킬때 사용하기 위해서 Face Rgb Color Index를 저장한다.
	A3DUns32 * pnFaceRgbColorIndex = new A3DUns32[nFacesCount];
	A3DMiscCascadedAttributesData psMaxAttrData;

	A3DMiscCascadedAttributes * pcAttribute = nullptr;
	
	A3DStatus nStatus = A3DMiscCascadedAttributesCreate(&pcAttribute);

/*
	A3DMiscCascadedAttributesPush(pcAttribute, pcInRepItem, pcInParentAttr);
	A3DMiscCascadedAttributesData sTestData;
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, sTestData);
	A3DMiscCascadedAttributesGet(pcAttribute, &sTestData);

	if (A3D_FALSE == sTestData.m_bShow) {
		int i = 0;
	}
*/

	A3DMiscCascadedAttributes * pcParentAttr = (A3DMiscCascadedAttributes *)pcInParentAttr;

	A3DMiscCascadedAttributes * pcEntityRefAttrs = nullptr;

	if (nullptr != pcInEntityRef) {
		A3DMiscCascadedAttributesCreate(&pcEntityRefAttrs);
		A3DMiscCascadedAttributesPush(pcEntityRefAttrs, pcInEntityRef, pcInParentAttr);
		pcParentAttr = pcEntityRefAttrs;
	}

	// nMaxRgbColorIndex를 찾아 낸다. 
	for (A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++) {
		A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[nFaceIndex];
		A3DMiscCascadedAttributesData psAttrData;

		CreateAndPushCascadedAttributesTessFace(pcInRepItem, pcInTess3D, &cTessFaceData, nFaceIndex, pcParentAttr, &pcAttribute, &psAttrData);

		A3DUns32 nRgbColorIndex = psAttrData.m_sStyle.m_uiRgbColorIndex;

		pnFaceRgbColorIndex[nFaceIndex] = nRgbColorIndex;

		StyleDefine sStyleDefine;
		// 찾은 경우 Count 증가, 나중에 가장 많이 사용되는 Style을 찾기 위해서 저장한다.
		if (true == mStyleDefineMap.Lookup(nRgbColorIndex, sStyleDefine)) {
			sStyleDefine.nCount++;
			mStyleDefineMap.SetAt(nRgbColorIndex, sStyleDefine);
		}
		else { // 새로 삽입하는 경우 Count는 1, 나중에 사용하기 위해서 필요한 값들을 저장해 놓는다.
			sStyleDefine.nCount = 1;
			sStyleDefine.nRgbColorIndex = nRgbColorIndex;
			CopyMemory(&sStyleDefine.cStyleData, &psAttrData.m_sStyle, sizeof(A3DGraphStyleData));
			mStyleDefineMap.SetAt(nRgbColorIndex, sStyleDefine);
		}

		// Max Count Style을 찾아내서 대표 Style로 지정할 준비를 한다.
		if (nMaxCount < sStyleDefine.nCount) {
			nMaxRgbColorIndex = sStyleDefine.nRgbColorIndex;
			nMaxFaceIndex = nFaceIndex;
			nMaxCount = sStyleDefine.nCount;
			CopyMemory(&psMaxAttrData, &psAttrData, sizeof(A3DMiscCascadedAttributesData));
		}

		A3DMiscCascadedAttributesGet(NULL, &psAttrData);
	}

	A3DMiscCascadedAttributesDelete(pcAttribute);

	if (nullptr != pcEntityRefAttrs) {
		A3DMiscCascadedAttributesDelete(pcEntityRefAttrs);
	}

	//mStyleDefineMap.RemoveAll();

	// Max Color Index 값을 Parent Segment에 적용시켜 대표 Color Style로 지정한다.
	// 또한 내부에서 Style Segment를 생성하고 Map 저장한다.
	SetFaceStyle(cInSegment, psMaxAttrData);

	LogIncreaseTabIndex(2);

	H3DF::ShellKit cShellKit;
	cShellKit.CreateShellWrapper();
	cShellKit.BeginAddFaces(nPointCount);

	A3DUns32 * pnFaceStartIndexes = new A3DUns32[nFacesCount + 1];
	UINT nTriangleFaceCount = 0;

	for (A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++)
	{
		A3DTessFaceData & cTessFaceData = cTess3dData.m_psFaceTessData[nFaceIndex];
		cConFaceInfo.pnIndices = &cTessFaceData;

		//== Draw Edge Line ========================================================================
		A3DUns32 nStartWireIndex = cTessFaceData.m_uiStartWire;
		int index = 0;
		for (A3DUns32 j = 0; j < cTessFaceData.m_uiSizesWiresSize; ++j)
		{
			A3DUns32 size = cTessFaceData.m_puiSizesWires[j] & ~(kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);
			A3DUns32 flags = cTessFaceData.m_puiSizesWires[j] & (kA3DTessFaceDataWireIsNotDrawn | kA3DTessFaceDataWireIsClosing);

			if (flags & kA3DTessFaceDataWireIsNotDrawn) {
				index += size;
				continue;
			}

			H3DF::PointArray acWirePoints;
			acWirePoints.resize(size);

			for (A3DUns32 k = 0; k < size; ++k) {
				acWirePoints[k] = m_pcPoints[cTess3dData.m_puiWireIndexes[nStartWireIndex + index++] / 3];
			}

			H3DF::LineKey cLineKey = cInSegment.InsertLine(acWirePoints.size(), acWirePoints.data());

			// 대용량 파일에서 메모리 소모가 심하므로 사용하지 않은다. 심한 경우 20%이상 메모리를 소모한다.
// 			if (true == bSolidSegmentFlag) {
// 				H3DF::UserData::SetTopologyType(cLineKey, (DWORD)TopologyType::Edge);
// 			}
		}

		cConFaceInfo.nOutTriSizeIndex = 0;	// 한 Triangle Type당 하나씩
		cConFaceInfo.nOutTriStartIndex = cTessFaceData.m_uiStartTriangulated;
		cConFaceInfo.nOutTriColorIndex = 0;
		cConFaceInfo.fInNormalCosine = m_fNormalAngleCosine;

		// fill out the RGBA vertex color array (if necessary) 1
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

		pnFaceStartIndexes[nFaceIndex] = nTriangleFaceCount;

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle) {
			nTriangleFaceCount += ConvertTessFaceDataTriangle(cConFaceInfo, cShellKit);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleFan(cConFaceInfo, cShellKit);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleStripe(cConFaceInfo, cShellKit);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleOneNormal(cConFaceInfo, cShellKit);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleFanOneNormal(cConFaceInfo, cShellKit);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal) {
			nTriangleFaceCount += ConvertTessFaceDataTriangleStripeOneNormal(cConFaceInfo, cShellKit);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured) {
			nTriangleFaceCount += ConveTessFaceDataTriangleTextured(cConFaceInfo, cShellKit);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeTextured) {
			nTriangleFaceCount += ConveTessFaceDataTriangleStripeTextured(cConFaceInfo, cShellKit);
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanTextured) {
			continue;
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormalTextured) {
			continue;
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured) {
			continue;
		}

		if (cTessFaceData.m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured) {
			continue;
		}
	}

	pnFaceStartIndexes[nFacesCount] = nTriangleFaceCount;

	cShellKit.Regenerate();

	cShellKit.EndAddFaces();


	if (0 < cShellKit.GetPointCount()) {
		// Region 설정
		A3DUns32 nRegionIndex = 1;
		for (A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++) {
			int nFaceStartIndex = pnFaceStartIndexes[nFaceIndex];
			int nFaceEndIndex = pnFaceStartIndexes[nFaceIndex + 1];

			cShellKit.SetRegionFaces(nRegionIndex, nFaceStartIndex, nFaceEndIndex - nFaceStartIndex);
			nRegionIndex++;
		}

		//== StyleMap이 있는 경우 처리 ================================================================

		if (0 < mStyleDefineMap.GetCount()) {
			nRegionIndex = 1;
			for (A3DUns32 nFaceIndex = 0; nFaceIndex < nFacesCount; nFaceIndex++) {

				// Color Index값을 이용해서 Region을 Style을 적용한다.
				A3DUns32 nRgbColorIndex = pnFaceRgbColorIndex[nFaceIndex];

				// Max Color Index가 아니면 개별 Region에 Style을 적용한다.
				if (nMaxRgbColorIndex != nRgbColorIndex) {
					// nRgbColorIndex를 이용해서 Style을 검색 찾은 경우 Style을 젹용한다.
					bool bSucessFlag = true;

					StyleDefine sStyleDefine;
					// 사전에 등록되어 있는 Style값을 찾는다.
					if (true == mStyleDefineMap.Lookup(nRgbColorIndex, sStyleDefine)) {
						H3DF::MaterialKit cMaterial;
						if (false == FindMaterial(sStyleDefine.cStyleData, cMaterial)) {
							if (false == CreateMaterial(sStyleDefine.cStyleData, cMaterial)) {
								continue;
							}
						}

						cShellKit.SetRegionMaterial(nRegionIndex, cMaterial);
					}

				}

				nRegionIndex++;
			}
		}
	}
	else {
		cShellKit.DeleteShellWrapperKey();
	}

	mStyleDefineMap.RemoveAll();

	REMOVE_ARRAY(pnFaceStartIndexes);
	REMOVE_ARRAY(pnFaceRgbColorIndex);

	LogDecreaseTabIndex(2);

	Log(2, L"DrawTess3DFaceRegion: %s, %d, Style Count: %d", LogHexStr((DWORD_PTR)pcInTess3D), nTriangleFaceCount, m_mMaterialMappingStyleMap.GetCount());

	CHECK_A3D_RETURN(A3DTess3DGet(nullptr, &cTess3dData));

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 8-1-1. Triangle Face Data 변환
UINT TdfImport::ConvertTessFaceDataTriangle(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32  nInVertexParamSize = 0;

	A3DUns32 nTriangleCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangle: %d", nTriangleCount);

	for (A3DUns32 nTriIndex = 0; nTriIndex < nTriangleCount; nTriIndex++)
	{
		//A3DUns32 nNormalIndex = cInFaceInfo.nOutTriStartIndex++;

		nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;
		nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		//nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += 2;

		nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;
		nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		//nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += 2;

		nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;
		nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		//nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += 2;

		AddTriangle(cInFaceInfo, nFaceLispnIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
	}

	return nTriangleCount;
}

UINT TdfImport::ConvertTessFaceDataTriangle(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	//int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	//int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32  nInVertexParamSize = 0;

	A3DUns32 nTriangleCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangle: %d", nTriangleCount);

	for (A3DUns32 nTriIndex = 0; nTriIndex < nTriangleCount; nTriIndex++)
	{
		nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;
		nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		//nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += 2;


		nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;
		nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		//nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += 2;

		nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;
		nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		//nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += 2;

		cInShellKit.AddNextFaceWithDistinctNormals(m_pcPoints, m_pcNormals, nFaceLispnIndices, nFaceVertexNormalIndices, 3);
	}

	return nTriangleCount;
}

// 8-1-2. Triangle Fan Data 변환
UINT TdfImport::ConvertTessFaceDataTriangleFan(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32  nInVertexParamSize = 0;

	A3DUns32 nTriFanCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleFan: %d", nTriFanCount);

	for (A3DUns32 nFanIndex = 0; nFanIndex < nTriFanCount; nFanIndex++)
	{
		A3DUns32 nTriFanIndex = cInFaceInfo.nOutTriStartIndex + 2;
		A3DUns32 nTriColorIndex = cInFaceInfo.nOutTriColorIndex;
		A3DUns32 nTriFanPointSize = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex];
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

		for (A3DUns32 tri = 0; tri < nTriFanPointSize - 2; tri++)
		{
			nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
			nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;

			nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nTriFanIndex++] / 3;
			nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[nTriFanIndex++] / 3;

			nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex] / 3;
			// nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex + 1];
			nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex + 1] / 3;

			AddTriangle(cInFaceInfo, nFaceLispnIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
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

UINT TdfImport::ConvertTessFaceDataTriangleFan(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceNormalIndices[3];			// vertex normal indices into the "global" normal array

	A3DUns32 nTriSizeIndex = cInFaceInfo.nOutTriSizeIndex;
	A3DUns32 nTriStartIndex = cInFaceInfo.nOutTriStartIndex;

	A3DUns32 nStripesCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex++];
	A3DUns32 nFaceCount = 0;

	A3DUns32 * pnIndices = cInFaceInfo.pnInIndices;

	for (A3DUns32 stripe = 0; stripe < nStripesCount; stripe++)
	{
		int nPointCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex++];
		int nLocalTriStartIndex = nTriStartIndex;

		for (int i = 0; i < nPointCount - 1; ++i)
		{
			if (!(i % 2) || 1)
			{
				nFaceLispnIndices[0] = pnIndices[nTriStartIndex + 1] / 3;
				nFaceNormalIndices[0] = pnIndices[nTriStartIndex] / 3;

				nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
				nLocalTriStartIndex += 2;

				nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
			}
			else
			{
				nFaceLispnIndices[2] = pnIndices[nTriStartIndex + 1] / 3;
				nFaceNormalIndices[2] = pnIndices[nTriStartIndex] / 3;

				nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
				nLocalTriStartIndex += 2;

				nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
			}

			cInShellKit.AddNextFaceWithDistinctNormals(m_pcPoints, m_pcNormals, nFaceLispnIndices, nFaceNormalIndices, 3);
			nFaceCount++;
		}

		nTriStartIndex += nPointCount * 2;
	}

	cInFaceInfo.nOutTriSizeIndex = nTriSizeIndex;
	cInFaceInfo.nOutTriStartIndex = nTriStartIndex;

	return nFaceCount;
}

// 8-1-3. Triangle Stripe Data 변환
UINT TdfImport::ConvertTessFaceDataTriangleStripe(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array

	A3DUns32 nStripesCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];
	A3DUns32 nTriFaceCount = 0;

	// Log(2, L"ConvertTessFaceDataTriangleStripe: %d", nStripesCount);

	bool hasVertexColor = false; // INT2bool(cTessFaceData.m_uiRGBAVerticesSize);
	if (0 < cInFaceInfo.pnIndices->m_uiRGBAVerticesSize) {
		hasVertexColor = true;
	}

	for (A3DUns32 nStripeIndex = 0; nStripeIndex < nStripesCount; nStripeIndex++)
	{
		A3DUns32 nTriStripIndex = cInFaceInfo.nOutTriStartIndex;
		A3DUns32 nPointCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex];
		cInFaceInfo.nOutTriSizeIndex++;

		if (0 == nPointCount) {
			assert(false);
			continue;
		}

		for (A3DUns32 tri = 0; tri < nPointCount - 2; tri++)
		{
			if ((tri % 2) == 0)
			{
				nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex -= 2;
			}
			else
			{
				nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex += 2;

				nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex + 1] / 3;
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex] / 3;
				nTriStripIndex -= 2;
			}

			A3DUns32  nInVertexParamSize = 0; //(tc_size == 0 ? 0 : 2),
			AddTriangle(cInFaceInfo, nFaceLispnIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);

			nTriFaceCount++;
		}


		cInFaceInfo.nOutTriStartIndex += 2 * nPointCount;
	}

	return nTriFaceCount;
}

UINT TdfImport::ConvertTessFaceDataTriangleStripe(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceNormalIndices[3];			// vertex normal indices into the "global" normal array

	A3DUns32 nTriSizeIndex = cInFaceInfo.nOutTriSizeIndex;
	A3DUns32 nTriStartIndex = cInFaceInfo.nOutTriStartIndex;

	A3DUns32 nStripesCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex++];
	A3DUns32 nFaceCount = 0;

	A3DUns32 * pnIndices = cInFaceInfo.pnInIndices;

	for (A3DUns32 nStripeIndex = 0; nStripeIndex < nStripesCount; nStripeIndex++)
	{
		A3DUns32 nPointCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex++];

		if (0 == nPointCount) {
			assert(false);
			continue;
		}

		A3DUns32 nLocalTriStartIndex = nTriStartIndex;

		// TRACE(L"Point Count, StartIndex, vcpos2 : %d, %d, %d\r\n", nPointCount, nLocalTriangleStartIndex, vcpos2);

		for (A3DUns32 nIndex = 0; nIndex < nPointCount - 2; nIndex++)
		{
			if (!(nIndex % 2))
			{
				nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
				nLocalTriStartIndex += 2;

				nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
				nLocalTriStartIndex += 2;

				nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
				nLocalTriStartIndex -= 2;
			}
			else
			{
				nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
				nLocalTriStartIndex += 2;

				nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
				nLocalTriStartIndex += 2;

				nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex + 1] / 3;
				nFaceNormalIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
				nLocalTriStartIndex -= 2;
			}

			cInShellKit.AddNextFaceWithDistinctNormals(m_pcPoints, m_pcNormals, nFaceLispnIndices, nFaceNormalIndices, 3);

			nFaceCount++;
		}

		nTriStartIndex += nPointCount * 2;
	}

	cInFaceInfo.nOutTriSizeIndex = nTriSizeIndex;
	cInFaceInfo.nOutTriStartIndex = nTriStartIndex;

	return nFaceCount;
}

// 8-1-4. Triangle Stripe One Normal 변환
UINT TdfImport::ConvertTessFaceDataTriangleOneNormal(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32 nInVertexParamSize = 0;

	A3DUns32 nTriangleCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleOneNormal: %d", nTriangleCount);

	for (A3DUns32 nTriIndex = 0; nTriIndex < nTriangleCount; nTriIndex++)
	{
		A3DUns32 nNormalIndex = cInFaceInfo.nOutTriStartIndex++;

		nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		AddTriangle(cInFaceInfo, nFaceLispnIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
	}

	return nTriangleCount;
}

UINT TdfImport::ConvertTessFaceDataTriangleOneNormal(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	//int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32 nInVertexParamSize = 0;

	A3DUns32 nTriangleCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleOneNormal: %d", nTriangleCount);

	for (A3DUns32 nTriIndex = 0; nTriIndex < nTriangleCount; nTriIndex++)
	{
		A3DUns32 nNormalIndex = cInFaceInfo.nOutTriStartIndex++;

		nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;

		cInShellKit.AddNextFaceWithDistinctNormals(m_pcPoints, m_pcNormals, nFaceLispnIndices, nFaceVertexNormalIndices, 3);
	}

	return nTriangleCount;
}

// 8-1-5. Triangle Fan One Normal Data 변환
UINT TdfImport::ConvertTessFaceDataTriangleFanOneNormal(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32 nInVertexParamSize = 0;

	A3DUns32 nTriFanCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleFanOneNormal: %d", nTriFanCount);

	for (A3DUns32 fan = 0; fan < nTriFanCount; fan++)
	{
		A3DUns32	nTriFanIndex = cInFaceInfo.nOutTriStartIndex + 2;
		A3DUns32	nTriColorIndex = cInFaceInfo.nOutTriColorIndex;
		A3DUns32	nTriFanPointSize = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex];
		bool		bSingleNormalFlag = false;								// flag indicating if there are uniform vertex normals per triangle fan

		// determine if we have one normal per vertex or just one per triangle fan
		if (cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex] & kA3DTessFaceDataNormalSingle) {
			bSingleNormalFlag = true;
			nTriFanPointSize &= ~kA3DTessFaceDataNormalSingle;
		}

		cInFaceInfo.nOutTriSizeIndex++;

		A3DUns32 nFirstColorIndex = nTriColorIndex;
		if (0 < cInFaceInfo.aInColors.size()) {
			nTriColorIndex++;
		}

		for (A3DUns32 tri = 0; tri < nTriFanPointSize - 2; tri++)
		{
			nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
			nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1];
			nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 3;
			if (cInFaceInfo.aInColors.size() > 0) {
				nFaceVertexColorIndices[0] = nFirstColorIndex;
			}

			nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[(bSingleNormalFlag ? cInFaceInfo.nOutTriStartIndex : nTriFanIndex++)] / 3;
			nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[nTriFanIndex];
			nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[nTriFanIndex++] / 3;
			if (cInFaceInfo.aInColors.size() > 0)
				nFaceVertexColorIndices[1] = nTriColorIndex++;

			nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[(bSingleNormalFlag ? cInFaceInfo.nOutTriStartIndex : nTriFanIndex)] / 3;
			nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex + (bSingleNormalFlag ? 0 : 1)];
			nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[nTriFanIndex + (bSingleNormalFlag ? 0 : 1)] / 3;
			if (cInFaceInfo.aInColors.size() > 0)
				nFaceVertexColorIndices[2] = nTriColorIndex;

			AddTriangle(cInFaceInfo, nFaceLispnIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
		}

		if (true == bSingleNormalFlag) {
			cInFaceInfo.nOutTriStartIndex += nTriFanPointSize + 1;
		}
		else {
			cInFaceInfo.nOutTriStartIndex += 2 * nTriFanPointSize;
		}

		if (cInFaceInfo.aInColors.size() > 0) {
			cInFaceInfo.nOutTriColorIndex += nTriFanPointSize;
		}
	}

	return nTriFanCount;
}

UINT TdfImport::ConvertTessFaceDataTriangleFanOneNormal(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceNormalIndices[3];			// vertex normal indices into the "global" normal array

	A3DUns32 nTriSizeIndex = cInFaceInfo.nOutTriSizeIndex;
	A3DUns32 nTriStartIndex = cInFaceInfo.nOutTriStartIndex;
	A3DUns32 * pnIndices = cInFaceInfo.pnInIndices;

	A3DUns32 nTriFanCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleFanOneNormal: %d", nTriFanCount);

	bool bSingleNormal = false;
	A3DUns32 nFaceCount = 0;

	for (A3DUns32 nFanIndex = 0; nFanIndex < nTriFanCount; nFanIndex++)
	{
		if (cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex] & kA3DTessFaceDataNormalSingle) {
			bSingleNormal = true;
		}

		int nPointCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex] & ~kA3DTessFaceDataNormalSingle;
		nTriSizeIndex++;

		A3DUns32 nLocalTriStartIndex = nTriStartIndex;

		if (!bSingleNormal)
		{
			nTriStartIndex += 2;
			for (int i = 0; i < nPointCount - 1; ++i)
			{
				if (!(i % 2))
				{
					nFaceLispnIndices[0] = pnIndices[nTriStartIndex + 1] / 3;
					nFaceNormalIndices[0] = pnIndices[nTriStartIndex] / 3;

					nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
					nLocalTriStartIndex += 2;

					nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
				}
				else
				{
					nFaceLispnIndices[2] = pnIndices[nTriStartIndex + 1] / 3;
					nFaceNormalIndices[2] = pnIndices[nTriStartIndex] / 3;

					nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
					nLocalTriStartIndex += 2;

					nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
				}

				cInShellKit.AddNextFaceWithDistinctNormals(m_pcPoints, m_pcNormals, nFaceLispnIndices, nFaceNormalIndices, 3);
				nFaceCount++;
			}
			nTriStartIndex += nPointCount * 2;
		}
		else
		{
			nLocalTriStartIndex += 2;

			for (int i = 0; i < nPointCount - 2; ++i) {
				if (!(i % 2) || 1)
				{
					nFaceLispnIndices[0] = pnIndices[nTriStartIndex + 1] / 3;
					nFaceNormalIndices[0] = pnIndices[nTriStartIndex] / 3;

					nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[1] = pnIndices[nTriStartIndex] / 3;
					++nLocalTriStartIndex;

					nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[2] = pnIndices[nTriStartIndex] / 3;
				}
				else
				{
					nFaceLispnIndices[2] = pnIndices[nTriStartIndex + 1] / 3;
					nFaceNormalIndices[2] = pnIndices[nTriStartIndex] / 3;

					nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[1] = pnIndices[nTriStartIndex] / 3;
					++nLocalTriStartIndex;

					nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[0] = pnIndices[nTriStartIndex] / 3;
				}

				cInShellKit.AddNextFaceWithDistinctNormals(m_pcPoints, m_pcNormals, nFaceLispnIndices, nFaceNormalIndices, 3);
				nFaceCount++;
			}

			nTriStartIndex += (nPointCount + 1);
		}
	}

	cInFaceInfo.nOutTriSizeIndex = nTriSizeIndex;
	cInFaceInfo.nOutTriStartIndex = nTriStartIndex;

	return nFaceCount;
}

// 8-1-6. Triangle Stripe One Normal Data 변환
UINT TdfImport::ConvertTessFaceDataTriangleStripeOneNormal(ConvertFaceInfo & cInFaceInfo)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3];	// vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3];		// vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3];		// vertex color indices into the RGBA color array
	A3DUns32 nInVertexParamSize = 0;

	A3DUns32 nStripesCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	// Log(2, L"ConvertTessFaceDataTriangleStripeOneNormal: %d", nStripesCount);

	for (A3DUns32 nStripIndex = 0; nStripIndex < nStripesCount; nStripIndex++)
	{
		A3DUns32 nTriStripIndex = cInFaceInfo.nOutTriStartIndex;
		A3DUns32 nTriColorIndex = cInFaceInfo.nOutTriColorIndex;
		A3DUns32 nTriStripPointSize = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex];
		bool bSingleNormal = false;								// flag indicating if there are uniform vertex normals per tri-nStripIndex

		// determine if we have a normal per vertex or just one per triangle nStripIndex
		if (cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex] & kA3DTessFaceDataNormalSingle) {
			bSingleNormal = true;
			nTriStripPointSize &= ~kA3DTessFaceDataNormalSingle;
		}

		cInFaceInfo.nOutTriSizeIndex++;

		if (true == bSingleNormal) {
			nTriStripIndex++;
		}

		for (A3DUns32 nTriIndex = 0; nTriIndex < nTriStripPointSize - 2; nTriIndex++)
		{
			if ((nTriIndex % 2) == 0)
			{
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex++)] / 3;
				nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex];
				nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex++] / 3;
				if (cInFaceInfo.aInColors.size() > 0) {
					nFaceVertexColorIndices[0] = nTriColorIndex++;
				}

				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex + 0)] / 3;
				nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 0 : 1)];
				nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 0 : 1)] / 3;
				if (cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[1] = nTriColorIndex;

				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex + 2)] / 3;
				nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 1 : 3)];
				nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 1 : 3)] / 3;
				if (cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[2] = nTriColorIndex + 1;
			}
			else
			{
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex + 4)] / 3;
				nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 2 : 5)];
				nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 2 : 5)] / 3;
				if (cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[0] = nTriColorIndex + 2;

				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex + 2)] / 3;
				nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 1 : 3)];
				nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[nTriStripIndex + (bSingleNormal ? 1 : 3)] / 3;
				if (cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[1] = nTriColorIndex + 1;

				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[(bSingleNormal ? cInFaceInfo.nOutTriStartIndex : nTriStripIndex++)] / 3;
				nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex];
				nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[nTriStripIndex++] / 3;
				if (cInFaceInfo.aInColors.size() > 0)
					nFaceVertexColorIndices[2] = nTriColorIndex++;
			}

			AddTriangle(cInFaceInfo, nFaceLispnIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, nInVertexParamSize);
		}

		if (bSingleNormal)
			cInFaceInfo.nOutTriStartIndex += nTriStripPointSize + 1;
		else
			cInFaceInfo.nOutTriStartIndex += 2 * nTriStripPointSize;

		if (0 < cInFaceInfo.aInColors.size()) {
			cInFaceInfo.nOutTriColorIndex += nTriStripPointSize;
		}
	}

	return nStripesCount;
}

UINT TdfImport::ConvertTessFaceDataTriangleStripeOneNormal(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit)
{
	int nFaceLispnIndices[3];			// facelist indices into the "global" point array
	int nFaceNormalIndices[3];	// vertex normal indices into the "global" normal array

	A3DUns32 nTriSizeIndex = cInFaceInfo.nOutTriSizeIndex;
	A3DUns32 nTriStartIndex = cInFaceInfo.nOutTriStartIndex;
	A3DUns32 * pnIndices = cInFaceInfo.pnInIndices;

	A3DUns32 nStripesCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex++];

	UINT nFaceCount = 0;
	bool bSingleNormal = false;

	for (A3DUns32 nStripeIndex = 0; nStripeIndex < nStripesCount; nStripeIndex++) {
		if (cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex] & kA3DTessFaceDataNormalSingle) {
			bSingleNormal = true;
		}

		A3DUns32 nPointCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[nTriSizeIndex] & ~kA3DTessFaceDataNormalSingle;
		nTriSizeIndex++;

		A3DUns32 nLocalTriStartIndex = nTriStartIndex;

		if (!bSingleNormal) {
			for (A3DUns32 nIndex = 0; nIndex < nPointCount - 2; nIndex++) {
				if (!(nIndex % 2)) {
					nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
					nLocalTriStartIndex += 2;

					nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
					nLocalTriStartIndex += 2;

					nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
					nLocalTriStartIndex -= 2;
				}
				else {
					nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
					nLocalTriStartIndex += 2;

					nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
					nLocalTriStartIndex += 2;

					nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex + 1] / 3;
					nFaceNormalIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
					nLocalTriStartIndex -= 2;
				}

				cInShellKit.AddNextFaceWithDistinctNormals(m_pcPoints, m_pcNormals, nFaceLispnIndices, nFaceNormalIndices, 3);
				nFaceCount++;
			}

			nTriStartIndex += nPointCount * 2;
		}
		else
		{
			nLocalTriStartIndex++;
			for (A3DUns32 nIndex = 0; nIndex < nPointCount - 2; nIndex++) {
				if (!(nIndex % 2)) {
					nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[0] = pnIndices[nTriStartIndex] / 3;
					nLocalTriStartIndex++;

					nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[1] = pnIndices[nTriStartIndex] / 3;
					nLocalTriStartIndex++;

					nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[2] = pnIndices[nTriStartIndex] / 3;
					nLocalTriStartIndex--;
				}
				else {
					nFaceLispnIndices[2] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[2] = pnIndices[nTriStartIndex] / 3;
					nLocalTriStartIndex++;

					nFaceLispnIndices[1] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[1] = pnIndices[nTriStartIndex] / 3;
					nLocalTriStartIndex++;

					nFaceLispnIndices[0] = pnIndices[nLocalTriStartIndex] / 3;
					nFaceNormalIndices[0] = pnIndices[nTriStartIndex] / 3;
					nLocalTriStartIndex--;
				}

				cInShellKit.AddNextFaceWithDistinctNormals(m_pcPoints, m_pcNormals, nFaceLispnIndices, nFaceNormalIndices, 3);
				nFaceCount++;
			}

			nTriStartIndex += (nPointCount + 1);
		}
	}

	cInFaceInfo.nOutTriSizeIndex = nTriSizeIndex;
	cInFaceInfo.nOutTriStartIndex = nTriStartIndex;

	return nFaceCount;
}

UINT TdfImport::ConveTessFaceDataTriangleTextured(ConvertFaceInfo & cInFaceInfo)
{
	A3DUns32 nTextureCoordSize = 0;
	if (cInFaceInfo.pnIndices->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured) {
		nTextureCoordSize = cInFaceInfo.pnIndices->m_uiTextureCoordIndexesSize;
	}

	int nFaceLispnIndices[3]; // facelist indices into the "global" point array
	int nFaceVertexNormalIndices[3]; // vertex normal indices into the "global" normal array
	int nFaceVertexParamIndices[3]; // vertex parameter indices into the "global" parameter array
	int nFaceVertexColorIndices[3]; // vertex color indices into the RGBA color array

	A3DUns32 nTriangleCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];

	for (A3DUns32 tri = 0; tri < nTriangleCount; tri++)
	{
		A3DUns32 nNormalIndex = cInFaceInfo.nOutTriStartIndex++;

		nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nNormalIndex] / 3;
		nFaceVertexParamIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += nTextureCoordSize;
		nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		if (cInFaceInfo.aInColors.size() > 0) {
			nFaceVertexColorIndices[0] = cInFaceInfo.nOutTriColorIndex++;
		}

		nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		nFaceVertexParamIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += nTextureCoordSize;
		nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		if (cInFaceInfo.aInColors.size() > 0) {
			nFaceVertexColorIndices[1] = cInFaceInfo.nOutTriColorIndex++;
		}

		nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		nFaceVertexParamIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex];
		cInFaceInfo.nOutTriStartIndex += nTextureCoordSize;
		nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex++] / 3;
		if (cInFaceInfo.aInColors.size() > 0) {
			nFaceVertexColorIndices[2] = cInFaceInfo.nOutTriColorIndex++;
		}

		AddTriangle(cInFaceInfo, nFaceLispnIndices, nFaceVertexNormalIndices, nFaceVertexParamIndices, nFaceVertexColorIndices, (0 == nTextureCoordSize ? 0 : 2));
	}

	return nTriangleCount;
}

UINT TdfImport::ConveTessFaceDataTriangleTextured(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit)
{
	int nFaceLispnIndices[3]; // facelist indices into the "global" point array
	int nFaceNormalIndices[3]; // vertex normal indices into the "global" normal array
	int nFaceTextrueCoordIndices[3]; // vertex normal indices into the "global" texture coordinate array

	//int nFaceVertexParamIndices[3]; // vertex parameter indices into the "global" parameter array
	//int nFaceVertexColorIndices[3]; // vertex color indices into the RGBA color array

	A3DUns32 nTriangleCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];
	A3DUns32 nTextureCoordSize = cInFaceInfo.pnIndices->m_uiTextureCoordIndexesSize;
	A3DUns32 nAddCount = 2 + nTextureCoordSize;

	for (A3DUns32 nIndex = 0; nIndex < nTriangleCount; nIndex++)
	{
		nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1 + nTextureCoordSize] / 3;
		nFaceNormalIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		nFaceTextrueCoordIndices[0] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 2;
		cInFaceInfo.nOutTriStartIndex += nAddCount;

		nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1 + nTextureCoordSize] / 3;
		nFaceNormalIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		nFaceTextrueCoordIndices[1] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 2;
		cInFaceInfo.nOutTriStartIndex += nAddCount;

		nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1 + nTextureCoordSize] / 3;
		nFaceNormalIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex] / 3;
		nFaceTextrueCoordIndices[2] = cInFaceInfo.pnInIndices[cInFaceInfo.nOutTriStartIndex + 1] / 2;
		cInFaceInfo.nOutTriStartIndex += nAddCount;\

		cInShellKit.AddNextFaceWithDistinctNormalsAndTexture(m_pcPoints, m_pcNormals, m_pcTextureCoords, nFaceLispnIndices, nFaceNormalIndices, nFaceTextrueCoordIndices, 3);
	}

	return nTriangleCount;
}

UINT TdfImport::ConveTessFaceDataTriangleStripeTextured(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit)
{
	int nFaceLispnIndices[3];
	int nFaceVertexNormalIndices[3];
	int nFaceTextrueCoordIndices[3];

	A3DUns32 nTextureCoordSize = cInFaceInfo.pnIndices->m_uiTextureCoordIndexesSize;
	A3DUns32 nAddCount = 2 + nTextureCoordSize;

	UINT nTriangleCount = 0;

	int numstripes = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];
	for (int stripe = 0; stripe < numstripes; ++stripe)
	{
		A3DUns32 nPointCount = cInFaceInfo.pnIndices->m_puiSizesTriangulated[cInFaceInfo.nOutTriSizeIndex++];
		A3DUns32 nTriSizeIndex = cInFaceInfo.nOutTriSizeIndex;
		for (A3DUns32 nIndex = 0; nIndex < nPointCount - 2; ++nIndex)
		{
			if (!(nIndex % 2))
			{
				nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[nTriSizeIndex + nTextureCoordSize + 1] / 3;
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nTriSizeIndex] / 3;
				nFaceTextrueCoordIndices[0] = cInFaceInfo.pnInIndices[nTriSizeIndex + 1] / 2;
				nTriSizeIndex += nAddCount;

				nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[nTriSizeIndex + nTextureCoordSize + 1] / 3;
				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nTriSizeIndex] / 3;
				nFaceTextrueCoordIndices[1] = cInFaceInfo.pnInIndices[nTriSizeIndex + 1] / 2;
				nTriSizeIndex += nAddCount;

				nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[nTriSizeIndex + nTextureCoordSize + 1] / 3;
				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nTriSizeIndex] / 3;
				nFaceTextrueCoordIndices[2] = cInFaceInfo.pnInIndices[nTriSizeIndex + 1] / 2;
				nTriSizeIndex -= nAddCount;
			}
			else
			{
				nFaceLispnIndices[2] = cInFaceInfo.pnInIndices[nTriSizeIndex + nTextureCoordSize + 1] / 3;
				nFaceVertexNormalIndices[2] = cInFaceInfo.pnInIndices[nTriSizeIndex] / 3;
				nFaceTextrueCoordIndices[2] = cInFaceInfo.pnInIndices[nTriSizeIndex + 1] / 2;
				nTriSizeIndex += nAddCount;

				nFaceLispnIndices[1] = cInFaceInfo.pnInIndices[nTriSizeIndex + nTextureCoordSize + 1] / 3;
				nFaceVertexNormalIndices[1] = cInFaceInfo.pnInIndices[nTriSizeIndex] / 3;
				nFaceTextrueCoordIndices[1] = cInFaceInfo.pnInIndices[nTriSizeIndex + 1] / 2;
				nTriSizeIndex += nAddCount;

				nFaceLispnIndices[0] = cInFaceInfo.pnInIndices[nTriSizeIndex + nTextureCoordSize + 1] / 3;
				nFaceVertexNormalIndices[0] = cInFaceInfo.pnInIndices[nTriSizeIndex] / 3;
				nFaceTextrueCoordIndices[0] = cInFaceInfo.pnInIndices[nTriSizeIndex + 1] / 2;
				nTriSizeIndex -= nAddCount;
			}

			cInShellKit.AddNextFaceWithDistinctNormalsAndTexture(m_pcPoints, m_pcNormals, m_pcTextureCoords, nFaceLispnIndices, nFaceVertexNormalIndices, nFaceTextrueCoordIndices, 3);
			nTriangleCount++;
		}

		cInFaceInfo.nOutTriSizeIndex += nPointCount * nAddCount;
	}

	return nTriangleCount;
}

// 8-2-1. Tess3D Wire 처리
A3DStatus TdfImport::DrawTess3DWire(const A3DTess3DWire * pTess3DWire, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
	const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cInSegment)
{
	A3DEEntityType eType;
	CHECK_A3D_RETURN(A3DEntityGetType(pcRepItem, &eType));

	LogIncreaseTabIndex(2);

	Log(2, "DrawTess3DWire: %s, %s", LogHexStrA((DWORD_PTR)pTess3DWire), Dmi3dx::GetA3dEntityTypeString(eType));

	switch (eType)
	{
		// kA3DReadTessOnly Mode로 읽을 경우, kA3DTypeRiPolyWire로 바로 들어옴. Curve의 형식을 알수는 없음.
		case kA3DTypeRiPolyWire:
			return DrawPolyWires(pTess3DWire, pcTessBaseData, pcRepItem, pcParentAttr, cInSegment);
			break;

			/*
					case kA3DTypeRiCurve:
						assert(false);
						return A3D_ERROR;
					break;
			*/
	}

	A3DRiCurve * pcRiCurve = (A3DRiCurve *)pcRepItem;

	A3DRiCurveData cRiCurveData;
	A3D_INITIALIZE_DATA(A3DRiCurveData, cRiCurveData);
	A3DStatus nStatus = A3DRiCurveGet(pcRiCurve, &cRiCurveData);

	if (nullptr == cRiCurveData.m_pBody) {
		A3DRiCurveGet(nullptr, &cRiCurveData);
	}

	A3DTopoSingleWireBodyData cSingleWireBodyData;
	A3D_INITIALIZE_DATA(A3DTopoSingleWireBodyData, cSingleWireBodyData);
	nStatus = A3DTopoSingleWireBodyGet(cRiCurveData.m_pBody, &cSingleWireBodyData);

	if (nullptr != cSingleWireBodyData.m_pWireEdge) {
		A3DTopoWireEdgeData cWireEdgeData;
		A3D_INITIALIZE_DATA(A3DTopoWireEdgeData, cWireEdgeData);
		nStatus = A3DTopoWireEdgeGet(cSingleWireBodyData.m_pWireEdge, &cWireEdgeData);

		A3DEEntityType eEntityType = kA3DTypeUnknown;
		CHECK_A3D_RETURN(A3DEntityGetType(cWireEdgeData.m_p3dCurve, &eEntityType));

		Log(2, "3dCurve type: %s, %s", LogHexStrA((DWORD_PTR)cWireEdgeData.m_p3dCurve), Dmi3dx::GetA3dEntityTypeString(eEntityType));
	}

	SetLineStyle(pTess3DWire, cInSegment, pcParentAttr);

	const A3DDouble * pdCoord = &pcTessBaseData->m_pdCoords[0];

	A3DUns32 nPointCount = pcTessBaseData->m_uiCoordSize / 3;

	H3DF::PointArray acWirePoints;
	acWirePoints.resize(nPointCount);

	for (A3DUns32 nIndex = 0; nIndex < nPointCount; nIndex++)
	{
		acWirePoints[nIndex].x = pcTessBaseData->m_pdCoords[(nIndex * 3)];
		acWirePoints[nIndex].y = pcTessBaseData->m_pdCoords[(nIndex * 3) + 1];
		acWirePoints[nIndex].z = pcTessBaseData->m_pdCoords[(nIndex * 3) + 2];
	}

	cInSegment.InsertLine(acWirePoints.size(), acWirePoints.data());

	LogDecreaseTabIndex(2);

	return nStatus;
}

A3DStatus TdfImport::DrawPolyWires(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
	const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cInSegment)
{
	LogIncreaseTabIndex(2);

	Log(2, L"DrawPolyWires: %s", LogHexStr((DWORD_PTR)pcTess3D));

	LogDecreaseTabIndex(2);

	A3DTess3DWireData sWireData;
	A3D_INITIALIZE_DATA(A3DTess3DWireData, sWireData);
	CHECK_A3D_RETURN(A3DTess3DWireGet(pcTess3D, &sWireData));

	cInSegment.GetVisibilityControl().SetLines(true);

	if (nullptr == sWireData.m_puiSizesWires) {
		A3DUns32 nPointCount = pcTessBaseData->m_uiCoordSize / 3;
		H3DF::PointArray aPoints;
		aPoints.resize(nPointCount);

		A3DUns32 nPointIndex = 0;
		for (A3DUns32 nIndex = 0; nIndex < nPointCount; nIndex++) {
			nPointIndex = nIndex * 3;
			aPoints[nIndex].x = pcTessBaseData->m_pdCoords[nPointIndex];
			aPoints[nIndex].y = pcTessBaseData->m_pdCoords[nPointIndex + 1];
			aPoints[nIndex].z = pcTessBaseData->m_pdCoords[nPointIndex + 2];
		}

		cInSegment.InsertLine(aPoints.size(), aPoints.data());
	}
	else
	{
		A3DUns32 nIndex = 0;
		A3DUns32 nInfoIndex = 0;
		A3DUns32 nPointIndex = 0;
		bool bClosed = false;
		Point cPoint;

		H3DF::PointArray aPoints;

		while (nIndex < sWireData.m_uiSizesWiresSize)
		{
			nInfoIndex = sWireData.m_puiSizesWires[nIndex];

			if (nInfoIndex & kA3DTess3DWireDataIsClosing) {
				bClosed = true;
				nInfoIndex -= kA3DTess3DWireDataIsClosing;
			}

			if (!(nInfoIndex & kA3DTess3DWireDataIsContinuous)) //Continuious
			{
				if (0 < aPoints.size())
				{
					cInSegment.InsertLine(aPoints.size(), aPoints.data());
					aPoints.resize(0);
				}
			}
			else {
				nInfoIndex -= kA3DTess3DWireDataIsContinuous;
			}

			for (A3DUns32 i = 0; i < nInfoIndex; i++)
			{
				nPointIndex = sWireData.m_puiSizesWires[nIndex + 1 + i];
				cPoint.x = pcTessBaseData->m_pdCoords[nPointIndex];
				cPoint.y = pcTessBaseData->m_pdCoords[nPointIndex + 1];
				cPoint.z = pcTessBaseData->m_pdCoords[nPointIndex + 2];
				aPoints.push_back(cPoint);
			}

			if (bClosed)
			{
				nPointIndex = sWireData.m_puiSizesWires[nIndex + 1];
				cPoint.x = pcTessBaseData->m_pdCoords[nPointIndex];
				cPoint.y = pcTessBaseData->m_pdCoords[nPointIndex + 1];
				cPoint.z = pcTessBaseData->m_pdCoords[nPointIndex + 2];
				aPoints.push_back(cPoint);

				cInSegment.InsertLine(aPoints.size(), aPoints.data());
				aPoints.resize(0);
			}

			nIndex += nInfoIndex + 1;
		}
	}

	SetLineStyle(cInSegment, pcParentAttr);

	return A3D_SUCCESS;
}

// 8-2-3. Facet 갯수를 가져옴
A3DUns32 TdfImport::Tess3DDataGetNumberOfFacets(const A3DTess3DData * pcTess3DData)
{
	A3DUns32 ui;
	A3DUns32 uiNbFacets = 0;
	for (ui = 0; ui < pcTess3DData->m_uiFaceTessSize; ui++)
	{
		uiNbFacets += TessFaceDataGetNumberOfFacets(&pcTess3DData->m_psFaceTessData[ui]);
	}

	return uiNbFacets;
}

// 8-2-1-1. Facet 갯수를 가져옴
A3DUns32 TdfImport::TessFaceDataGetNumberOfFacets(const A3DTessFaceData * pcTessFaceData)
{
	A3DUns32 iNbFacet = 0;
	A3DUns32 iNumberIndex = 0;
	A3DUns32 iNbEntities = 0;
	A3DUns32 iStartId;
	A3DUns32 iId;

#define SIZE_TRIANGLE_ENTITY(index) ((index < pcTessFaceData->m_uiSizesTriangulatedSize ? \
	pcTessFaceData->m_puiSizesTriangulated[index] & kA3DTessFaceDataNormalMask : 0))

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangle)
	{
		iNbFacet += SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iNumberIndex++;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFan)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for (iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripe)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for (iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormal)
	{
		iNbFacet += SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iNumberIndex++;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormal)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for (iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormal)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for (iId = 0; iId < iNbEntities; iId++)
		{
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;
		}
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleTextured)
	{
		iNbFacet += SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iNumberIndex++;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanTextured)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for (iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeTextured)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for (iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleOneNormalTextured)
	{
		iNbFacet += SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iNumberIndex++;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleFanOneNormalTextured)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for (iId = 0; iId < iNbEntities; iId++)
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;

		iNumberIndex += iNbEntities + 1;
	}

	if (pcTessFaceData->m_usUsedEntitiesFlags & kA3DTessFaceDataTriangleStripeOneNormalTextured)
	{
		iNbEntities = SIZE_TRIANGLE_ENTITY(iNumberIndex);
		iStartId = iNumberIndex + 1;

		for (iId = 0; iId < iNbEntities; iId++)
		{
			iNbFacet += SIZE_TRIANGLE_ENTITY((iStartId + iId)) - 2;
		}
	}

#undef SIZE_TRIANGLE_ENTITY

	return iNbFacet;
}

// 8-3. Build Markup 
A3DStatus TdfImport::BuildMarkup(A3DTess3D * pcTess3d, A3DTessBaseData * pcTessBaseData, H3DF::SegmentKey & cParentSegment)
{
	A3DTessMarkupData sData;
	A3D_INITIALIZE_DATA(A3DTessMarkupData, sData);

	CHECK_A3D_RETURN(A3DTessMarkupGet(pcTess3d, &sData));

	PolylineArray aPolyline;
	StringArray aStrings;
	PMI::TextAttributesArray aTextAttributes;
	PMI::Options cOptions;
	PolygonArray aPolygons;

	GetMarkupTesselation(pcTessBaseData, &sData, aPolyline, aPolygons, aStrings, aTextAttributes, &cOptions);

	A3DTessMarkupGet(nullptr, &sData);

	return A3D_SUCCESS;
}

//== Utility =======================================================================================

// 7-10-1. Normal Index를 조정하는 함수
// Vertex는 중복이 되고 Normal Index는 각각 별도로 들어온다 (vertex index에서 찾도록 검사)
void TdfImport::MatchVertexNormal(TessIndexMap & maNormalIndexMap, A3DUns32 nVertexIndex, A3DUns32 & nNormalIndex)
{
	A3DUns32 nFindNormalIndex;
	if (true == maNormalIndexMap.Lookup(nVertexIndex, nFindNormalIndex)) {
		nNormalIndex = nFindNormalIndex;
	}
	else {
		maNormalIndexMap.SetAt(nVertexIndex, nNormalIndex);
	}
}

// 7-10-2. Point Index Map과 Normal Index Map, Point, Normal Index를 이용해서 MbTrianle Vector를 구성한다.
// 주어진 Triangle Vector에 생성된 Vector를 추가시킴.
bool TdfImport::ConvertFaceList(TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap,
	A3DUns32 * pnFacePointIndex, A3DUns32 * pnFaceNormalIndex, H3DF::IntArray & anFacelistArray, H3DF::IntArray & anVertexNoramlIndexArray)
{
	// Triangle을 구성할 Point Index
	A3DUns32 nPointIndex[3];

	for (int nIndex = 0; nIndex < 3; nIndex++) {
		// 사전에 저장된 Face Point Index를 찾는다. 찾은 경우 새롭게 생성되어지고 있는 Point Index를 이용해서
		// Face Index를 구성한다. 여기서 생성되는 Face Index는 개별 Face의 Index이기 때문에 0부터 시작하도록 구성한다.
		if (false == maPointIndexMap.Lookup(pnFacePointIndex[nIndex], nPointIndex[nIndex])) {
			A3DUns32 nCount = (A3DUns32)maPointIndexMap.GetCount();
			maPointIndexMap.SetAt(pnFacePointIndex[nIndex], nCount);
			nPointIndex[nIndex] = nCount;
		}
	}

	// Triangle을 구성할 Normal Index
	A3DUns32 nNormalIndex[3];

	for (int nIndex = 0; nIndex < 3; nIndex++) {
		// 사전에 저장된 Face Point Index를 찾는다. 찾은 경우 새롭게 생성되어지고 있는 Point Index를 이용해서
		// Face Index를 구성한다. 여기서 생성되는 Face Index는 개별 Face의 Index이기 때문에 0부터 시작하도록 구성한다.
		if (false == maNormalIndexMap.Lookup(pnFaceNormalIndex[nIndex], nNormalIndex[nIndex])) {
			A3DUns32 nCount = (A3DUns32)maNormalIndexMap.GetCount();
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

void TdfImport::AddTriangle(ConvertFaceInfo & cInFaceInfo,
	int const pnInFaceLispnIndices[3],			// facelist indices into the "global" point array for this triangle
	int const pnInFaceVertexNromalIndices[3],	// vertex normal indices into the "global" normal array for this triangle
	int const pnInFaceVertexParamIndices[3],	// optional vertex parameter indices into the "global" parameter array for this triangle
	int const pnInFaceVertexColorIndices[3],	// optional vertex color indices into the RGBA color array for this triangle
	A3DUns32  nInVertexParamSize)				// vertex parameter size (0 if no parameters)
{
	if (pnInFaceLispnIndices[0] == pnInFaceLispnIndices[1] || pnInFaceLispnIndices[1] == pnInFaceLispnIndices[2] || pnInFaceLispnIndices[0] == pnInFaceLispnIndices[2]) {
		return;
	}

	// Face List 숫자 추가
	cInFaceInfo.aOutFaceList.push_back(3);

	H3DF::Vector cNormal;
	bool bZeroLengthFlag = false;

	double dLength0 = m_pcNormals[pnInFaceVertexNromalIndices[0]].LengthSquared();
	double dLength1 = m_pcNormals[pnInFaceVertexNromalIndices[1]].LengthSquared();
	double dLength2 = m_pcNormals[pnInFaceVertexNromalIndices[2]].LengthSquared();

	if (0.01 > dLength0 || 0.01 > dLength1 || 0.01 > dLength2) {
		bZeroLengthFlag = true;
		H3DF::Vector cX = m_pcPoints[pnInFaceLispnIndices[1]] - m_pcPoints[pnInFaceLispnIndices[0]];
		H3DF::Vector cY = m_pcPoints[pnInFaceLispnIndices[2]] - m_pcPoints[pnInFaceLispnIndices[0]];

		cNormal = cX.Cross(cY);
		cNormal.Normalize();
	}

	for (int nVertexIndex = 0; nVertexIndex < 3; nVertexIndex++)
	{
		// Two cases:
		// (1) Either we haven't seen this vertex before (or we have but our normal, if present, or vertex parameter, if present, 
		//     or vertex color, if present, is different), in which case we need to add this point, possibly normal, possibly vertex parameter
		//     and possibly vertex color to our "local" arrays and note the mapping between the "global" index and the new "local" index; OR
		// (2) We have seen this vertex before and it has the same normals if present (and vertex parameter if present, and vertex color if present)
		//     as a previous encounter, in which case we can reuse the "local" index.

		auto cOutIndexIterator = cInFaceInfo.mOutIndexMap[pnInFaceLispnIndices[nVertexIndex]].cbegin();

		while (true)
		{
			// Index Map에서 FaceListIndex를 발견하지 못한 경우 처리
			if (cOutIndexIterator == cInFaceInfo.mOutIndexMap[pnInFaceLispnIndices[nVertexIndex]].cend())
			{
				cInFaceInfo.aOutFacePoints.push_back(m_pcPoints[pnInFaceLispnIndices[nVertexIndex]]);
				cInFaceInfo.aOutFaceList.push_back(static_cast<int>(cInFaceInfo.aOutFacePoints.size() - 1));

				if (0 < m_nNormalCount) {
					if (false == bZeroLengthFlag) {
						cInFaceInfo.aOutFaceVertexNormals.push_back(m_pcNormals[pnInFaceVertexNromalIndices[nVertexIndex]]);
					}
					else {
						cInFaceInfo.aOutFaceVertexNormals.push_back(cNormal);
					}
				}

				if (0 < nInVertexParamSize) {
					for (A3DUns32 k = 0; k < nInVertexParamSize; k++) {
						cInFaceInfo.aOutFaceVertexParams.push_back(cInFaceInfo.aInParams[pnInFaceVertexParamIndices[nVertexIndex] + k]);
					}
				}

				if (0 < cInFaceInfo.aInColors.size()) {
					cInFaceInfo.aOutFaceVertexColors.push_back(cInFaceInfo.aInColors[pnInFaceVertexColorIndices[nVertexIndex]]);
				}

				cInFaceInfo.mOutIndexMap[pnInFaceLispnIndices[nVertexIndex]].push_back(static_cast<int>(cInFaceInfo.aOutFacePoints.size() - 1));
				cInFaceInfo.aOutVertexRefs.push_back(1);

				break;
			}
			// Index Map에서 FaceListIndex를 발견한 경우 처리
			else if (false == bZeroLengthFlag)
			{
				if (0 == m_nNormalCount ||
					cInFaceInfo.aOutFaceVertexNormals[*cOutIndexIterator].Dot(m_pcNormals[pnInFaceVertexNromalIndices[nVertexIndex]]) >= cInFaceInfo.fInNormalCosine)
				{
					bool bMatchingFlag = true;
					for (A3DUns32 k = 0; k < nInVertexParamSize; k++) {
						if (cInFaceInfo.aOutFaceVertexParams[*cOutIndexIterator + k] != cInFaceInfo.aInParams[pnInFaceVertexParamIndices[nVertexIndex] + k])
						{
							bMatchingFlag = false;
							break;
						}
					}

					if (cInFaceInfo.aInColors.size() > 0) {
						if (cInFaceInfo.aOutFaceVertexColors[*cOutIndexIterator] != cInFaceInfo.aInColors[pnInFaceVertexColorIndices[nVertexIndex]]) {
							bMatchingFlag = false;
						}
					}

					if (true == bMatchingFlag)
					{
						// 찾은 Index를 이용해서 Face List를 추가한다.
						cInFaceInfo.aOutFaceList.push_back(*cOutIndexIterator);
						cInFaceInfo.aOutVertexRefs[*cOutIndexIterator] += 1;

						if (0 < m_nNormalCount)
						{
							int ref_count = cInFaceInfo.aOutVertexRefs[*cOutIndexIterator];

							cInFaceInfo.aOutFaceVertexNormals[*cOutIndexIterator] = (ref_count - 1) / (float)ref_count * cInFaceInfo.aOutFaceVertexNormals[*cOutIndexIterator] +
								m_pcNormals[pnInFaceVertexNromalIndices[nVertexIndex]] / (float)ref_count;

							cInFaceInfo.aOutFaceVertexNormals[*cOutIndexIterator].Normalize();
						}
						break;
					}
				} // if(m_pcNormals.empty() ||
			} // else if(false == bZeroLengthFlag)

			cOutIndexIterator++;
		}
	}
}

// 8. Face Draw Style 정의
A3DStatus TdfImport::SetFaceStyle(const A3DRootBaseWithGraphics * pcBase, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcBase, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DStatus eStatus = SetFaceStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus TdfImport::SetFaceStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributesData cAttrsData;
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, cAttrsData);
	A3DMiscCascadedAttributesGet(pcParentAttr, &cAttrsData);

	A3DStatus eStatus = SetFaceStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus TdfImport::SetFaceStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData)
{
	return SetFaceStyle(cSegment, cAttrsData.m_sStyle);
}

A3DStatus TdfImport::SetFaceStyle(H3DF::SegmentKey & cSegment, const A3DGraphStyleData cStyleData)
{
	SegmentKey cStyleSegment;
	if (true == FindMaterialMapping(cStyleData, cStyleSegment)) {
		SetStyle(cSegment, cStyleSegment);
	}
	else {
		H3DF::MaterialKit cMaterial;
		if (A3D_SUCCESS == GetMaterial(cStyleData, cMaterial)) {
			SetFaceMaterialMapping(cStyleData, cMaterial, cSegment);
		}
	}

	return A3D_SUCCESS;
}

A3DStatus TdfImport::SetLineStyle(const A3DRootBaseWithGraphics * pcBase, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcBase, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DStatus eStatus = SetLineStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus TdfImport::SetLineStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributesData cAttrsData;
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, cAttrsData);
	A3DMiscCascadedAttributesGet(pcParentAttr, &cAttrsData);

	A3DStatus eStatus = SetLineStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus TdfImport::SetLineStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData)
{
	SegmentKey cStyleSegment;
	if (true == FindLineMaterialMapping(cAttrsData, cStyleSegment)) {
		SetStyle(cSegment, cStyleSegment);
	}
	else {
		H3DF::MaterialKit cMaterial;
		if (A3D_SUCCESS == GetMaterial(cAttrsData.m_sStyle, cMaterial)) {
			SetLineMaterialMapping(cAttrsData, cMaterial, cSegment);
		}
	}

	return A3D_SUCCESS;
}

A3DStatus TdfImport::SetMarkerStyle(const A3DRootBaseWithGraphics * pcBase, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributes * pcAttrs;
	A3DMiscCascadedAttributesData cAttrsData;
	CreateAndPushCascadedAttributes(pcBase, pcParentAttr, &pcAttrs, &cAttrsData);

	A3DStatus eStatus = SetMarkerStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesDelete(pcAttrs);
	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus TdfImport::SetMarkerStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr)
{
	A3DMiscCascadedAttributesData cAttrsData;
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, cAttrsData);
	A3DMiscCascadedAttributesGet(pcParentAttr, &cAttrsData);

	A3DStatus eStatus = SetMarkerStyle(cSegment, cAttrsData);

	A3DMiscCascadedAttributesGet(nullptr, &cAttrsData);

	return eStatus;
}

A3DStatus TdfImport::SetMarkerStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData)
{
	SegmentKey cStyleSegment;
	if (true == FindMarkerMaterialMapping(cAttrsData, cStyleSegment)) {
		SetStyle(cSegment, cStyleSegment);
	}
	else {
		H3DF::MaterialKit cMaterial;
		if (A3D_SUCCESS == GetMaterial(cAttrsData.m_sStyle, cMaterial)) {
			SetMarkerMaterialMapping(cAttrsData, cMaterial, cSegment);
		}
	}

	return A3D_SUCCESS;
}

A3DStatus TdfImport::GetMaterial(const A3DMiscCascadedAttributesData & cAttrsData, A3DInt32 * pnUVCoordinatesIndex,
	A3DUns8 * pucTextureDimension, H3DF::MaterialKit & cMaterialKit)
{
	const A3DGraphStyleData * pcStyleData = &cAttrsData.m_sStyle;

	if (nullptr == pcStyleData) {
		return A3D_ERROR;
	}

	LogIncreaseTabIndex(2);

	A3DStatus nRetStatus = A3D_SUCCESS;

	bool bTransparencyDefined = (1 == cAttrsData.m_sStyle.m_bIsTransparencyDefined) ? true : false;
	float fTransparency = cAttrsData.m_sStyle.m_ucTransparency / 255.0f;

	if (A3D_TRUE == pcStyleData->m_bMaterial)
	{
		A3DDouble adColor[4] = { 0.0, 0.0, 0.0, 1.0 };

		A3DGraphMaterialData materialdata;
		A3D_INITIALIZE_DATA(A3DGraphMaterialData, materialdata);

		nRetStatus = A3DGlobalGetGraphMaterialData(pcStyleData->m_uiRgbColorIndex, &materialdata);
		A3DBool bMaterialIsTexture = A3D_FALSE;
		A3DGlobalIsMaterialTexture(pcStyleData->m_uiRgbColorIndex, &bMaterialIsTexture);

		if (A3D_TRUE == bMaterialIsTexture)
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

			CStringA strTextureName;
			strTextureName.Format("texture_%d", sTextureAppData.m_uiTextureDefinitionIndex);
			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);

			CStringA strTextureOption;
			strTextureOption.Format("source = image %u", sTextureData.m_uiPictureIndex);

			if (sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				strTextureOption.Append(", parameterization source = reflection vector");
			}
			else {
				strTextureOption.Append(", parameterization source = uv");
			}

			if (sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				cMaterialKit.SetMirror(strTextureName, cDiffuseColor);
			}
			else {
				cMaterialKit.SetDiffuseTexture(strTextureName, cDiffuseColor);
				cMaterialKit.SetDiffuseTextureOption(strTextureOption);
			}

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

			H3DF::RGBAColor cDiffuseColor(sTextureDefinitionData.m_dRed, sTextureDefinitionData.m_dGreen,
				sTextureDefinitionData.m_dBlue, sTextureDefinitionData.m_dAlpha);*/

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
			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dDiffuseAlpha);
			if (true == bTransparencyDefined) { cDiffuseColor.alpha = fTransparency; }

			cMaterialKit.SetDiffuseColor(cDiffuseColor);

			//Log(2, L"DrawStyle: DiffuseColor R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);
/*
			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiAmbient, &sRgbColorData));
			H3DF::RGBAColor cAmbientColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dAmbientAlpha);
			cMaterialKit.SetFaceColor(cAmbientColor, H3DF::Material::Color::Channel::);
*/

//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiEmissive, &sRgbColorData));
			H3DF::RGBAColor cEmissiveColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dEmissiveAlpha);
			if (true == bTransparencyDefined) { cEmissiveColor.alpha = fTransparency; }
			cMaterialKit.SetEmission(cEmissiveColor);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiSpecular, &sRgbColorData));
			H3DF::RGBAColor cSpecularColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dSpecularAlpha);
			if (true == bTransparencyDefined) { cSpecularColor.alpha = fTransparency; }
			cMaterialKit.SetSpecular(cSpecularColor);

			// 			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			// 			nRetStatus = A3DGlobalGetGraphMaterialData(A3D_DEFAULT_MATERIAL_INDEX, &sMaterialData);

			cMaterialKit.SetGloss(sMaterialData.m_dShininess);
		}
	}
	else
	{
		// m_uiRgbColorIndex : A global index to either an `A3DGraphMaterialData` or an `A3DGraphRgbColorData` according to the value of `m_bMaterial`.
		if (A3D_DEFAULT_COLOR_INDEX != pcStyleData->m_uiRgbColorIndex) {
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(pcStyleData->m_uiRgbColorIndex, &sRgbColorData));

			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);
			cMaterialKit.SetDiffuseColor(cDiffuseColor);

			//Log(2, L"DrawStyle: R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
		}
	}

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

// 8-1. 일반 DrawStyle 정의 
A3DStatus TdfImport::GetMaterial(const A3DGraphStyleData & cInStyleData, H3DF::MaterialKit & cOutMaterial)
{
	LogIncreaseTabIndex(2);

	bool bTransparencyDefined = (1 == cInStyleData.m_bIsTransparencyDefined) ? true : false;
	float fTransparency = cInStyleData.m_ucTransparency / 255.0f;

	A3DStatus nRetStatus = A3D_SUCCESS;

	if (A3D_TRUE == cInStyleData.m_bMaterial)
	{
		A3DGraphMaterialData cMaterialData;
		A3D_INITIALIZE_DATA(A3DGraphMaterialData, cMaterialData);

		nRetStatus = A3DGlobalGetGraphMaterialData(cInStyleData.m_uiRgbColorIndex, &cMaterialData);

		A3DBool bMaterialIsTexture = A3D_FALSE;
		A3DGlobalIsMaterialTexture(cInStyleData.m_uiRgbColorIndex, &bMaterialIsTexture);

		if (A3D_TRUE == bMaterialIsTexture)
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);
			nRetStatus = A3DGlobalGetGraphRgbColorData(cMaterialData.m_uiDiffuse, &sRgbColorData);

			A3DGraphTextureApplicationData sTextureAppData;
			A3D_INITIALIZE_DATA(A3DGraphTextureApplicationData, sTextureAppData);
			A3DGlobalGetGraphTextureApplicationData(cInStyleData.m_uiRgbColorIndex, &sTextureAppData);

			A3DGraphTextureDefinitionData sTextureData;
			A3D_INITIALIZE_DATA(A3DGraphTextureDefinitionData, sTextureData);
			A3DGlobalGetGraphTextureDefinitionData(sTextureAppData.m_uiTextureDefinitionIndex, &sTextureData);

			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);

			CStringA strTextureName;
			strTextureName.Format("texture_%d", sTextureAppData.m_uiTextureDefinitionIndex);

			if (sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				cOutMaterial.SetMirror(strTextureName, cDiffuseColor);
			}
			else {
				cOutMaterial.SetDiffuseTexture(strTextureName, cDiffuseColor);
			}

/*
			if(sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				strTextureName.Format(L"environment = texture_%d, mirror = (r = 0.5 g = 0.5 b = 0.5))",
					sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sTextureAppData.m_uiTextureDefinitionIndex);
			}
			else {
				strTextureName.Format(L"environment = texture_%d",
					sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sTextureAppData.m_uiTextureDefinitionIndex);
			}
*/

			CStringA strTextureOption;
			strTextureOption.Format("source = image %u", sTextureData.m_uiPictureIndex);

			if (sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				strTextureOption.Append(", parameterization source = reflection vector");
			}
			else {
				strTextureOption.Append(", parameterization source = uv");
			}

			cOutMaterial.SetDiffuseTextureOption(strTextureOption);
		}
		else
		{
			// 변수 초기화
			A3DGraphMaterialData sMaterialData;
			A3D_INITIALIZE_DATA(A3DGraphMaterialData, sMaterialData);
			// Color Index를 이용해서 Color 정보 수집
			CHECK_A3D_RETURN(A3DGlobalGetGraphMaterialData(cInStyleData.m_uiRgbColorIndex, &sMaterialData));

			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);
			// Diffuse Color 정보 수집
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiDiffuse, &sRgbColorData));

			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dDiffuseAlpha);
			if (true == bTransparencyDefined) { 
				cDiffuseColor.alpha = fTransparency; 
			}
			cOutMaterial.SetDiffuseColor(cDiffuseColor);

			//Log(2, L"DrawStyle: DiffuseColor R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);
/*
			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiAmbient, &sRgbColorData));
			H3DF::RGBAColor cAmbientColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dAmbientAlpha);
			cMaterialKit.SetFaceColor(cAmbientColor, H3DF::Material::Color::Channel::);
*/

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiEmissive, &sRgbColorData));
			H3DF::RGBAColor cEmissiveColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dEmissiveAlpha);
			if (true == bTransparencyDefined) { cEmissiveColor.alpha = fTransparency; }
			cOutMaterial.SetEmission(cEmissiveColor);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiSpecular, &sRgbColorData));
			H3DF::RGBAColor cSpecularColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dSpecularAlpha);
			if (true == bTransparencyDefined) { cSpecularColor.alpha = fTransparency; }
			cOutMaterial.SetSpecular(cSpecularColor);

			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			nRetStatus = A3DGlobalGetGraphMaterialData(A3D_DEFAULT_MATERIAL_INDEX, &sMaterialData);
			cOutMaterial.SetGloss(sMaterialData.m_dShininess);
		}
	}
	else
	{
		// m_uiRgbColorIndex : A global index to either an `A3DGraphMaterialData` or an `A3DGraphRgbColorData` according to the value of `m_bMaterial`.
		if (A3D_DEFAULT_COLOR_INDEX != cInStyleData.m_uiRgbColorIndex)
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(cInStyleData.m_uiRgbColorIndex, &sRgbColorData));

			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);
			if (true == bTransparencyDefined) { cDiffuseColor.alpha = fTransparency; }
			cOutMaterial.SetDiffuseColor(cDiffuseColor);

			//Log(2, L"DrawStyle: R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);

			A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
		}
	}

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

bool TdfImport::FindMaterial(const A3DGraphStyleData & cInStyleData, H3DF::MaterialKit & cOutMaterial)
{
	CStringA strStyleText;

	if (false == cInStyleData.m_bMaterial) {
		strStyleText.Format("%d_%d", cInStyleData.m_uiRgbColorIndex, cInStyleData.m_ucTransparency);
	}
	else {
		strStyleText.Format("mat_%d_%d", cInStyleData.m_uiRgbColorIndex, cInStyleData.m_ucTransparency);
	}
		
	if (true == m_mMaterialMap.Lookup(strStyleText, cOutMaterial)) {
		return true;
	}

	return false;
}

bool TdfImport::CreateMaterial(const A3DGraphStyleData & cInStyleData, H3DF::MaterialKit & cOutMaterial)
{
	H3DF::MaterialKit cMaterialKit;
	if (A3D_SUCCESS == GetMaterial(cInStyleData, cMaterialKit)) {
		CStringA strStyleText;
		if (false == cInStyleData.m_bMaterial) {
			strStyleText.Format("%d_%d", cInStyleData.m_uiRgbColorIndex, cInStyleData.m_ucTransparency);
		}
		else {
			strStyleText.Format("mat_%d_%d", cInStyleData.m_uiRgbColorIndex, cInStyleData.m_ucTransparency);
		}
		

		m_mMaterialMap.SetAt(strStyleText, cMaterialKit);
		cOutMaterial = cMaterialKit;

		return true;
	}

	return false;
}

// 9. Context Scale 구하는 함수
bool TdfImport::ParseTopoContextScale(const A3DTopoBody * pcBody, double & dTopoContextScale)
{
	// 아래쪽에서 문제가 생겨서 값을 가져올 수 없는 경우를 대비해서 Scale값을 1.0을 저장한다.
	dTopoContextScale = 1.0;

	// Scale을 구하기 위해서 Context Data를 가져온다.
	A3DTopoBodyData cTopoBodyData;
	A3D_INITIALIZE_DATA(A3DTopoBodyData, cTopoBodyData);
	A3DStatus nResult = A3DTopoBodyGet(pcBody, &cTopoBodyData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	A3DTopoContextData cTopoContextData;
	A3D_INITIALIZE_DATA(A3DTopoContextData, cTopoContextData);
	nResult = A3DTopoContextGet(cTopoBodyData.m_pContext, &cTopoContextData);
	if (A3D_SUCCESS != nResult) {
		A3DTopoBodyGet(nullptr, &cTopoBodyData);
		return false;
	}

	if (A3D_TRUE == cTopoContextData.m_bHaveScale) {
		dTopoContextScale = cTopoContextData.m_dScale;
	}

	A3DTopoBodyGet(nullptr, &cTopoBodyData);
	A3DTopoContextGet(nullptr, &cTopoContextData);

	return true;
}

// 20. Draw용 Transforamtion을 계산하는 부분
A3DStatus TdfImport::DrawTransformation(const A3DMiscTransformation * pcTransformation)
{
	if (nullptr == pcTransformation) {
		return A3D_SUCCESS;
	}

	A3DEEntityType eType = kA3DTypeUnknown;
	A3DEntityGetType(pcTransformation, &eType);

	switch (eType)
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

A3DStatus TdfImport::PopulateTextures(H3DF::SegmentKey & cSegment)
{
	// 	InitializeMagick(".");
	// 
	// 	HC_Set_Visibility("image = off");

	A3DGlobal * pcGlobal = nullptr;
	A3DGlobalGetPointer(&pcGlobal);

	A3DGlobalData globalData;
	A3D_INITIALIZE_DATA(A3DGlobalData, globalData);

	A3DGlobalGet(pcGlobal, &globalData);

	A3DGraphPictureData cPictureData;
	A3D_INITIALIZE_DATA(A3DGraphPictureData, cPictureData);

	if (0 == globalData.m_uiPicturesSize) {
		return A3D_SUCCESS;
	}

	//cSegment.Open();

	H3DF::MaterialMappingControl cMappingControl = cSegment.GetMaterialMappingControl();
	cMappingControl.InitPopulateTextures();

	// This while loop is loading all the images.
	//VArray<unsigned char> pixels;

	for (UINT nIndex = 0; nIndex < globalData.m_uiPicturesSize; nIndex++)
	{
		CHECK_A3D_RETURN(A3DGlobalGetGraphPictureData(nIndex, &cPictureData));

		bool bSizeOk = (cPictureData.m_uiPixelWidth != 0 && cPictureData.m_uiPixelHeight != 0);

		if (bSizeOk && cPictureData.m_eFormat == kA3DPictureBitmapRgbaByte) {
			InvertImage(cPictureData.m_pucBinaryData, cPictureData.m_uiPixelWidth, cPictureData.m_uiPixelHeight, true);
			cMappingControl.InsertPicture(nIndex, cPictureData.m_uiPixelWidth, cPictureData.m_uiPixelHeight, cPictureData.m_pucBinaryData);
			//HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgba, name = image %u, local = on", nIndex), cPictureData.m_uiPixelWidth, cPictureData.m_uiPixelHeight, cPictureData.m_pucBinaryData);
		}
		else if (bSizeOk && cPictureData.m_eFormat == kA3DPictureBitmapRgbByte) {
			InvertImage(cPictureData.m_pucBinaryData, cPictureData.m_uiPixelWidth, cPictureData.m_uiPixelHeight, false);
			cMappingControl.InsertPicture(nIndex, cPictureData.m_uiPixelWidth, cPictureData.m_uiPixelHeight, cPictureData.m_pucBinaryData);
			//HC_Insert_Image(0.0, 0.0, 0.0, H_FORMAT_TEXT("rgb, name = image %u, local = on", nIndex), cPictureData.m_uiPixelWidth, cPictureData.m_uiPixelHeight, cPictureData.m_pucBinaryData);
		}
		else {
			cMappingControl.InsertDifaultPicture(nIndex, cPictureData.m_uiSize, cPictureData.m_pucBinaryData);
		}
	}

/*
	for(A3DUns32 nIndex = 0; nIndex < globalData.m_uiTextureDefinitionsSize; nIndex++)
	{
		Query::GraphTextureDefinition cTextureDefinition(nIndex);
		if(!cTextureDefinition.IsValid()) {
			continue;
		}

		A3DGraphTextureDefinitionData const & cData = cTextureDefinition.data;

		CString strTextureOptions;
		strTextureOptions.Format(L"source = image %u", cData.m_uiPictureIndex);

		if(cData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection)
		{
			strTextureOptions.Append(L", parameterization source = reflection vector");
		}
		else
		{
			strTextureOptions.Append(L", parameterization source = uv");
		}

		strTextureOptions.Append(cData.m_eTextureFunction == kA3DTextureFunctionModulate ? L", modulate" : L", no modulate");

		CString strTiling;
		switch(cData.m_eTextureWrappingModeS)
		{
			case kA3DTextureWrappingModeMirroredRepeat:
			{
				strTiling = L"mirror";
			}	break;

			case kA3DTextureWrappingModeClamp:
			case kA3DTextureWrappingModeClampToBorder:
			case kA3DTextureWrappingModeClampToEdge:
			{
				strTiling = L"drop";
			}	break;

			case kA3DTextureWrappingModeUnknown:
			case kA3DTextureWrappingModeRepeat:
			default:
			{
				strTiling = L"repeat";
			}
		}

		CString strText;
		strText.Format(L", tiling = %s", strTiling);
		strTextureOptions.Append(strText);

		if(cData.m_pTextureTransfo != nullptr)
		{
			A3DGraphTextureTransformationData transformationData;
			A3D_INITIALIZE_DATA(A3DGraphTextureTransformationData, transformationData);
			A3DGraphTextureTransformationGet(cData.m_pTextureTransfo, &transformationData);
			float pfTextureMatrix[16] = { 0 };
			for(int i = 0; i < 16; ++i) {
				pfTextureMatrix[i] = static_cast<float>(transformationData.m_dMatrix[i]);
			}

			char pchTextureTransformSegment[4096] = "";
			cMappingControl.SetTextureMatrix(pfTextureMatrix, pchTextureTransformSegment);

// 			HC_Compute_Matrix_Inverse(textureMatrix, textureMatrix);
//
// 			char textureTransformSegment[4096] = "";
// 			HC_Open_Segment("/include library/texture_transformations");
// 			{
// 				HC_Open_Segment("");
// 				{
// 					HC_Show_Segment(HC_Create_Segment("."), textureTransformSegment);
// 					HC_Set_Texture_Matrix(textureMatrix);
// 				}
// 				HC_Close_Segment();
// 			}
// 			HC_Close_Segment();

			//strText.Format(L", transform = %s", textureTransformSegment);
			//strTextureOptions.Append(strText);
		}

		strTextureOptions.Append(L", decal, interpolation filter = on, no decimation filter, no downsampling, parameter offset = 0");

		strText.Format(L"texture_%u", nIndex);
		cMappingControl.SetDefineLocalTexture(nIndex, strTextureOptions);
		//HC_Define_Local_Texture(H_FORMAT_TEXT("texture_%u", ui), strTextureOptions);
	}
*/

	cMappingControl.EndPopulateTextures();

	//cSegment.Close();

	return A3D_SUCCESS;
}

A3DStatus TdfImport::GetTextureMapping(const A3DMiscCascadedAttributesData & cAttrsData, H3DF::MaterialKit & cMaterialKit)
{
	const A3DGraphStyleData * pcStyleData = &cAttrsData.m_sStyle;

	if (pcStyleData == nullptr) {
		return A3D_ERROR;
	}

	LogIncreaseTabIndex(2);

	bool bTransparencyDefined = (1 == cAttrsData.m_sStyle.m_bIsTransparencyDefined) ? true : false;
	float fTransparency = cAttrsData.m_sStyle.m_ucTransparency / 255.0f;

	A3DStatus nRetStatus = A3D_SUCCESS;

	if (A3D_TRUE == pcStyleData->m_bMaterial)
	{
		A3DGraphMaterialData cMaterialData;
		A3D_INITIALIZE_DATA(A3DGraphMaterialData, cMaterialData);

		nRetStatus = A3DGlobalGetGraphMaterialData(pcStyleData->m_uiRgbColorIndex, &cMaterialData);
		A3DBool bMaterialIsTexture = A3D_FALSE;
		A3DGlobalIsMaterialTexture(pcStyleData->m_uiRgbColorIndex, &bMaterialIsTexture);

		if (A3D_TRUE == bMaterialIsTexture)
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);
			nRetStatus = A3DGlobalGetGraphRgbColorData(cMaterialData.m_uiDiffuse, &sRgbColorData);

			A3DGraphTextureApplicationData sTextureAppData;
			A3D_INITIALIZE_DATA(A3DGraphTextureApplicationData, sTextureAppData);
			A3DGlobalGetGraphTextureApplicationData(pcStyleData->m_uiRgbColorIndex, &sTextureAppData);

			A3DGraphTextureDefinitionData sTextureData;
			A3D_INITIALIZE_DATA(A3DGraphTextureDefinitionData, sTextureData);
			A3DGlobalGetGraphTextureDefinitionData(sTextureAppData.m_uiTextureDefinitionIndex, &sTextureData);

			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);

			CStringA strTextureName;
			strTextureName.Format("texture_%d", sTextureAppData.m_uiTextureDefinitionIndex);

			if (sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				cMaterialKit.SetMirror(strTextureName, cDiffuseColor);
			}
			else {
				cMaterialKit.SetDiffuseTexture(strTextureName, cDiffuseColor);
			}

/*
			if(sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				strTextureName.Format(L"environment = texture_%d, mirror = (r = 0.5 g = 0.5 b = 0.5))",
					sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sTextureAppData.m_uiTextureDefinitionIndex);
			}
			else {
				strTextureName.Format(L"environment = texture_%d",
					sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sTextureAppData.m_uiTextureDefinitionIndex);
			}
*/

			CStringA strTextureOption;
			strTextureOption.Format("source = image %u", sTextureData.m_uiPictureIndex);

			if (sTextureData.m_uiMappingAttributes & kA3DTextureMappingSphericalReflection) {
				strTextureOption.Append(", parameterization source = reflection vector");
			}
			else {
				strTextureOption.Append(", parameterization source = uv");
			}

			cMaterialKit.SetDiffuseTextureOption(strTextureOption);
		}
		else
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);

			A3DGraphMaterialData sMaterialData;
			A3D_INITIALIZE_DATA(A3DGraphMaterialData, sMaterialData);

			CHECK_A3D_RETURN(A3DGlobalGetGraphMaterialData(pcStyleData->m_uiRgbColorIndex, &sMaterialData));
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiDiffuse, &sRgbColorData));
			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dDiffuseAlpha);
			if (true == bTransparencyDefined) { cDiffuseColor.alpha = fTransparency; }
			cMaterialKit.SetDiffuseColor(cDiffuseColor);

			//Log(2, L"DrawStyle: DiffuseColor R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);
/*
			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiAmbient, &sRgbColorData));
			H3DF::RGBAColor cAmbientColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dAmbientAlpha);
			cMaterialKit.SetFaceColor(cAmbientColor, H3DF::Material::Color::Channel::);
*/

//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiEmissive, &sRgbColorData));
			H3DF::RGBAColor cEmissiveColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dEmissiveAlpha);
			if (true == bTransparencyDefined) { cEmissiveColor.alpha = fTransparency; }
			cMaterialKit.SetEmission(cEmissiveColor);

			//nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(sMaterialData.m_uiSpecular, &sRgbColorData));
			H3DF::RGBAColor cSpecularColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue, sMaterialData.m_dSpecularAlpha);
			if (true == bTransparencyDefined) { cSpecularColor.alpha = fTransparency; }
			cMaterialKit.SetSpecular(cSpecularColor);

			// 			nRetStatus = A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
			// 			nRetStatus = A3DGlobalGetGraphMaterialData(A3D_DEFAULT_MATERIAL_INDEX, &sMaterialData);
			cMaterialKit.SetGloss(sMaterialData.m_dShininess);
		}
	}
	else
	{
		// m_uiRgbColorIndex : A global index to either an `A3DGraphMaterialData` or an `A3DGraphRgbColorData` according to the value of `m_bMaterial`.
		if (A3D_DEFAULT_COLOR_INDEX != pcStyleData->m_uiRgbColorIndex)
		{
			A3DGraphRgbColorData sRgbColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sRgbColorData);
			CHECK_A3D_RETURN(A3DGlobalGetGraphRgbColorData(pcStyleData->m_uiRgbColorIndex, &sRgbColorData));

			H3DF::RGBAColor cDiffuseColor(sRgbColorData.m_dRed, sRgbColorData.m_dGreen, sRgbColorData.m_dBlue);
			if (true == bTransparencyDefined) { cDiffuseColor.alpha = fTransparency; }
			cMaterialKit.SetDiffuseColor(cDiffuseColor);

			//Log(2, L"DrawStyle: R:%f, G:%f, B:%f, A:%f", cDiffuseColor.red, cDiffuseColor.green, cDiffuseColor.blue, cDiffuseColor.alpha);

			//A3DGlobalGetGraphRgbColorData(A3D_DEFAULT_COLOR_INDEX, &sRgbColorData);
		}
	}

	LogDecreaseTabIndex(2);

	return A3D_SUCCESS;
}

A3DStatus TdfImport::SetTextureMapping(H3DF::SegmentKey cSegment, A3DMiscCascadedAttributesData & cAttrData)
{
	H3DF::MaterialKit cMaterial;
	if (A3D_SUCCESS != GetMaterial(cAttrData.m_sStyle, cMaterial)) {
		return A3D_ERROR;
	}

	H3DF::MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetFaceMaterial(cMaterial);

	cSegment.SetMaterialMapping(cMaterialMapping);

	return A3D_SUCCESS;
}

void TdfImport::InvertImage(unsigned char * imagebuffer, int width, int height, bool rgba)
{
	unsigned char * row = new unsigned char[width * 4];

	for (int y = 0; y < height / 2; y++)
	{
		int w;
		if (rgba)
			w = 4;
		else
			w = 3;

		memcpy(row, imagebuffer + (width * (height - 1 - y) * w), width * w);
		memcpy(imagebuffer + (width * (height - 1 - y) * w), imagebuffer + (width * w * y),
			width * w);
		memcpy(imagebuffer + (width * w * y), row, width * w);
	}
}

//== Attribute 관련 함수 =============================================================================

// 1-1. Parent에서 받은(계단식으로) Attribute를 이용해서, Attribute를 생성
A3DStatus TdfImport::CreateAndPushCascadedAttributes(const A3DRootBaseWithGraphics * pcBase, const A3DMiscCascadedAttributes * pcParentAttr,
	A3DMiscCascadedAttributes ** ppcAttr, A3DMiscCascadedAttributesData * pcAttrData)
{
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesCreate(ppcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesPush(*ppcAttr, pcBase, pcParentAttr));

	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, (*pcAttrData));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesGet(*ppcAttr, pcAttrData));

	return A3D_SUCCESS;
}

// 1-2. Parent에서 받은(계단식으로) Attribute를 이용해서, Attribute를 생성, Data는 생성하지 않음.
A3DStatus TdfImport::CreateAndPushCascadedAttributes(const A3DRootBaseWithGraphics * pcBase, const A3DMiscCascadedAttributes * pcParentAttr,
	A3DMiscCascadedAttributes ** ppcAttr)
{
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesCreate(ppcAttr));
	CHECK_A3D_RETURN(A3DMiscCascadedAttributesPush(*ppcAttr, pcBase, pcParentAttr));

	return A3D_SUCCESS;
}

// 2. Tess Face용 Attribute 생성
A3DStatus TdfImport::CreateAndPushCascadedAttributesTessFace(const A3DRiRepresentationItem * pcRepItem, const A3DTessBase * pcTessBase,
	const A3DTessFaceData * pcTessFaceData, A3DUns32 nFaceIndex, const A3DMiscCascadedAttributes * pcParentAttribute,
	A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData)
{
	A3DStatus nStatus = A3DMiscCascadedAttributesPushTessFace(*pcAttrs, pcRepItem, pcTessBase, pcTessFaceData, nFaceIndex, pcParentAttribute);
	
	A3D_INITIALIZE_DATA(A3DMiscCascadedAttributesData, (*pcAttributesData));
	nStatus = A3DMiscCascadedAttributesGet(*pcAttrs, pcAttributesData);

	return nStatus;
}

// 4. Show 상태를 확인
A3DStatus TdfImport::IsShow(const A3DRootBaseWithGraphics * pGraphics)
{
	if (pGraphics == nullptr) {
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
	if (sRootData.m_pGraphics) {
		bShow = (sGraphicsData.m_usBehaviour & kA3DGraphicsShow) != 0;
	}
	else {
		bShow = A3D_TRUE;
	}

	CHECK_A3D_RETURN(A3DGraphicsGet(nullptr, &sGraphicsData));
	CHECK_A3D_RETURN(A3DRootBaseWithGraphicsGet(nullptr, &sRootData));

	if (A3D_TRUE == bShow) {
		return A3D_SUCCESS;
	}

	return A3D_ERROR;
}

// 5. 주어진 Material Mapping을 이용해서 StylesIncludeKey에 새로운 Style을 추가하고 주어진 Segment에 적용.
bool TdfImport::SetFaceMaterialMapping(const A3DGraphStyleData & sStyleData, H3DF::MaterialKit const & cInKit, H3DF::SegmentKey & cSegment)
{
	CStringA strStyleText;

	if (false == sStyleData.m_bMaterial) {
		strStyleText.Format("%d_%d", sStyleData.m_uiRgbColorIndex, sStyleData.m_ucTransparency);
	}
	else {
		strStyleText.Format("mat_%d_%d", sStyleData.m_uiRgbColorIndex, sStyleData.m_ucTransparency);
	}

	H3DF::SegmentKey cStyleSegment = m_cIncludeStyles.Subsegment(strStyleText);

	// 입력된 Matrial을 Face에 적용한다.
	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetFaceMaterial(cInKit);

	cStyleSegment.SetMaterialMapping(cMaterialMapping);

	H3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

	// 	if(INVALID_KEY == cStyle.KeyValue()) { 
	// 		return false;
	// 	}

	m_mMaterialMappingStyleMap.SetAt(strStyleText, cStyleSegment);

	return true;
}

bool TdfImport::SetLineMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, H3DF::MaterialKit const & cInKit, H3DF::SegmentKey & cSegment)
{
	RGBAColor cDiffuseColor;
	if (false == cInKit.ShowDiffuseColor(cDiffuseColor)) {
		return false;
	}

	H3DF::SegmentKey cStyleSegment = m_cIncludeStyles.Subsegment("line_mat_%d", cAttrData.m_sStyle.m_uiRgbColorIndex);

	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetLineColor(cDiffuseColor);

	cStyleSegment.SetMaterialMapping(cMaterialMapping);

	H3DF::StyleControl cStyleControl = cSegment.GetStyleControl();
	H3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

	// line pattern 관련 설정
	A3DUns32 nLinePatternIndex = cAttrData.m_sStyle.m_uiLinePatternIndex;
	A3DGraphLinePatternData cLinePatternData;
	A3D_INITIALIZE_DATA(A3DGraphLinePatternData, cLinePatternData);
	if (A3D_SUCCESS == A3DGlobalGetGraphLinePatternData(nLinePatternIndex, &cLinePatternData)) {
		bool bStatus = false;
		H3DF::LinePattern::Default eLineType = H3DF::LinePattern::Default::None;
		switch (cLinePatternData.m_uiNumberOfLengths)
		{
			case 2:
			{
				double d1 = cLinePatternData.m_pdLengths[0];
				double d2 = cLinePatternData.m_pdLengths[1];
				if (0.0 != d2)
				{
					if (fabs(d1 / d2 - 1) < 1e-3) {
						eLineType = H3DF::LinePattern::Default::Dotted; // HC_Set_Line_Pattern("...");	// 짧은 점선
					}
					else {
						eLineType = H3DF::LinePattern::Default::Dashed; // HC_Set_Line_Pattern("- -");	// 긴 점선
					}
				}
			}
			break;

			case 4:
				eLineType = H3DF::LinePattern::Default::DashDot; // HC_Set_Line_Pattern("-.");	// 일점쇄선
				break;

			default:
			{
				if (cLinePatternData.m_uiNumberOfLengths > 4) {
					eLineType = H3DF::LinePattern::Default::Dash2Dot; // HC_Set_Line_Pattern("-..");	// 이점쇄선
				}
			}
			break;
		}

		if (H3DF::LinePattern::Default::None != eLineType) {

			switch (eLineType)
			{
				case H3DF::LinePattern::Default::None:
				case H3DF::LinePattern::Default::Solid:
					break;

				case H3DF::LinePattern::Default::DashDot:
					cStyleSegment.GetLineAttributeControl().SetPattern("-.");
					break;

				case H3DF::LinePattern::Default::Dashed:
					cStyleSegment.GetLineAttributeControl().SetPattern("- -");
					break;

				case H3DF::LinePattern::Default::Dotted:
					cStyleSegment.GetLineAttributeControl().SetPattern("...");
					break;

				case H3DF::LinePattern::Default::Dash2Dot:
					cStyleSegment.GetLineAttributeControl().SetPattern("-..");
					break;

				default:
					DEBUG_STOP;
					break;
			}
		}
	}

	m_mLineMaterialMappingStyleMap.insert(std::make_pair(cAttrData.m_sStyle.m_uiRgbColorIndex, cStyleSegment));

	return true;
}

bool TdfImport::SetMarkerMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, H3DF::MaterialKit const & cInKit, H3DF::SegmentKey & cSegment)
{
	RGBAColor cDiffuseColor;
	if (false == cInKit.ShowDiffuseColor(cDiffuseColor)) {
		return false;
	}

	H3DF::SegmentKey cStyleSegment = m_cIncludeStyles.Subsegment("marker_mat_%d", cAttrData.m_sStyle.m_uiRgbColorIndex);

	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetMarkerColor(cDiffuseColor);

	cStyleSegment.SetMaterialMapping(cMaterialMapping);

	H3DF::StyleControl cStyleControl = cSegment.GetStyleControl();
	H3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

	// 	if(INVALID_KEY == cStyle.KeyValue()) {
	// 		return false;
	// 	}

	m_mMarkerMaterialMappingStyleMap.insert(std::make_pair(cAttrData.m_sStyle.m_uiRgbColorIndex, cStyleSegment));

	return true;
}

// 5-1. 주어진 스타일을 이용해서 새로운 Face Style Segment를 생성
// 각각의 Style은 line, face 별로 적용할 수 있기 때문에, 별도로 생성한다.
bool TdfImport::CreateFaceStyleSegment(const A3DGraphStyleData & sStyleData, H3DF::MaterialKit const & cInKit, H3DF::SegmentKey & cOutStyleSegment)
{
	CStringA strStyleText;

	if (false == sStyleData.m_bMaterial) {
		strStyleText.Format("%d_%d", sStyleData.m_uiRgbColorIndex, sStyleData.m_ucTransparency);
	}
	else {
		strStyleText.Format("mat_%d_%d", sStyleData.m_uiRgbColorIndex, sStyleData.m_ucTransparency);
	}

	cOutStyleSegment = m_cIncludeStyles.Subsegment(strStyleText);

	// 입력된 Matrial을 Face에 적용한다.
	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetFaceMaterial(cInKit);

	cOutStyleSegment.SetMaterialMapping(cMaterialMapping);

	m_mMaterialMappingStyleMap.SetAt(strStyleText, cOutStyleSegment);

	return true;
}

// 5-2. 주어진 Material Mapping을 이용해서 
bool TdfImport::SetStyle(H3DF::SegmentKey & cSegment, H3DF::SegmentKey & cStyleSegment)
{
	H3DF::StyleKey cStyle = cSegment.GetStyleControl().PushSegment(cStyleSegment);

	if (INVALID_KEY == cStyle.KeyValue()) {
		return false;
	}

	return true;
}

// 6. 미리 저장되어 있는 Material Mapping Kit을 찾아오는 함수
bool TdfImport::FindMaterialMapping(const A3DGraphStyleData & sStyleData, SegmentKey & cOutStyleSegment)
{
	CStringA strStyleText;

	if (false == sStyleData.m_bMaterial) {
		strStyleText.Format("%d_%d", sStyleData.m_uiRgbColorIndex, sStyleData.m_ucTransparency);
	}
	else {
		strStyleText.Format("mat_%d_%d", sStyleData.m_uiRgbColorIndex, sStyleData.m_ucTransparency);
	}

	if (true == m_mMaterialMappingStyleMap.Lookup(strStyleText, cOutStyleSegment)) {
		return true;
	}

	return false;
}

bool TdfImport::FindLineMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, SegmentKey & cOutStyleSegment)
{
	auto pcIterator = m_mLineMaterialMappingStyleMap.find(cAttrData.m_sStyle.m_uiRgbColorIndex);

	if (pcIterator == m_mLineMaterialMappingStyleMap.end()) {
		return false;
	}

	cOutStyleSegment = pcIterator->second;

	return true;
}

bool TdfImport::FindMarkerMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, SegmentKey & cOutStyleSegment)
{
	auto pcIterator = m_mMarkerMaterialMappingStyleMap.find(cAttrData.m_sStyle.m_uiRgbColorIndex);

	if (pcIterator == m_mMarkerMaterialMappingStyleMap.end()) {
		return false;
	}

	cOutStyleSegment = pcIterator->second;

	return true;
}

bool TdfImport::FindMaterialMapping(CString strGeometry, H3DF::MaterialMappingKit const & cInKit, SegmentKey & cOutStyleSegment)
{
	for (auto & cMaterialStyle : m_vcMaterialMappingStyleVector) {
		if (0 == cMaterialStyle.strGeometry.CompareNoCase(strGeometry)) {
			if (cMaterialStyle.cMaterialMappingKit == cInKit) {
				cOutStyleSegment = cMaterialStyle.cStyleSegment;
				return true;
			}
		}
	}

	return false;
}

// == C3D 관련 Utility 함수 =========================================================================

// 1. 3DX Location을 C3D Matrix로 변환해서 돌려줌
A3DStatus TdfImport::GetMatrix(A3DMiscTransformation * pcLocation, MbMatrix3D & cMatrix)
{
	if (nullptr == pcLocation) {
		return A3D_ERROR;
	}

	A3DEEntityType eType;
	A3DEntityGetType(pcLocation, &eType);

	if (eType == kA3DTypeMiscCartesianTransformation)
	{
		A3DMiscCartesianTransformationData sData;
		A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, sData);

		CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(pcLocation, &sData));

		MbCartPoint3D cOrigin = GetMbCartPoint3D(sData.m_sOrigin);
		MbVector3D cAxisX = GetMbVector3D(sData.m_sXVector);
		MbVector3D cAxisY = GetMbVector3D(sData.m_sYVector);
		MbVector3D cAxisZ;

		if (sData.m_ucBehaviour & kA3DTransformationMirror) {
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
	else if (eType == kA3DTypeMiscGeneralTransformation)
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

A3DStatus TdfImport::GetMatrix(A3DMiscTransformation * pcLocation, H3DF::MatrixKit & cOutMatrix)
{
	if (nullptr == pcLocation) {
		return A3D_ERROR;
	}

	A3DEEntityType eType;
	A3DEntityGetType(pcLocation, &eType);

	if (eType == kA3DTypeMiscCartesianTransformation)
	{
		A3DMiscCartesianTransformationData sData;
		A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, sData);

		CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(pcLocation, &sData));

		H3DF::Vector cXAxis(sData.m_sXVector.m_dX, sData.m_sXVector.m_dY, sData.m_sXVector.m_dZ);
		H3DF::Vector cYAxis(sData.m_sYVector.m_dX, sData.m_sYVector.m_dY, sData.m_sYVector.m_dZ);
		H3DF::Vector cZAxis;
		H3DF::Point cOrigin(sData.m_sOrigin.m_dX, sData.m_sOrigin.m_dY, sData.m_sOrigin.m_dZ);

		if (sData.m_ucBehaviour & kA3DTransformationMirror) {
			cZAxis = cYAxis.Cross(cXAxis);
		}
		else {
			cZAxis = cXAxis.Cross(cYAxis);
		}

		cXAxis *= sData.m_sScale.m_dX;
		cYAxis *= sData.m_sScale.m_dY;
		cZAxis *= sData.m_sScale.m_dZ;

		cOutMatrix.SetXAxis(cXAxis);
		cOutMatrix.SetYAxis(cYAxis);
		cOutMatrix.SetZAxis(cZAxis);
		cOutMatrix.SetOrigin(cOrigin);

		CHECK_A3D_RETURN(A3DMiscCartesianTransformationGet(nullptr, &sData));
	}
	else if (eType == kA3DTypeMiscGeneralTransformation)
	{
		A3DMiscGeneralTransformationData cTransformationData;
		A3D_INITIALIZE_DATA(A3DMiscGeneralTransformationData, cTransformationData);

		CHECK_A3D_RETURN(A3DMiscGeneralTransformationGet(pcLocation, &cTransformationData));

		cOutMatrix.SetXAxis(cTransformationData.m_adCoeff[0], cTransformationData.m_adCoeff[1], cTransformationData.m_adCoeff[2]);
		cOutMatrix.SetYAxis(cTransformationData.m_adCoeff[4], cTransformationData.m_adCoeff[5], cTransformationData.m_adCoeff[6]);
		cOutMatrix.SetZAxis(cTransformationData.m_adCoeff[8], cTransformationData.m_adCoeff[9], cTransformationData.m_adCoeff[10]);
		cOutMatrix.SetOrigin(cTransformationData.m_adCoeff[12], cTransformationData.m_adCoeff[13], cTransformationData.m_adCoeff[14]);

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

//== 계산 관련 함수 ===================================================================================

void TdfImport::MatrixMatrixMult(double m[16], const double o[16])
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

void TdfImport::LoadMatrixIdentity()
{
	memset(m_dMatrixStack[m_nMatrixPos], 0, 16 * sizeof(double));
	m_dMatrixStack[m_nMatrixPos][0] = 1.0;
	m_dMatrixStack[m_nMatrixPos][5] = 1.0;
	m_dMatrixStack[m_nMatrixPos][10] = 1.0;
	m_dMatrixStack[m_nMatrixPos][15] = 1.0;
}

void TdfImport::MultMatrix(const double m[16])
{
	MatrixMatrixMult(m_dMatrixStack[m_nMatrixPos], m);
}

void TdfImport::PushMatrix()
{
	if (m_nMatrixPos < 7)
	{
		m_nMatrixPos++;
		memcpy(m_dMatrixStack[m_nMatrixPos], m_dMatrixStack[m_nMatrixPos - 1], 16 * sizeof(double));
	}
}

void TdfImport::PopMatrix()
{
	if (m_nMatrixPos > 0)
	{
		m_nMatrixPos--;
	}
}

void TdfImport::GetMatrix(double m[16])
{
	memcpy(m, m_dMatrixStack[m_nMatrixPos], 16 * sizeof(double));
}

void TdfImport::VectorCross(const A3DVector3dData * X, const A3DVector3dData * Y, A3DVector3dData * Z)
{
	Z->m_dX = X->m_dY * Y->m_dZ - X->m_dZ * Y->m_dY;
	Z->m_dY = X->m_dZ * Y->m_dX - X->m_dX * Y->m_dZ;
	Z->m_dZ = X->m_dX * Y->m_dY - X->m_dY * Y->m_dX;
}

void TdfImport::VectorMatrixMult(A3DDouble dX, A3DDouble dY, A3DDouble dZ, double adMatrix[16],
	A3DDouble * pdResX, A3DDouble * pdResY, A3DDouble * pdResZ)
{
	*pdResX = adMatrix[0] * dX + adMatrix[4] * dY + adMatrix[8] * dZ + adMatrix[12];
	*pdResY = adMatrix[1] * dX + adMatrix[5] * dY + adMatrix[9] * dZ + adMatrix[13];
	*pdResZ = adMatrix[2] * dX + adMatrix[6] * dY + adMatrix[10] * dZ + adMatrix[14];
}

void TdfImport::AllocVector3dArray(A3DVector3dData ** ppacArray, A3DUns32 uiSize)
{
	A3DUns32 ui;
	*ppacArray = (A3DVector3dData *)malloc(uiSize * sizeof(A3DVector3dData));
	for (ui = 0; ui < uiSize; ui++)
		A3D_INITIALIZE_DATA(A3DVector3dData, ((*ppacArray)[ui]));
}

void TdfImport::AllocNormalsAndPoints(A3DVector3dData ** ppasNormals, A3DUns32 uiNormalSize, A3DVector3dData ** ppasPoints,
	A3DUns32 uiPointSize)
{
	AllocVector3dArray(ppasNormals, uiNormalSize);
	AllocVector3dArray(ppasPoints, uiPointSize);
}

void TdfImport::BoundingBoxAddPoint(A3DBoundingBoxData * e, double x, double y, double z)
{
	if (e == nullptr)
		return;

	if (x < e->m_sMin.m_dX) e->m_sMin.m_dX = x;
	if (y < e->m_sMin.m_dY) e->m_sMin.m_dY = y;
	if (z < e->m_sMin.m_dZ) e->m_sMin.m_dZ = z;
	if (x > e->m_sMax.m_dX) e->m_sMax.m_dX = x;
	if (y > e->m_sMax.m_dY) e->m_sMax.m_dY = y;
	if (z > e->m_sMax.m_dZ) e->m_sMax.m_dZ = z;
}

//== 변수 Pointer 관리 함수 ===========================================================================
void TdfImport::PointerArrayInitialize(A3DPointerArray * pcArray)
{
	if (pcArray == nullptr)
		return;

	pcArray->m_uiAllocated = 0;
	pcArray->m_uiSize = 0;
	pcArray->m_ppPointers = nullptr;
}

void TdfImport::PointerArrayFree(A3DPointerArray * pcArray)
{
	pcArray->m_uiAllocated = 0;
	pcArray->m_uiSize = 0;
	free(pcArray->m_ppPointers);
}

void TdfImport::PointerArrayTerminate(A3DPointerArray * pcArray)
{
	PointerArrayFree(pcArray);
}

void * TdfImport::MiscRealloc(void * p, A3DUns32 uiOldSize, A3DUns32 uiNewSize)
{
	void * newp = malloc(uiNewSize);
	memcpy(newp, p, uiOldSize);
	free(p);

	return newp;
}

UINT TdfImport::PointerArrayAdd(A3DPointerArray * pcArray, void * pcPointer)
{
	if (pcArray == nullptr)
		return 0;

	if (pcArray->m_uiAllocated == 0)
	{
		pcArray->m_uiAllocated = 2;
		pcArray->m_ppPointers = (void **)malloc(pcArray->m_uiAllocated * sizeof(void *));
	}

	if (pcArray->m_uiSize == pcArray->m_uiAllocated)
	{
		pcArray->m_uiAllocated *= 2;
		pcArray->m_ppPointers = (void **)MiscRealloc(pcArray->m_ppPointers, pcArray->m_uiSize * sizeof(void *),
			pcArray->m_uiAllocated * sizeof(void *));
	}

	pcArray->m_ppPointers[pcArray->m_uiSize] = pcPointer;
	pcArray->m_uiSize++;

	return pcArray->m_uiSize;
}

int TdfImport::PointerArrayFind(A3DPointerArray * pcArray, void * pcPointer)
{
	for (UINT ui = 0; ui < pcArray->m_uiSize; ui++)
	{
		if (pcArray->m_ppPointers[ui] == pcPointer)
			return ui;
	}

	return -1;
}

UINT TdfImport::PointerArrayAddUnique(A3DPointerArray * pcArray, void * pcPointer)
{
	if (pcArray == nullptr) {
		return 0;
	}

	if (PointerArrayFind(pcArray, pcPointer) != -1)
		return pcArray->m_uiSize;

	return PointerArrayAdd(pcArray, pcPointer);
}

UINT TdfImport::PointerArrayAddArray(A3DPointerArray * pcArray, void ** const ppcPointers, UINT uiSize)
{
	for (UINT ui = 0; ui < uiSize; ui++) {
		PointerArrayAdd(pcArray, ppcPointers[ui]);
	}

	return pcArray->m_uiSize;
}

//== Log 관련 함수 ===================================================================================
void TdfImport::CreateLog(int nId, const WCHAR * pchFilePathName)
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

void TdfImport::Log(int nId, LPCWSTR chMessage, ...)
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

void TdfImport::Log(int nId, LPCSTR chMessage, ...)
{
#ifdef USED_LOG_MANAGER
	va_list cArgList;
	va_start(cArgList, chMessage);

	CStringA strBuffer;
	strBuffer.FormatV(chMessage, cArgList);

	va_end(cArgList);

	CString strText(strBuffer);
	LogManager::GetInstance()->WriteLog(nId, strText);
#endif
}

void TdfImport::LogIncreaseTabIndex(int nId)
{
#ifdef USED_LOG_MANAGER
	LogManager::IncreaseTabIndex(nId);
#endif
}

void TdfImport::LogDecreaseTabIndex(int nId)
{
#ifdef USED_LOG_MANAGER
	LogManager::DecreaseTabIndex(nId);
#endif
}

CString TdfImport::LogHexStr(DWORD_PTR nValue)
{
#ifdef USED_LOG_MANAGER
	if (0 == nValue) {
		return L"NULL";
	}

	return LogManager::HexStr(nValue);
#endif
	return L"";
}

CStringA TdfImport::LogHexStrA(DWORD_PTR nValue)
{
#ifdef USED_LOG_MANAGER
	if(0 == nValue) {
		return "NULL";
	}

	CStringA strValue(LogManager::HexStr(nValue));
	return strValue;
#endif
	return "";
}

CString TdfImport::LogBoolStr(bool bValue)
{
#ifdef USED_LOG_MANAGER
	return LogManager::BoolStr(bValue);
#endif
	return L"";
}

CStringA TdfImport::LogBoolStrA(bool bValue)
{
#ifdef USED_LOG_MANAGER
	CStringA strValue(LogManager::BoolStr(bValue));
	return strValue;
#endif
	return "";
}

void TdfImport::LogVector3dData(A3DVector3dData & cData, CStringA strPrompt)
{
	if (true == strPrompt.IsEmpty()) {
		Log(2, "VectorData: %f, %f, %f", cData.m_dX, cData.m_dY, cData.m_dZ);
	}
	else {
		Log(2, "%s: %f, %f, %f", strPrompt, cData.m_dX, cData.m_dY, cData.m_dZ);
	}
}

void TdfImport::LogMkpViewData(A3DMkpView * pcView, A3DMkpViewData & cViewData)
{
	CString strName;
	GetName(pcView, strName);

	Log(2, L"MkpView: %s, '%s'", LogHexStr((DWORD_PTR)pcView), strName);

	LogIncreaseTabIndex(2); {
		LogSurfPlane(cViewData.m_pPlane);

		// SceneDisplayParameters 정보 수집
		if (nullptr != cViewData.m_pSceneDisplayParameters) {
			Log(2, "Scene Display Parameters Information");

			LogIncreaseTabIndex(2); {
/*
				typedef struct
				{
					A3DUns16        m_usStructSize;                  //   Reserved: will be initialized by \ref A3D_INITIALIZE_DATA.
					A3DBool         m_bIsActive;                     ///< Indicates whether the scene is enabled or not.
					A3DGraphCamera * m_pCamera;                       ///< Camera information.
					A3DUns32        m_uiLightSize;                   ///< The number of lights
					A3DEntity ** m_ppLights;                      ///< Array of lights
					A3DUns32        m_uiPlaneSize;                   ///< The number of clipping planes.
					A3DSurfPlane ** m_ppClippingPlanes;              ///< The clipping planes
					A3DBool         m_bHasRotationCenter;            ///< Indicates whether the scene as a rotation center or not.
					A3DVector3dData m_sRotationCenter;               ///< The rotation center.
					A3DUns32        m_uiBackgroundStyleIndex;        ///< A style index for background color and rendering mode.
					A3DUns32        m_uiDefaultStyleIndex;           ///< A default style index.
					A3DUns32        m_uiDefaultPerTypeIndexSize;     ///< The number of type specific style indexes.
					A3DUns32 * m_puiDefaultStyleIndexesPerType; ///< An array of default style indexes per types referenced to in `m_puiTypesOfDefaultStyleIndexes`
					A3DEEntityType * m_puiTypesOfDefaultStyleIndexes; ///< An array of types

				} A3DGraphSceneDisplayParametersData;
*/

				A3DGraphSceneDisplayParametersData sData;
				A3D_INITIALIZE_DATA(A3DGraphSceneDisplayParametersData, sData);
				if (A3D_SUCCESS == A3DGraphSceneDisplayParametersGet(cViewData.m_pSceneDisplayParameters, &sData)) {
					Log(2, "IsActive: %s", LogBoolStrA(sData.m_bIsActive));
					LogGraphCamera(sData.m_pCamera);
					Log(2, "LightSize: %d", sData.m_uiLightSize);
					Log(2, "PlaneSize: %d", sData.m_uiPlaneSize);
					for (A3DUns32 nIndex = 0; nIndex < sData.m_uiPlaneSize; nIndex++) {
						LogSurfPlane(sData.m_ppClippingPlanes[nIndex]);
					}
					Log(2, "HasRotationCenter: %s", LogBoolStrA(sData.m_bHasRotationCenter));
					LogVector3dData(sData.m_sRotationCenter, "RotationCenter");
					Log(2, "BackgroundStyleIndex: %d", sData.m_uiBackgroundStyleIndex);
					Log(2, "DefaultStyleIndex: %d", sData.m_uiDefaultStyleIndex);
					Log(2, "DefaultPerTypeIndexSize: %d", sData.m_uiDefaultPerTypeIndexSize);
				}

				A3DGraphSceneDisplayParametersGet(nullptr, &sData);
			} LogDecreaseTabIndex(2);
		}

		if (A3D_TRUE == cViewData.m_bIsAnnotationView) {
			Log(2, "Annotation View Type");
		}

		if (A3D_TRUE == cViewData.m_bIsDefaultView) {
			Log(2, "Default View Type");
		}

		Log(2, "Linked Items Size: %d", cViewData.m_uiLinkedItemsSize);

		Log(2, "Display Filter Size: %d", cViewData.m_uiDisplayFilterSize);

	} LogDecreaseTabIndex(2);
}

void TdfImport::LogSurfPlane(A3DSurfPlane * pcInSurfPlane)
{
	if (nullptr == pcInSurfPlane) {
		return;
	}

	Log(2, "SurfPlane Information: %s", LogHexStrA((DWORD_PTR)pcInSurfPlane));

	A3DSurfPlaneData cData;
	A3D_INITIALIZE_DATA(A3DSurfPlaneData, cData);

	if (A3D_SUCCESS == A3DSurfPlaneGet(pcInSurfPlane, &cData)) {
		LogIncreaseTabIndex(2); {
			Log(2, "Origin: %f, %f, %f", cData.m_sTrsf.m_sOrigin.m_dX, cData.m_sTrsf.m_sOrigin.m_dY, cData.m_sTrsf.m_sOrigin.m_dZ);
			Log(2, "X Axis: %f, %f, %f", cData.m_sTrsf.m_sXVector.m_dX, cData.m_sTrsf.m_sXVector.m_dY, cData.m_sTrsf.m_sXVector.m_dZ);
			Log(2, "Y Axis: %f, %f, %f", cData.m_sTrsf.m_sYVector.m_dX, cData.m_sTrsf.m_sYVector.m_dY, cData.m_sTrsf.m_sYVector.m_dZ);

			CStringA strBehaviourInfo;
			if (kA3DTransformationIdentity & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Identity ";
			}
			if (kA3DTransformationTranslate & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Translate ";
			}
			if (kA3DTransformationRotate & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Rotate ";
			}
			if (kA3DTransformationMirror & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Mirror ";
			}
			if (kA3DTransformationScale & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "Scale ";
			}
			if (kA3DTransformationNonUniformScale & cData.m_sTrsf.m_ucBehaviour) {
				strBehaviourInfo += "NonUniformScale ";
			}

			Log(2, "Scale: %f, Behaviour: %s", cData.m_sTrsf.m_sScale, strBehaviourInfo);

			// Log(2, "Min Domain: %f, %f", sSurfPlaneData.m_sParam.m_sUVDomain.m_sMin.m_dX, sSurfPlaneData.m_sParam.m_sUVDomain.m_sMin.m_dY);
			// Log(2, "Max Domain: %f, %f", sSurfPlaneData.m_sParam.m_sUVDomain.m_sMax.m_dX, sSurfPlaneData.m_sParam.m_sUVDomain.m_sMax.m_dY);

			Log(2, L"SwapUV: %s", LogBoolStr(cData.m_sParam.m_bSwapUV));

			// Log(2, "CoeffA: %f, %f", sSurfPlaneData.m_sParam.m_dUCoeffA, sSurfPlaneData.m_sParam.m_dVCoeffA);
			// Log(2, "CoeffB: %f, %f", sSurfPlaneData.m_sParam.m_dUCoeffB, sSurfPlaneData.m_sParam.m_dVCoeffB);
		} LogDecreaseTabIndex(2);

		A3DSurfPlaneGet(nullptr, &cData);
	}
}

void TdfImport::LogGraphCamera(A3DGraphCamera * pcInCamera)
{
	if (nullptr == pcInCamera) {
		return;
	}

	Log(2, "Graph Camera Information: %s", LogHexStrA((DWORD_PTR)pcInCamera));

	LogIncreaseTabIndex(2);
	A3DGraphCameraData sCameraData;
	A3D_INITIALIZE_DATA(A3DGraphCameraData, sCameraData);

	if (A3D_SUCCESS == A3DGraphCameraGet(pcInCamera, &sCameraData)) {
		LogIncreaseTabIndex(2); {
			Log(2, "Orthographic: %s", LogBoolStrA(sCameraData.m_bOrthographic));
			LogVector3dData(sCameraData.m_sLocation, "Location");
			LogVector3dData(sCameraData.m_sLookAt, "LookAt");
			LogVector3dData(sCameraData.m_sUp, "Up");
			Log(2, "Fovy: %f, %f", sCameraData.m_dXFovy, sCameraData.m_dYFovy);
			Log(2, "AspectRatio: %f", sCameraData.m_dAspectRatio);
			Log(2, "ZNear: %f, ZFar: %f, ZoomFactor: %f", sCameraData.m_dZNear, sCameraData.m_dZFar, sCameraData.m_dZoomFactor);
		} LogDecreaseTabIndex(2);

		A3DGraphCameraGet(nullptr, &sCameraData);
	}

	LogDecreaseTabIndex(2);
}

/*
typedef struct
{
	A3DUns16 m_usStructSize;								// Reserved: will be initialized by \ref A3D_INITIALIZE_DATA.
	A3DBool m_bMarkupShowControl;							/ *!< Show/hide markup follows pointed entity. * /
	A3DBool m_bMarkupDeleteControl;							/ *!< Delete markup follows pointed entity. * /
	A3DBool m_bLeaderShowControl;							/ *!< Show leader follows pointed entity. * /
	A3DBool m_bLeaderDeleteControl;							/ *!< Delete leader follows pointed entity. * /
	A3DAsmProductOccurrence * m_pTargetProductOccurrence;	/ *!< If non-null, this member references a remote product occurrence that contains the reference. * /
	A3DEntity * m_pReference;								/ *!< Pointer on the referenced entity. Only A3DRiRepresentationItem, A3DAsmProductOccurrence, A3DMiscReferenceOnTopology and A3DMkpMarkup are accepted \version 2.2 * /
} A3DMiscMarkupLinkedItemData;
*/

void TdfImport::LogMarkupLinkedItem(A3DMiscMarkupLinkedItemData & cData, A3DMiscEntityReferenceData & cRefData, DWORD_PTR nInAddreass)
{
	Log(2, "MarkupLinkedItem Information: %s", LogHexStrA(nInAddreass));

	LogIncreaseTabIndex(2); {
		Log(2, "MarkupShowControl: %s", LogBoolStrA(cData.m_bMarkupShowControl));
		Log(2, "MarkupDeleteControl: %s", LogBoolStrA(cData.m_bMarkupDeleteControl));
		Log(2, "LeaderShowControl: %s", LogBoolStrA(cData.m_bLeaderShowControl));
		Log(2, "LeaderDeleteControl: %s", LogBoolStrA(cData.m_bLeaderDeleteControl));
		Log(2, "TargetProductOccurrence: %s", LogHexStrA((DWORD_PTR)cData.m_pTargetProductOccurrence));

		CString strName;
		GetName(cData.m_pReference, strName);

		Log(2, "Reference: %s, Type: %s, '%s'", LogHexStrA((DWORD_PTR)cData.m_pReference), Dmi3dx::GetA3dEntityTypeString(cData.m_pReference), CStringA(strName));

		Log(2, "RefData Referenced entity: %s, Type: %s", LogHexStrA((DWORD_PTR)cRefData.m_pEntity), Dmi3dx::GetA3dEntityTypeString(cRefData.m_pEntity));
		Log(2, "RefData CoordinateSystem: %s", LogHexStrA((DWORD_PTR)cRefData.m_pCoordinateSystem), Dmi3dx::GetA3dEntityTypeString(cData.m_pReference));
	} LogDecreaseTabIndex(2);
}

//== CAD Model 관련 함수 ========================================================================
H3DF::Component * TdfImport::AddComponent(SegmentKey & cInSegment, IncludeKey & cInInclude, CString strInName, H3DF::Component::Type eInType, H3DF::Component & cInParentComponent)
{
	DEBUG_VALID(m_pcCADModel);

	H3DF::CADModelImpl * pcCdModelImpl = dynamic_cast<H3DF::CADModelImpl *>(m_pcCADModel->GetImpl());
	DEBUG_VALID(pcCdModelImpl);

	H3DF::Component * pcComponent = new H3DF::Component();
	DEBUG_VALID(pcComponent);

	H3DF::ComponentImpl::SetData(*pcComponent, strInName, cInSegment.KeyValue(), cInInclude.KeyValue(), eInType);
	H3DF::ComponentImpl::AddSubComponent(cInParentComponent, *pcComponent);

	pcCdModelImpl->MapSetAt(cInSegment.KeyValue(), pcComponent);

	return pcComponent;
}

H3DF::Component * TdfImport::AddComponent(SegmentKey & cInSegment, CString strInName, H3DF::Component::Type eInType, H3DF::Component & cInParentComponent)
{
	DEBUG_VALID(m_pcCADModel);

	H3DF::CADModelImpl * pcCdModelImpl = dynamic_cast<H3DF::CADModelImpl *>(m_pcCADModel->GetImpl());
	DEBUG_VALID(pcCdModelImpl);

	H3DF::Component * pcComponent = new H3DF::Component();
	DEBUG_VALID(pcComponent);

	H3DF::ComponentImpl::SetData(*pcComponent, strInName, cInSegment.KeyValue(), INVALID_KEY, eInType);
	H3DF::ComponentImpl::AddSubComponent(cInParentComponent, *pcComponent);

	pcCdModelImpl->MapSetAt(cInSegment.KeyValue(), pcComponent);

	return pcComponent;
}