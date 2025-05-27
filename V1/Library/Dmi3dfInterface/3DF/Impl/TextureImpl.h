#include "../3DF.h"
#include "../Image.h"
#include "../Material.h"
#include "../Kit.h"
#include "../Math.h"
#include "../Math.Matrix.h"

#include "DefinitionImpl.h"

namespace H3DF
{
	class TextureOptionsKitImpl : public Impl
	{
	public:
		TextureOptionsKitImpl() { m_eType = H3DF::Type::TextureOptionsKit; }

		void Copy(TextureOptionsKitImpl * pcInThat) {
			m_eParameterization = pcInThat->m_eParameterization;
		}

		void GetDefinitionString(CStringA & strOutDefinition) const;

		bool m_bDecal = false;
		bool m_bDecalFlag = false;

		bool m_bDownSampling = false;
		bool m_bDownSamplingFlag = false;

		bool m_bModulation = false;
		bool m_bModulationFlag = false;

		size_t m_nParameterOffset = 0;
		bool m_bParameterOffsetFlag = false;

		Material::Texture::Parameterization m_eParameterization;
		bool m_bParameterizationFlag = false;

		Material::Texture::Tiling m_eTiling;
		bool m_bTilingFlag = false;

		Material::Texture::Interpolation m_eInterpolation;
		bool m_bInterpolationFlag = false;

		Material::Texture::Decimation m_eDecimation;
		bool m_bDecimationFlag = false;

		MatrixKit m_cTransform;
		bool m_bTransformFlag = false;

		float m_fValueScaleMin = 0.0f;
		float m_fValueScaleMax = 1.0f;
		bool m_bValueScaleFlag = false;
	};


	class TextureDefinitionImpl : public DefinitionImpl
	{
	public:
		TextureDefinitionImpl() { m_eType = H3DF::Type::TextureDefinition; }

		void Set(TextureDefinitionImpl * pcInThat) {
			m_strName = pcInThat->m_strName;
		}

		CStringA m_strName;
	};
}

