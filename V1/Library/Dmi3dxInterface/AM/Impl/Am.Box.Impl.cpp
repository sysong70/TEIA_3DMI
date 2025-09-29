#include "StdAfx.h"

#include "AM.Box.Impl.h"

using namespace AM;

//== BoxKitImpl Class ==============================================================================
AM::BoxKitImpl::BoxKitImpl()
{
	SetType(AM::Type::BoxKit);
}

std::unique_ptr<Impl> AM::BoxKitImpl::Clone() const
{
	CLONE_IMPL(BoxKitImpl);
}

//== BoxImpl Class =================================================================================
AM::BoxImpl::BoxImpl()
{
	SetType(AM::Type::Box);
}

std::unique_ptr<Impl> AM::BoxImpl::Clone() const
{
	CLONE_IMPL(BoxImpl);
}

Result AM::BoxImpl::writeDatal(std::ostream * os) noexcept
{
	if (nullptr == os) {
		return Result::Fail(Error::InvalidArgument, "ostream is null.");
	}

	const auto & kit = m_kit;

	(*os) << "\n";
	(*os) << "NEW BOX" << "\n";

	// 1. POS 속성 출력
	//    kit.m_position (Box의 중심)을 Formatter::POS를 사용하여 출력합니다.
	(*os) << Formatter::POS(kit.m_position) << "\n";

	// 2. ORI 속성 출력 (Z축 기반)
	//    kit.m_orientation은 Z축 벡터라고 가정하고, Formatter::ORI(zAxis)를 사용하여 Y축을 자동 생성하고 출력합니다.
	//    NOTE: BoxParameters에는 X, Y, Z축이 모두 있지만, Formatter::ORI(zAxis)를 사용합니다.
	(*os) << Formatter::ORI(kit.m_orientation) << "\n";

	// 3. XLEN, YLEN, ZLEN 속성 출력
	//    Formatter::Mm은 길이와 단위를 포맷팅합니다.
	(*os) << "XLEN " << Formatter::Mm(kit.m_size.x) << "\n";
	(*os) << "YLEN " << Formatter::Mm(kit.m_size.y) << "\n";
	(*os) << "ZLEN " << Formatter::Mm(kit.m_size.z) << "\n";

	(*os) << "END" << "\n";

	if (!os->good()) {
		return Result::Fail(Error::WriteFailed, "BoxImpl::WriteDatal: Stream write failure.");
	}

	return Result::Ok();
}