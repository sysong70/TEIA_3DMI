#pragma once

#include "3DF.h"

#include "3DF.Kit.h"
#include "3DF.Control.h"

OPEN_3DF_NAMESPACE

class Search
{
public:
	// Space Enumeration of which segments to look in when performing a search.
	enum class Space : uint32_t
	{
		SegmentOnly,				// Search the current segment only.
		Subsegments,				// Search the current segment and any subsegments.
		SubsegmentsAndIncludes		// Search the current segment, any subsegments and any includes.
	};

	// Behavior Enumeration of the search behaviors.
	enum class Behavior : uint32_t
	{
		Exhaustive,					// Find every instance of the specified types in the specified search space.
		FirstMatch					// Find the first instance of any of the specified types in the specified search space.
	};
	
	// Type Enumeration of the types that can be searched for.
	enum class Type : uint32_t
	{
		None										= 0x00000000,	// No type.
		Everything									= 0x00ffffff,	// Search for all attributes, geometry, segments, includes and includers.

		Include										= 0x00000043,	// Search for include keys.
		Segment										= 0x00000044,	// Search for segments.

		Includer									= 0x00000100,	// Search for segments which include the current segment being searched.

		// Geometry
		Geometry									= 0x10000000,	// Search for all geometry types.

		CuttingSection								= 0x10000027,	// Search for cutting sections.
		Shell										= 0x10000029,	// Search for shells.
		Mesh						 				= 0x1000002a,	// Search for meshes.
		Grid										= 0x1000002b,	// Search for grids.
		NURBSSurface								= 0x1000002c,	// Search for NURBS surfaces.
		Cylinder									= 0x1000002e,	// Search for cylinders.
		Sphere										= 0x1000002f,	// Search for spheres.
		Polygon										= 0x10000030,	// Search for polygons.
		Circle										= 0x10000031,	// Search for circles.
		//unused									= 0x10000032,
		CircularWedge								= 0x10000033,	// Search for circular wedges.
		Ellipse										= 0x10000034,	// Search for ellipses.
		Line										= 0x10000035,	// Search for lines.
		NURBSCurve									= 0x10000038,	// Search for NURBS curves.
		CircularArc									= 0x10000039,	// Search for circular arcs.
		EllipticalArc								= 0x1000003a,	// Search for elliptical arcs.
		InfiniteLine								= 0x1000003b,	// Search for infinite lines.
		//unused									= 0x1000003c,
		Marker										= 0x1000003d,	// Search for markers.
		Text										= 0x1000003f,	// Search for text.
		Reference									= 0x10000042,	// Search for geometry references.

		DistantLight								= 0x10000100,	// Search for distant lights.
		Spotlight									= 0x10000101,	// Search for spotlights.
		InfiniteRay									= 0x10000102,	// Search for infinite rays.

		// Attributes
		Attribute									= 0x20000000,	// Search for all attributes.
		
		Priority									= 0x20000001,	// Search for priority settings.

		// Simple Attributes
		Material									= 0x2000000b,	// Search for material settings.
		PBRMaterial									= 0x2000000c,	// Search for pbr material settings.
		Camera										= 0x2000000f,	// Search for cameras.
		ModellingMatrix								= 0x20000011,	// Search for modelling matrices.
		UserData									= 0x20000022,	// Search for user data settings.
		TextureMatrix								= 0x20000024,	// Search for texture matrices.

		// Complex Attributes
		Culling										= 0x20001000,	// Search for any culling attribute settings.
		CullingFace								    = 0x20001001,	// Search for face culling settings.
		CullingExtent								= 0x20001002,	// Search for extent culling settings.
		CullingVector								= 0x20001003,	// Search for vector culling settings.
		CullingVectorTolerance						= 0x20001004,	// Search for vector tolerance culling settings.
		CullingFrustum								= 0x20001005,	// Search for frustum culling settings.
		CullingDeferralExtent						= 0x20001006,	// Search for deferral extent culling settings.
		CullingVolume								= 0x20001007,	// Search for volume culling settings.
		CullingDistance								= 0x20001008,	// Search for distance culling settings.

		CurveAttribute								= 0x20002000,	// Search for any curve attribute settings.
		CurveAttributeBudget						= 0x20002001,	// Search for curve budget settings.
		CurveAttributeContinuedBudget				= 0x20002002,	// Search for curve continued budget settings.
		CurveAttributeViewDependent					= 0x20002003,	// Search for curve view dependent settings.
		CurveAttributeMaximumDeviation				= 0x20002004,	// Search for curve maximum deviation settings.
		CurveAttributeMaximumAngle					= 0x20002005,	// Search for curve maximum angle settings.
		CurveAttributeMaximumLength					= 0x20002006,	// Search for curve maximum length settings.

