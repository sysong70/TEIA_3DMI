#pragma once

#include "../Image.h"
#include "../Shape.h"

#include "KeyImpl.h"

namespace H3DF
{
	class PortfolioKeyImpl : public KeyImpl
	{
	public:
		std::unique_ptr<Impl> Clone() const override {
			auto pcClone = std::make_unique<PortfolioKeyImpl>();
			pcClone->Copy(this);
			return pcClone;
		}

		void Copy(const PortfolioKeyImpl * pcInThat)
		{
			if (pcInThat == nullptr) {
				DEBUG_STOP;
				return;
			}

			KeyImpl::Copy(pcInThat);
		}

		static CStringA FormatString(Image::Format cInFormat);

		// ----- Shape Data 생성 -----
		static bool CreateShapeData(ShapeElementArray & arInShapeElements, std::vector<float> & vfOutData);
		static void CreateShapePointData(int nFormatCode, H3DF::ShapePoint & cInShapePoint, std::vector<float> & vfOutData);
	};
}