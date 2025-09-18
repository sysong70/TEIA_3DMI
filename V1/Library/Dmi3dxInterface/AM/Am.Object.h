#pragma once

#include "AM.h"
#include "../3DX.h"

#include <3DF/Math.h>
#include <Common_Define.h>

#include <memory>
#include <filesystem>

namespace AM
{
	class Object
	{
	public:
		Object();
		Object(Object const & that);
		virtual ~Object();

		static const AM::Type staticType = AM::Type::None;
		AM::Type ObjectType() const { return staticType; }
		virtual AM::Type Type() const;

		Object const & operator = (Object const & that);

		Object(Object && that) noexcept;
		Object & operator=(Object && that) noexcept;

		Result Bind(std::ostream * os) noexcept;

	private:
		friend class Impl;
		std::unique_ptr<Impl> m_impl;
	};	

	namespace Formatter 
	{
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

		static std::string POS(const H3DF::Point & position)
		{
			H3DF::DPoint tempPosition = position;
			return POS(tempPosition);
		}

		static std::string POS(const H3DF::DPoint & position)
		{
			std::ostringstream oss;
			oss << "POS X " << Mm(position.x) << " Y " << Mm(position.y) << " Z " << Mm(position.z);
			return oss.str();
		}

		// ORI Y is <vec> and Z is <vec>  형식으로 출력 (단순 축명/부호 표기)
		static std::string ORI(const H3DF::Vector & yAixs, const H3DF::Vector & zAxis)
		{
			H3DF::DVector tempYAxis = yAixs;
			H3DF::DVector tempZAxis = zAxis;

			return ORI(tempYAxis, tempZAxis);
		}

		static std::string ORI(const H3DF::DVector & yAxis, const H3DF::DVector & zAxis)
		{
			// 간단화: 축이 ±X/±Y/±Z인 경우에만 축명 표기. 그 외는 수치 벡터 표기.
			auto dirName = [](const H3DF::DVector & aixs)->std::string {
				const double ax = std::abs(aixs.x), ay = std::abs(aixs.y), az = std::abs(aixs.z);

				auto signName = [](double s, const char * nm) {
					return (s >= 0) ? std::string(nm) : std::string("-") + nm;
				};

				if (ax > 0.999 && ay < 0.001 && az < 0.001) return signName(aixs.x, "X");
				if (ay > 0.999 && ax < 0.001 && az < 0.001) return signName(aixs.y, "Y");
				if (az > 0.999 && ax < 0.001 && ay < 0.001) return signName(aixs.z, "Z");

				std::ostringstream oss; oss << std::fixed << std::setprecision(3)
					<< "(" << aixs.x << "," << aixs.y << "," << aixs.z << ")";

				return oss.str();
			};

			std::ostringstream oss;
			oss << "ORI Y is " << dirName(yAxis) << " and Z is " << dirName(zAxis);
			return oss.str();
		}

		// Primitive 공통 끝
		constexpr std::string_view END() noexcept
		{
			return "END";
		}
	};
}