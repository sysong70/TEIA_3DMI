#include "StdAfx.h"

#include "AM.Equipment.Impl.h"

using namespace AM;

AM::EquipmentImpl::EquipmentImpl()
{
	SetType(AM::Type::Equipment);
}


std::unique_ptr<Impl> AM::EquipmentImpl::Clone() const
{
	// 1. 새로운 EquipmentImpl 객체를 생성합니다.
	auto impl = std::make_unique<EquipmentImpl>();

	// 2. 기본 멤버 복사
	impl->m_title = m_title;
	impl->m_builtIn = m_builtIn;

	// 3. m_templates 깊은 복사 (순회하며 각 unique_ptr의 소유권을 안전하게 복제)
	//    Template::Clone()이 TemplateImpl의 깊은 복사를 수행한다고 가정합니다.
	for (const auto & templPtr : m_templates) {
		if (!templPtr) continue;

		// Template 객체 자체가 복사 생성자(Deep Copy)를 가지고 있다고 가정합니다.
		// *templPtr는 AM::Template 객체이며, 복사본을 만들어 unique_ptr에 담습니다.
		impl->m_templates.emplace_back(std::make_unique<AM::Template>(*templPtr));
	}

	return impl;
}

Result AM::EquipmentImpl::writeDatal(std::ostream * os) noexcept
{
	if (nullptr == os) {
		return Result::Fail(Error::InvalidArgument, "ostream is null.");
	}

	// m_title은 Equipment::setTitle로 설정된 string_view 또는 string 멤버라고 가정
	const std::string equipmentPath = std::string("/") + std::string(m_title);

	// 1. INPUT BEGIN marker
	(*os) << "INPUT BEGIN" << "\n";

	// 2. NEW EQUIPMENT header 
	(*os) << "NEW EQUIPMENT " << equipmentPath << "\n";
	(*os) << "BUIL " << (m_builtIn ? "true" : "false") << "\n";
	(*os) << "DSCO unset" << "\n";
	(*os) << "PTSP unset" << "\n";
	(*os) << "INSC unset" << "\n";

	// 3. Template 순회 및 Bind 위임
	// m_templates는 std::vector<Template>라고 가정합니다.

	for (auto & templatePtr  : m_templates) {
		// 하위 Template 객체의 Bind(std::ostream * os)를 호출하여 직렬화 책임을 위임합니다.
		if (templatePtr->writeDatal(os) != Result::Ok()) {
			// Template 직렬화 실패 시, 장비 전체의 실패로 간주하고 바로 반환
			return Result::Fail(Error::WriteFailed, "EquipmentImpl::WriteDatal: Failed to write template data.");
		}
	}

	// 4. INPUT END and FINISH markers
	(*os) << "INPUT END  EQUIPMENT " << equipmentPath << "\n";
	(*os) << "INPUT FINISH" << "\n";

	if (!os->good()) {
		return Result::Fail(Error::WriteFailed, "EquipmentImpl::WriteDatal: Stream write failure.");
	}

	return Result::Ok();
}