		CylinderAttribute							= 0x20003000,	// Search for any cylinder attribute settings.
		CylinderAttributeTessellation				= 0x20003001,	// Search for cylinder tessellation settings.
		CylinderAttributeOrientation				= 0x20003002,	// Search for cylinder orientation settings.

		EdgeAttribute								= 0x20004000,	// Search for any edge attribute settings.
		EdgeAttributePattern						= 0x20004016,	// Search for edge pattern settings.
		EdgeAttributeWeight							= 0x20004017,	// Search for edge weight settings.
		EdgeAttributeHardAngle						= 0x20004018,	// Search for edge weight settings.

		LightingAttribute							= 0x20005000,	// Search for any lighting attribute settings.
		LightingAttributeInterpolation				= 0x20005001,	// Search for lighting interpolation settings.

		LineAttribute								= 0x20006000,	// Search for any line attribute settings.
		LineAttributePattern						= 0x2000601a,	// Search for line pattern settings.
		LineAttributeWeight							= 0x2000601b,	// Search for line weight settings.

		MarkerAttribute								= 0x20007000,	// Search for any marker attribute settings.
		MarkerAttributeGlyphRotation				= 0x2000701a,	// Search for marker glyph rotation settings.
		MarkerAttributeSize							= 0x2000701c,	// Search for marker size settings.
		MarkerAttributeSymbol						= 0x2000701d,	// Search for marker symbol settings.
		MarkerAttributePreference					= 0x2000701e,	// Search for marker drawing preference settings.

		SurfaceAttribute							= 0x20008000,	// Search for any surface attribute settings.
		SurfaceAttributeBudget						= 0x20008001,	// Search for surface budget settings.
		SurfaceAttributeMaximumFacetDeviation		= 0x20008002,	// Search for surface maximum facet deviation settings.
		SurfaceAttributeMaximumFacetAngle			= 0x20008003,	// Search for surface maximum facet angle settings.
		SurfaceAttributeMaximumFacetWidth			= 0x20008004,	// Search for surface maximum facet width settings.
		SurfaceAttributeTrimCurveBudget				= 0x20008005,	// Search for surface trim curve budget settings.
		SurfaceAttributeMaximumTrimCurveDeviation	= 0x20008006,	// Search for surface maximum trim curve deviation settings.

		Selectability								= 0x20009000,	// Search for any selectability settings.
		SelectabilityWindows						= 0x20009001,	// Search for window selectability settings.
		SelectabilityEdges							= 0x20009002,	// Search for edge selectability settings.
		SelectabilityFaces							= 0x20009003,	// Search for face selectability settings.
		SelectabilityLights							= 0x20009004,	// Search for light selectability settings.
		SelectabilityLines							= 0x20009005,	// Search for line selectability settings.
		SelectabilityMarkers						= 0x20009006,	// Search for marker selectability settings.
		SelectabilityVertices						= 0x20009007,	// Search for vertex selectability settings.
		SelectabilityText							= 0x20009008,	// Search for text selectability settings.

		SphereAttribute								= 0x2000a000,	// Search for any sphere attribute settings.
		SphereAttributeTessellation					= 0x2000a001,	// Search for sphere tessellation settings.

		Subwindow									= 0x2000b000,	// Search for any subwindow settings.
		SubwindowEitherType							= 0x2000b001,	// Search for either standard or lightweight subwindow settings.
		SubwindowStandard							= 0x2000b00e,	// Search for standard subwindow settings.
		SubwindowLightweight						= 0x2000b002,	// Search for lightweight subwindow settings.
		SubwindowBackground							= 0x2000b003,	// Search for subwindow background settings.
		SubwindowBorder								= 0x2000b004,	// Search for subwindow border settings.
		SubwindowRenderingAlgorithm					= 0x2000b005,   // Search for subwindow rendering algorithm.

