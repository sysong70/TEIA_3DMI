#pragma once

#include "Json.h"

#include <3DF/Segment.h>
#include <3DF/Material.h>

#include <3DF/PMI.Entity.h>

#include "../Signal/Signal.h"

#include "3DX.ImportBase.h"

#include <atlcoll.h>
#include <vector>
#include <unordered_map>
#include <chrono>

OPEN_3DX_NAMESPACE

using TessIndexMap = CAtlMap<A3DUns32, A3DUns32>;
using IndexHash = CAtlMap<A3DUns32, std::vector<int>>;

struct ImportOption
{
	//----- Parent 관련 변수 -----
	H3DF::SegmentKey cParentSegment;
	A3DMiscCascadedAttributes * pcParentAttr = nullptr;
};

struct MaterialMappingStyleKit
{
	CString strGeometry;
	H3DF::SegmentKey cStyleSegment;
	H3DF::MaterialMappingKit cMaterialMappingKit;
};

class TdfImport : public ImportBase
{
public:
	TdfImport(_3DXSignal::Interface * pc3dxInterface);
	~TdfImport();

	typedef struct
	{
		UINT m_uiAllocated;
		UINT m_uiSize;
		void ** m_ppPointers;
	} A3DPointerArray;

	// == File TdfImport 관련 함수 =================================================================
	bool FileImport(CString strFilePathName, H3DF::SegmentKey & cModelSegment, Signal::Delivery & cInDelivery, CString & strErrorMessage);

	// == 3DX 설정 관련 함수 =====================================================================
protected:
	bool SetDefaultParamsLoadData(A3DRWParamsLoadData & cParamsLoadData);

	// == 3DF 변환 관련 함수 =====================================================================
protected:

	struct ConvertFaceInfo
	{
		A3DUns32 * pnInIndices;							// "global" index array for normals, points and vertex parameters
		//H3DF::PointArray aInPoints;						// "global" point array
		//H3DF::VectorArray aInNormals;					// "global" normal array
		H3DF::FloatArray aInParams;						// optional "global" parameter array
		H3DF::RGBAColorArray aInColors;					// optional RGBA color array
		A3DTessFaceData * pnIndices;				// tessellation data for *this* (CAD) face
		float fInNormalCosine;							// cosine limit for determining equal normals

		A3DUns32 nOutTriSizeIndex;						// offset into A3DTessFaceData::m_puiSizesTriangulated
		A3DUns32 nOutTriStartIndex;						// offset into in_indices
		A3DUns32 nOutTriColorIndex;						// offset into in_colors
		IndexHash mOutIndexMap;							// mapping of "global" indices to "local" indices for *this* (CAD) face
		H3DF::IntArray aOutVertexRefs;					// count of references to a particular (vertex, normal) pair
		H3DF::PointArray aOutFacePoints;				// points for *this* (CAD) face
		H3DF::IntArray aOutFaceList;					// facelist for *this* (CAD) face (wrt to points for *this* (CAD) face)
		H3DF::IntArray aOutFaceVertexNormalsIndex;		// Face list에 대응되는 Normal Index를 저장.
		H3DF::VectorArray aOutFaceVertexNormals;		// vertex normals for *this* (CAD) face
		H3DF::FloatArray aOutFaceVertexParams;			// optional vertex parameters for *this* (CAD) face
		H3DF::RGBAColorArray aOutFaceVertexColors;		// optional RGBA vertex colors for *this* (CAD) face

		ConvertFaceInfo() {
			pnInIndices = nullptr;
			pnIndices = nullptr;
		}
	};

	bool ParseModelFile(const A3DAsmModelFile * pcAsmModelFile, H3DF::SegmentKey & cModelSegment);

	// == Product Occurrences 관련 함수 =========================================================
	A3DStatus ParseProductOccurrence(A3DAsmProductOccurrence * pcOccurrence, A3DMiscCascadedAttributes * pcParentAttr, double dModelScale, H3DF::SegmentKey & cParentSegment);

	A3DStatus ProductOccurrenceGetLocation(const A3DAsmProductOccurrenceData * pcPoData, H3DF::MatrixKit & cTransMatrix);
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
	A3DStatus ParsePart(const A3DAsmPartDefinition * pcPart, const A3DMiscCascadedAttributes * pcParentAttr, double dModelScale, H3DF::SegmentKey & cParentSegment);

