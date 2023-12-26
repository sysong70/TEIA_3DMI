#include "StdAfx.h"

#include "AttributeLock.h"

#include "./Impl/ControlImpl.h"

#include "Segment.h"
#include "./Impl/SegmentImpl.h"

#include <HUtility.h>
#include <HTools.h>

using namespace H3DF;

namespace H3DF
{
	class AttributeLockControlPrivate : public ControlImpl
	{
	public:
		AttributeLockControlPrivate() { m_eType = H3DF::Type::AttributeLockControl; }

		void Copy(AttributeLockControlPrivate * pcInThat) {
			ControlImpl::Copy(pcInThat);
		}

		CStringA GetTypeString(AttributeLock::Type eInType);
	};
}

CStringA H3DF::AttributeLockControlPrivate::GetTypeString(AttributeLock::Type eInType)
{
	CStringA strTypeString;

	switch (eInType)
	{
		case H3DF::AttributeLock::Type::Everything:
			strTypeString = "everything";
			break;

		case H3DF::AttributeLock::Type::Visibility:
			strTypeString = "visibility";
			break;

		case H3DF::AttributeLock::Type::VisibilityCuttingSections:
			strTypeString = "visibility = (cutting sections)";
			break;

		case H3DF::AttributeLock::Type::VisibilityCutEdges:
			strTypeString = "visibility = (cut edges)";
			break;

		case H3DF::AttributeLock::Type::VisibilityCutFaces:
			strTypeString = "visibility = (cut faces)";
			break;

		case H3DF::AttributeLock::Type::VisibilityWindows:
			strTypeString = "visibility = (windows)";
			break;

		case H3DF::AttributeLock::Type::VisibilityText:
			strTypeString = "visibility = (text)";
			break;

		case H3DF::AttributeLock::Type::VisibilityLines:
			strTypeString = "visibility = (lines)";
			break;

		case H3DF::AttributeLock::Type::VisibilityEdgeLights:
			strTypeString = "visibility = (edge lights)";
			break;

		case H3DF::AttributeLock::Type::VisibilityMarkerLights:
			strTypeString = "visibility = (marker lights)";
			break;

		case H3DF::AttributeLock::Type::VisibilityFaceLights:
			strTypeString = "visibility = (face lights)";
			break;

		case H3DF::AttributeLock::Type::VisibilityGenericEdges:
			strTypeString = "visibility = (generic edges)";
			break;

		case H3DF::AttributeLock::Type::VisibilityHardEdges:
			strTypeString = "visibility = (hard edges)";
			break;

		case H3DF::AttributeLock::Type::VisibilityAdjacentEdges:
			strTypeString = "visibility = (adjacent edges)";
			break;

		case H3DF::AttributeLock::Type::VisibilityInteriorSilhouetteEdges:
			strTypeString = "visibility = (interior silhouette edges)";
			break;

		case H3DF::AttributeLock::Type::VisibilityShadowEmitting:
			strTypeString = "visibility = (shadow emitting)";
			break;

		case H3DF::AttributeLock::Type::VisibilityShadowReceiving:
			strTypeString = "visibility = (shadow receiving)";
			break;

		case H3DF::AttributeLock::Type::VisibilityShadowCasting:
			strTypeString = "visibility = (shadow casting)";
			break;

		case H3DF::AttributeLock::Type::VisibilityMarkers:
			strTypeString = "visibility = (markers)";
			break;

		case H3DF::AttributeLock::Type::VisibilityVertices:
			strTypeString = "visibility = (vertices)";
			break;

		case H3DF::AttributeLock::Type::VisibilityFaces:
			strTypeString = "visibility = (faces)";
			break;

		case H3DF::AttributeLock::Type::VisibilityPerimeterEdges:
			strTypeString = "visibility = (perimeter edges)";
			break;

		case H3DF::AttributeLock::Type::VisibilityNonCulledEdges:
			strTypeString = "visibility = (non culled edges)";
			break;

		case H3DF::AttributeLock::Type::VisibilityMeshQuadEdges:
			strTypeString = "visibility = (mesh quad edges)";
			break;

		case H3DF::AttributeLock::Type::VisibilityCutGeometry:
			strTypeString = "visibility = (cut geometry)";
			break;

		case H3DF::AttributeLock::Type::VisibilityEdges:
			strTypeString = "visibility = (edges)";
			break;
		case H3DF::AttributeLock::Type::VisibilityGeometry:
			strTypeString = "visibility = (geometry)";
			break;

		case H3DF::AttributeLock::Type::VisibilityLights:
			strTypeString = "visibility = (lights)";
			break;

		case H3DF::AttributeLock::Type::VisibilityShadows:
			strTypeString = "visibility = (shadows)";
			break;

		case H3DF::AttributeLock::Type::Material:
			strTypeString = "material";
			break;

		case H3DF::AttributeLock::Type::MaterialGeometry:
			strTypeString = "color = (geometry)";
			break;

		case H3DF::AttributeLock::Type::MaterialCutGeometry:
			strTypeString = "color = (cut geometry)";
			break;

		case H3DF::AttributeLock::Type::MaterialAmbientLightUpColor:
			strTypeString = "color = (ambient up)";
			break;
				
		case H3DF::AttributeLock::Type::MaterialAmbientLightDownColor:
			strTypeString = "color = (ambient down)";
			break;

		case H3DF::AttributeLock::Type::MaterialAmbientLightColor:
			strTypeString = "color = (ambient light)";
			break;

		case H3DF::AttributeLock::Type::MaterialWindowColor:
			strTypeString = "color = (windows)";
			break;

		case H3DF::AttributeLock::Type::MaterialWindowContrastColor:
			strTypeString = "color = (window contrast)";
			break;

		case H3DF::AttributeLock::Type::MaterialLightColor:
			strTypeString = "color = (lights)";
			break;

		case H3DF::AttributeLock::Type::MaterialLineColor:
			strTypeString = "color = (lines)";
			break;

		case H3DF::AttributeLock::Type::MaterialMarkerColor:
			strTypeString = "color = (markers)";
			break;

		case H3DF::AttributeLock::Type::MaterialTextColor:
			strTypeString = "color = (text)";
			break;

		case H3DF::AttributeLock::Type::MaterialCutEdgeColor:
			strTypeString = "color = (cut edges)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertex:
			strTypeString = "color = (vertices)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexDiffuse:
			strTypeString = "color = (vertices)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexDiffuseColor:
			strTypeString = "color = (vertices = diffuse)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexDiffuseAlpha:
			strTypeString = "color = (vertices = diffuse alpha)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexDiffuseTexture:
			strTypeString = "color = (vertices = diffuse texture)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexSpecular:
			strTypeString = "color = (vertices = specular)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexMirror:
			strTypeString = "color = (vertices = mirror)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexTransmission:
			strTypeString = "color = (vertices = transmission)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexEmission:
			strTypeString = "color = (vertices = emission)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexEnvironment:
			strTypeString = "color = (vertices = environment)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexBump:
			strTypeString = "color = (vertices = bump)";
			break;

		case H3DF::AttributeLock::Type::MaterialVertexGloss:
			strTypeString = "color = (vertices = gloss)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdge:
			strTypeString = "color = (edges)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeDiffuse:
			strTypeString = "color = (edges = diffuse)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeDiffuseColor:
			strTypeString = "color = (edges = diffuse color)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeDiffuseAlpha:
			strTypeString = "color = (edges = diffuse alpha)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeDiffuseTexture:
			strTypeString = "color = (edges = diffuse texture)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeSpecular:
			strTypeString = "color = (edges = specular)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeMirror:
			strTypeString = "color = (edges = mirror)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeTransmission:
			strTypeString = "color = (edges = transmission)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeEmission:
			strTypeString = "color = (edges = emission)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeEnvironment:
			strTypeString = "color = (edges = environment)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeBump:
			strTypeString = "color = (edges = bump)";
			break;

		case H3DF::AttributeLock::Type::MaterialEdgeGloss:
			strTypeString = "color = (edges = gloss)";
			break;

		case H3DF::AttributeLock::Type::MaterialFace:
			strTypeString = "color = (faces)";
			break;

		case H3DF::AttributeLock::Type::MaterialFaceDiffuse:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceDiffuseColor:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceDiffuseAlpha:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceDiffuseTexture:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceSpecular:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceMirror:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceTransmission:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceEmission:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceEnvironment:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceBump:
			break;
		case H3DF::AttributeLock::Type::MaterialFaceGloss:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFace:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceDiffuse:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceDiffuseColor:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceDiffuseAlpha:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceDiffuseTexture:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceSpecular:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceMirror:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceTransmission:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceEmission:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceEnvironment:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceBump:
			break;
		case H3DF::AttributeLock::Type::MaterialBackFaceGloss:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFace:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceDiffuse:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceDiffuseColor:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceDiffuseAlpha:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceDiffuseTexture:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceSpecular:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceMirror:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceTransmission:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceEmission:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceEnvironment:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceBump:
			break;
		case H3DF::AttributeLock::Type::MaterialFrontFaceGloss:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFace:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceDiffuse:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceDiffuseColor:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceDiffuseAlpha:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceDiffuseTexture:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceSpecular:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceMirror:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceTransmission:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceEmission:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceEnvironment:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceBump:
			break;
		case H3DF::AttributeLock::Type::MaterialCutFaceGloss:
			break;
		case H3DF::AttributeLock::Type::Camera:
			break;

		default:
			ASSERT(false);
			break;
	}

	return strTypeString;
}

