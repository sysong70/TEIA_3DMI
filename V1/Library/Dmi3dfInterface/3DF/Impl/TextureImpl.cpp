#include "StdAfx.h"

#include "TextureImpl.h"

using namespace H3DF;

void H3DF::TextureOptionsKitImpl::GetDefinitionString(CStringA & strOutDefinition) const
{
	CStringA strText;

	if (true == m_bDecalFlag) {
		if (true == m_bDecal) {
			strOutDefinition += "decal";
		}
		else {
			strOutDefinition += "no decal";
		}
	}

	if (true == m_bDownSamplingFlag) {
		strOutDefinition += strOutDefinition.IsEmpty() ? "" : ", ";

		if (true == m_bDownSampling) {
			strOutDefinition += "down-sampling = on";
		}
		else {
			strOutDefinition += "down-sampling = off";
		}
	}

	if (true == m_bModulationFlag) {
		strOutDefinition += strOutDefinition.IsEmpty() ? "" : ", ";

		if (true == m_bModulation) {
			strOutDefinition += "modulate";
		}
		else {
			strOutDefinition += "no modulate";
		}
	}

	if (true == m_bParameterOffsetFlag) {
		strText.Format("parameter offset = %d", m_nParameterOffset);

		strOutDefinition += strOutDefinition.IsEmpty() ? "" : ", ";
		strOutDefinition += strText;
	}

	if (true == m_bParameterizationFlag) {
		strText = "parameterization source = ";
		switch (m_eParameterization)
		{
			case Material::Texture::Parameterization::Cylinder:
				strText += "cylinder";
				break;

			case Material::Texture::Parameterization::PhysicalReflection:
				strText += "physical reflection";
				break;

			case Material::Texture::Parameterization::Object:
				strText += "object";
				break;

			case Material::Texture::Parameterization::NaturalUV:
				strText += "natural uv";
				break;

			case Material::Texture::Parameterization::ReflectionVector:
				strText += "reflection vector";
				break;

			case Material::Texture::Parameterization::SurfaceNormal:
				strText += "surface normal";
				break;

			case Material::Texture::Parameterization::Sphere:
				strText += "sphere";
				break;

			case Material::Texture::Parameterization::UV:
				strText += "uv";
				break;

			case Material::Texture::Parameterization::World:
				strText += "world";
				break;

			default:
				DEBUG_STOP;
				break;
		}

		strOutDefinition += strOutDefinition.IsEmpty() ? "" : ", ";
		strOutDefinition += strText;
	}

	if (true == m_bTilingFlag) {
		strText = "tiling = ";
		switch (m_eTiling)
		{
			case Material::Texture::Tiling::Clamp:
				strText += "clamp";
				break;

			case Material::Texture::Tiling::Repeat:
				strText += "repeat";
				break;

			case Material::Texture::Tiling::Reflect:
				strText += "reflect";
				break;

			case Material::Texture::Tiling::Trim:
				strText += "trim";
				break;

			default:
				DEBUG_STOP;
				break;
		}

		strOutDefinition += strOutDefinition.IsEmpty() ? "" : ", ";
		strOutDefinition += strText;
	}

	if (true == m_bInterpolationFlag) {
		strText = "interpolation filter = ";
		switch (m_eInterpolation)
		{
			case Material::Texture::Interpolation::None:
				strText += "off";
				break;

			case Material::Texture::Interpolation::Bilinear:
				strText += "bilinear";
				break;

			default:
				DEBUG_STOP;
				break;
		}

		strOutDefinition += strOutDefinition.IsEmpty() ? "" : ", ";
		strOutDefinition += strText;
	}

	if (true == m_bDecimationFlag) {
		strText = "decimation filter = ";
		switch (m_eDecimation)
		{
			case Material::Texture::Decimation::None:
				strText += "off";
				break;

			case Material::Texture::Decimation::Anisotropic:
				strText += "anisotropic";
				break;

			case Material::Texture::Decimation::Mipmap:
				strText += "mipmap";
				break;

			default:
				DEBUG_STOP;
				break;
		}
		strOutDefinition += strOutDefinition.IsEmpty() ? "" : ", ";
		strOutDefinition += strText;
	}
}