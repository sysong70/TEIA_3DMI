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

        H3DF::Type ObjectType() const override { return H3DF::Type::TextureOptionsKit; };

        TextureOptionsKit(TextureOptionsKit && cInThat) noexcept;
        TextureOptionsKit & operator = (TextureOptionsKit && cInThat) noexcept;

		void Set(TextureOptionsKit const & cInKit);
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

		H3DF::Type ObjectType() const override { return H3DF::Type::TextureDefinition; };

		TextureDefinition(TextureDefinition && cInThat) noexcept;
		TextureDefinition & operator = (TextureDefinition && cInThat) noexcept;

		void Set(TextureDefinition const & cInKit);
		TextureDefinition const & operator = (TextureDefinition const & cInKit);
    };
}