		TextAttribute								= 0x2000c000,	// Search for any text attribute settings.
		TextAttributeAlignment						= 0x2000c01e,	// Search for text alignment settings.
		TextAttributeBold							= 0x2000c002,	// Search for bold text settings.
		TextAttributeItalic							= 0x2000c003,	// Search for italic text settings.
		TextAttributeOverline						= 0x2000c004,	// Search for overline text settings.
		TextAttributeStrikethrough					= 0x2000c005,	// Search for strikethrough text settings.
		TextAttributeUnderline						= 0x2000c006,	// Search for underline text settings.
		TextAttributeSlant							= 0x2000c007,	// Search for text slant settings.
		TextAttributeLineSpacing					= 0x2000c008,	// Search for text line spacing settings.
		TextAttributeRotation						= 0x2000c00a,	// Search for text rotation settings.
		TextAttributeExtraSpace						= 0x2000c00b,	// Search for text extra space settings.
		TextAttributeGreeking						= 0x2000c00c,	// Search for text greeking settings.
		TextAttributeSizeTolerance					= 0x2000c00d,	// Search for text size tolerance settings.
		TextAttributeSize							= 0x2000c00e,	// Search for text size settings.
		TextAttributeFont							= 0x2000c00f,	// Search for text font settings.
		TextAttributeTransform						= 0x2000c010,	// Search for text transform settings.
		TextAttributeRenderer						= 0x2000c011,	// Search for text renderer settings.
		TextAttributePreference						= 0x2000c012,	// Search for text preference settings.
		TextAttributePath							= 0x2000c020,	// Search for text path settings.
		TextAttributeSpacing						= 0x2000c021,	// Search for text spacing settings.
		TextAttributeBackground						= 0x2000c022,	// Search for text background settings.
		TextAttributeBackgroundMargins				= 0x2000c023,	// Search for text background margin settings.
		TextAttributeBackgroundStyle				= 0x2000c024,	// Search for text background style settings.
		TextAttributeLeaderLine						= 0x2000c025,	// Search for text leader line settings.

		Transparency								= 0x2000d000,	// Search for any transparency settings.
		TransparencyMethod							= 0x2000d001,	// Search for transparency method settings.
		TransparencyAlgorithm						= 0x2000d002,	// Search for transparency algorithm settings.
		TransparencyDepthPeelingLayers				= 0x2000d004,	// Search for depth peeling layers settings.
		TransparencyDepthPeelingMinimumArea			= 0x2000d005,	// Search for depth peeling minimum area settings.
		TransparencyDepthWriting					= 0x2000d006,	// Search for depth writing settings.

		Visibility									= 0x2000e000,	// Search for any visibility settings.
		VisibilityCuttingSections					= 0x2000e001,	// Search for cutting section visibility settings.
		VisibilityCutEdges							= 0x2000e002,	// Search for cut edge visibility settings.
		VisibilityCutFaces							= 0x2000e003,	// Search for cut face visibility settings.
		VisibilityWindows							= 0x2000e004,	// Search for window visibility settings.
		VisibilityText								= 0x2000e005,	// Search for text visibility settings.
		VisibilityLines								= 0x2000e006,	// Search for line visibility settings.
		VisibilityEdgeLights						= 0x2000e007,	// Search for edge light visibility settings.
		VisibilityMarkerLights						= 0x2000e008,	// Search for marker light visibility settings.
		VisibilityFaceLights						= 0x2000e009,	// Search for face light visibility settings.
		VisibilityGenericEdges						= 0x2000e00a,	// Search for generic edge visibility settings.
		VisibilityHardEdges							= 0x2000e00b,	// Search for hard edge visibility settings.
		VisibilityAdjacentEdges						= 0x2000e00c,	// Search for adjacent edge visibility settings.
		VisibilityInteriorSilhouetteEdges			= 0x2000e00d,	// Search for interior silhouette edge visibility settings.
		VisibilityShadowEmitting					= 0x2000e00e,	// Search for shadow emitting visibility settings.
		VisibilityShadowReceiving					= 0x2000e00f,	// Search for shadow receiving visibility settings.
		VisibilityShadowCasting						= 0x2000e010,	// Search for shadow casting visibility settings.
		VisibilityMarkers							= 0x2000e011,	// Search for marker visibility settings.
		VisibilityVertices							= 0x2000e012,	// Search for vertex visibility settings.
		VisibilityFaces								= 0x2000e013,	// Search for face visibility settings.
		VisibilityPerimeterEdges					= 0x2000e014,	// Search for perimeter edge visibility settings.
		VisibilityNonCulledEdges					= 0x2000e015,	// Search for non-culled edge visibility settings.
		VisibilityMeshQuadEdges						= 0x2000e016,	// Search for mesh quad edge visibility settings.
		VisibilityEdges								= 0x2000e017,	// Search for any edge visibility settings.
		VisibilityLeaderLines						= 0x2000e018,	// Search for text leader line visibility settings.

