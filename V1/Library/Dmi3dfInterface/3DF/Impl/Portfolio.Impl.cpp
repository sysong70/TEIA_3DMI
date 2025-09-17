#include "StdAfx.h"

#include "Portfolio.Impl.h"

#include "../Portfolio.h"

#define SHAPE_FORMAT_XY_BOUNDING_FOUR_MARGIN			6
#define SHAPE_FORMAT_XY_BOUNDING_THREE_MARGIN			5
#define SHAPE_FORMAT_XY_BOUNDING_TWO_MARGIN				4
#define SHAPE_FORMAT_XY_BOUNDING_ONE_MARGIN				3
#define SHAPE_FORMAT_XY_BOUNDING_SINGLE_FORMAT3			2
#define SHAPE_FORMAT_XY_BOUNDING_SINGLE					1
#define SHAPE_FORMAT_XY_BOUNDING_SAME_1					0
#define SHAPE_FORMAT_XY_BOUNDING_RADIUS					-3
#define SHAPE_FORMAT_XY_BOUNDING_RADIUS_ONE_MARGIN		-4
#define SHAPE_FORMAT_XY_BOUNDING_RADIUS_TWO_MARGIN		-5
#define SHAPE_FORMAT_XY_BOUNDING_RADIUS_THREE_MARGIN	-6
#define SHAPE_FORMAT_XY_BOUNDING_RADIUS_FOUR_MARGIN		-7

CStringA H3DF::PortfolioKeyImpl::FormatString(Image::Format cInFormat)
{
	CStringA strFormatText;

	switch (cInFormat)
	{
		case H3DF::Image::Format::RGB:
			strFormatText = "rgb";
			break;
		case H3DF::Image::Format::RGBA:
			strFormatText = "rgba";
			break;
		case H3DF::Image::Format::ARGB:
			strFormatText = "argb";
			break;
		case H3DF::Image::Format::Mapped8:
			strFormatText = "mapped8";
			break;
		case H3DF::Image::Format::Grayscale:
			strFormatText = "grayscale";
			break;
		case H3DF::Image::Format::Bmp:
			strFormatText = "bmp";
			break;
		case H3DF::Image::Format::Jpeg:
			strFormatText = "jpeg";
			break;
		case H3DF::Image::Format::Png:
			strFormatText = "png";
			break;
		case H3DF::Image::Format::Targa:
			strFormatText = "targa";
			break;
		case H3DF::Image::Format::DXT1:
			strFormatText = "dxt1";
			break;
		case H3DF::Image::Format::DXT3:
			strFormatText = "dxt3";
			break;
		case H3DF::Image::Format::DXT5:
			strFormatText = "dxt5";
			break;
		default:
			DEBUG_STOP;
			break;
	}

	return strFormatText;
}

/*
6	x bounding of the coordinate, y bounding of the coordinate, and four margin values coordinate of a point is defined with six numbers: 1, 0, 1, 0, 1, 0 (A point is defined with two coordinates)	
5	x/y bounding + three margin values	1, 0, 1, 0, -1
4	x/y bounding + two margin values	1, 0, 1, 0
3	x/y bounding + one margin value	1, 0, 1
2	Single coordinate value (applies to both x and y bounding of the coordinate) format 3, except it has no margin value.	
1	Single coordinate value (applies to both x and y bounding of the coordinate). This format is provided for convenience. Each coordinate in the point is defined with a single value: 1, 1 1 includes the default margins.)	
0	Same as format 1.	
-3	x bounding of the coordinate, y bounding of the coordinate, and a radius value	0, 1, 0.5
-4	x bounding of the coordinate, y bounding of the coordinate, a radius value, and a margin value	0, 1, 0.5, -0.5
-5	x bounding of the coordinate, y bounding of the coordinate, a radius value, and two margin values	0, 1, 0.5, 0, -0.5
-6	x bounding of the coordinate, y bounding of the coordinate, a radius value, and three margin values	0, 1, 0.5, 0, 0, -0.5
-7	x bounding of the coordinate, y bounding of the coordinate, a radius value, and four margin values
*/

