#pragma once

#include <A3DSDKIncludes.h>

namespace H3DX
{
	class SimplifierKitImpl
	{
	public:
		SimplifierKitImpl();
// 		void SetMesh(const Mesh& mesh);
// 		void SetMesh(const Mesh& mesh, const std::vector<UINT>& indices);
// 		void SetMesh(const Mesh& mesh, const std::vector<UINT>& indices, const std::vector<UINT>& vertexIndices);
// 		void Simplify(float ratio);
// 		void Simplify(float ratio, const std::vector<UINT>& indices);
// 		void Simplify(float ratio, const std::vector<UINT>& indices, const std::vector<UINT>& vertexIndices);
// 		const Mesh & GetMesh() const;
	};

	class SimplifierImpl
	{
	public:
		SimplifierImpl();
	
	protected:
		bool CopyBrepData(const A3DTopoBrepData * pcInBrepData, A3DTopoBrepData *& pcOutBrepData);
		bool CopyBrepDataData(const A3DTopoBrepDataData & cInBrepDataData, A3DTopoBrepDataData & cOutBrepDataData);
		bool CopyConnex(const A3DTopoConnex * pcInConnex, A3DTopoConnex *& pcOutCopyConnex);
		bool CopyShell(const A3DTopoShell * pcInShell, A3DTopoShell *& pcOutCopyShell);
		bool CopyFace(const A3DTopoFace * pcInFace, A3DTopoFace *& pcOutCopyFace);
		bool CopyFaceWithOuterLoopOnly(const A3DTopoFace * pcInFace, A3DTopoFace *& pcOutCopyFace);
		bool CopyLoop(const A3DTopoLoop * pcInLoop, A3DTopoLoop *& pcOutCopyLoop);
		bool CopyCoEdge(const A3DTopoCoEdge * pcInCoEdge, A3DTopoCoEdge *& pcOutCopyCoEdge);
		bool CopyEdge(const A3DTopoEdge * pcInEdge, A3DTopoEdge *& pcOutCopyEdge);
		bool CopyVertex(const A3DTopoVertex * pcInVertex, A3DTopoVertex *& pcOutCopyVertex);
		bool CopyIntervalData(const A3DIntervalData & cInIntervalData, A3DIntervalData & cOutCopyIntervalData);

		// ----- 정보 수집 함수 -----
		bool GetBrepDataData(const A3DTopoBrepData * pcInBrepData, A3DTopoBrepDataData & cOutBrepDataData);

		const A3DTopoBrepData * m_pcOriginalBrepData =nullptr;
		A3DTopoBrepData * m_pcSimplifiedBrepData = nullptr;
	};
}