		VisualEffects								= 0x2000f000,	// Search for any visual effects settings.
		VisualEffectsPostProcessEffectsEnabled		= 0x2000f001,	// Search for visual effects post-process effects enabled settings.
		VisualEffectsAntiAliasing					= 0x2000f002,	// Search for visual effects anti-aliasing settings.
		VisualEffectsShadowMaps						= 0x2000f003,	// Search for visual effects shadow maps settings.
		VisualEffectsSimpleShadow					= 0x2000f004,	// Search for visual effects simple shadow settings.
		VisualEffectsSimpleShadowPlane				= 0x2000f005,	// Search for visual effects simple shadow plane settings.
		VisualEffectsSimpleShadowLightDirection		= 0x2000f006,	// Search for visual effects simple shadow light direction settings.
		VisualEffectsSimpleShadowColor				= 0x2000f007,	// Search for visual effects simple shadow color settings.
		VisualEffectsSimpleReflection				= 0x2000f008,	// Search for visual effects simple reflection settings.
		VisualEffectsSimpleReflectionPlane			= 0x2000f009,	// Search for visual effects simple reflection plane settings.
		VisualEffectsSimpleReflectionVisibility		= 0x2000f00a,	// Search for visual effects simple reflection visibility settings.
		VisualEffectsEyeDomeLightingBackColor		= 0x2000f00b,	// Search for visual effects eye dome lighting back color settings.

		Performance									= 0x20010000,	// Search for any performance settings.
		PerformanceDisplayLists						= 0x20010001,	// Search for performance display lists settings.
		PerformanceStaticModel						= 0x20010002,	// Search for performance static model settings.
		PerformanceTextHardwareAcceleration			= 0x20010004,	// Search for performance display lists settings.
		StaticModelSegment							= 0x40000043,	// Search for static model segments.

		DrawingAttribute							= 0x20011000,	// Search for any drawing attribute settings.
		DrawingAttributePolygonHandedness			= 0x20011001,	// Search for drawing attribute polygon handedness settings.
		DrawingAttributeDepthRange					= 0x20011002,	// Search for drawing attribute depth range settings.
		DrawingAttributeFaceDisplacement			= 0x20011003,	// Search for drawing attribute face displacement settings.
		DrawingAttributeGeneralDisplacement			= 0x20011004,	// Search for drawing attribute general displacement settings.
		DrawingAttributeVertexDisplacement			= 0x20011005,	// Search for drawing attribute vertex displacement settings.
		DrawingAttributeOverlay						= 0x20011006,	// Search for drawing attribute overlay settings.
		DrawingAttributeDeferral					= 0x20011007,	// Search for drawing attribute deferral settings.
		DrawingAttributeVertexDecimation			= 0x20011008,	// Search for drawing attribute vertex decimation settings.
		DrawingAttributeVertexRandomization			= 0x20011009,	// Search for drawing attribute vertex randomization settings.
		DrawingAttributeOverrideInternalColor		= 0x2001100a,	// Search for drawing attribute override internal color settings.
		DrawingAttributeClipRegion					= 0x20011013,	// Search for drawing attribute clip region settings.
		DrawingAttributeWorldHandedness				= 0x20011019,	// Search for drawing attribute world handedness settings.

		
		HiddenLineAttribute							= 0x20012000,	// Search for any hidden line attribute settings.
		HiddenLineAttributeAlgorithm				= 0x20012001,	// Search for hidden line algorithm settings.
		HiddenLineAttributeColor					= 0x20012002,	// Search for hidden line color settings.
		HiddenLineAttributeDimFactor				= 0x20012003,	// Search for hidden line dim factor settings.
		HiddenLineAttributeFaceDisplacement			= 0x20012004,	// Search for hidden line face displacement settings.
		HiddenLineAttributeLinePattern				= 0x20012005,	// Search for hidden line pattern settings.
		HiddenLineAttributeLineSort					= 0x20012006,	// Search for hidden line sort settings.
		HiddenLineAttributeRenderFaces				= 0x20012007,	// Search for hidden line render faces settings.
		HiddenLineAttributeRenderText				= 0x20012008,	// Search for hidden line render text settings.
		HiddenLineAttributeSilhouetteCleanup		= 0x20012009, 	// Search for hidden line silhouette cleanup settings.
		HiddenLineAttributeTransparencyCutoff		= 0x2001200a,	// Search for hidden line transparency cutoff settings.
		HiddenLineAttributeVisibility				= 0x2001200b,	// Search for hidden line visibility settings.
		HiddenLineAttributeWeight					= 0x2001200c,	// Search for hidden line weight settings.
		

