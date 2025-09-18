#pragma once

#include "Impl/3DX.Simplifier.Impl.h"

namespace H3DX
{
	class SimplifierKit
	{
	public:
		SimplifierKit();
// 		void SetMesh(const Mesh& mesh);
// 		void SetMesh(const Mesh& mesh, const std::vector<UINT>& indices);
// 		void SetMesh(const Mesh& mesh, const std::vector<UINT>& indices, const std::vector<UINT>& vertexIndices);
// 		void Simplify(float ratio);
// 		void Simplify(float ratio, const std::vector<UINT>& indices);
// 		void Simplify(float ratio, const std::vector<UINT>& indices, const std::vector<UINT>& vertexIndices);
// 		const Mesh & GetMesh() const;
	};

	class Simplifier : public SimplifierImpl
	{
	public:
		Simplifier();

		// Load BREP data from A3DTopoBrepData
		bool Initialize(const A3DTopoBrepData * pcInBrepData);

		// Run simplification
		bool Simplify();

		// Export simplified result
		const A3DTopoBrepData * GetSimplifiedData() const;
	};
}