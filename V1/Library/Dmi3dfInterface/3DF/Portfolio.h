#pragma once

#include "Segment.h"
#include "Control.h"
#include "Image.h"
#include "Texture.h"
#include "Shape.h"
#include <deque>

namespace H3DF
{
	class API_3DF PortfolioKey : public Key
	{
	public:
		PortfolioKey();
		PortfolioKey(HC_KEY nInKey);
		explicit PortfolioKey(Key const & cInThat);
		PortfolioKey(PortfolioKey const & cInThat);

		H3DF::Type ObjectType() const override { return H3DF::Type::PortfolioKey; };

		void Set(PortfolioKey const & cInThat);
		PortfolioKey & operator = (PortfolioKey const & cInThat);

		TextureDefinition DefineTexture(CStringA strName, ImageDefinition const & cInSource);
		TextureDefinition DefineTexture(CStringA strName, ImageDefinition const & cInSource, TextureOptionsKit const & cInOptions);
		
		ImageDefinition DefineImage(CStringA strInName, ImageKit const & cInSource);
		NamedStyleDefinition DefineNamedStyle(CStringA strInName, SegmentKey const & cInStyleSource);

		ShapeDefinition DefineShape(CStringA strInName, ShapeKit const & cInSource);
		PortfolioKey & UndefineShape(CStringA strInName);
	};

	using PortfolioKeyArray = std::vector<PortfolioKey>;

	class API_3DF PortfolioControl : public Control
	{
	public:
		explicit PortfolioControl(SegmentKey & cInSegmentKey);
		PortfolioControl(PortfolioControl const & cInThat);

		H3DF::Type ObjectType() const override { return H3DF::Type::PortfolioControl; };

		void Set(PortfolioControl const & cInThat);
		PortfolioControl & operator = (PortfolioControl const & cInThat);

		size_t GetCount() const;

		PortfolioControl & Push(PortfolioKey const & cInPortfolio);
		
		bool Pop();
		bool Pop(PortfolioKey & cOutPortfolio);

		PortfolioControl & Set(PortfolioKey const & cInPortfolio);
		PortfolioControl & Set(PortfolioKeyArray const & cInPortfolios);

		PortfolioControl & UnsetTop();
		PortfolioControl & UnsetEverything();

		bool ShowTop(PortfolioKey & cOutPortfolio) const;
		bool Show(PortfolioKeyArray & cOutPortfolios) const;

	private:
		PortfolioControl();
	};
}