		SegmentStyle								= 0x20013001,	// Search for any segment style settings.
		NamedStyle									= 0x20013002,	// Search for any named style settings.
		MaterialPalette								= 0x20013003,	// Search for any material palette settings.
		Portfolio									= 0x20013004,	// Search for any portfolio settings.

		ContourLine									= 0x20014000,	// Search for any contour line settings.
		ContourLineVisibility						= 0x20014001,	// Search for contour line visibility settings.
		ContourLinePosition							= 0x20014002,	// Search for contour line position settings.
		ContourLineColor							= 0x20014003,	// Search for contour line color settings.
		ContourLinePattern							= 0x20014004,	// Search for contour line pattern settings.
		ContourLineWeight							= 0x20014005,	// Search for contour line weight settings.
		ContourLineLighting							= 0x20014006,	// Search for contour line lighting settings.

		Condition									= 0x20015000,	// Search for any condition settings.

		Bounding									= 0x20016000,	// Search for any bounding settings.
		BoundingVolume								= 0x20016001,	// Search for bounding volume settings.
		BoundingExclusion							= 0x20016002,	// Search for bounding exclusion settings.
		
		AttributeLock								= 0x20017000,	// Search for any attribute lock settings.
		AttributeLockSetting						= 0x20017001,	// Search for attribute lock setting settings.
		AttributeLockSubsegmentOverride				= 0x20017002,	// Search for attribute lock subsegment override settings.

		TransformMask								= 0x20018000,	// Search for any transform mask settings.
		TransformMaskCamera							= 0x20018001,	// Search for any transform mask camera settings.
		TransformMaskCameraTranslation				= 0x20018002,	// Search for transform mask translation settings.
		TransformMaskCameraScale					= 0x20018003,	// Search for transform mask scale settings.
		TransformMaskCameraOffset					= 0x20018004,	// Search for transform mask offset settings.
		TransformMaskCameraRotation					= 0x20018005,	// Search for transform mask rotation settings.
		TransformMaskCameraPerspectiveScale			= 0x20018006,	// Search for transform mask perspective scale settings.
		TransformMaskCameraProjection				= 0x20018007,	// Search for transform mask projection settings.
		TransformMaskModellingMatrix				= 0x20018008,	// Search for any transform mask modelling matrix settings.
		TransformMaskModellingMatrixTranslation		= 0x20018009,	// Search for transform mask modelling matrix translation settings.
		TransformMaskModellingMatrixScale			= 0x20018010,	// Search for transform mask modelling matrix scale settings.
		TransformMaskModellingMatrixOffset			= 0x20018011,	// Search for transform mask modelling matrix offset settings.
		TransformMaskModellingMatrixRotation		= 0x20018012,	// Search for transform mask modelling matrix rotation settings.

		ColorInterpolation							= 0x20019000,	// Search for any color interpolation settings.
		ColorInterpolationFaceColor					= 0x20019001,	// Search for color interpolation face color settings.
		ColorInterpolationEdgeColor					= 0x20019002,	// Search for color interpolation edge color settings.
		ColorInterpolationMarkerColor				= 0x20019003,	// Search for color interpolation marker color settings.
		ColorInterpolationFaceIndex					= 0x20019004,	// Search for color interpolation face index settings.
		ColorInterpolationEdgeIndex					= 0x20019005,	// Search for color interpolation edge index settings.
		ColorInterpolationMarkerIndex				= 0x20019006,	// Search for color interpolation marker index settings.

		CuttingSectionAttribute						= 0x2001a000,	// Search for any cutting section attribute settings.
		CuttingSectionAttributeCuttingLevel			= 0x2001a001,	// Search for cutting section cutting level settings.
		CuttingSectionAttributeCappingLevel			= 0x2001a002,	// Search for cutting section capping level settings.
		CuttingSectionAttributeMaterialPreference	= 0x2001a003,	// Search for cutting section material preference settings.
		CuttingSectionAttributeEdgeWeight			= 0x2001a004,	// Search for cutting section edge weight settings.
		CuttingSectionAttributeTolerance			= 0x2001a005,	// Search for cutting section tolerance settings.
		CuttingSectionAttributeCappingUsage			= 0x2001a006,	// Search for cutting section capping level settings.

	};

private:
	Search() {}
};

using SearchTypeArray = std::vector<Search::Type, Allocator<Search::Type> >;

class SearchOptionsKit : public Kit
{
public:
	SearchOptionsKit();
	SearchOptionsKit(SearchOptionsKit const & cInKit);

	void Set(SearchOptionsKit const & cInThat);
	SearchOptionsKit & operator=(SearchOptionsKit && cInThat);
};

CLOSE_3DF_NAMESPACE