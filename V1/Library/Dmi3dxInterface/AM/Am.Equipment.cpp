#include "StdAfx.h"

#include "Am.Equipment.h"

#include "./Impl/Am.Impl.h"
#include "./Impl/Am.Equipment.Impl.h"

using namespace AM;

AM::Equipment::Equipment()
{
	SET_IMPL(Equipment);
	AM::Impl::setImpl(*this, std::unique_ptr<AM::EquipmentImpl>());
}

AM::Equipment::~Equipment()
{

}

void AM::Equipment::setTitile(std::string_view title)
{
	IMPL(Equipment);
	impl->m_title = title;
}

void AM::Equipment::setBuiltIn(bool builtIn)
{
	IMPL(Equipment);
	impl->m_builtIn = builtIn;
}