#pragma once

//== HOOPS 관련 Define ==============================================================================
#ifndef HC_POINTER_SIZED_INT
#if defined(_M_IA64) || defined(_M_AMD64) || defined(WIN64) || defined(_WIN64) || defined(_M_X64)
#   define  HC_POINTER_SIZED_INT    __int64
#else
#   define  HC_POINTER_SIZED_INT    long
#endif
#endif

#ifndef HC_KEY
#   define  HC_KEY      HC_POINTER_SIZED_INT
#endif

#ifndef INVALID_KEY
#	define INVALID_KEY (-1L)
#endif

#ifndef _3DF_INLINE
#	define H3DF_INLINE __forceinline
#endif

#ifndef _3DF_UNREFERENCED
#	define _3DF_UNREFERENCED(param) ((void)(param))
#endif

#ifdef H3DF_EXPORT
#	define API_3DF __declspec (dllexport)
#	define DLLEXPORT_TEMPLATE
#else
#	define API_3DF __declspec (dllimport)
#	define DLLEXPORT_TEMPLATE extern
#endif

#define OPEN_3DF_NAMESPACE namespace H3DF {
#define CLOSE_3DF_NAMESPACE }

#define USING_3DF_NAMESPACE using namespace H3DF;

#include <vector>



namespace H3DF
{
#	define PATTERN_BUFFER_SIZE		1024
#	define STYLE_BUFFER_SIZE		128

	//== Forward Declarations ======================================================================
	class Object;
	class Canvas;
	class View;
	class WindowKey;
	class Model;
	class Key;
	class SegmentKey;
	class IncludeKey;
	class MaterialMappingKit;
	class PortfolioKey;
	class ShellKey;
	class CircleKit;
	class CircleKey;
	class LineKit;
	class LineKey;
	class PolygonKit;
	class PolygonKey;
	class BoundingKit;

	class SelectabilityKit;
	class SelectabilityControl;

	class CameraKit;
	class CameraControl;

	class LineAttributeKit;
	class LineAttributeControl;

	class MatrixKit;
	class CameraKit;
	class PerformanceKit;

	class WorldPoint;
	class InnerWindowPoint;
	class WindowPoint;
	class PixelPoint;

	class Style;
	class StyleKey;
	class NamedStyleDefinition;

	class CuttingSectionKit;
	class CuttingSectionKey;
	class CuttingSectionAttributeKit;
	class CuttingSectionAttributeControl;

	class PerformanceKit;
	class PerformanceControl;

	class SelectionItem;
	class SelectionResults;
	class SelectionOptionsKit;
	class SelectionControl;
	class HighlightControl;

	class ConditionalExpression;
	class ConditionControl;

	class SelectionOptionsControl;

	class VisibilityControl;
	class VisualEffectsControl;

	class MarkerAttributeControl;
	class MaterialMappingControl;

	class BoundingKit;
	class BoundingControl;

	class AttributeLockControl;

	class ApplicationWindowOptionsKit;

	class GeometryKey;
	class ReferenceKey;

	class PortfolioKey;

	class DrawingAttributeKit;
	class DrawingAttributeControl;

	class ColorInterpolationKit;
	class ColorInterpolationControl;

	class CADModel;
	class Component;
	class MetaData;
	class StringMetaData;

	class KeyImpl;

	//==============================================================================================

	namespace Operator {
		enum class Result : UINT
		{
			None			= 0,
			Pass			= 1 << 1,
			Consume			= 1 << 2,
			Complete		= 1 << 3,
		};
	}

	// The Type class is a concept class that contains type information for all classes.
	enum class Type : uint32_t
	{
		None = 0x00000000,
		GenericMask = 0xffffff00,

