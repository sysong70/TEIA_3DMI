#include "../3DF.h"
#include "../Image.h"
#include "../Material.h"
#include "../Kit.h"
#include "../Math.h"

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

		Material::Texture::Parameterization m_eParameterization = Material::Texture::Parameterization::None;
	};


	class TextureDefinitionImpl : public DefinitionImpl
	{
	public:
		TextureDefinitionImpl() { m_eType = H3DF::Type::TextureDefinition; }

		void Copy(TextureDefinitionImpl * pcInThat) {
			m_strName = pcInThat->m_strName;
		}

		CStringA m_strName;
	};
}

