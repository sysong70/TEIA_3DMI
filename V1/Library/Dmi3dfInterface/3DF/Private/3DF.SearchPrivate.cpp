#include "StdAfx.h"

#include "3DF.SearchPrivate.h"

USING_3DF_NAMESPACE

CString SearchPrivate::GetSearchTypeString(Search::Type eType)
{
	CString strType;

	switch (eType)
	{
		case H3DF::Search::Type::Everything:
			strType = _T("everything");
			break;

		case H3DF::Search::Type::Include:
			strType = _T("include");
			break;

		case H3DF::Search::Type::Segment:
			strType = _T("segment");
			break;

		case H3DF::Search::Type::Includer:
			break;

		case H3DF::Search::Type::Geometry:
			strType = _T("geometry");
			break;

		case H3DF::Search::Type::Shell:
			strType = _T("shell");	
			break;

		case H3DF::Search::Type::Line:
			strType = _T("lines, polylines");
			break;

/*
		case H3DF::Search::Type::CuttingSection:
			break;

		case H3DF::Search::Type::Shell:
			break;
		case H3DF::Search::Type::Mesh:
			break;
		case H3DF::Search::Type::Grid:
			break;
		case H3DF::Search::Type::NURBSSurface:
			break;
		case H3DF::Search::Type::Cylinder:
			break;
		case H3DF::Search::Type::Sphere:
			break;
		case H3DF::Search::Type::Polygon:
			break;
		case H3DF::Search::Type::Circle:
			break;
		case H3DF::Search::Type::CircularWedge:
			break;
		case H3DF::Search::Type::Ellipse:
			break;
		case H3DF::Search::Type::NURBSCurve:
			break;
		case H3DF::Search::Type::CircularArc:
			break;
		case H3DF::Search::Type::EllipticalArc:
			break;
		case H3DF::Search::Type::InfiniteLine:
			break;
		case H3DF::Search::Type::Marker:
			break;
		case H3DF::Search::Type::Text:
			break;
		case H3DF::Search::Type::Reference:
			break;
		case H3DF::Search::Type::DistantLight:
			break;
		case H3DF::Search::Type::Spotlight:
			break;
		case H3DF::Search::Type::InfiniteRay:
			break;
		case H3DF::Search::Type::Attribute:
			break;
		case H3DF::Search::Type::Priority:
			break;
		case H3DF::Search::Type::Material:
			break;
		case H3DF::Search::Type::PBRMaterial:
			break;
		case H3DF::Search::Type::Camera:
			break;
		case H3DF::Search::Type::ModellingMatrix:
			break;
		case H3DF::Search::Type::UserData:
			break;
		case H3DF::Search::Type::TextureMatrix:
			break;
		case H3DF::Search::Type::Culling:
			break;
		case H3DF::Search::Type::CullingFace:
			break;
		case H3DF::Search::Type::CullingExtent:
			break;
		case H3DF::Search::Type::CullingVector:
			break;
		case H3DF::Search::Type::CullingVectorTolerance:
			break;
		case H3DF::Search::Type::CullingFrustum:
			break;
		case H3DF::Search::Type::CullingDeferralExtent:
			break;
		case H3DF::Search::Type::CullingVolume:
			break;
		case H3DF::Search::Type::CullingDistance:
			break;
		case H3DF::Search::Type::CurveAttribute:
			break;
		case H3DF::Search::Type::CurveAttributeBudget:
			break;
		case H3DF::Search::Type::CurveAttributeContinuedBudget:
			break;
		case H3DF::Search::Type::CurveAttributeViewDependent:
			break;
		case H3DF::Search::Type::CurveAttributeMaximumDeviation:
			break;
		case H3DF::Search::Type::CurveAttributeMaximumAngle:
			break;
		case H3DF::Search::Type::CurveAttributeMaximumLength:
			break;
		case H3DF::Search::Type::CylinderAttribute:
			break;
		case H3DF::Search::Type::CylinderAttributeTessellation:
			break;
		case H3DF::Search::Type::CylinderAttributeOrientation:
			break;
		case H3DF::Search::Type::EdgeAttribute:
			break;
		case H3DF::Search::Type::EdgeAttributePattern:
			break;
		case H3DF::Search::Type::EdgeAttributeWeight:
			break;
		case H3DF::Search::Type::EdgeAttributeHardAngle:
			break;
		case H3DF::Search::Type::LightingAttribute:
			break;
		case H3DF::Search::Type::LightingAttributeInterpolation:
			break;
		case H3DF::Search::Type::LineAttribute:
			break;
		case H3DF::Search::Type::LineAttributePattern:
			break;
		case H3DF::Search::Type::LineAttributeWeight:
			break;
		case H3DF::Search::Type::MarkerAttribute:
			break;
		case H3DF::Search::Type::MarkerAttributeGlyphRotation:
			break;
		case H3DF::Search::Type::MarkerAttributeSize:
			break;
		case H3DF::Search::Type::MarkerAttributeSymbol:
			break;
		case H3DF::Search::Type::MarkerAttributePreference:
			break;
		case H3DF::Search::Type::SurfaceAttribute:
			break;
		case H3DF::Search::Type::SurfaceAttributeBudget:
			break;
		case H3DF::Search::Type::SurfaceAttributeMaximumFacetDeviation:
			break;
		case H3DF::Search::Type::SurfaceAttributeMaximumFacetAngle:
			break;
		case H3DF::Search::Type::SurfaceAttributeMaximumFacetWidth:
			break;
		case H3DF::Search::Type::SurfaceAttributeTrimCurveBudget:
			break;
		case H3DF::Search::Type::SurfaceAttributeMaximumTrimCurveDeviation:
			break;
		case H3DF::Search::Type::Selectability:
			break;
		case H3DF::Search::Type::SelectabilityWindows:
			break;
		case H3DF::Search::Type::SelectabilityEdges:
			break;
		case H3DF::Search::Type::SelectabilityFaces:
			break;
		case H3DF::Search::Type::SelectabilityLights:
			break;
		case H3DF::Search::Type::SelectabilityLines:
			break;
		case H3DF::Search::Type::SelectabilityMarkers:
			break;
		case H3DF::Search::Type::SelectabilityVertices:
			break;
		case H3DF::Search::Type::SelectabilityText:
			break;
		case H3DF::Search::Type::SphereAttribute:
			break;
		case H3DF::Search::Type::SphereAttributeTessellation:
			break;
		case H3DF::Search::Type::Subwindow:
			break;
		case H3DF::Search::Type::SubwindowEitherType:
			break;
		case H3DF::Search::Type::SubwindowStandard:
			break;
		case H3DF::Search::Type::SubwindowLightweight:
			break;
		case H3DF::Search::Type::SubwindowBackground:
			break;
		case H3DF::Search::Type::SubwindowBorder:
			break;
		case H3DF::Search::Type::SubwindowRenderingAlgorithm:
			break;
		case H3DF::Search::Type::TextAttribute:
			break;
		case H3DF::Search::Type::TextAttributeAlignment:
			break;
		case H3DF::Search::Type::TextAttributeBold:
			break;
		case H3DF::Search::Type::TextAttributeItalic:
			break;
		case H3DF::Search::Type::TextAttributeOverline:
			break;
		case H3DF::Search::Type::TextAttributeStrikethrough:
			break;
		case H3DF::Search::Type::TextAttributeUnderline:
			break;
		case H3DF::Search::Type::TextAttributeSlant:
			break;
		case H3DF::Search::Type::TextAttributeLineSpacing:
			break;
		case H3DF::Search::Type::TextAttributeRotation:
			break;
		case H3DF::Search::Type::TextAttributeExtraSpace:
			break;
		case H3DF::Search::Type::TextAttributeGreeking:
			break;
		case H3DF::Search::Type::TextAttributeSizeTolerance:
			break;
		case H3DF::Search::Type::TextAttributeSize:
			break;
		case H3DF::Search::Type::TextAttributeFont:
			break;
		case H3DF::Search::Type::TextAttributeTransform:
			break;
		case H3DF::Search::Type::TextAttributeRenderer:
			break;
		case H3DF::Search::Type::TextAttributePreference:
			break;
		case H3DF::Search::Type::TextAttributePath:
			break;
		case H3DF::Search::Type::TextAttributeSpacing:
			break;
		case H3DF::Search::Type::TextAttributeBackground:
			break;
		case H3DF::Search::Type::TextAttributeBackgroundMargins:
			break;
		case H3DF::Search::Type::TextAttributeBackgroundStyle:
			break;
		case H3DF::Search::Type::TextAttributeLeaderLine:
			break;
		case H3DF::Search::Type::Transparency:
			break;
		case H3DF::Search::Type::TransparencyMethod:
			break;
		case H3DF::Search::Type::TransparencyAlgorithm:
			break;
		case H3DF::Search::Type::TransparencyDepthPeelingLayers:
			break;
		case H3DF::Search::Type::TransparencyDepthPeelingMinimumArea:
			break;
		case H3DF::Search::Type::TransparencyDepthWriting:
			break;
		case H3DF::Search::Type::Visibility:
			break;
		case H3DF::Search::Type::VisibilityCuttingSections:
			break;
		case H3DF::Search::Type::VisibilityCutEdges:
			break;
		case H3DF::Search::Type::VisibilityCutFaces:
			break;
		case H3DF::Search::Type::VisibilityWindows:
			break;
		case H3DF::Search::Type::VisibilityText:
			break;
		case H3DF::Search::Type::VisibilityLines:
			break;
		case H3DF::Search::Type::VisibilityEdgeLights:
			break;
		case H3DF::Search::Type::VisibilityMarkerLights:
			break;
		case H3DF::Search::Type::VisibilityFaceLights:
			break;
		case H3DF::Search::Type::VisibilityGenericEdges:
			break;
		case H3DF::Search::Type::VisibilityHardEdges:
			break;
		case H3DF::Search::Type::VisibilityAdjacentEdges:
			break;
		case H3DF::Search::Type::VisibilityInteriorSilhouetteEdges:
			break;
		case H3DF::Search::Type::VisibilityShadowEmitting:
			break;
		case H3DF::Search::Type::VisibilityShadowReceiving:
			break;
		case H3DF::Search::Type::VisibilityShadowCasting:
			break;
		case H3DF::Search::Type::VisibilityMarkers:
			break;
		case H3DF::Search::Type::VisibilityVertices:
			break;
		case H3DF::Search::Type::VisibilityFaces:
			break;
		case H3DF::Search::Type::VisibilityPerimeterEdges:
			break;
		case H3DF::Search::Type::VisibilityNonCulledEdges:
			break;
		case H3DF::Search::Type::VisibilityMeshQuadEdges:
			break;
		case H3DF::Search::Type::VisibilityEdges:
			break;
		case H3DF::Search::Type::VisibilityLeaderLines:
			break;
		case H3DF::Search::Type::VisualEffects:
			break;
		case H3DF::Search::Type::VisualEffectsPostProcessEffectsEnabled:
			break;
		case H3DF::Search::Type::VisualEffectsAntiAliasing:
			break;
		case H3DF::Search::Type::VisualEffectsShadowMaps:
			break;
		case H3DF::Search::Type::VisualEffectsSimpleShadow:
			break;
		case H3DF::Search::Type::VisualEffectsSimpleShadowPlane:
			break;
		case H3DF::Search::Type::VisualEffectsSimpleShadowLightDirection:
			break;
		case H3DF::Search::Type::VisualEffectsSimpleShadowColor:
			break;
		case H3DF::Search::Type::VisualEffectsSimpleReflection:
			break;
		case H3DF::Search::Type::VisualEffectsSimpleReflectionPlane:
			break;
		case H3DF::Search::Type::VisualEffectsSimpleReflectionVisibility:
			break;
		case H3DF::Search::Type::VisualEffectsEyeDomeLightingBackColor:
			break;
		case H3DF::Search::Type::Performance:
			break;
		case H3DF::Search::Type::PerformanceDisplayLists:
			break;
		case H3DF::Search::Type::PerformanceStaticModel:
			break;
		case H3DF::Search::Type::PerformanceTextHardwareAcceleration:
			break;
		case H3DF::Search::Type::StaticModelSegment:
			break;
		case H3DF::Search::Type::DrawingAttribute:
			break;
		case H3DF::Search::Type::DrawingAttributePolygonHandedness:
			break;
		case H3DF::Search::Type::DrawingAttributeDepthRange:
			break;
		case H3DF::Search::Type::DrawingAttributeFaceDisplacement:
			break;
		case H3DF::Search::Type::DrawingAttributeGeneralDisplacement:
			break;
		case H3DF::Search::Type::DrawingAttributeVertexDisplacement:
			break;
		case H3DF::Search::Type::DrawingAttributeOverlay:
			break;
		case H3DF::Search::Type::DrawingAttributeDeferral:
			break;
		case H3DF::Search::Type::DrawingAttributeVertexDecimation:
			break;
		case H3DF::Search::Type::DrawingAttributeVertexRandomization:
			break;
		case H3DF::Search::Type::DrawingAttributeOverrideInternalColor:
			break;
		case H3DF::Search::Type::DrawingAttributeClipRegion:
			break;
		case H3DF::Search::Type::DrawingAttributeWorldHandedness:
			break;
		case H3DF::Search::Type::HiddenLineAttribute:
			break;
		case H3DF::Search::Type::HiddenLineAttributeAlgorithm:
			break;
		case H3DF::Search::Type::HiddenLineAttributeColor:
			break;
		case H3DF::Search::Type::HiddenLineAttributeDimFactor:
			break;
		case H3DF::Search::Type::HiddenLineAttributeFaceDisplacement:
			break;
		case H3DF::Search::Type::HiddenLineAttributeLinePattern:
			break;
		case H3DF::Search::Type::HiddenLineAttributeLineSort:
			break;
		case H3DF::Search::Type::HiddenLineAttributeRenderFaces:
			break;
		case H3DF::Search::Type::HiddenLineAttributeRenderText:
			break;
		case H3DF::Search::Type::HiddenLineAttributeSilhouetteCleanup:
			break;
		case H3DF::Search::Type::HiddenLineAttributeTransparencyCutoff:
			break;
		case H3DF::Search::Type::HiddenLineAttributeVisibility:
			break;
		case H3DF::Search::Type::HiddenLineAttributeWeight:
			break;
		case H3DF::Search::Type::SegmentStyle:
			break;
		case H3DF::Search::Type::NamedStyle:
			break;
		case H3DF::Search::Type::MaterialPalette:
			break;
		case H3DF::Search::Type::Portfolio:
			break;
		case H3DF::Search::Type::ContourLine:
			break;
		case H3DF::Search::Type::ContourLineVisibility:
			break;
		case H3DF::Search::Type::ContourLinePosition:
			break;
		case H3DF::Search::Type::ContourLineColor:
			break;
		case H3DF::Search::Type::ContourLinePattern:
			break;
		case H3DF::Search::Type::ContourLineWeight:
			break;
		case H3DF::Search::Type::ContourLineLighting:
			break;
		case H3DF::Search::Type::Condition:
			break;
		case H3DF::Search::Type::Bounding:
			break;
		case H3DF::Search::Type::BoundingVolume:
			break;
		case H3DF::Search::Type::BoundingExclusion:
			break;
		case H3DF::Search::Type::AttributeLock:
			break;
		case H3DF::Search::Type::AttributeLockSetting:
			break;
		case H3DF::Search::Type::AttributeLockSubsegmentOverride:
			break;
		case H3DF::Search::Type::TransformMask:
			break;
		case H3DF::Search::Type::TransformMaskCamera:
			break;
		case H3DF::Search::Type::TransformMaskCameraTranslation:
			break;
		case H3DF::Search::Type::TransformMaskCameraScale:
			break;
		case H3DF::Search::Type::TransformMaskCameraOffset:
			break;
		case H3DF::Search::Type::TransformMaskCameraRotation:
			break;
		case H3DF::Search::Type::TransformMaskCameraPerspectiveScale:
			break;
		case H3DF::Search::Type::TransformMaskCameraProjection:
			break;
		case H3DF::Search::Type::TransformMaskModellingMatrix:
			break;
		case H3DF::Search::Type::TransformMaskModellingMatrixTranslation:
			break;
		case H3DF::Search::Type::TransformMaskModellingMatrixScale:
			break;
		case H3DF::Search::Type::TransformMaskModellingMatrixOffset:
			break;
		case H3DF::Search::Type::TransformMaskModellingMatrixRotation:
			break;
		case H3DF::Search::Type::ColorInterpolation:
			break;
		case H3DF::Search::Type::ColorInterpolationFaceColor:
			break;
		case H3DF::Search::Type::ColorInterpolationEdgeColor:
			break;
		case H3DF::Search::Type::ColorInterpolationMarkerColor:
			break;
		case H3DF::Search::Type::ColorInterpolationFaceIndex:
			break;
		case H3DF::Search::Type::ColorInterpolationEdgeIndex:
			break;
		case H3DF::Search::Type::ColorInterpolationMarkerIndex:
			break;
		case H3DF::Search::Type::CuttingSectionAttribute:
			break;
		case H3DF::Search::Type::CuttingSectionAttributeCuttingLevel:
			break;
		case H3DF::Search::Type::CuttingSectionAttributeCappingLevel:
			break;
		case H3DF::Search::Type::CuttingSectionAttributeMaterialPreference:
			break;
		case H3DF::Search::Type::CuttingSectionAttributeEdgeWeight:
			break;
		case H3DF::Search::Type::CuttingSectionAttributeTolerance:
			break;
		case H3DF::Search::Type::CuttingSectionAttributeCappingUsage:
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
		case H3DF::Search::Space::SegmentOnly:
			strSpace = L".";
			break;

		case H3DF::Search::Space::Subsegments:
			strSpace = L"...";
			break;

		case H3DF::Search::Space::SubsegmentsAndIncludes:
			strSpace = L"....";
			break;
	}

	return strSpace;
}