		World = 0x00000001,
		UTF8 = 0x00000002,
		EventDispatcher = 0x00000003,
		EventHandler = 0x00000004,
		EventNotifier = 0x00000005,
		UpdateNotifier = 0x00000006,
		SearchResults = 0x00000008,
		FontSearchResults = 0x00000009,
		SelectionResults = 0x0000000a,
		SelectionItem = 0x0000000b,
		TreeContext = 0x0000000c,
		StreamToolkit = 0x0000000d,
		DriverEventHandler = 0x0000000e,
		HighlightSearchResults = 0x0000000f,
		OptimizeMappingResults = 0x00000010,

		SearchResultsIterator = 0x01000001,
		FontSearchResultsIterator = 0x01000002,
		SelectionResultsIterator = 0x01000003,
		HighlightSearchResultsIterator = 0x01000004,
		OptimizeMappingResultsIterator = 0x01000005,

		Kit = 0x01000000,
		MarkerKit = 0x01000010,
		SphereAttributeKit = 0x01000011,
		TextAttributeKit = 0x01000012,
		TransparencyKit = 0x01000013,
		VisibilityKit = 0x01000014,
		VisualEffectsKit = 0x01000015,
		CuttingSectionAttributeKit = 0x01000016,
		CircleKit = 0x01000017,
		CircularArcKit = 0x01000018,
		CircularWedgeKit = 0x01000019,
		CuttingSectionKit = 0x0100001a,
		CylinderKit = 0x0100001b,
		DistantLightKit = 0x0100001c,
		EllipseKit = 0x0100001d,
		EllipticalArcKit = 0x0100001e,
		InfiniteLineKit = 0x0100001f,
		LineKit = 0x01000020,
		ShellKit = 0x01000027,
		MaterialKit = 0x01000029,
		CameraKit = 0x01000031,
		BoundingKit = 0x01000032,
		CullingKit = 0x01000033,
		CurveAttributeKit = 0x01000034,
		CylinderAttributeKit = 0x01000035,
		EdgeAttributeKit = 0x01000036,
		LightingAttributeKit = 0x01000037,
		LineAttributeKit = 0x01000038,
		MarkerAttributeKit = 0x01000039,
		MaterialMappingKit = 0x0100003a,
		SelectabilityKit = 0x0100003e,
		SelectionOptionsKit = 0x0100003f,
		ApplicationWindowOptionsKit = 0x01000042,
		HighlightOptionsKit = 0x01000043,
		LinePatternParallelKit = 0x01000044,
		SubwindowKit = 0x01000045,
		PerformanceKit = 0x01000046,
		HiddenLineAttributeKit = 0x01000047,
		DrawingAttributeKit = 0x01000048,
		LegacyShaderKit = 0x01000049,
		DebuggingKit = 0x0100004a,
		ContourLineKit = 0x0100004b,
		AttributeLockKit = 0x01000056,
		TransformMaskKit = 0x01000057,
		ColorInterpolationKit = 0x01000058,
		UpdateOptionsKit = 0x01000059,
		ImageExportOptionsKit = 0x0100005a,
		OBJImportOptionsKit = 0x0100005b,
		OBJImportResultsKit = 0x0100005c,
		STLImportOptionsKit = 0x0100005d,
		STLImportResultsKit = 0x0100005e,
		ShellOptimizationOptionsKit = 0x0100005f,
		ShellRelationOptionsKit = 0x01000060,
		ShellRelationResultsKit = 0x01000061,
		GridKit = 0x01000062,
		CutGeometryGatheringOptionsKit = 0x01000063,
		SegmentOptimizationOptionsKit = 0x01000064,
		PointCloudImportOptionsKit = 0x01000065,
		PointCloudImportResultsKit = 0x01000066,
		ShapeKit = 0x01000067,
		HighlightSearchOptionsKit = 0x01000068,
		PBRMaterialKit = 0x01000069,
		PixelShaderKit = 0x0100006a,
		VertexShaderKit = 0x0100006b,
		ShaderTextureKit = 0x0100006c,
		ShaderSamplerKit = 0x0100006d,

		Key = 0x10000000,
		IncludeKey = 0x10000001,
		PortfolioKey = 0x10000002,
		StyleKey = 0x10000003,

