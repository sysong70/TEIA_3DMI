#pragma once

#include "Json.h"

#include <3DF/3DF.Segment.h>
#include <3DF/3DF.Math.h>
#include <3DF/3DF.MaterialMapping.h>

#include "3DX.ImportBase.h"

#include <atlcoll.h>
#include <vector>

using TessIndexMap = CAtlMap<A3DUns32, A3DUns32>;

 OPEN_3DX_NAMESPACE

struct ImportOption
{
	//----- Parent 관련 변수 -----
	_3DF::SegmentKey cParentSegment;
	A3DMiscCascadedAttributes * pcParentAttr = nullptr;
};

struct MaterialMappingStyleKit
{
	_3DF::SegmentKey cStyleSegment;
	_3DF::MaterialMappingKit cMaterialMappingKit;
};

class _3DfImport : public ImportBase
{
public:
	_3DfImport(_3DXSignal::Interface * pc3dxInterface);
	~_3DfImport();

	typedef struct
	{
		UINT m_uiAllocated;
		UINT m_uiSize;
		void ** m_ppPointers;
	} A3DPointerArray;

	// == File _3DfImport 관련 함수 =================================================================
	bool FileImport(CString strFilePathName, _3DF::SegmentKey & cModelSegment, CString & strErrorMessage);

	// == 3DX 설정 관련 함수 =====================================================================
protected:
	bool SetDefaultParamsLoadData(A3DRWParamsLoadData & cParamsLoadData);

	// == 3DF 변환 관련 함수 =====================================================================
protected:
	bool DrawModel(const A3DAsmModelFile * pcAsmModelFile, _3DF::SegmentKey & cModelSegment);

