#include <StdAfx.h>

#include "3DX.Simplifier.h"


//== SimplifierKit Class ==========================================================================
H3DX::SimplifierKit::SimplifierKit()
{

}

//== Simplifier Class =============================================================================
H3DX::Simplifier::Simplifier() 
{
}

bool H3DX::Simplifier::Initialize(const A3DTopoBrepData * pcInBrepData)
{
	// 입력된 원본 데이터 저장.
	m_pcOriginalBrepData = pcInBrepData;

	return true; // Return true if initialization is successful
}

bool H3DX::Simplifier::Simplify()
{
	CopyBrepData(m_pcOriginalBrepData, m_pcSimplifiedBrepData);

	return true;
}

const A3DTopoBrepData * H3DX::Simplifier::GetSimplifiedData() const
{
	// Return the simplified data
	return m_pcSimplifiedBrepData;
}