		SegmentKey = 0x10200000,
		WindowKey = 0x10600000,

		GeometryKey = 0x10100000,
		ReferenceKey = 0x10100001,
		CircleKey = 0x10100002,
		CircularArcKey = 0x10100003,
		CircularWedgeKey = 0x10100004,
		CuttingSectionKey = 0x10100005,
		CylinderKey = 0x10100006,
		EllipseKey = 0x10100007,
		EllipticalArcKey = 0x10100008,
		InfiniteLineKey = 0x10100009,
		LineKey = 0x1010000a,
		DistantLightKey = 0x1010000b,
		SpotlightKey = 0x1010000c,
		MarkerKey = 0x1010000d,
		MeshKey = 0x1010000e,
		NURBSCurveKey = 0x1010000f,
		NURBSSurfaceKey = 0x10100010,
		PolygonKey = 0x10100011,
		ShellKey = 0x10100012,
		SphereKey = 0x10100013,
		TextKey = 0x10100014,
		GridKey = 0x10100015,

		Definition = 0x20000000,
		NamedStyleDefinition = 0x20000001,
		TextureDefinition = 0x20000002,
		LinePatternDefinition = 0x20000003,
		GlyphDefinition = 0x20000004,
		CubeMapDefinition = 0x20000005,
		ImageDefinition = 0x20000006,
		MaterialPaletteDefinition = 0x20000007,
		LegacyShaderDefinition = 0x20000008,
		ShapeDefinition = 0x20000009,

		SegmentStyle = 0x20013001,
		NamedStyle = 0x20013002,
		MaterialPalette = 0x20013003,
		Portfolio = 0x20013004,

		Control = 0x50000000,
		CameraControl = 0x50000001,
		SelectabilityControl = 0x50000002,
		MarkerAttributeControl = 0x50000003,
		SphereAttributeControl = 0x50000004,
		LightingAttributeControl = 0x50000005,
		CylinderAttributeControl = 0x50000006,
		TextAttributeControl = 0x50000007,
		LineAttributeControl = 0x50000008,
		EdgeAttributeControl = 0x50000009,
		CurveAttributeControl = 0x5000000a,
		ModellingMatrixControl = 0x5000000b,
		TextureMatrixControl = 0x5000000c,
		CullingControl = 0x5000000d,
		TransparencyControl = 0x5000000e,
		MaterialMappingControl = 0x5000000f,
		NURBSSurfaceAttributeControl = 0x50000010,
		PostProcessEffectsControl = 0x50000011,
		BoundingControl = 0x50000012,
		VisualEffectsControl = 0x50000013,
		SelectionOptionsControl = 0x50000014,
		HighlightOptionsControl = 0x50000015,
		DefinitionControl = 0x50000016,
		SelectionControl = 0x50000017,
		HighlightControl = 0x50000018,
		StandAloneWindowOptionsControl = 0x50600019, //Is a Window Key
		OffScreenWindowOptionsControl = 0x5060001a, //Is a Window Key
		ApplicationWindowOptionsControl = 0x5060001b, //Is a Window Key
		VisibilityControl = 0x5000001c,
		SubwindowControl = 0x5000001d,
		PerformanceControl = 0x5000001e,
		HiddenLineAttributeControl = 0x5000001f,
		DrawingAttributeControl = 0x50000020,
		DebuggingControl = 0x50000021,
		ContourLineControl = 0x50000022,
		StyleControl = 0x50000023,
		ConditionControl = 0x50000024,
		PortfolioControl = 0x50000025,
		WindowInfoControl = 0x50000026,
		AttributeLockControl = 0x50000027,
		TransformMaskControl = 0x50000028,
		ColorInterpolationControl = 0x50000029,
		UpdateOptionsControl = 0x50600030, //Is a Window Key
		CuttingSectionAttributeControl = 0x50000031,

		Sprocket = 0x80000000,
		Canvas = 0x80000001,
		Layout = 0x80000002,
		View = 0x80000003,
		Model = 0x80000004,
		Operator = 0x80000005,
		SprocketPath = 0x80000007,