	// == Product Occurrences 관련 함수 =========================================================
	A3DStatus DrawProductOccurrence(A3DAsmProductOccurrence * pcOccurrence, _3DF::SegmentKey & cParentSegment, A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus ProductOccurrenceGetLocation(const A3DAsmProductOccurrenceData * pcPoData, _3DF::MatrixKit & cTransMatrix);
	A3DStatus ProductOccurrenceGetLocation(const A3DAsmProductOccurrenceData * psPOccData, A3DMiscCartesianTransformation ** ppLocation);
	A3DStatus ProductOccurrenceGetExternalData(const A3DAsmProductOccurrenceData * pcPOccData,
		A3DAsmProductOccurrence ** ppcExternalData);
	A3DStatus ProductOccurrenceGetMarkups(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray);
	A3DStatus ProductOccurrenceGetViews(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray);

	A3DStatus ProductOccurrenceGetChild(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray);
	A3DStatus ProductOccurrenceGetOccurrences(const A3DAsmProductOccurrenceData * pcPOccData, A3DPointerArray * pcArray);

	A3DStatus AnnotationGetMarkups(const A3DMkpAnnotationEntity * pcAnnotation, A3DPointerArray * pcArray);
	A3DStatus AnnotationSetGetMarkups(const A3DMkpAnnotationSet * pcAnnotationSet, A3DPointerArray * pcArray);
	A3DStatus AnnotationReferenceGetMarkups(const A3DMkpAnnotationItem * /*pcAnnotationItem*/, A3DPointerArray * /*pcArray*/);
	A3DStatus AnnotationItemGetMarkups(const A3DMkpAnnotationItem * pcAnnotationItem, A3DPointerArray * pcArray);

	//== Part 관련 함수 =========================================================================
	A3DStatus ProductOccurrenceGetPart(const A3DAsmProductOccurrenceData * pcPOccData, A3DAsmPartDefinition ** ppcPart);

	//== Draw 관련 함수 =========================================================================
	A3DStatus DrawPartDefinition(const A3DAsmPartDefinition * pcPart, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus DrawRepresentationItem(const A3DRiRepresentationItem * pcRepItem, _3DF::SegmentKey & cParentSegment,
		const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus DrawSet(const A3DRiSet * pSet, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawMarkupView(const A3DMkpView * pcView, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus DrawAnnotation(const A3DMkpAnnotationEntity * pcAnnotation, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus DrawAnnotationSet(const A3DMkpAnnotationSet * pcAnnotationSet, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus DrawAnnotationReference(const A3DMkpAnnotationItem * /*pcAnnotationItem*/, const A3DMiscCascadedAttributes * /*pcParentAttr*/);
	A3DStatus DrawAnnotationItem(const A3DMkpAnnotationItem * pcAnnotationItem, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawMarkup(const A3DMkpMarkup * pcMarkup, _3DF::SegmentKey & cParentSegment, _3DF::MaterialMappingKit const & cParentMaterialMapping, 
		const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawTessBase(A3DTessBase * pcTessBase, const A3DRiRepresentationItem * pcRepItem,
		_3DF::SegmentKey & cParentSegment, _3DF::MaterialMappingKit const & cParentMaterialMapping, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawTess3D(const A3DTess3D * pcTess3D, const A3DRiRepresentationItem * pcRepItem,
		_3DF::SegmentKey & cParentSegment, _3DF::MaterialMappingKit const & cParentMaterialMapping, const A3DMiscCascadedAttributes * pcParentAttr);
	UINT DrawTessFaceDataTriangle(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriangleIndices, A3DUns32 & nTriangleSizeIndex, A3DUns32 & nTriangleStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray);
	UINT DrawTessFaceDataTriangleFan(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray);
	UINT DrawTessFaceDataTriangleStripe(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		A3DUns32 & nTriColorIndex, TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray);
	UINT DrawTessFaceDataTriangleOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray);
	UINT DrawTessFaceDataTriangleFanOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray);
	UINT DrawTessFaceDataTriangleStripeOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray);
	
	A3DUns32 Tess3DDataGetNumberOfFacets(const A3DTess3DData * pcTess3DData);
	A3DUns32 TessFaceDataGetNumberOfFacets(const A3DTessFaceData * pcTessFaceData);

	void MatchVertexNormal(TessIndexMap & maNormalIndexMap, A3DUns32 nVertexIndex, A3DUns32 & nNormalIndex);
	bool ConvertFaceList(TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap,
		A3DUns32 * pnFacePointIndex, A3DUns32 * pnFaceNormalIndex, _3DF::IntArray & anFacelistArray);

	A3DStatus DrawStyle(const A3DMiscCascadedAttributesData & cAttrsData, A3DInt32 * pnUVCoordinatesIndex, A3DUns8 * pucTextureDimension, _3DF::MaterialMappingKit & cMaterialKit);
	A3DStatus DrawStyle(const A3DMiscCascadedAttributesData & cAttrsData, _3DF::MaterialMappingKit & cMaterialKit);

	A3DStatus DrawTransformation(const A3DMiscTransformation * pcTransformation);

	//== Attribute 관련 함수 ====================================================================
	A3DStatus CreateAndPushCascadedAttributes(const A3DRootBaseWithGraphics * pcBase, const A3DMiscCascadedAttributes * pcParentAttr,
		A3DMiscCascadedAttributes ** ppcAttr, A3DMiscCascadedAttributesData * pcAttrData);

	A3DStatus CreateAndPushCascadedAttributesTessFace(const A3DRiRepresentationItem * pcRepItem, const A3DTessBase * pcTessBase,
		const A3DTessFaceData * pcTessFaceData, A3DUns32 nFaceIndex, const A3DMiscCascadedAttributes * pcParentAttribute,
		A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData);

	A3DStatus CreateAndPushCascadedAttributesFace(const A3DRiRepresentationItem * pcRepItem,
		const A3DTessBase * pcTessBase, const A3DTessFaceData * pcTessFaceData, A3DUns32 uiFaceIndex,
		const A3DMiscCascadedAttributes * pcParentAttr, A3DMiscCascadedAttributes ** ppcAttr, A3DMiscCascadedAttributesData * pcAttrData);

	bool IsShow(const A3DRootBaseWithGraphics * pGraphics);

	bool SetStyleMaterialMapping(_3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cSegment);
	bool SetStyle(_3DF::SegmentKey & cSegment, _3DF::SegmentKey & cStyleSegment);
	bool FindMaterialMapping(_3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cOutStyleSegment);

	// == C3D 관련 Utility 함수 =================================================================
protected:
	A3DStatus GetMatrix(A3DMiscTransformation * pcLocation, MbMatrix3D & cMatrix);
	A3DStatus GetMatrix(A3DMiscTransformation * pcLocation, _3DF::MatrixKit & cOutMatrix);

private:
	CString m_strCadFileName;

	bool m_bGlobalDataFlag = false;
	A3DGlobalData m_cGlobalData;

	A3DUTF8Char * m_pchRepresentationItemName = nullptr;

	bool m_bDrawMarkups = true;

	//----- Segment Header -----
	_3DF::SegmentKey * m_pcModelSegment = nullptr;
	_3DF::SegmentKey m_cPartsIncludeSegment;
	_3DF::SegmentKey m_cPoccsIncludeSegment;
	_3DF::SegmentKey m_cRisIncludeSegment;

	CAtlMap<DWORD_PTR, HC_KEY> m_mPartsMap;

	// MaterialMap Style 키를 저장하는 Vector
	std::vector<MaterialMappingStyleKit> m_vcMaterialMappingStyleVector;

	// Segment Key Name 뒤부분에 붙는 Id값
	DWORD m_nIncrementalId = 0;
	DWORD m_nShellMaterialId = 0;

	//== 계산 관련 함수 ==========================================================================
private:
	double m_dMatrixStack[32][16];
	UINT m_nMatrixPos = 0;
	double m_dMatrix[16];

	void MatrixMatrixMult(double m[16], const double o[16]);
	void LoadMatrixIdentity();
	void MultMatrix(const double m[16]);
	void PushMatrix();
	void PopMatrix();
	void GetMatrix(double m[16]);

	void VectorCross(const A3DVector3dData * X, const A3DVector3dData * Y, A3DVector3dData * Z);
	void VectorMatrixMult(A3DDouble dX, A3DDouble dY, A3DDouble dZ, double adMatrix[16],
		A3DDouble * pdResX, A3DDouble * pdResY, A3DDouble * pdResZ);

	void AllocVector3dArray(A3DVector3dData ** ppacArray, A3DUns32 uiSize);
	void AllocNormalsAndPoints(A3DVector3dData ** ppasNormals, A3DUns32 uiNormalSize, A3DVector3dData ** ppasPoints, A3DUns32 uiPointSize);

	void BoundingBoxAddPoint(A3DBoundingBoxData * e, double x, double y, double z);

	//== 변수 Pointer 관리 함수 ===========================================================================
	void PointerArrayInitialize(A3DPointerArray * pcArray);
	void PointerArrayFree(A3DPointerArray * pcArray);
	void PointerArrayTerminate(A3DPointerArray * pcArray);
	void * MiscRealloc(void * p, A3DUns32 uiOldSize, A3DUns32 uiNewSize);
	UINT PointerArrayAdd(A3DPointerArray * pcArray, void * pcPointer);
	int PointerArrayFind(A3DPointerArray * pcArray, void * pcPointer);
	UINT PointerArrayAddUnique(A3DPointerArray * pcArray, void * pcPointer);
	UINT PointerArrayAddArray(A3DPointerArray * pcArray, void ** const ppcPointers, UINT uiSize);

	//== Log 관련 함수 ==========================================================================
	void CreateLog(int nId, const WCHAR * pchFilePathName);
	void Log(int nId, LPCWSTR chMessage, ...);
	void LogIncreaseTabIndex(int nId);
	void LogDecreaseTabIndex(int nId);
	CString LogHexStr(DWORD_PTR nValue);
	CString LogBoolStr(bool bValue);
};
  
 CLOSE_3DX_NAMESPACE