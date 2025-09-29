#include "StdAfx.h"

#include "Am.Equipment.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Equipment.Impl.h"

#include "Am.Template.h"

using namespace AM;

AM::Equipment::Equipment()
{
}

AM::Equipment::~Equipment()
{

}

Equipment & AM::Equipment::setTitile(std::string_view title)
{
	auto * impl = ENSURE_IMPL(Equipment);
	if (nullptr != impl) {
		impl->m_title = title;
	}

	return *this;
}

Equipment & AM::Equipment::setBuiltIn(bool builtIn)
{
	auto * impl = ENSURE_IMPL(Equipment);
	if (nullptr != impl) {
		impl->m_builtIn = builtIn;
	}

	return *this;
}

Result AM::Equipment::writeDatal(std::ostream * os) noexcept
{
	// ENSURE_IMPL을 사용하여 Impl 객체가 유효함을 보장하고 포인터를 얻습니다.
	auto * impl = ENSURE_IMPL(Equipment);
	if (nullptr == impl) {
		// Impl 확보 실패 시 에러 반환 (DEBUG_STOP이 걸리지 않았다면)
		return Result::Fail(Error::NotInitialized, "Equipment implementation is not valid.");
	}

	return impl->writeDatal(os);
}

AM::Template & AM::Equipment::insertTemplate(TemplateKit const & kit)
{
	auto * impl = ENSURE_IMPL(Equipment);
	if (nullptr == impl) {
		DEBUG_STOP;
	}

	auto templ = std::make_unique<Template>();
	templ->set(kit);

	Template * returnTemplate = templ.get();

	impl->m_templates.emplace_back(std::move(templ));

	return *returnTemplate;
}