		Component = 0x80000200,
		ComponentMarkupView= 0x80000201,

		Filter = 0x80000600,
		Capture = 0x80000a00,
		CADModel = 0x80000300,
		ComponentPath = 0x81001000,

		SprocketKit = 0x81000000,

		MetaData = 0x80001000,
		IntegerMetaData = 0x80001001,
		UnsignedIntegerMetaData = 0x80001002,
		DoubleMetaData = 0x80001003,
		StringMetaData = 0x80001004,
		TimeMetaData = 0x80001005,
		BooleanMetaData = 0x80001006,
		DwordPtrMetaData = 0x80001007,

		SprocketControl = 0xD0000000,
		OperatorControl = 0xD0000008,
		NavigationCubeControl = 0xD0000009,
		AxisTriadControl = 0xD000000A,
		AnimationControl = 0xD000000B,

		PMI						= 0xE0000000,
		Datum					= 0xE0000001,
		Dimension				= 0xE0000002,
		FeatureControlFrame		= 0xE0000003,
		Generic					= 0xE0000004,
		Note					= 0xE0000005,
		Roughness				= 0xE0000006,
		TextAttributes			= 0xE0000007,
		Options					= 0xE0000008,
		PmiView					= 0xE0000009,
		Frame					= 0xE000000A,
		Draw					= 0xE000000B,
		Orintation				= 0xE000000C,

		ConditionalExpression = 0x09000000,
		NOTCondition = 0x09000001,
		ANDCondition = 0x09000002,
		ORCondition = 0x09000003,
		XORCondition = 0x09000004,
		EQCondition = 0x09000005,
		NEQCondition = 0x09000006,
		GTCondition = 0x09000007,
		LTCondition = 0x09000008,
		GTEQCondition = 0x09000009,
		LTEQCondition = 0x0900000A,
	};

	// MetaDataIndex는 UserDataIndex와 다른 값을 사용해야 함.
	// 저장할 때 Index를 구별자로 사용하기 때문
	enum class UserDataIndex : uint32_t
	{
		None									= 0x00000000,

		Type									= 0x01000001,
		Name									= 0x01000002,

		// Component 관련 Data Index
		ComponentType							= 0x02000001,
		ComponentStatus							= 0x02000002,
		IncludedCount							= 0x02000003,
		ReferenceCount							= 0x02000004,

		// PMI 관련 Data Index
		PMI										= 0x03000000,
		CameraKit								= 0x03000001,
		PmiType									= 0x03000002,
		RoughnessObtentionType					= 0x03000003,
		RoughnessApplicabilityType				= 0x03000004,
		RoughnessModeType						= 0x03000005,
		DatumType								= 0x03000006,
		DimensionType							= 0x03000007,
		DimensionSubType						= 0x03000008,
	};

	// MetaDataIndex는 UserDataIndex와 다른 값을 사용해야 함.
	// 저장할 때 Index를 구별자로 사용하기 때문
	enum class MetaDataIndex : uint32_t
	{
		None									= 0x00000000,

		ViewMatrix								= 0x03000001,
		Camera									= 0x03000002,
		CuttingPlanes							= 0x03000003,
		PlaneString								= 0x03000004,
	};

	class API_3DF ViewDirection
	{
	public:
		enum class Mode
		{
			top,
			bottom,
			front,
			back,
			left,
			right,

			py_nz,
			py_pz,
			ny_pz,
			ny_nz,

			nx_nz,
			nx_pz,
			px_pz,
			px_nz,

			nx_py,
			px_py,
			px_ny,
			nx_ny,

			nx_py_nz,
			nx_py_pz,
			nx_ny_pz,
			nx_ny_nz,

			px_py_pz, // ISO
			px_py_nz,
			px_ny_nz,
			px_ny_pz,

			Count,
			Unknown
		};
	private:
		ViewDirection() {}
	};

