#include "StdAfx.h"

#include "AM.Template.Impl.h"

#include "../Am.Template.h"

using namespace AM;

//== TemplateKitImpl Class =========================================================================
AM::TemplateKitImpl::TemplateKitImpl()
{
	SetType(AM::Type::TemplateKit);
}

std::unique_ptr<Impl> AM::TemplateKitImpl::Clone() const
{
	CLONE_IMPL(TemplateKitImpl);
}

//== TemplateImpl Class ============================================================================
AM::TemplateImpl::TemplateImpl()
{
	SetType(AM::Type::Template);
}

std::unique_ptr<Impl> AM::TemplateImpl::Clone() const
{
	CLONE_IMPL(TemplateImpl);
}

Result AM::TemplateImpl::writeDatal(std::ostream * os) noexcept
{
	if (nullptr == os) {
		return Result::Fail(Error::InvalidArgument, "ostream is null.");
	}

	const auto & kit = m_kit;

	// 1. NEW TMPLATE 헤더 출력 (TemplateImpl::WriteDatal의 첫 번째 책임)
	(*os) << "\n";
	(*os) << "NEW TMPLATE" << "\n";
	(*os) << "DESC '" << kit.m_description << "'" << "\n";
	(*os) << "PURP " << kit.m_property << "\n";
	// USER WEIGHT (optional) 출력 로직은 생략되었습니다.

	// 2. 하위 Geometry 순회 및 WriteDatal 위임
	// m_geometries는 std::vector<std::unique_ptr<AM::Geometry>>라고 가정
	for (auto & geometry : m_kit.m_geometries) {
		if (nullptr == geometry) {
			continue;
		}

		(*os) << "\n";

		// 하위 Geometry 객체의 WriteDatal(std::ostream * os)를 호출하여 직렬화 책임을 위임합니다.
		// 이 호출은 최종 프리미티브(Cylinder/Box)의 WriteDatal이 됩니다.
		if (Result::Ok() != geometry->writeDatal(os)) {
			// 오류 발생 시, 닫는 블록(END) 없이 바로 실패를 전파합니다.
			return Result::Fail(Error::WriteFailed, "Failed to write geometry data.");
		}
	}

	// 3. END 블록 닫기 (TemplateImpl::WriteDatal의 마지막 책임)
	(*os) << "\n";
	(*os) << "END" << "\n";

	return Result::Ok();
}