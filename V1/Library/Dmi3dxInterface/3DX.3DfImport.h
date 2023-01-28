#pragma once

#include "Json.h"

#include <3DF/3DF.Segment.h>
#include <3DF/3DF.Math.h>
#include <3DF/3DF.MaterialMapping.h>

#include "3DX.ImportBase.h"

#include <atlcoll.h>
#include <vector>
#include <unordered_map>

 OPEN_3DX_NAMESPACE

using TessIndexMap = CAtlMap<A3DUns32, A3DUns32>;
using IndexHash = std::unordered_map<A3DUns32, _3DF::IntArray>;

struct ImportOption
{
	//----- Parent 관련 변수 -----
	_3DF::SegmentKey cParentSegment;
	A3DMiscCascadedAttributes * pcParentAttr = nullptr;
};

struct MaterialMappingStyleKit
{
	CString strGeometry;
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

	struct ConvertFaceInfo
	{
		A3DUns32 * pnInIndices;							// "global" index array for normals, points and vertex parameters
		_3DF::PointArray aInPoints;						// "global" point array
		_3DF::VectorArray aInNormals;					// "global" normal array
		_3DF::FloatArray aInParams;						// optional "global" parameter array
		_3DF::RGBAColorArray aInColors;					// optional RGBA color array
		A3DTessFaceData * pcInTessFaceData;				// tessellation data for *this* (CAD) face
		float fInNormalCosine{};							// cosine limit for determining equal normals

		A3DUns32 nOutTriSizeIndex{};					// offset into A3DTessFaceData::m_puiSizesTriangulated
		A3DUns32 nOutTriStartIndex{};					// offset into in_indices
		A3DUns32 nOutTriColorIndex{};					// offset into in_colors
		IndexHash mOutIndexMap;							// mapping of "global" indices to "local" indices for *this* (CAD) face
		_3DF::IntArray aOutVertexRefs;					// count of references to a particular (vertex, normal) pair
		_3DF::PointArray aOutFacePoints;				// points for *this* (CAD) face
		_3DF::IntArray aOutFaceList;					// facelist for *this* (CAD) face (wrt to points for *this* (CAD) face)
		_3DF::IntArray aOutFaceVertexNormalsIndex;		// Face list에 대응되는 Normal Index를 저장.
		_3DF::VectorArray aOutFaceVertexNormals;		// vertex normals for *this* (CAD) face
		_3DF::FloatArray aOutFaceVertexParams;			// optional vertex parameters for *this* (CAD) face
		_3DF::RGBAColorArray aOutFaceVertexColors;		// optional RGBA vertex colors for *this* (CAD) face

		ConvertFaceInfo() {
			pnInIndices = nullptr;
			pcInTessFaceData = nullptr;
		}
	};

	bool ParseModelFile(const A3DAsmModelFile * pcAsmModelFile, _3DF::SegmentKey & cModelSegment);

	// == Product Occurrences 관련 함수 =========================================================
	A3DStatus ParseProductOccurrence(A3DAsmProductOccurrence * pcOccurrence, A3DMiscCascadedAttributes * pcParentAttr, double dModelScale, _3DF::SegmentKey & cParentSegment);

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
	A3DStatus ParsePart(const A3DAsmPartDefinition * pcPart, const A3DMiscCascadedAttributes * pcParentAttr, double dModelScale, _3DF::SegmentKey & cParentSegment);