	class API_3DF Camera
	{
	public:
		enum class Mode : uint32_t
		{
			Multi = 1,
			Pan,
			Orbit,
			OrbitTurntable,
			Zoom,
			ZoomBox,
		};

		enum class Projection : uint32_t
		{
			Default = 1,
			Perspective,
			Orthographic,
			Stretched,
		};
		
	private:
		Camera() = default;
	};

	class API_3DF Rendering
	{
	public:
		enum class Mode
		{
			Gouraud,
			GouraudWithLines,
			Flat,
			FlatWithLines,
			Phong,
			PhongWithLines,
			HiddenLine,
			FastHiddenLine,
			Wireframe,
			Tessellated,
			Default = Gouraud
		};

	private:
		Rendering() {}
	};

	class API_3DF Performance
	{
	public:
		enum class DisplayLists : uint32_t
		{
			None,		// Display Lists will not be used for rendering the scene.
			Geometry,	// Display Lists will be created on a per-geometry basis. If the geometry is modified, the display list will be regenerated.
			Segment		// Display Lists will be created on a per-segment basis. If any geometry in the segment is modified, the display list will be regenerated.
		};

		// StaticModel is a technique used for improving rendering performance.
		// This setting tells the system that the segment tree affected by the attribute will remain 'static' or unchanging.
		// The system will create an internal, optimized segment tree which is used for rendering in lieu of the normal tree.
		// The original segment tree is untouched and can be used normally.

		// If changes are made in a part of the segment tree that is subject to the static model attribute, the internal tree will be regenerated, with a few exceptions:
		// If geometry is deleted or edited, the internal tree will not need to be regenerated.
		enum class StaticModel : uint32_t
		{
			None,				// No static model will be used, rendering will be done from the segment tree.
			Attribute,			// An optimized segment tree will be used for rendering. The tree will be sorted by attributes.  
			AttributeSpatial	// An optimized segment tree will be used for rendering. The tree will be sorted based on both attributes and spatial locations of objects. This is particularly useful for very large, spatially dispersed models.
		};

		// StaticConditions indicates how conditional expressions will be handled inside a StaticModel.
		// A model segment which has no conditionals expressions, or only has expressions satisfied by conditions
		// set within the segment should not be affected by this.
		enum class StaticConditions : uint32_t
		{
			// Conditional expressions will be maintained in the static tree. The tree may be referenced from multiple places (such as view) without forcing
			// regeneration, but the static tree will be sub-optimal
			Independent,

			// Conditional expressions will be evaluated assuming the current condition state available when the static tree is generated is the only one of interest.  The static tree will resolve
			// any use of conditions for better performance, but changes to this state will cause it to regenerate, and a static tree used in multiple places (such as different views) 
			// may thrash (continuously regenerate multiple times every update).
			Single

			// placeholder.  Option would generate different optimized trees at the cost of memory.
			//Multiple		//!< An optimized segment tree will be used for rendering. The tree will be sorted based on both attributes and spatial locations of objects.  This is particularly useful for very large, spatially dispersed models.
		};

	private:
		Performance() = default;
	};

	class API_3DF VisualEffects
	{
	public:
		enum class ShadowMode
		{
			None = 0,
			Soft = 1,
			Hard = 2
		};

	private:
		VisualEffects() = default;
	};

	class API_3DF Selectability
	{
	public:
		// Selectability Value
		enum class Value : uint32_t
		{
			Off = 0,	// Not selectable.
			On,			// Selectable if visible.
			ForcedOn	// Selectable even if not visible. 
		};

	private:
		Selectability() = default;
	};

	class HighlightMode
	{
	public:
		enum class Type : uint32_t
		{
			DefaultConditional,			// use default conditional highlighting
			Quickmoves,					// use quick move reference highlighting
			InverseTransparency,		// do nothing to the selected item, but make the rest of the model transparent.
			ColoredInverseTransparency	// color the selected item and make the rest of the model transparent.
		};
	private:
		HighlightMode() = default;
	};

