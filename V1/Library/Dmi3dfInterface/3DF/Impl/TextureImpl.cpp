#include "StdAfx.h"

#include "TextureImpl.h"

using namespace H3DF;

void H3DF::TextureOptionsKitImpl::GetDefinitionString(CStringA & strOutDefinition) const
{
	CStringA strText;

	if (Material::Texture::Parameterization::None != m_eParameterization) {
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

		strOutDefinition += strText;
	}

}