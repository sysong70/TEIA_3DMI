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

		static const H3DF::Type staticType = H3DF::Type::PortfolioKey;
		H3DF::Type ObjectType() const { return staticType; };

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
		explicit PortfolioControl(SegmentKey & cInSegment);
		PortfolioControl(PortfolioControl const & cInThat);

		virtual ~PortfolioControl();

		static const H3DF::Type staticType = H3DF::Type::PortfolioControl;
		H3DF::Type ObjectType() const { return staticType; };

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