	class API_3DF AttributeLock
	{
	public:
		enum class Type : uint32_t
		{
			Everything = 0x01000000,

			Visibility = 0x02000000,

			VisibilityCuttingSections,
			VisibilityCutEdges,
			VisibilityCutFaces,
			VisibilityWindows,
			VisibilityText,
			VisibilityLines,
			VisibilityEdgeLights,
			VisibilityMarkerLights,
			VisibilityFaceLights,
			VisibilityGenericEdges,
			VisibilityHardEdges,
			VisibilityAdjacentEdges,
			VisibilityInteriorSilhouetteEdges,
			VisibilityShadowEmitting,
			VisibilityShadowReceiving,
			VisibilityShadowCasting,
			VisibilityMarkers,
			VisibilityVertices,
			VisibilityFaces,
			VisibilityPerimeterEdges,
			VisibilityNonCulledEdges,
			VisibilityMeshQuadEdges,
			VisibilityCutGeometry,
			VisibilityEdges,
			VisibilityGeometry,
			VisibilityLights,
			VisibilityShadows,

			Material = 0x03000000,

			MaterialGeometry,
			MaterialCutGeometry,

			MaterialAmbientLightUpColor,
			MaterialAmbientLightDownColor,
			MaterialAmbientLightColor,

			MaterialWindowColor,
			MaterialWindowContrastColor,

			MaterialLightColor,
			MaterialLineColor,
			MaterialMarkerColor,
			MaterialTextColor,
			MaterialCutEdgeColor,

			MaterialVertex,
			MaterialVertexDiffuse,
			MaterialVertexDiffuseColor,
			MaterialVertexDiffuseAlpha,
			MaterialVertexDiffuseTexture,
			MaterialVertexSpecular,
			MaterialVertexMirror,
			MaterialVertexTransmission,
			MaterialVertexEmission,
			MaterialVertexEnvironment,
			MaterialVertexBump,
			MaterialVertexGloss,

			MaterialEdge,
			MaterialEdgeDiffuse,
			MaterialEdgeDiffuseColor,
			MaterialEdgeDiffuseAlpha,
			MaterialEdgeDiffuseTexture,
			MaterialEdgeSpecular,
			MaterialEdgeMirror,
			MaterialEdgeTransmission,
			MaterialEdgeEmission,
			MaterialEdgeEnvironment,
			MaterialEdgeBump,
			MaterialEdgeGloss,

			MaterialFace,
			MaterialFaceDiffuse,
			MaterialFaceDiffuseColor,
			MaterialFaceDiffuseAlpha,
			MaterialFaceDiffuseTexture,
			MaterialFaceSpecular,
			MaterialFaceMirror,
			MaterialFaceTransmission,
			MaterialFaceEmission,
			MaterialFaceEnvironment,
			MaterialFaceBump,
			MaterialFaceGloss,

			MaterialBackFace,
			MaterialBackFaceDiffuse,
			MaterialBackFaceDiffuseColor,
			MaterialBackFaceDiffuseAlpha,
			MaterialBackFaceDiffuseTexture,
			MaterialBackFaceSpecular,
			MaterialBackFaceMirror,
			MaterialBackFaceTransmission,
			MaterialBackFaceEmission,
			MaterialBackFaceEnvironment,
			MaterialBackFaceBump,
			MaterialBackFaceGloss,

			MaterialFrontFace,
			MaterialFrontFaceDiffuse,
			MaterialFrontFaceDiffuseColor,
			MaterialFrontFaceDiffuseAlpha,
			MaterialFrontFaceDiffuseTexture,
			MaterialFrontFaceSpecular,
			MaterialFrontFaceMirror,
			MaterialFrontFaceTransmission,
			MaterialFrontFaceEmission,
			MaterialFrontFaceEnvironment,
			MaterialFrontFaceBump,
			MaterialFrontFaceGloss,

