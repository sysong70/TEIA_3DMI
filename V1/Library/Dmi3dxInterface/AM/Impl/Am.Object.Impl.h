#pragma once

#include "../AM.h"

#include "Am.Impl.h"

#include <filesystem>

namespace AM
{
	class ObjectImpl : public Impl
	{
	public:
		ObjectImpl();

		std::unique_ptr<Impl> Clone() const override;

		virtual Result writeDatal(std::ostream * os) noexcept;

	private:
		std::ostream * m_os = nullptr;
	};

	namespace Formatter
	{
		enum class Axis : uint8_t {
			X,
			Y,
			Z
		};

		// CPP 파일에서 정의될 함수들의 선언
		std::string axisToStr(Axis a);
		void BuildXYFromAxis(H3DF::DVector zAxis, H3DF::DVector & xOut, H3DF::DVector & yOut);
		std::string formatVectorAsDatalOri(const H3DF::DVector & vec, const char * label);
		std::string ORI(const H3DF::DVector & yAxis, const H3DF::DVector & zAxis);


		// 1. 인라인 정의 유지 (작고, 성능에 중요하며, static이 아닌 유틸리티)

		static std::string Quoted(std::string_view s)
		{
			std::ostringstream oss;
			oss << '\'' << s << '\'';
			return oss.str();
		}

		static std::string Mm(double v)
		{
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(3) << v << "mm";
			return oss.str();
		}

		static std::string Kg(double v)
		{
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(3) << v << "kg";
			return oss.str();
		}

		// 2. POS 래퍼 함수는 정의 유지
		static std::string POS(const H3DF::Point & position);
		static std::string POS(const H3DF::DPoint & position)
		{
			std::ostringstream oss;
			oss << "POS X " << Mm(position.x) << " Y " << Mm(position.y) << " Z " << Mm(position.z);
			return oss.str();
		}
		static std::string POS(const H3DF::Point & position)
		{
			H3DF::DPoint tempPosition = position;
			return POS(tempPosition);
		}

		// 3. ORI (Z축만 받는 버전) 선언
		std::string ORI(const H3DF::DVector & zAxis);


		// 4. Primitive 공통 끝
		constexpr std::string_view END() noexcept
		{
			return "END";
		}
	};
}

