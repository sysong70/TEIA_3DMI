#pragma once

#include "3DF.h"
#include "Geometry.h"

#include "Math.h"

#include "Material.h"

namespace H3DF
{
	class API_3DF ShellKit : public Kit
	{
	public:
		ShellKit();
		virtual ~ShellKit();

		static const H3DF::Type staticType = H3DF::Type::ShellKit;
		H3DF::Type ObjectType() const override { return staticType; };

		size_t GetPointCount() const;
		size_t GetFacelistCount() const;
		size_t GetTristripsCount() const;

		ShellKit & SetPoints(int nInPointCount, Point const * pcInPoints);
		ShellKit & SetPoints(int nInPointCount, PointArray const & acInPoints);
		ShellKit & SetPoints(PointArray const & acInPoints);

		ShellKit & SetNormals(int nInNormalCount, VectorArray const & acInVectors);
		ShellKit & SetNormals(VectorArray const & acInVectors);

		ShellKit & SetFacelist(IntArray const & acInFacelist);
		ShellKit & SetTristrips(IntArray const & acInTristrips);
		ShellKit & SetParameters(FloatArray const & aInParameters);
		ShellKit & SetColors(RGBAColorArray const & aInColors);

		bool ShowPoints(int & nOutPointCount, Point const *& pcOutPoints) const;
		bool ShowPoints(int & nOutPointCount, PointArray const *& acOutPoints) const;
		bool ShowPoints(PointArray const *& acOutPoints) const;

		bool ShowNormals(int & nOutNormalCount, VectorArray const *& acOutVectors) const;
		bool ShowNormals(VectorArray const *& acOutVectors) const;

		bool ShowFacelist(IntArray const *& acOutFacelist) const;
		bool ShowTristrips(IntArray const *& acOutTristrips) const;
		bool ShowParameters(FloatArray const *& aOutParameters) const;
		bool ShowColors(RGBAColorArray const *& aOutColors) const;

		ShellKit & SetMaterialMapping(MaterialMappingKit const & cInkit);

		//== Shell 생성용 함수 =======================================================================
		bool CreateShellWrapper();
		void Regenerate();
		void BeginAddFaces(int nFaceCount);
		void EndAddFaces();

		void SetRegionFaces(int nRegionIndex, int nFaceFirstIndex, int nFaceCount);
		void SetRegionMaterial(int nInRegionIndex, H3DF::MaterialKit & cInMaterial);

		void DeleteShellWrapperKey();

		void AddNextFaceWithDistinctNormals(Point const * pcInPoints, Vector const * pcInNormals, int const * pnFaceIndex, int const * pnNormalIndex, int nFaceCount);
 		void AddNextFaceWithDistinctNormalsAndTexture(Point const * pcInPoints, Vector const * pcInNormals, Point const * pcInTextureCoord, int const * pnFaceIndex, 
			int const * pnNormalIndex, int const * pnTextureIndex, int nFaceCount);
// 		void AddNextFaceWithDistinctNormalsAndColor(HPoint const * points, HPoint const * normals, int const * face1, int const * face2, HPoint * face3, int fnum);
	};

	class API_3DF ShellKey : public GeometryKey
	{
	public:
		ShellKey();
		ShellKey(HC_KEY nInKey);
		explicit ShellKey(Key const & cInKey);
		ShellKey(ShellKey const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::ShellKey;
		H3DF::Type ObjectType() const override { return staticType; };

		void Set(ShellKey const & cInThat);
		ShellKey & operator = (ShellKey const & cInThat);
	};
}