			MaterialCutFace,
			MaterialCutFaceDiffuse,
			MaterialCutFaceDiffuseColor,
			MaterialCutFaceDiffuseAlpha,
			MaterialCutFaceDiffuseTexture,
			MaterialCutFaceSpecular,
			MaterialCutFaceMirror,
			MaterialCutFaceTransmission,
			MaterialCutFaceEmission,
			MaterialCutFaceEnvironment,
			MaterialCutFaceBump,
			MaterialCutFaceGloss,

			Camera = 0x04000000,

			LineAttribute = 0x05000000,
			LineAttributePattern,
			LineAttributeWeight,

			EdgeAttribute = 0x05100000,
			EdgeAttributePattern,
			EdgeAttributeWeight,
		};

	private:
		AttributeLock() {}
	};

	class API_3DF Drawing {
	public:
		// Handedness is used to define the front face of a polygon as well as the orientation of the z-axis relative to the x-y plane.
		enum class Handedness : uint32_t {
			None,
			Left,
			Right
		};

		enum class Overlay : uint32_t {
			// No overlay will be used.  If geometry is moved, edited or highlighted, a full screen redraw will be triggered on the subsequent update.
			None,

			// Any geometry in a segment with this mode set will be redrawn on each update and it will appear on top of the scene
			// regardless of whether it should appear behind other geometry. This is the most efficient overlay mode.
			// 이 모드가 설정된 세그먼트의 모든 형상은 업데이트할 때마다 다시 그려지며 다른 형상 뒤에 표시되어야 하는지 여부에 관계없이 장면 위에 표시됩니다.이는 가장 효율적인 오버레이 모드입니다.
			Default,

			// Any geometry in a segment with this mode set will be drawn on each update and it will be positioned correctly with
			// respect to other geometry within the scene. This is the most accurate overlay mode, but is not as efficient as NormalOverlay.
			// 이 모드가 설정된 세그먼트의 모든 형상은 업데이트할 때마다 그려지며 장면 내의 다른 형상과 관련하여 올바르게 배치됩니다.이는 가장 정확한 오버레이 모드이지만 NormalOverlay만큼 효율적이지는 않습니다.
			// 3DF의 spriting 옵션과 같은 옵션임.
			WithZValues,

			// This setting is useful when you are applying a transparent highlight style to overlay geometry.
			// This setting suppresses the drawing of the geometry, instead drawing the highlight itself in its place.
			// If you are not using a transparent highlight style, this setting has no visible effect. This setting is
			// slightly more computationally expensive than the other overlay settings.
			// 이 설정은 오버레이 형상에 투명한 강조 스타일을 적용할 때 유용합니다. 이 설정은 형상 그리기를 억제하고 그 자리에 하이라이트 자체를 그립니다.
			// 투명한 강조 스타일을 사용하지 않는 경우 이 설정은 눈에 보이는 효과가 없습니다.이 설정은 다른 오버레이 설정보다 계산 비용이 약간 더 높습니다.
			InPlace,
		};

		// Enumerates the clip region operation types.
		enum class ClipOperation : uint32_t {
			Keep, // The geometry inside the clip region is drawn. Everything outside of it is clipped.
			Remove, // The geometry outside the clip region is drawn. Everything inside of it is clipped.
		};

		// Enumerates the coordinate spaces types for clip regions.
		enum class ClipSpace : uint32_t {
			Window, // The clip region is specified in window coordinates.
			World, // The clip region is specified in world coordinated.
			Object, // The clip region is specified in object coordinated.
		};

	private:
		Drawing() {}
	};

	using WindowHandle = DWORD_PTR;
	using Time = double;

	//== Allocator Template Specializations ========================================================

#ifndef TDF_UNREFERENCED
#	define TDF_UNREFERENCED(param) ((void)(param))
#endif

	class API_3DF Memory
	{
	public:
		static void * Allocate(size_t nInBytes, bool bInClearMemory = false);
		static void	Free(void * pInPointer);

	private:
		//! Private default constructor to prevent instantiation.
		Memory();
	};