bool H3DF::PortfolioKeyImpl::CreateShapeData(ShapeElementArray & arInShapeElements, std::vector<float> & vfOutData)
{
	if (arInShapeElements.empty()) {
		return false;
	}


	for(auto & cShapeElement : arInShapeElements) {

		H3DF::Type eType = cShapeElement.Type();

		if (H3DF::Type::PolygonShapeElement == cShapeElement.Type()) {
			PolygonShapeElement cPolygonShape(cShapeElement);

			ShapePointArray arShapePoints;
			if (false == cPolygonShape.ShowPoints(arShapePoints)) {
				DEBUG_STOP;
			}

			if (arShapePoints.empty()) {
				return false; // No points to process
			}

			// Format Code Check, 각 ShapePoint의 Margin의 최대 갯수를 파악해서 Format Code를 설정한다.
			size_t nMaxMarginCount = 0;
			for (auto & cShapePoint : arShapePoints) {
				FloatArray afMargins;
				if (true == cShapePoint.x.ShowMargins(afMargins)) {
					if (nMaxMarginCount < afMargins.size()) {
						nMaxMarginCount = afMargins.size();
					}
				}
			}

			int nFormatCode = 0;
			if(0 == nMaxMarginCount) {
				nFormatCode = SHAPE_FORMAT_XY_BOUNDING_SINGLE_FORMAT3;
			}
			else if(1 == nMaxMarginCount) {
				nFormatCode = SHAPE_FORMAT_XY_BOUNDING_ONE_MARGIN;
			}
			else if(2 == nMaxMarginCount) {
				nFormatCode = SHAPE_FORMAT_XY_BOUNDING_TWO_MARGIN;
			}
			else if(3 == nMaxMarginCount) {
				nFormatCode = SHAPE_FORMAT_XY_BOUNDING_THREE_MARGIN;
			}
			else if(4 == nMaxMarginCount) {
				nFormatCode = SHAPE_FORMAT_XY_BOUNDING_FOUR_MARGIN;
			}

			// Format code 추가
			vfOutData.emplace_back((float)nFormatCode);

			// Count
			vfOutData.emplace_back((float) (arShapePoints.size()));

			for (auto & cShapePoint : arShapePoints) {
				CreateShapePointData(nFormatCode, cShapePoint, vfOutData);
			}

			// End mark
			vfOutData.emplace_back(0);
		}
	}

	return true;
}

void H3DF::PortfolioKeyImpl::CreateShapePointData(int nFormatCode, H3DF::ShapePoint & cInShapePoint, std::vector<float> & vfOutData)
{
	FloatArray afMargins;

	// X 방향 처리
	vfOutData.emplace_back(cInShapePoint.x.x);
	vfOutData.emplace_back(cInShapePoint.x.y);

	if (true == cInShapePoint.x.ShowMargins(afMargins)) {
		if (false == afMargins.empty()) {
			switch (nFormatCode)
			{
				case SHAPE_FORMAT_XY_BOUNDING_SINGLE_FORMAT3:
				case SHAPE_FORMAT_XY_BOUNDING_SINGLE: {
				} break;

				case SHAPE_FORMAT_XY_BOUNDING_ONE_MARGIN: {
					vfOutData.emplace_back(afMargins[0]);
				} break;

				case SHAPE_FORMAT_XY_BOUNDING_TWO_MARGIN: {
					vfOutData.emplace_back(afMargins[0]);
					vfOutData.emplace_back((2 <= afMargins.size()) ? afMargins[1] : 0);
				} break;

				case SHAPE_FORMAT_XY_BOUNDING_THREE_MARGIN: {
					vfOutData.emplace_back(afMargins[0]);
					vfOutData.emplace_back((2 <= afMargins.size()) ? afMargins[1] : 0);
					vfOutData.emplace_back((3 <= afMargins.size()) ? afMargins[2] : 0);
				} break;

				case SHAPE_FORMAT_XY_BOUNDING_FOUR_MARGIN: {
					vfOutData.emplace_back(afMargins[0]);
					vfOutData.emplace_back((2 <= afMargins.size()) ? afMargins[1] : 0);
					vfOutData.emplace_back((3 <= afMargins.size()) ? afMargins[2] : 0);
					vfOutData.emplace_back((4 <= afMargins.size()) ? afMargins[3] : 0);
				} break;
			}
		}
	}

	// Y 방향 처리
	vfOutData.emplace_back(cInShapePoint.y.x);
	vfOutData.emplace_back(cInShapePoint.y.y);

	if (true == cInShapePoint.y.ShowMargins(afMargins)) {
		if (false == afMargins.empty()) {
			switch (nFormatCode)
			{
				case SHAPE_FORMAT_XY_BOUNDING_SINGLE: {
				} break;

				case SHAPE_FORMAT_XY_BOUNDING_ONE_MARGIN: {
					vfOutData.emplace_back(afMargins[0]);
				} break;

				case SHAPE_FORMAT_XY_BOUNDING_TWO_MARGIN: {
					vfOutData.emplace_back(afMargins[0]);
					vfOutData.emplace_back((2 <= afMargins.size()) ? afMargins[1] : 0);
				} break;

				case SHAPE_FORMAT_XY_BOUNDING_THREE_MARGIN: {
					vfOutData.emplace_back(afMargins[0]);
					vfOutData.emplace_back((2 <= afMargins.size()) ? afMargins[1] : 0);
					vfOutData.emplace_back((3 <= afMargins.size()) ? afMargins[2] : 0);
				} break;

				case SHAPE_FORMAT_XY_BOUNDING_FOUR_MARGIN: {
					vfOutData.emplace_back(afMargins[0]);
					vfOutData.emplace_back((2 <= afMargins.size()) ? afMargins[1] : 0);
					vfOutData.emplace_back((3 <= afMargins.size()) ? afMargins[2] : 0);
					vfOutData.emplace_back((4 <= afMargins.size()) ? afMargins[3] : 0);
				} break;
			}
		}
	}
}