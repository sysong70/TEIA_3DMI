#pragma once

#include "3DF.h"
#include "Kit.h"
#include "Definition.h"
#include "Material.h"
#include "Math.h"

namespace H3DF
{
    class API_3DF TextureOptionsKit : public Kit {
    public:
        TextureOptionsKit();
        TextureOptionsKit(TextureOptionsKit const & cInKit);

		static const H3DF::Type staticType = H3DF::Type::TextureOptionsKit;
		H3DF::Type ObjectType() const { return staticType; };

        TextureOptionsKit(TextureOptionsKit && cInThat) noexcept;
        TextureOptionsKit & operator = (TextureOptionsKit && cInThat) noexcept;

		TextureOptionsKit const & operator = (TextureOptionsKit const & cInKit);

		void Show(TextureOptionsKit & cOutKit) const;

		bool Empty() const;

        bool Equals(TextureOptionsKit const & cInKit) const;
		bool operator == (TextureOptionsKit const & cInKit) const;
		bool operator != (TextureOptionsKit const & cInKit) const;

        TextureOptionsKit & SetDecal(bool bInState);
        TextureOptionsKit & SetDownSampling(bool bInState);
        TextureOptionsKit & SetModulation(bool bInState);
        TextureOptionsKit & SetParameterOffset(size_t nInSffset);
        TextureOptionsKit & SetParameterizationSource(Material::Texture::Parameterization cInSource);
        TextureOptionsKit & SetTiling(Material::Texture::Tiling eInTiling);
        TextureOptionsKit & SetInterpolationFilter(Material::Texture::Interpolation eInFilter);
        TextureOptionsKit & SetDecimationFilter(Material::Texture::Decimation eInFilter);
        TextureOptionsKit & SetTransformMatrix(MatrixKit const & cInTransform);
        TextureOptionsKit & SetValueScale(float fInMin, float fInMax);

        bool ShowParameterizationSource(Material::Texture::Parameterization & cOutSource) const;
        bool ShowTransformMatrix(MatrixKit & cOutTransform) const;
    };

    class API_3DF TextureDefinition : public Definition {
    public:
        TextureDefinition();
        TextureDefinition(Definition const & cInThat);
        TextureDefinition(TextureDefinition const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::TextureDefinition;
		H3DF::Type ObjectType() const { return staticType; };

		TextureDefinition(TextureDefinition && cInThat) noexcept;
		TextureDefinition & operator = (TextureDefinition && cInThat) noexcept;

		TextureDefinition const & operator = (TextureDefinition const & cInKit);
    };
}