	template <typename T>
	class Allocator
	{
	public:
		typedef T					value_type;
		typedef value_type * pointer;
		typedef value_type const * const_pointer;
		typedef value_type & reference;
		typedef value_type const & const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;

		Allocator() {}
		Allocator(Allocator<T> const & in_that) { TDF_UNREFERENCED(in_that); }
		~Allocator() {}

		template <typename U> Allocator(Allocator<U> const &) {}

		template <typename U>
		struct rebind {
			typedef Allocator<U> other;
		};

		pointer address(reference x) const { return &x; }
		const_pointer address(const_reference x) const { return &x; }

		pointer allocate(size_type n, void * v = 0) { TDF_UNREFERENCED(v); return static_cast<pointer>(Memory::Allocate(n * sizeof(T))); }
		void deallocate(pointer p, size_type n) { TDF_UNREFERENCED(n); Memory::Free(p); }

#if defined(_MSC_VER) || defined (__APPLE__)
		void construct(pointer p, const_reference x) { new(p) T(x); }
		void construct(pointer p, value_type && x) { new(p) T(std::move(x)); }
#else
		template<typename U, typename... Args>
		void construct(U * p, Args&&... args) { new(p) U(std::forward<Args>(args)...); }
#endif

		void destroy(pointer p) { TDF_UNREFERENCED(p); p->~T(); }

		size_type max_size() const { return static_cast<size_type>(-1) / sizeof(T); }
	};

	template <typename T, typename U>
	bool operator == (const Allocator<T> &, const Allocator<U> &) { return true; }

	template <typename T, typename U>
	bool operator != (const Allocator<T> &, const Allocator<U> &) { return false; }


	//== Type Definitions ==========================================================================
	using SegmentKeyArray = std::vector<SegmentKey, Allocator<SegmentKey>>;
	using IncludeKeyArray = std::vector<IncludeKey, Allocator<IncludeKey>>;
	
	using ReferenceKeyArray = std::vector<ReferenceKey, Allocator<ReferenceKey>>;
	using LineArray = std::vector<LineKit, Allocator<LineKit>>;
	using PolylineArray = LineArray;
	using Polyline = LineKit;

	using WindowPointArray = std::vector<WindowPoint, Allocator<WindowPoint>>;
	using WorldPointArray = std::vector<WorldPoint, Allocator<WorldPoint>>;
	using PixelPointArray = std::vector<PixelPoint, Allocator<PixelPoint>>;

	using Polygon = PolygonKit;
	using PolygonArray = std::vector<PolygonKit, Allocator<PolygonKit>>;

	using StringArray = std::vector<CString, Allocator<CString>>;
	
	using IntPtrTArray = std::vector<intptr_t, Allocator<intptr_t>>;
	using ByteArray = std::vector<BYTE, Allocator<BYTE>>;
	using ByteArrayArray = std::vector<ByteArray, Allocator<ByteArray>>;

	using BoolArray = std::vector<bool, Allocator<bool>>;
	
	using AttributeLockTypeArray = std::vector<AttributeLock::Type, Allocator<AttributeLock::Type>>;

	using StyleKeyArray = std::vector<StyleKey, Allocator<StyleKey>>;

	using ConditionalExpressionArray = std::vector<ConditionalExpression, Allocator<ConditionalExpression>>;

	using ComponentArray = std::vector<Component *, Allocator<Component *>>;
	using MetadataArray = std::vector<MetaData *, Allocator<MetaData *>>;

	class TextAttributes;
	using TextAttributesArray = std::vector<TextAttributes, Allocator<TextAttributes>>;

	enum class ModelHandedness
	{
		Left,
		Right,
		None,
		NotSet
	};

	class API_3DF Impl
	{
	public:
		Impl() = default;
		virtual ~Impl() = default;

		H3DF::Type Type() const;
		void SetType(H3DF::Type eType);

		void SetImpl(Object * pcObject, Impl * pcImpl);

	protected:
		H3DF::Type m_eType = H3DF::Type::None;
	};
}