H3DF::AttributeLockControl::AttributeLockControl(SegmentKey & cInSegmentKey)
{
	AttributeLockControlPrivate * pcImpl = new AttributeLockControlPrivate();
	pcImpl->m_cOverrideKey = cInSegmentKey;

	m_pcImpl = pcImpl;
}

H3DF::AttributeLockControl::AttributeLockControl(AttributeLockControl const & cInThat)
{
	m_pcImpl = new AttributeLockControlPrivate();
	Set(cInThat);
}

void H3DF::AttributeLockControl::Set(AttributeLockControl const & cInThat)
{
	AttributeLockControlPrivate * pcImpl = (AttributeLockControlPrivate *) m_pcImpl;
	AttributeLockControlPrivate * pcInThatImpl = (AttributeLockControlPrivate *) cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

AttributeLockControl & H3DF::AttributeLockControl::operator = (AttributeLockControl const & cInThat)
{
	Set(cInThat);
	return *this;
}

AttributeLockControl & H3DF::AttributeLockControl::SetLock(AttributeLock::Type eInType, bool bInState)
{
	AttributeLockControlPrivate * pcImpl = (AttributeLockControlPrivate *) m_pcImpl;

	CStringA strOption;
	if (true == bInState) {
		strOption = "attribute lock = ";
	}
	else {
		strOption = "no attribute lock = ";
	}

	strOption += pcImpl->GetTypeString(eInType);

	SegmentKeyImpl::LocalOpen(pcImpl->m_cOverrideKey); {
		HC_Set_Rendering_Options(strOption);
	} SegmentKeyImpl::LocalClose(pcImpl->m_cOverrideKey);

	return *this;
}

