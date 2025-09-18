#include <StdAfx.h>

#include "AM.DatalConverter.h"

#include "../3DX.DataGuard.h"

#include <A3DSDKIncludes.h>
#include <A3DSDKMarkupDimension.h>

AM::DatalConverterKit::DatalConverterKit()
{

}

AM::DatalConverter::DatalConverter()
{

}

// 입력 받은 Data를 이용해서 Primitve 별로 처리를 해야함. 그러기 위해서는 A3DTopoBrepData를 분석해야함.
// A3DTopoBrepData는 BREP의 최상위 구조체로, 내부에 Face, Edge, Vertex 등의 정보를 포함하고 있음.
// 전개 함수를 작성하여 각 Primitve를 순회하면서 필요한 처리를 수행할 수 있음.
bool AM::DatalConverter::ExportBrep(gsl::not_null<const A3DTopoBrepData *> brepData) noexcept
{
    H3DX::TopoBrepDataGuard dataGuard(brepData);
    if (!dataGuard.IsValid()) {
        return false;
    }

    const auto & brepDataData = dataGuard.Data();

    for (A3DUns32 i = 0; i < brepDataData.m_uiConnexSize; ++i) {
        ExportTopoConnex(brepDataData.m_ppConnexes[i]);
    }

    return true;
}