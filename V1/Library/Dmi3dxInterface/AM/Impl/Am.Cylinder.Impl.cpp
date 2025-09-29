#include "StdAfx.h"

#include "AM.Cylinder.Impl.h"

using namespace AM;

//== CylinderKitImpl Class =========================================================================
AM::CylinderKitImpl::CylinderKitImpl()
{
	SetType(AM::Type::CylinderKit);
}

std::unique_ptr<Impl> AM::CylinderKitImpl::Clone() const
{
	CLONE_IMPL(CylinderKitImpl);
}

//== CylinderImpl Class ============================================================================
AM::CylinderImpl::CylinderImpl()
{
	SetType(AM::Type::Cylinder);
}

std::unique_ptr<Impl> AM::CylinderImpl::Clone() const
{
	CLONE_IMPL(CylinderImpl);
}

Result AM::CylinderImpl::writeDatal(std::ostream * os) noexcept
{
	if (nullptr == os) {
		return Result::Fail(Error::InvalidArgument, "ostream is null.");
	}

	const auto & kit = m_kit;

	// 1. Primitive Type 결정 및 Hole 여부 확인
	const bool isHole = (kit.m_productHint == "Hole");
	const std::string primitiveType = isHole ? "NEW NCYLINDER" : "NEW CYLINDER";

	(*os) << primitiveType << "\n";

	// 2. POS 속성 출력
	//    Formatter::POS는 위치 포맷팅을 담당
	(*os) << Formatter::POS(kit.m_position) << "\n";

	// 3. ORI 속성 출력
	(*os) << Formatter::ORI(kit.m_orientation) << "\n";

	// 4. DIAM 및 HEIG 속성 출력
	(*os) << "DIAM " << Formatter::Mm(kit.m_diameter) << "\n";
	(*os) << "HEIG " << Formatter::Mm(kit.m_height) << "\n";

	// 5. Hole Product Hint 출력
	if (isHole) {
		(*os) << "PRODHT 'Hole'" << "\n";
	}

	(*os) << "END" << "\n";

	if (!os->good()) {
		return Result::Fail(Error::WriteFailed, "CylinderImpl::WriteDatal: Stream write failure.");
	}

	return Result::Ok();
}