	A3DStatus ParseRiRepresentationItem(const A3DRiRepresentationItem * pcRepItem, _3DF::SegmentKey & cParentSegment,
		const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawSet(const A3DRiSet * pSet, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawRiBrepModel(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, 
		_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData);

	A3DStatus DrawRiPolyBrepModel(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, 
		_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData);

	A3DStatus DrawRiPolyWire(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, 
		_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData);

	A3DStatus DrawRiPointSet(const A3DRiRepresentationItem * pcRepItem, _3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr);

	A3DStatus DrawRiCurve(A3DRiCurve * pcInputRiCurve, _3DF::SegmentKey & cParentSegment, A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawMarkupView(const A3DMkpView * pcView, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawAnnotations(const A3DMkpAnnotationEntity ** pcAnnotation, A3DUns32 nAnnotationsSize, _3DF::SegmentKey & cParentSegment);
	A3DStatus DrawAnnotation(const A3DMkpAnnotationEntity * pcAnnotation, A3DMiscCascadedAttributes * pcParentAttr, _3DF::SegmentKey & cParentSegment);

	A3DStatus DrawAnnotationSet(const A3DMkpAnnotationSet * pcAnnotationSet, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus DrawAnnotationReference(const A3DMkpAnnotationItem * /*pcAnnotationItem*/, const A3DMiscCascadedAttributes * /*pcParentAttr*/);
	A3DStatus DrawAnnotationItem(const A3DMkpAnnotationItem * pcAnnotationItem, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawMarkup(const A3DMkpMarkup * pcMarkup, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawTessBase(A3DTessBase * pcTessBase, const A3DRiRepresentationItem * pcRepItem, _3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawTess3D(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem, const A3DMiscCascadedAttributes * pcParentAttr, _3DF::SegmentKey & cParentSegment);

	UINT ConvertTessFaceDataTriangle(ConvertFaceInfo & cInFaceInfo);
	UINT DrawTessFaceDataTriangle(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriangleIndices, A3DUns32 & nTriangleSizeIndex, A3DUns32 & nTriangleStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray);

	UINT ConvertTessFaceDataTriangleFan(ConvertFaceInfo & cInFaceInfo);
	UINT DrawTessFaceDataTriangleFan(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray);

	UINT ConvertTessFaceDataTriangleStripe(ConvertFaceInfo & cInFaceInfo);
	UINT DrawTessFaceDataTriangleStripe(A3DTessFaceData & cTessFaceData, _3DF::PointArray & acInPoints, _3DF::VectorArray acInNormals, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, 
		A3DUns32 & nTriStartIndex, A3DUns32 & nTriColorIndex, IndexHash & mOutIndexMap, TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray);

	UINT ConvertTessFaceDataTriangleOneNormal(ConvertFaceInfo & cInFaceInfo);
	UINT DrawTessFaceDataTriangleOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray);

	UINT ConvertTessFaceDataTriangleFanOneNormal(ConvertFaceInfo & cInFaceInfo);
	UINT DrawTessFaceDataTriangleFanOneNormal(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray);

	UINT ConvertTessFaceDataTriangleStripeOneNormal(ConvertFaceInfo & cInFaceInfo);

	UINT ConveTessFaceDataTriangleTextured(ConvertFaceInfo & cInFaceInfo);
	UINT DrawTessFaceDataTriangleTextured(A3DTessFaceData & cTessFaceData, A3DUns32 * pnTriIndices, A3DUns32 & nTriSizeIndex, A3DUns32 & nTriStartIndex,
		TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anNormalIndexArray);
	
	A3DStatus DrawTess3DWire(const A3DTess3DWire * pTess3DWire, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
		const A3DMiscCascadedAttributes * pcParentAttr, _3DF::SegmentKey & cParentSegment);

	A3DStatus DrawPolyWires(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
		const A3DMiscCascadedAttributes * pcParentAttr, _3DF::SegmentKey & cSegment);

	A3DUns32 Tess3DDataGetNumberOfFacets(const A3DTess3DData * pcTess3DData);
	A3DUns32 TessFaceDataGetNumberOfFacets(const A3DTessFaceData * pcTessFaceData);

	void MatchVertexNormal(TessIndexMap & maNormalIndexMap, A3DUns32 nVertexIndex, A3DUns32 & nNormalIndex);
	bool ConvertFaceList(TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap,
		A3DUns32 * pnFacePointIndex, A3DUns32 * pnFaceNormalIndex, _3DF::IntArray & anFacelistArray, _3DF::IntArray & anVertexNoramlIndexArray);

	void AddTriangle(ConvertFaceInfo & cInFaceInfo, int const pnInFaceListIndices[3], int const pnInFaceVertexNromalIndices[3],
		int const pnInFaceVertexParamIndices[3], int const pnInFaceVertexColorIndices[3], A3DUns32  nInVertexParamSize);

	A3DStatus SetFaceStyle(const A3DRootBaseWithGraphics * pcBase, _3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetFaceStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetFaceStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData);

	A3DStatus SetLineStyle(const A3DRootBaseWithGraphics * pcBase, _3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetLineStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetLineStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData);

	A3DStatus SetMarkerStyle(const A3DRootBaseWithGraphics * pcBase, _3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetMarkerStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetMarkerStyle(_3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData);

	A3DStatus GetMaterialMapping(const A3DMiscCascadedAttributesData & cAttrsData, A3DInt32 * pnUVCoordinatesIndex, A3DUns8 * pucTextureDimension, _3DF::MaterialMappingKit & cMaterialKit);
	A3DStatus GetMaterialMapping(const A3DMiscCascadedAttributesData & cAttrsData, _3DF::MaterialMappingKit & cMaterialKit);

	A3DStatus DrawTransformation(const A3DMiscTransformation * pcTransformation);

	//== Texture 관련 함수 ===========================================================================
	void PopulateTextures();

	//== Attribute 관련 함수 ====================================================================
	A3DStatus CreateAndPushCascadedAttributes(const A3DRootBaseWithGraphics * pcBase, const A3DMiscCascadedAttributes * pcParentAttr,
		A3DMiscCascadedAttributes ** ppcAttr, A3DMiscCascadedAttributesData * pcAttrData);

	A3DStatus CreateAndPushCascadedAttributesTessFace(const A3DRiRepresentationItem * pcRepItem, const A3DTessBase * pcTessBase,
		const A3DTessFaceData * pcTessFaceData, A3DUns32 nFaceIndex, const A3DMiscCascadedAttributes * pcParentAttribute,
		A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData);

	A3DStatus IsShow(const A3DRootBaseWithGraphics * pGraphics);

	bool SetFaceMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cSegment);
	bool SetLineMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cSegment);
	bool SetMarkerMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cSegment);

	bool SetStyle(_3DF::SegmentKey & cSegment, _3DF::SegmentKey & cStyleSegment);
	bool FindFaceMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::SegmentKey & cOutStyleSegment);
	bool FindLineMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::SegmentKey & cOutStyleSegment);
	bool FindMarkerMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, _3DF::SegmentKey & cOutStyleSegment);
	bool FindMaterialMapping(CString strGeometry, _3DF::MaterialMappingKit const & cInKit, _3DF::SegmentKey & cOutStyleSegment);

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

	float m_fNormalAngleCosine = 0.0f;

	//----- Segment Header -----
	_3DF::SegmentKey * m_pcModelSegment = nullptr;
	_3DF::SegmentKey m_cPartsIncludeSegment;
	_3DF::SegmentKey m_cPoccsIncludeSegment;
	_3DF::SegmentKey m_cRisIncludeSegment;
	_3DF::SegmentKey m_cPmiIncludeSegment;

	CAtlMap<DWORD_PTR, HC_KEY> m_mPartsMap;

	// MaterialMap Style 키를 저장하는 Vector
	std::vector<MaterialMappingStyleKit> m_vcMaterialMappingStyleVector;

	std::unordered_map<A3DUns32, _3DF::SegmentKey> m_mFaceMaterialMappingStyleMap;
	std::unordered_map<A3DUns32, _3DF::SegmentKey> m_mLineMaterialMappingStyleMap;
	std::unordered_map<A3DUns32, _3DF::SegmentKey> m_mMarkerMaterialMappingStyleMap;


	// Segment Key Name 뒤부분에 붙는 Id값
	DWORD m_nIncrementalId = 0;

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



	void  parseAttributes(const A3DEntity * pEntity);
};
  
 CLOSE_3DX_NAMESPACE