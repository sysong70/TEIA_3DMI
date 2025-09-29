#include "StdAfx.h"

#include "AM.Object.Impl.h"

using namespace AM;

AM::ObjectImpl::ObjectImpl()
{
	SetType(AM::Type::Object);
}

std::unique_ptr<Impl> AM::ObjectImpl::Clone() const
{
	auto impl = std::make_unique<ObjectImpl>(*this);
	DEBUG_VALID(impl.get());

	return impl;
}

Result AM::ObjectImpl::writeDatal(std::ostream * os) noexcept
{
	if (nullptr == os) {
		return Result::Fail(Error::InvalidArgument, "DatalWriter::Bind: os is null");
	}

	m_os = os;

	return Result::Ok();
}

// axisToStr 정의
std::string AM::Formatter::axisToStr(Axis a)
{
	switch (a) {
		case Axis::X:
			return "X";
		case Axis::Y:
			return "Y";
		default:
			return "Z";
	}
}

// BuildXYFromAxis 정의 (LNK2005 오류 방지를 위해 여기에 정의)
void AM::Formatter::BuildXYFromAxis(H3DF::DVector zAxis, H3DF::DVector & xOut, H3DF::DVector & yOut)
{
	// zAxis는 이미 DVector로 들어오므로, 복사본을 만들어 정규화합니다.
	auto z = zAxis;
	z.Normalize();

	// 수직 벡터 t를 안정적으로 선택합니다.
	H3DF::DVector t = (std::fabs(z.z) < 0.9f) ? H3DF::DVector(0, 0, 1) : H3DF::DVector(1, 0, 0);

	xOut = t.Cross(z);
	xOut.Normalize();

	yOut = z.Cross(xOut); // 오른손 좌표계
	yOut.Normalize();
}

// formatVectorAsDatalOri 정의
std::string AM::Formatter::formatVectorAsDatalOri(const H3DF::DVector & vec, const char * label)
{
	// 1. 성분 분석 및 정렬
	struct AxisComp {
		Axis ax;
		double val;
		double abs;
	};

	AxisComp comps[3] = {
	   { Axis::X, vec.x, std::abs(vec.x) },
	   { Axis::Y, vec.y, std::abs(vec.y) },
	   { Axis::Z, vec.z, std::abs(vec.z) }
	};

	// 절대값 기준으로 정렬: sA (가장 큰 성분), tA (두 번째 성분)
	std::sort(std::begin(comps), std::end(comps),
		[](const AxisComp & A, const AxisComp & B) {
		return A.abs > B.abs;
	});

	const AxisComp & sA = comps[0]; // Start Axis (시작축: 가장 큰 성분)
	const AxisComp & tA = comps[1]; // Toward Axis (향하는 축: 두 번째 성분)

	// 2. 각도 계산
	// atan2(Opposite, Adjacent)를 사용하여 [0, 90]도 사이의 각도를 얻습니다.
	double angleRad = std::atan2(tA.abs, sA.abs);
	double angleDeg = angleRad * 180.0 / M_PI;

	// 3. 포맷팅
	std::ostringstream oss;
	oss << label;

	// 시작 축 출력 (부호 포함)
	oss << (sA.val >= 0.0 ? "" : "-") << axisToStr(sA.ax);

	// 각도가 0이 아니면 [Angle] [Toward Axis]를 추가
	if (angleDeg > 1e-3) { // 0도 초과 시에만 각도와 두 번째 축 출력
		oss << " " << std::fixed << std::setprecision(3) << angleDeg;
		oss << " " << (tA.val >= 0.0 ? "" : "-") << axisToStr(tA.ax);
	}

	return oss.str();
}

// ORI (Z축만 받는 버전) 정의
std::string AM::Formatter::ORI(const H3DF::DVector & zAxis)
{
	H3DF::DVector z = zAxis;
	z.Normalize();

	H3DF::DVector y;
	H3DF::DVector x;

	BuildXYFromAxis(z, x, y);

	// yAxis를 기반으로 "ORI Y is ..." 부분 생성
	std::string y_line = formatVectorAsDatalOri(y, "ORI Y is ");

	// zAxis를 기반으로 "Z is ..." 부분 생성
	std::string z_line = formatVectorAsDatalOri(z, "Z is ");

	return y_line + " and " + z_line;
}

// ORI (Y, Z축을 모두 받는 버전) 정의
std::string AM::Formatter::ORI(const H3DF::DVector & yAxis, const H3DF::DVector & zAxis)
{
	// yAxis를 기반으로 "ORI Y is ..." 부분 생성
	std::string y_line = formatVectorAsDatalOri(yAxis, "ORI Y is ");

	// zAxis를 기반으로 "Z is ..." 부분 생성
	std::string z_line = formatVectorAsDatalOri(zAxis, "Z is ");

	// z_line에서 "Z is " 부분을 제거하고 결합
	size_t pos = z_line.find("Z is ");
	if (pos != std::string::npos) {
		z_line.erase(pos, 5);
	}

	return y_line + " and " + z_line;
}