	A3DStatus ParseRiRepresentationItem(const A3DRiRepresentationItem * pcRepItem, H3DF::SegmentKey & cParentSegment,
		const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawSet(const A3DRiSet * pSet, H3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus ParseRiBrepModel(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, 
		H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData);

	A3DStatus DrawRiPolyBrepModel(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, 
		H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData);

	A3DStatus DrawRiPolyWire(const A3DRiRepresentationItem * pcRepItem, const A3DRiRepresentationItemData & cRepItemData, 
		H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr, const A3DMiscCascadedAttributesData & cAttrData);

	A3DStatus DrawRiPointSet(const A3DRiRepresentationItem * pcRepItem, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcAttr);

	A3DStatus DrawRiCurve(A3DRiCurve * pcInputRiCurve, H3DF::SegmentKey & cParentSegment, A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawMarkupView(const A3DMkpView * pcView, H3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus ParseAnnotations(A3DMkpAnnotationEntity ** pcAnnotation, A3DUns32 nAnnotationsSize, H3DF::SegmentKey & cParentSegment);
	A3DStatus ParseAnnotation(const A3DMkpAnnotationEntity * pcAnnotation, A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cParentSegment);

	A3DStatus DrawAnnotationSet(const A3DMkpAnnotationSet * pcAnnotationSet, H3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus DrawAnnotationReference(const A3DMkpAnnotationItem * /*pcAnnotationItem*/, const A3DMiscCascadedAttributes * /*pcParentAttr*/);
	A3DStatus DrawAnnotationItem(const A3DMkpAnnotationItem * pcAnnotationItem, H3DF::SegmentKey & cParentSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus TraverseMarkup(const A3DMkpMarkup * pcMarkup, A3DMiscCascadedAttributesData * psAttribData, H3DF::SegmentKey & cParentSegment);
	A3DStatus GetMarkupTesselation(const A3DTessBaseData * psTessBaseData, const A3DTessMarkupData * psTessMarkupData, H3DF::PolylineArray & out_polylines, H3DF::PolygonArray & out_polygones,
		H3DF::StringArray & aOutStrings, H3DF::PMI::TextAttributesArray & cOutTextAttributes, H3DF::PMI::Options * pcOutPmiOptions = nullptr);
	A3DStatus GetLeaderLinesAndSymbols(const A3DMkpLeader * pMarkup, H3DF::PolylineArray & out_leader_lines, H3DF::PolygonArray & out_leader_symbols);

	A3DStatus DrawTessBase(A3DTessBase * pcTessBase, const A3DRiRepresentationItem * pcRepItem, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);

	A3DStatus DrawTess3D(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem, const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cParentSegment);
	A3DStatus DrawTess3DFaceRegion(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem, const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cSegment);

	UINT ConvertTessFaceDataTriangle(ConvertFaceInfo & cInFaceInfo);
	UINT ConvertTessFaceDataTriangle(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit);

	UINT ConvertTessFaceDataTriangleFan(ConvertFaceInfo & cInFaceInfo);
	UINT ConvertTessFaceDataTriangleFan(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit);

	UINT ConvertTessFaceDataTriangleStripe(ConvertFaceInfo & cInFaceInfo);
	UINT ConvertTessFaceDataTriangleStripe(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit);

	UINT ConvertTessFaceDataTriangleOneNormal(ConvertFaceInfo & cInFaceInfo);
	UINT ConvertTessFaceDataTriangleOneNormal(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit);

	UINT ConvertTessFaceDataTriangleFanOneNormal(ConvertFaceInfo & cInFaceInfo);
	UINT ConvertTessFaceDataTriangleFanOneNormal(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit);

	UINT ConvertTessFaceDataTriangleStripeOneNormal(ConvertFaceInfo & cInFaceInfo);
	UINT ConvertTessFaceDataTriangleStripeOneNormal(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit);

	UINT ConveTessFaceDataTriangleTextured(ConvertFaceInfo & cInFaceInfo);
	UINT ConveTessFaceDataTriangleTextured(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit);

	UINT ConveTessFaceDataTriangleStripeTextured(ConvertFaceInfo & cInFaceInfo, H3DF::ShellKit & cInShellKit);

	A3DStatus DrawTess3DWire(const A3DTess3DWire * pTess3DWire, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
		const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cParentSegment);

	A3DStatus DrawPolyWires(const A3DTess3D * pcTess3D, const A3DTessBaseData * pcTessBaseData, const A3DRiRepresentationItem * pcRepItem,
		const A3DMiscCascadedAttributes * pcParentAttr, H3DF::SegmentKey & cSegment);

	A3DUns32 Tess3DDataGetNumberOfFacets(const A3DTess3DData * pcTess3DData);
	A3DUns32 TessFaceDataGetNumberOfFacets(const A3DTessFaceData * pcTessFaceData);

	A3DStatus BuildMarkup(A3DTess3D * pcTess3d, A3DTessBaseData * pcTessBaseData, H3DF::SegmentKey & cParentSegment);

	void MatchVertexNormal(TessIndexMap & maNormalIndexMap, A3DUns32 nVertexIndex, A3DUns32 & nNormalIndex);
	bool ConvertFaceList(TessIndexMap & maPointIndexMap, TessIndexMap & maNormalIndexMap,
		A3DUns32 * pnFacePointIndex, A3DUns32 * pnFaceNormalIndex, H3DF::IntArray & anFacelistArray, H3DF::IntArray & anVertexNoramlIndexArray);

	void AddTriangle(ConvertFaceInfo & cInFaceInfo, int const pnInFaceListIndices[3], int const pnInFaceVertexNromalIndices[3],
		int const pnInFaceVertexParamIndices[3], int const pnInFaceVertexColorIndices[3], A3DUns32  nInVertexParamSize);
// 	void AddTriangle_IndexHash(ConvertFaceInfo & cInFaceInfo, int const pnInFaceListIndices[3], int const pnInFaceVertexNromalIndices[3],
// 		int const pnInFaceVertexParamIndices[3], int const pnInFaceVertexColorIndices[3], A3DUns32  nInVertexParamSize);

	A3DStatus SetFaceStyle(const A3DRootBaseWithGraphics * pcBase, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetFaceStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetFaceStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData);
	A3DStatus SetFaceStyle(H3DF::SegmentKey & cSegment, const A3DGraphStyleData cStyleData);

	A3DStatus SetLineStyle(const A3DRootBaseWithGraphics * pcBase, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetLineStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetLineStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData);

	A3DStatus SetMarkerStyle(const A3DRootBaseWithGraphics * pcBase, H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetMarkerStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributes * pcParentAttr);
	A3DStatus SetMarkerStyle(H3DF::SegmentKey & cSegment, const A3DMiscCascadedAttributesData & cAttrsData);
	
	A3DStatus GetMaterial(const A3DMiscCascadedAttributesData & cAttrsData, A3DInt32 * pnUVCoordinatesIndex, A3DUns8 * pucTextureDimension, H3DF::MaterialKit & cMaterialKit);
	A3DStatus GetMaterial(const A3DGraphStyleData & cInStyleData, H3DF::MaterialKit & cOutMaterial);
	bool FindMaterial(const A3DGraphStyleData & cInStyleData, H3DF::MaterialKit & cOutMaterial);
	bool CreateMaterial(const A3DGraphStyleData & cInStyleData, H3DF::MaterialKit & cOutMaterial);

	bool ParseTopoContextScale(const A3DTopoBody * pcBody, double & dTopoContextScale);

	A3DStatus DrawTransformation(const A3DMiscTransformation * pcTransformation);

	//== Texture 관련 함수 ===========================================================================
	A3DStatus PopulateTextures(H3DF::SegmentKey & cSegment);
	A3DStatus GetTextureMapping(const A3DMiscCascadedAttributesData & cAttrsData, H3DF::MaterialKit & cMaterialKit);
	A3DStatus SetTextureMapping(H3DF::SegmentKey cSegment, A3DMiscCascadedAttributesData & sAttrData);

	void InvertImage(unsigned char * imagebuffer, int width, int height, bool rgba);

	//== Attribute 관련 함수 ====================================================================
	A3DStatus CreateAndPushCascadedAttributes(const A3DRootBaseWithGraphics * pcBase, const A3DMiscCascadedAttributes * pcParentAttr,
		A3DMiscCascadedAttributes ** ppcAttr, A3DMiscCascadedAttributesData * pcAttrData);

	A3DStatus CreateAndPushCascadedAttributesTessFace(const A3DRiRepresentationItem * pcRepItem, const A3DTessBase * pcTessBase,
		const A3DTessFaceData * pcTessFaceData, A3DUns32 nFaceIndex, const A3DMiscCascadedAttributes * pcParentAttribute,
		A3DMiscCascadedAttributes ** pcAttrs, A3DMiscCascadedAttributesData * pcAttributesData);

	A3DStatus IsShow(const A3DRootBaseWithGraphics * pGraphics);

	bool SetFaceMaterialMapping(const A3DGraphStyleData & sStyleData, H3DF::MaterialKit const & cInKit, H3DF::SegmentKey & cSegment);
	bool SetLineMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, H3DF::MaterialKit const & cInKit, H3DF::SegmentKey & cSegment);
	bool SetMarkerMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, H3DF::MaterialKit const & cInKit, H3DF::SegmentKey & cSegment);

	bool CreateFaceStyleSegment(const A3DGraphStyleData & sStyleData, H3DF::MaterialKit const & cInKit, H3DF::SegmentKey & cOutStyleSegment);
	bool SetStyle(H3DF::SegmentKey & cSegment, H3DF::SegmentKey & cStyleSegment);
	bool FindMaterialMapping(const A3DGraphStyleData & sStyleData, H3DF::SegmentKey & cOutStyleSegment);
	bool FindLineMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, H3DF::SegmentKey & cOutStyleSegment);
	bool FindMarkerMaterialMapping(const A3DMiscCascadedAttributesData & cAttrData, H3DF::SegmentKey & cOutStyleSegment);
	bool FindMaterialMapping(CString strGeometry, H3DF::MaterialMappingKit const & cInKit, H3DF::SegmentKey & cOutStyleSegment);

	// == C3D 관련 Utility 함수 =====================================================================
protected:
	A3DStatus GetMatrix(A3DMiscTransformation * pcLocation, MbMatrix3D & cMatrix);
	A3DStatus GetMatrix(A3DMiscTransformation * pcLocation, H3DF::MatrixKit & cOutMatrix);

private:
	CString m_strCadFileName;

	bool m_bGlobalDataFlag = false;
	A3DGlobalData m_cGlobalData;

	A3DUTF8Char * m_pchRepresentationItemName = nullptr;

	bool m_bDrawMarkups = true;

	float m_fNormalAngleCosine = 0.0f;

	double m_dCadModelUnit = 1.0;

	//----- Segment Header -----
	H3DF::SegmentKey * m_pcModelSegment = nullptr;
	H3DF::SegmentKey m_cModelIncludeKey;
	H3DF::SegmentKey m_cStylesIncludeKey;
	H3DF::SegmentKey m_cPartsIncludeSegment;
	H3DF::SegmentKey m_cPoccsIncludeSegment;
	H3DF::SegmentKey m_cRisIncludeSegment;
	H3DF::SegmentKey m_cPmiIncludeSegment;

	CAtlMap<DWORD_PTR, HC_KEY> m_mPartsMap;

	// MaterialMap Style 키를 저장하는 Vector
	std::vector<MaterialMappingStyleKit> m_vcMaterialMappingStyleVector;

	CAtlMap<CStringA, H3DF::MaterialKit> m_mMaterialMap;
	CAtlMap<CStringA, H3DF::SegmentKey> m_mMaterialMappingStyleMap;
	std::unordered_map<A3DUns32, H3DF::SegmentKey> m_mLineMaterialMappingStyleMap;
	std::unordered_map<A3DUns32, H3DF::SegmentKey> m_mMarkerMaterialMappingStyleMap;

	//----- Tessellation 관련 -----
	H3DF::Point * m_pcPoints = nullptr;
	A3DUns32 m_nPointCount = 0;
	A3DUns32 m_nMaxPointCount = 0;

	H3DF::Vector * m_pcNormals = nullptr;
	A3DUns32 m_nNormalCount = 0;
	A3DUns32 m_nMaxNormalCount = 0;

	H3DF::Point * m_pcTextureCoords = nullptr;
	A3DUns32 m_nTextureCoordCount = 0;
	A3DUns32 m_nMaxTextureCoordCount = 0;

	// Segment Key Name 뒤부분에 붙는 Id값
	DWORD m_nIncrementalId = 0;
	DWORD m_nViewId = 0;
	DWORD m_nMarkupId = 0;

	A3DEModellerType m_eModellerType = kA3DModellerUnknown;

	Signal::Delivery * m_pcInDelivery = nullptr;

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
	void Log(int nId, LPCSTR chMessage, ...);
	void LogIncreaseTabIndex(int nId);
	void LogDecreaseTabIndex(int nId);
	CString LogHexStr(DWORD_PTR nValue);
	CString LogBoolStr(bool bValue);

	void  parseAttributes(const A3DEntity * pEntity);
};
  
 CLOSE_3DX_NAMESPACE