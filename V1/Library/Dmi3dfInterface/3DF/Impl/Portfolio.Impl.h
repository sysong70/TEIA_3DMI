#pragma once

#include "../Image.h"
#include "../Shape.h"

namespace H3DF
{
	class PortfolioKeyImpl : public Impl
	{
	public:
		static CStringA FormatString(Image::Format cInFormat);

		// ----- Shape Data 생성 -----
		static bool CreateShapeData(ShapeElementArray & arInShapeElements, std::vector<float> & vfOutData);
		static void CreateShapePointData(int nFormatCode, H3DF::ShapePoint & cInShapePoint, std::vector<float> & vfOutData);
	};
}