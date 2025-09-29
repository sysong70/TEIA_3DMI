#include <StdAfx.h>

#include "AM.DatalConverter.h"

#include "../3DX.DataGuard.h"

#include "Am.Equipment.h"

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
bool AM::DatalConverter::ExportBrep(const A3DTopoBrepData * brepData, std::string_view fileTitle) noexcept
{
    H3DX::TopoBrepDataGuard dataGuard(brepData);
    if (!dataGuard.IsValid()) {
        return false;
    }

    const auto & brepDataData = dataGuard.Data();

    // 1. 모델링 단계 (변환)
	AM::Equipment equipment;
    equipment.setTitile(fileTitle);
    
    for (A3DUns32 i = 0; i < brepDataData.m_uiConnexSize; ++i) {
        ProcessTopoConnex(brepDataData.m_ppConnexes[i], equipment);
    }

	// 2. 직렬화 단계 (파일 저장)

	 // DatalConverterImpl::m_writer를 통해 파일 경로를 얻어 파일 스트림을 엽니다.
	 // 여기서는 임시 파일 경로를 사용하거나, DatalConverter가 파일 경로를 알고 있어야 합니다.

	std::filesystem::path outputPath = L"Z:\\Generated.mac"; // 실제 경로로 대체 필요

	std::ofstream ofs(outputPath);
	if (!ofs.is_open()) {
		// 파일 열기 실패 시 오류 처리
		return false;
	}

	// equipment 객체에 파일 쓰기(WriteDatal)를 위임하고, 스트림을 전달합니다.
	if (equipment.writeDatal(&ofs) != Result::Ok()) {
		ofs.close();
		return false; // 직렬화 실패
	}

	ofs.close();

    return true;
}