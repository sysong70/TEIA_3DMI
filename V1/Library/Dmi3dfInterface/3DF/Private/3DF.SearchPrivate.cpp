#include "StdAfx.h"

#include "3DF.SearchPrivate.h"

USING_3DF_NAMESPACE

CString SearchPrivate::GetSearchTypeString(Search::Type eType)
{
	CString strType;

	switch (eType)
	{
		case TDF::Search::Type::Everything:
			strType = _T("everything");
			break;

		case TDF::Search::Type::Include:
			strType = _T("include");
			break;

		case TDF::Search::Type::Segment:
			strType = _T("segment");
			break;

		case TDF::Search::Type::Includer:
			break;

		case TDF::Search::Type::Geometry:
			strType = _T("geometry");
			break;

		case TDF::Search::Type::Shell:
			strType = _T("shell");	
			break;

		case TDF::Search::Type::Line:
			strType = _T("lines, polylines");
			break;

/*
		case TDF::Search::Type::CuttingSection:
			break;

		case TDF::Search::Type::Shell:
			break;
		case TDF::Search::Type::Mesh:
			break;
		case TDF::Search::Type::Grid:
			break;
		case TDF::Search::Type::NURBSSurface:
			break;
		case TDF::Search::Type::Cylinder:
			break;
		case TDF::Search::Type::Sphere:
			break;
		case TDF::Search::Type::Polygon:
			break;
		case TDF::Search::Type::Circle:
			break;
		case TDF::Search::Type::CircularWedge:
			break;
		case TDF::Search::Type::Ellipse:
			break;
		case TDF::Search::Type::NURBSCurve:
			break;
		case TDF::Search::Type::CircularArc:
			break;
		case TDF::Search::Type::EllipticalArc:
			break;
		case TDF::Search::Type::InfiniteLine:
			break;
		case TDF::Search::Type::Marker:
			break;
		case TDF::Search::Type::Text:
			break;
		case TDF::Search::Type::Reference:
			break;
		case TDF::Search::Type::DistantLight:
			break;
		case TDF::Search::Type::Spotlight:
			break;
		case TDF::Search::Type::InfiniteRay:
			break;
		case TDF::Search::Type::Attribute:
			break;
		case TDF::Search::Type::Priority:
			break;
		case TDF::Search::Type::Material:
			break;
		case TDF::Search::Type::PBRMaterial:
			break;
		case TDF::Search::Type::Camera:
			break;
		case TDF::Search::Type::ModellingMatrix:
			break;
		case TDF::Search::Type::UserData:
			break;
		case TDF::Search::Type::TextureMatrix:
			break;
		case TDF::Search::Type::Culling:
			break;
		case TDF::Search::Type::CullingFace:
			break;
		case TDF::Search::Type::CullingExtent:
			break;
		case TDF::Search::Type::CullingVector:
			break;
		case TDF::Search::Type::CullingVectorTolerance:
			break;
		case TDF::Search::Type::CullingFrustum:
			break;
		case TDF::Search::Type::CullingDeferralExtent:
			break;
		case TDF::Search::Type::CullingVolume:
			break;
		case TDF::Search::Type::CullingDistance:
			break;
		case TDF::Search::Type::CurveAttribute:
			break;
		case TDF::Search::Type::CurveAttributeBudget:
			break;
		case TDF::Search::Type::CurveAttributeContinuedBudget:
			break;
		case TDF::Search::Type::CurveAttributeViewDependent:
			break;
		case TDF::Search::Type::CurveAttributeMaximumDeviation:
			break;
		case TDF::Search::Type::CurveAttributeMaximumAngle:
			break;
		case TDF::Search::Type::CurveAttributeMaximumLength:
			break;
		case TDF::Search::Type::CylinderAttribute:
			break;
		case TDF::Search::Type::CylinderAttributeTessellation:
			break;
		case TDF::Search::Type::CylinderAttributeOrientation:
			break;
		case TDF::Search::Type::EdgeAttribute:
			break;
		case TDF::Search::Type::EdgeAttributePattern:
			break;
		case TDF::Search::Type::EdgeAttributeWeight:
			break;
		case TDF::Search::Type::EdgeAttributeHardAngle:
			break;
		case TDF::Search::Type::LightingAttribute:
			break;
		case TDF::Search::Type::LightingAttributeInterpolation:
			break;
		case TDF::Search::Type::LineAttribute:
			break;
		case TDF::Search::Type::LineAttributePattern:
			break;
		case TDF::Search::Type::LineAttributeWeight:
			break;
		case TDF::Search::Type::MarkerAttribute:
			break;
		case TDF::Search::Type::MarkerAttributeGlyphRotation:
			break;
		case TDF::Search::Type::MarkerAttributeSize:
			break;
		case TDF::Search::Type::MarkerAttributeSymbol:
			break;
		case TDF::Search::Type::MarkerAttributePreference:
			break;
		case TDF::Search::Type::SurfaceAttribute:
			break;
		case TDF::Search::Type::SurfaceAttributeBudget:
			break;
		case TDF::Search::Type::SurfaceAttributeMaximumFacetDeviation:
			break;
		case TDF::Search::Type::SurfaceAttributeMaximumFacetAngle:
			break;
		case TDF::Search::Type::SurfaceAttributeMaximumFacetWidth:
			break;
		case TDF::Search::Type::SurfaceAttributeTrimCurveBudget:
			break;
		case TDF::Search::Type::SurfaceAttributeMaximumTrimCurveDeviation:
			break;
		case TDF::Search::Type::Selectability:
			break;
		case TDF::Search::Type::SelectabilityWindows:
			break;
		case TDF::Search::Type::SelectabilityEdges:
			break;
		case TDF::Search::Type::SelectabilityFaces:
			break;
		case TDF::Search::Type::SelectabilityLights:
			break;
		case TDF::Search::Type::SelectabilityLines:
			break;
		case TDF::Search::Type::SelectabilityMarkers:
			break;
		case TDF::Search::Type::SelectabilityVertices:
			break;
		case TDF::Search::Type::SelectabilityText:
			break;
		case TDF::Search::Type::SphereAttribute:
			break;
		case TDF::Search::Type::SphereAttributeTessellation:
			break;
		case TDF::Search::Type::Subwindow:
			break;
		case TDF::Search::Type::SubwindowEitherType:
			break;
		case TDF::Search::Type::SubwindowStandard:
			break;
		case TDF::Search::Type::SubwindowLightweight:
			break;
		case TDF::Search::Type::SubwindowBackground:
			break;
		case TDF::Search::Type::SubwindowBorder:
			break;
		case TDF::Search::Type::SubwindowRenderingAlgorithm:
			break;
		case TDF::Search::Type::TextAttribute:
			break;
		case TDF::Search::Type::TextAttributeAlignment:
			break;
		case TDF::Search::Type::TextAttributeBold:
			break;
		case TDF::Search::Type::TextAttributeItalic:
			break;
		case TDF::Search::Type::TextAttributeOverline:
			break;
		case TDF::Search::Type::TextAttributeStrikethrough:
			break;
		case TDF::Search::Type::TextAttributeUnderline:
			break;
		case TDF::Search::Type::TextAttributeSlant:
			break;
		case TDF::Search::Type::TextAttributeLineSpacing:
			break;
		case TDF::Search::Type::TextAttributeRotation:
			break;
		case TDF::Search::Type::TextAttributeExtraSpace:
			break;
		case TDF::Search::Type::TextAttributeGreeking:
			break;
		case TDF::Search::Type::TextAttributeSizeTolerance:
			break;
		case TDF::Search::Type::TextAttributeSize:
			break;
		case TDF::Search::Type::TextAttributeFont:
			break;
		case TDF::Search::Type::TextAttributeTransform:
			break;
		case TDF::Search::Type::TextAttributeRenderer:
			break;
		case TDF::Search::Type::TextAttributePreference:
			break;
		case TDF::Search::Type::TextAttributePath:
			break;
		case TDF::Search::Type::TextAttributeSpacing:
			break;
		case TDF::Search::Type::TextAttributeBackground:
			break;
		case TDF::Search::Type::TextAttributeBackgroundMargins:
			break;
		case TDF::Search::Type::TextAttributeBackgroundStyle:
			break;
		case TDF::Search::Type::TextAttributeLeaderLine:
			break;
		case TDF::Search::Type::Transparency:
			break;
		case TDF::Search::Type::TransparencyMethod:
			break;
		case TDF::Search::Type::TransparencyAlgorithm:
			break;
		case TDF::Search::Type::TransparencyDepthPeelingLayers:
			break;
		case TDF::Search::Type::TransparencyDepthPeelingMinimumArea:
			break;
		case TDF::Search::Type::TransparencyDepthWriting:
			break;
		case TDF::Search::Type::Visibility:
			break;
		case TDF::Search::Type::VisibilityCuttingSections:
			break;
		case TDF::Search::Type::VisibilityCutEdges:
			break;
		case TDF::Search::Type::VisibilityCutFaces:
			break;
		case TDF::Search::Type::VisibilityWindows:
			break;
		case TDF::Search::Type::VisibilityText:
			break;
		case TDF::Search::Type::VisibilityLines:
			break;
		case TDF::Search::Type::VisibilityEdgeLights:
			break;
		case TDF::Search::Type::VisibilityMarkerLights:
			break;
		case TDF::Search::Type::VisibilityFaceLights:
			break;
		case TDF::Search::Type::VisibilityGenericEdges:
			break;
		case TDF::Search::Type::VisibilityHardEdges:
			break;
		case TDF::Search::Type::VisibilityAdjacentEdges:
			break;
		case TDF::Search::Type::VisibilityInteriorSilhouetteEdges:
			break;
		case TDF::Search::Type::VisibilityShadowEmitting:
			break;
		case TDF::Search::Type::VisibilityShadowReceiving:
			break;
		case TDF::Search::Type::VisibilityShadowCasting:
			break;
		case TDF::Search::Type::VisibilityMarkers:
			break;
		case TDF::Search::Type::VisibilityVertices:
			break;
		case TDF::Search::Type::VisibilityFaces:
			break;
		case TDF::Search::Type::VisibilityPerimeterEdges:
			break;
		case TDF::Search::Type::VisibilityNonCulledEdges:
			break;
		case TDF::Search::Type::VisibilityMeshQuadEdges:
			break;
		case TDF::Search::Type::VisibilityEdges:
			break;
		case TDF::Search::Type::VisibilityLeaderLines:
			break;
		case TDF::Search::Type::VisualEffects:
			break;
		case TDF::Search::Type::VisualEffectsPostProcessEffectsEnabled:
			break;
		case TDF::Search::Type::VisualEffectsAntiAliasing:
			break;
		case TDF::Search::Type::VisualEffectsShadowMaps:
			break;
		case TDF::Search::Type::VisualEffectsSimpleShadow:
			break;
		case TDF::Search::Type::VisualEffectsSimpleShadowPlane:
			break;
		case TDF::Search::Type::VisualEffectsSimpleShadowLightDirection:
			break;
		case TDF::Search::Type::VisualEffectsSimpleShadowColor:
			break;
		case TDF::Search::Type::VisualEffectsSimpleReflection:
			break;
		case TDF::Search::Type::VisualEffectsSimpleReflectionPlane:
			break;
		case TDF::Search::Type::VisualEffectsSimpleReflectionVisibility:
			break;
		case TDF::Search::Type::VisualEffectsEyeDomeLightingBackColor:
			break;
		case TDF::Search::Type::Performance:
			break;
		case TDF::Search::Type::PerformanceDisplayLists:
			break;
		case TDF::Search::Type::PerformanceStaticModel:
			break;
		case TDF::Search::Type::PerformanceTextHardwareAcceleration:
			break;
		case TDF::Search::Type::StaticModelSegment:
			break;
		case TDF::Search::Type::DrawingAttribute:
			break;
		case TDF::Search::Type::DrawingAttributePolygonHandedness:
			break;
		case TDF::Search::Type::DrawingAttributeDepthRange:
			break;
		case TDF::Search::Type::DrawingAttributeFaceDisplacement:
			break;
		case TDF::Search::Type::DrawingAttributeGeneralDisplacement:
			break;
		case TDF::Search::Type::DrawingAttributeVertexDisplacement:
			break;
		case TDF::Search::Type::DrawingAttributeOverlay:
			break;
		case TDF::Search::Type::DrawingAttributeDeferral:
			break;
		case TDF::Search::Type::DrawingAttributeVertexDecimation:
			break;
		case TDF::Search::Type::DrawingAttributeVertexRandomization:
			break;
		case TDF::Search::Type::DrawingAttributeOverrideInternalColor:
			break;
		case TDF::Search::Type::DrawingAttributeClipRegion:
			break;
		case TDF::Search::Type::DrawingAttributeWorldHandedness:
			break;
		case TDF::Search::Type::HiddenLineAttribute:
			break;
		case TDF::Search::Type::HiddenLineAttributeAlgorithm:
			break;
		case TDF::Search::Type::HiddenLineAttributeColor:
			break;
		case TDF::Search::Type::HiddenLineAttributeDimFactor:
			break;
		case TDF::Search::Type::HiddenLineAttributeFaceDisplacement:
			break;
		case TDF::Search::Type::HiddenLineAttributeLinePattern:
			break;
		case TDF::Search::Type::HiddenLineAttributeLineSort:
			break;
		case TDF::Search::Type::HiddenLineAttributeRenderFaces:
			break;
		case TDF::Search::Type::HiddenLineAttributeRenderText:
			break;
		case TDF::Search::Type::HiddenLineAttributeSilhouetteCleanup:
			break;
		case TDF::Search::Type::HiddenLineAttributeTransparencyCutoff:
			break;
		case TDF::Search::Type::HiddenLineAttributeVisibility:
			break;
		case TDF::Search::Type::HiddenLineAttributeWeight:
			break;
		case TDF::Search::Type::SegmentStyle:
			break;
		case TDF::Search::Type::NamedStyle:
			break;
		case TDF::Search::Type::MaterialPalette:
			break;
		case TDF::Search::Type::Portfolio:
			break;
		case TDF::Search::Type::ContourLine:
			break;
		case TDF::Search::Type::ContourLineVisibility:
			break;
		case TDF::Search::Type::ContourLinePosition:
			break;
		case TDF::Search::Type::ContourLineColor:
			break;
		case TDF::Search::Type::ContourLinePattern:
			break;
		case TDF::Search::Type::ContourLineWeight:
			break;
		case TDF::Search::Type::ContourLineLighting:
			break;
		case TDF::Search::Type::Condition:
			break;
		case TDF::Search::Type::Bounding:
			break;
		case TDF::Search::Type::BoundingVolume:
			break;
		case TDF::Search::Type::BoundingExclusion:
			break;
		case TDF::Search::Type::AttributeLock:
			break;
		case TDF::Search::Type::AttributeLockSetting:
			break;
		case TDF::Search::Type::AttributeLockSubsegmentOverride:
			break;
		case TDF::Search::Type::TransformMask:
			break;
		case TDF::Search::Type::TransformMaskCamera:
			break;
		case TDF::Search::Type::TransformMaskCameraTranslation:
			break;
		case TDF::Search::Type::TransformMaskCameraScale:
			break;
		case TDF::Search::Type::TransformMaskCameraOffset:
			break;
		case TDF::Search::Type::TransformMaskCameraRotation:
			break;
		case TDF::Search::Type::TransformMaskCameraPerspectiveScale:
			break;
		case TDF::Search::Type::TransformMaskCameraProjection:
			break;
		case TDF::Search::Type::TransformMaskModellingMatrix:
			break;
		case TDF::Search::Type::TransformMaskModellingMatrixTranslation:
			break;
		case TDF::Search::Type::TransformMaskModellingMatrixScale:
			break;
		case TDF::Search::Type::TransformMaskModellingMatrixOffset:
			break;
		case TDF::Search::Type::TransformMaskModellingMatrixRotation:
			break;
		case TDF::Search::Type::ColorInterpolation:
			break;
		case TDF::Search::Type::ColorInterpolationFaceColor:
			break;
		case TDF::Search::Type::ColorInterpolationEdgeColor:
			break;
		case TDF::Search::Type::ColorInterpolationMarkerColor:
			break;
		case TDF::Search::Type::ColorInterpolationFaceIndex:
			break;
		case TDF::Search::Type::ColorInterpolationEdgeIndex:
			break;
		case TDF::Search::Type::ColorInterpolationMarkerIndex:
			break;
		case TDF::Search::Type::CuttingSectionAttribute:
			break;
		case TDF::Search::Type::CuttingSectionAttributeCuttingLevel:
			break;
		case TDF::Search::Type::CuttingSectionAttributeCappingLevel:
			break;
		case TDF::Search::Type::CuttingSectionAttributeMaterialPreference:
			break;
		case TDF::Search::Type::CuttingSectionAttributeEdgeWeight:
			break;
		case TDF::Search::Type::CuttingSectionAttributeTolerance:
			break;
		case TDF::Search::Type::CuttingSectionAttributeCappingUsage:
			break;
*/

		default:

			break;
	}

	return strType;
}

CString SearchPrivate::GetSearchSpaceString(Search::Space eInSpace)
{
	CString strSpace;

	switch (eInSpace)
	{
		case TDF::Search::Space::SegmentOnly:
			strSpace = L".";
			break;

		case TDF::Search::Space::Subsegments:
			strSpace = L"...";
			break;

		case TDF::Search::Space::SubsegmentsAndIncludes:
			strSpace = L"....";
			break;
	}

	return strSpace;
}