#pragma once

#include "3DF.h"

/*
#ifndef MVO_API
#	ifdef _3DF_EXPORT
#		define MVO_API __declspec (dllexport)
#	else
#		define MVO_API __declspec (dllimport)
#	endif
#endif

#include <HGlobals.h>
*/

#include <limits>

#ifndef PI
#	define PI 3.1415926535897932384626433832795028841971693993751
#endif

#define TEMPLATE_API_H3DF __declspec(dllexport)

#pragma warning(push)
#pragma warning(disable : 4251) // Not an issue as long as debug and release libraries aren't mixed

OPEN_3DF_NAMESPACE

template <typename T>	H3DF_INLINE	T		Abs(T const & a) { return  a < 0 ? -a : a; }
template <typename T>	H3DF_INLINE	int		Compare(T const & a, T const & b) { return a == b ? 0 : a < b ? -1 : 1; }
template <typename T>	H3DF_INLINE	int		Sign(T const & a) { return Compare(a, (T) 0); }
template <typename T>	H3DF_INLINE	void	Swap(T & a, T & b) { T temp = a; a = b; b = temp; }
template <typename T>	H3DF_INLINE	int		Floor(T const & a) { return ((a > 0 || (T) (int) a == a) ? (int) a : ((int) a - 1)); }
template <typename T>	H3DF_INLINE	int		Ceiling(T const & a) { return ((a < 0 || (T) (int) a == a) ? (int) a : ((int) a + 1)); }

template <typename T>	H3DF_INLINE	T const & Min(T const & a, T const & b) { return  a < b ? a : b; }
template <typename T>	H3DF_INLINE	T const & Min(T const & a, T const & b, T const & c) { return  Min(Min(a, b), c); }
template <typename T>	H3DF_INLINE	T const & Min(T const & a, T const & b, T const & c, T const & d) { return Min(Min(a, b, c), d); }
template <typename T>	H3DF_INLINE	T const & Min(T const & a, T const & b, T const & c, T const & d, T const & e) { return Min(Min(a, b, c, d), e); }
template <typename T>	H3DF_INLINE	T const & Min(T const & a, T const & b, T const & c, T const & d, T const & e, T const & f) { return Min(Min(a, b, c, d, e), f); }

template <typename T>	H3DF_INLINE	T const & Max(T const & a, T const & b) { return  a > b ? a : b; }
template <typename T>	H3DF_INLINE	T const & Max(T const & a, T const & b, T const & c) { return  Max(Max(a, b), c); }
template <typename T>	H3DF_INLINE	T const & Max(T const & a, T const & b, T const & c, T const & d) { return Max(Max(a, b, c), d); }
template <typename T>	H3DF_INLINE	T const & Max(T const & a, T const & b, T const & c, T const & d, T const & e) { return Max(Max(a, b, c, d), e); }
template <typename T>	H3DF_INLINE	T const & Max(T const & a, T const & b, T const & c, T const & d, T const & e, T const & f) { return Max(Max(a, b, c, d, e), f); }

template <typename T>	H3DF_INLINE	T const & Clamp(T const & x, T const & min, T const & max) { return x < min ? min : x > max ? max : x; }

//== Float Template Specializations ================================================================

template <typename F>	struct Float_Traits {};
template <> struct Float_Traits<float>
{
	typedef double	Alternative;
	static const int Type = 1;
	static float Epsilon() { return 1.0e-30f; }
};
template <> struct Float_Traits<double>
{
	typedef float	Alternative;
	static const int Type = 2;
	static double Epsilon() { return 1.0e-300; }
};

/// The Float class is a concept class that exposes a number of useful utilities for working with floating point numbers.
class API_3DF Float
{
private:
	enum Parts
	{
# if HOOPS_BIGENDIAN
		High, Low
# else
		Low, High
# endif
	};

	// & functions for a float represented in an int, * version for a double in an array of 2 ints
	static H3DF_INLINE bool is_infinite(int32_t const & v) { return (v & 0x7FFFFFFF) == 0x7F800000; }
	static H3DF_INLINE bool is_infinite(uint32_t const & v) { return (v & 0x7FFFFFFF) == 0x7F800000; }
	static H3DF_INLINE bool is_infinite(int32_t const * v) { return (v[High] & 0x7FFFFFFF) == 0x7FF00000 && v[Low] == 0; }
	static H3DF_INLINE bool is_infinite(uint32_t const * v) { return (v[High] & 0x7FFFFFFF) == 0x7FF00000 && v[Low] == 0; }

	static H3DF_INLINE bool is_nan(int32_t const & v) {
		uint32_t exp = v & 0x7F800000, mantissa = v & 0x007FFFFF;
		return exp == 0x7F800000 && mantissa != 0;
	}
	static H3DF_INLINE bool is_nan(uint32_t const & v) {
		uint32_t exp = v & 0x7F800000, mantissa = v & 0x007FFFFF;
		return exp == 0x7F800000 && mantissa != 0;
	}
	static H3DF_INLINE bool is_nan(int32_t const * v) {
		uint32_t exp = v[High] & 0x7FF00000, mantissa_high = v[High] & 0x000FFFFF;
		return exp == 0x7FF00000 && (mantissa_high | v[Low]) != 0;
	}
	static H3DF_INLINE bool is_nan(uint32_t const * v) {
		uint32_t exp = v[High] & 0x7FF00000, mantissa_high = v[High] & 0x000FFFFF;
		return exp == 0x7FF00000 && (mantissa_high | v[Low]) != 0;
	}

	static H3DF_INLINE bool is_special(int32_t const & v) { return (v & 0x7F800000) == 0x7F800000; }
	static H3DF_INLINE bool is_special(uint32_t const & v) { return (v & 0x7F800000) == 0x7F800000; }
	static H3DF_INLINE bool is_special(int32_t const * v) { return (v[High] & 0x7FF00000) == 0x7FF00000; }
	static H3DF_INLINE bool is_special(uint32_t const * v) { return (v[High] & 0x7FF00000) == 0x7FF00000; }
public:

	/*! The 32-bit float representation of infinity. */
	static const float Infinity;
	/*! The 32-bit float representation of negative infinity. */
	static const float NegativeInfinity;

	/*! See if the value is either infinity */
	static H3DF_INLINE bool IsInfinite(float const & a) { return is_infinite(extract_uint32_t(a)); }
	static H3DF_INLINE bool IsInfinite(double const & a) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		return is_infinite(v);
	}

	/*! See if the value is Not-A-Number */
	static H3DF_INLINE bool IsNAN(float const & a) { return is_nan(extract_uint32_t(a)); }
	static H3DF_INLINE bool IsNAN(double const & a) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		return is_nan(v);
	}

	/*! See if the value is not "normal" (infinite or NaN) */
	static H3DF_INLINE bool IsAbnormal(float const & a) { return is_special(extract_uint32_t(a)); }
	static H3DF_INLINE bool IsAbnormal(double const & a) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		return is_special(v);
	}

	// Checks two floats for equality within a specified tolerance.
	// The tolerance is specified in float increments that scale with the floats themselves.
	static H3DF_INLINE bool Equals(float const & a, float const & b, int tolerance = 32);
	static H3DF_INLINE bool Equals(double const & a, double const & b, int tolerance = 32);

	template <typename Alloc>
	static H3DF_INLINE bool Equals(std::vector<float, Alloc> const & a, std::vector<float, Alloc> const & b, int tolerance = 32)
	{
		if(a.size() != b.size())
			return false;

		auto it1 = a.begin();
		auto it2 = b.begin();
		auto const end = a.end();
		for(; it1 != end; ++it1, ++it2)
		{
			if(!Equals(*it1, *it2, tolerance))
				return false;
		}
		return true;
	}

	static H3DF_INLINE uint32_t extract_sign_bit(float const & a) {
		return extract_uint32_t(a) & 0x80000000;
	}
	static H3DF_INLINE uint32_t extract_sign_bit(double const & a) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		return v[High] & 0x80000000;
	}

	static H3DF_INLINE void apply_sign_bit(float & a, uint32_t const & sign_bit) {
		uint32_t v = extract_uint32_t(a);
		v &= 0x7FFFFFFF;
		v |= sign_bit;
		inject_uint32_t(a, v);
	}
	static H3DF_INLINE void apply_sign_bit(double & a, uint32_t const & sign_bit) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		v[High] &= 0x7FFFFFFF;
		v[High] |= sign_bit;
		memcpy(&a, v, sizeof(double));
	}


	static H3DF_INLINE unsigned char unit_to_byte(float const & a) {
		uint32_t v = extract_uint32_t(a);

		v &= 0x7FFFFFFF;
		if(v < 0x3B800000)
			return 0;

		v--;

		uint32_t exp = v >> 23;
		uint32_t man = (v & 0x007FFFFF) | 0x00800000;

		return (unsigned char) (man >> (16 + 126 - exp));
	}

	static H3DF_INLINE unsigned char unit_to_byte_scaled(float const & a, unsigned char mix) {
		uint32_t v = extract_uint32_t(a);

		v &= 0x7FFFFFFF;
		if(v < 0x3B800000)
			return 0;

		v--;

		uint32_t exp = v >> 23;
		uint32_t man = (v & 0x007FFFFF) | 0x00800000;

		uint32_t x = (man >> (16 + 126 - exp));

		return (unsigned char) ((x * (mix + 1)) >> 8);
	}


	static H3DF_INLINE bool match(float const & a, float const & b) {
		uint32_t va = extract_uint32_t(a);
		uint32_t vb = extract_uint32_t(b);

		if(((va | vb) & 0x7FFFFFFF) == 0)
			return true;

		return va == vb;
	}
	static H3DF_INLINE bool match(double const & a, double const & b) {
		return a == b;
	}


	static H3DF_INLINE void replace_if_smaller(float & a, float const & b) {
		if(b < a)
			a = b;
	}
	static H3DF_INLINE void replace_if_smaller(double & a, double const & b) {
		if(b < a)
			a = b;
	}

	static H3DF_INLINE void replace_if_larger(float & a, float const & b) {
		if(b > a)
			a = b;
	}
	static H3DF_INLINE void replace_if_larger(double & a, double const & b) {
		if(b > a)
			a = b;
	}


	static H3DF_INLINE uint32_t extract_uint32_t(float const & a) {
		uint32_t i;
		memcpy(&i, &a, sizeof(float));
		return i;
	}

	static H3DF_INLINE void inject_uint32_t(float & a, uint32_t const & i) {
		memcpy(&a, &i, sizeof(float));
	}

 	static H3DF_INLINE float C2F(unsigned char x) {
 		return (float)x * (1.0f/255.0f);
 	}

	// SSE convenience functions
	static H3DF_INLINE void pack_4(float const & f, float * m) {
		memcpy(&m[0], &f, sizeof(float));
		memcpy(&m[1], &f, sizeof(float));
		memcpy(&m[2], &f, sizeof(float));
		memcpy(&m[3], &f, sizeof(float));
	}

	static H3DF_INLINE void pack_4(float const & f0, float const & f1, float const & f2, float const & f3, float * m) {
		memcpy(&m[0], &f0, sizeof(float));
		memcpy(&m[1], &f1, sizeof(float));
		memcpy(&m[2], &f2, sizeof(float));
		memcpy(&m[3], &f3, sizeof(float));
	}

	static H3DF_INLINE void unpack_4(float * f0, float const * const m) {
		memcpy(f0, m, sizeof(float) * 4);
	}

	static H3DF_INLINE void unpack_4(float & f0, float & f1, float & f2, float & f3, float const * const m) {
		memcpy(&f0, &m[0], sizeof(float));
		memcpy(&f1, &m[1], sizeof(float));
		memcpy(&f2, &m[2], sizeof(float));
		memcpy(&f3, &m[3], sizeof(float));
	}

private:

	Float();
};

H3DF_INLINE bool Float::Equals(float const & a, float const & b, int tolerance) {
	int32_t va = Float::extract_uint32_t(a);
	int32_t vb = Float::extract_uint32_t(b);

	if(is_special(va) || is_special(vb)) {
		if(is_infinite(va) || is_infinite(vb))
			return va == vb;  // final check is for sign bits same
		if(is_nan(va) || is_nan(vb))
			return false;
	}

	int const close_to_zero = 0x36A00000; // (approx) 5.0e-6f;
	if((va & 0x7FFFFFFF) == 0)
		return (vb & 0x7FFFFFFF) < close_to_zero;
	else if((vb & 0x7FFFFFFF) == 0)
		return (va & 0x7FFFFFFF) < close_to_zero;

	uint32_t sign_mask = va ^ vb;
	(int32_t &) sign_mask >>= 31;

	int32_t diff = ((va + sign_mask) ^ (sign_mask & 0x7FFFFFFF)) - vb;
	int32_t v1 = tolerance + diff;
	int32_t v2 = tolerance - diff;
	return (v1 | v2) >= 0;
}

H3DF_INLINE bool Float::Equals(double const & a, double const & b, int tolerance) {
	int32_t va[2], vb[2];
	memcpy(va, &a, sizeof(double));
	memcpy(vb, &b, sizeof(double));

	if(is_special(va) || is_special(vb)) {
		if(is_infinite(va) || is_infinite(vb))
			return va[High] == vb[High] && va[Low] == vb[Low]; // final check is for sign bits same
		if(is_nan(va) || is_nan(vb))
			return false;
	}

	if((va[High] == 0 && va[Low] == 0) || (vb[High] == 0 && vb[Low] == 0))
		return Abs(a - b) < 0.000000000000005;

	if(extract_sign_bit(a) != extract_sign_bit(b))
		return a == b; //-V550

	if(va[High] != vb[High])
		return false;

	return Abs(va[Low] - vb[Low]) <= tolerance;
}

/*! The Coordinate class is a concept class that contains information about various coordinate spaces. */
class Coordinate
{
public:

	/*! \enum Space
		The Space enum is a list of all coordinate spaces used in Visualize.
	*/
	enum class Space : uint32_t
	{
		Object,					// An infinite 3D Cartesian coordinate system local to the object or segment itself.
		World,					// An infinite 3D Cartesian coordinate system where objects reside after their modelling transformations have been applied.
		Camera,					// A space defined by a camera's view of world space with the origin at the camera position, y-axis along the camera's up vector and z-axis pointing toward the camera target.
		Window,					// A rectangle ([-1,1] in x and y directions) with the origin at center of the outer window.
		Pixel,					// The pixel position of the outer window counting from the top-left corner
		InnerWindow,			// A rectangle ([-1,1] in x and y directions) with the origin at center of the inner window.
		InnerPixel,				// The pixel position of the inner window counting from the top-left corner
		//NormalizedInnerWindow,	// Same as InnerWindow except z is in [0,1] with 0 at the camera limit and 1 at infinity
		//NormalizedInnerPixel,	// Same as InnerPixel except z is in [0,1] with 0 at the camera limit and 1 at infinity
		//NormalizedWindow,		// Same as Window except z is in [0,1] with 0 at the camera limit and 1 at infinity
		//NormalizedPixel,		// Same as Pixel except z is in [0,1] with 0 at the camera limit and 1 at infinity
		ScreenRange,
	};

private:
	Coordinate() {}
};

template <typename F> class TEMPLATE_API_H3DF Vector_3D;
template <typename F> class TEMPLATE_API_H3DF Plane_3D;
template <typename F> class TEMPLATE_API_H3DF Vector_2D;
template <typename F> class TEMPLATE_API_H3DF Point_2D;

template <typename F>
class TEMPLATE_API_H3DF Point_3D
{
public:
	F x;
	F y;
	F z;

	Point_3D() { x = 0, y = 0, z = 0; }
	Point_3D(F v1, F v2, F v3 = (F)0.0) : x(v1), y(v2), z(v3) {}

	template <typename D>
	Point_3D(Point_3D<D> const & that) : x((F) that.x), y((F) that.y), z((F) that.z) {}

	Point_3D(Vector_3D<F> const & v) : x(v.x), y(v.y), z(v.z) {}
	Point_3D(Vector_2D<F> const & v) : x(v.x), y(v.y), z((F)0.0) {}
	Point_3D(Point_2D<F> const & that) : x(that.x), y(that.y), z((F)0.0) {}

	void Set(F X, F Y, F Z) { x = X; y = Y; z = Z; };

	Point_3D const	operator- () const	{ return Point_3D (-x, -y, -z); }

	bool operator == (Point_3D const & p) const { return  x == p.x && y == p.y && z == p.z; }
	bool operator != (Point_3D const & p) const { return  !(*this == p); }

	bool Equals(Point_3D const & p, int in_tolerance = 32) const {
		return	Float::Equals(x, p.x, in_tolerance) && Float::Equals(y, p.y, in_tolerance) && Float::Equals(z, p.z, in_tolerance);
	}

	Point_3D const operator + (const Point_3D & p) const { return Point_3D(x + p.x, y + p.y, z + p.z); }
	Vector_3D<F> const operator - (Point_3D const & p) const;

	template <typename D>
	H3DF_INLINE Point_3D const operator + (Vector_3D<D> const & v) const { return Point_3D((F)(x + v.x), (F)(y + v.y), (F)(z + v.z)); }

	template <typename D>
	H3DF_INLINE Point_3D const operator - (Vector_3D<D> const & v) const {return Point_3D(x - v.x, y - v.y, z - v.z); }

	Point_3D & operator *= (F s) { x *= s; y *= s; z *= s;  return *this; }
	Point_3D & operator /= (F s) { return operator*= ((F)1 / s); }
	Point_3D const operator * (F s) const { return Point_3D(x * s, y * s, z * s); }
	Point_3D const operator / (F s) const { return operator* ((F)1 / s); }

	F & operator [] (size_t i) { return (&x)[i]; }
	F const & operator [] (size_t i) const { return (&x)[i]; }

	// Vector_3D 관련 연산자
	Point_3D & operator += (Vector_3D<F> const & v);
	Point_3D & operator -= (Vector_3D<F> const & v);
	Point_3D & operator *= (Vector_3D<F> const & v);
	Point_3D & operator /= (Vector_3D<F> const & v);
	Point_3D const operator + (Vector_2D<F> const & v) const;
	Point_3D const operator - (Vector_2D<F> const & v) const;
	Point_3D const operator * (Vector_3D<F> const & v) const;
	Point_3D const operator / (Vector_3D<F> const & v) const;

	// Vector_2D 관련 연산자
	Point_3D & operator += (Vector_2D<F> const & v);
	Point_3D & operator -= (Vector_2D<F> const & v);
	Point_3D & operator *= (Vector_2D<F> const & v);
	Point_3D & operator /= (Vector_2D<F> const & v);
	Point_3D const operator * (Vector_2D<F> const & v) const;
	Point_3D const operator / (Vector_2D<F> const & v) const;

	static H3DF_INLINE Point_3D	Origin() {return Point_3D (0, 0, 0);};
	static H3DF_INLINE Point_3D	Zero() {return Point_3D (0, 0, 0);}; //-V524

	double	DistanceWith(Point_3D const & p) const;

	// Drop Point 함수는 3D 좌표를 2D 좌표로 변환하는 함수이다. (이 함수는 3D Point가 주어지는 Plane 위에 있어야 정확한 값으로 계산된다.)
	Point_2D<F> DropPoint(Point_3D cOrigin, Vector_3D<F> cXAxis, Vector_3D<F> cYAxis);

	// 3D Point를 주어진 Plane 위에 Projection 시킨다.
	Point_3D ProjectionPoint(Point_3D cOrigin, Vector_3D<F> cNormal);
};

template <typename F, typename S>
H3DF_INLINE Point_3D<F>	operator * (S s, Point_3D<F> const & a) { return Point_3D<F>(F(s * a.x), F(s * a.y), F(s * a.z)); }

template <typename F>
H3DF_INLINE Point_3D<F> Midpoint(Point_3D<F> const & a, Point_3D<F> const & b) {
	return Point_3D<F>(a.x + b.x, a.y + b.y, a.z + b.z) * 0.5f;
}

template <typename F>
H3DF_INLINE Point_3D<F> Midpoint(Point_3D<F> const & a, Point_3D<F> const & b, Point_3D<F> const & c) {
	return Point_3D<F>(a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z) * (F)(1.0 / 3.0);
}

template <typename F>
H3DF_INLINE bool Is_Abnormal(Point_3D<F> const & p) {
	return Is_Abnormal(p.x) || Is_Abnormal(p.y) || Is_Abnormal(p.z);
}

// template <typename F>
// H3DF_INLINE	Point_3D<F>::Point_3D(Vector_3D<F> const & v) : x(v.x), y(v.y), z(v.z) {}

template <typename F>
H3DF_INLINE	Point_3D<F> & Point_3D<F>::operator += (Vector_3D<F> const & v) { x += v.x; y += v.y; z += v.z;  return *this; }

template <typename F>
H3DF_INLINE	Point_3D<F> & Point_3D<F>::operator -= (Vector_3D<F> const & v) { x -= v.x; y -= v.y; z -= v.z;  return *this; }

template <typename F>
H3DF_INLINE	Vector_3D<F> const	Point_3D<F>::operator - (Point_3D<F> const & p) const { return Vector_3D<F>(x - p.x, y - p.y, z - p.z); }

template <typename F>
H3DF_INLINE	Point_3D<F> & Point_3D<F>::operator *= (Vector_3D<F> const & v) { x *= v.x; y *= v.y; z *= v.z;  return *this; }

template <typename F>
H3DF_INLINE	Point_3D<F> & Point_3D<F>::operator /= (Vector_3D<F> const & v) { x /= v.x; y /= v.y; z /= v.z;  return *this; }

template <typename F>
H3DF_INLINE	Point_3D<F> const Point_3D<F>::operator * (Vector_3D<F> const & v) const { return Point_3D<F>(x * v.x, y * v.y, z * v.z); }

template <typename F>
H3DF_INLINE	Point_3D<F> const Point_3D<F>::operator / (Vector_3D<F> const & v) const { return Point_3D<F>(x / v.x, y / v.y, z / v.z); }

template <typename F>
double Point_3D<F>::DistanceWith(Point_3D const & p) const {
	return (*this - p).Length();
}

template <typename F>
Point_2D<F> Point_3D<F>::DropPoint(Point_3D<F> cOrigin, Vector_3D<F> cXAxis, Vector_3D<F> cYAxis)
{
	Vector_3D<F> cVec = *this -cOrigin;
	Point_2D<F> cProjectionPoint;

	cProjectionPoint.x = cVec.x * cXAxis.x + cVec.y * cXAxis.y + cVec.z * cXAxis.z;
	cProjectionPoint.y = cVec.x * cYAxis.x + cVec.y * cYAxis.y + cVec.z * cYAxis.z;

	return cProjectionPoint;
}

template <typename F>
// Origin Point와 Normal Vector를 이용해서 Projection Point를 구함.
Point_3D<F> Point_3D<F>::ProjectionPoint(Point_3D<F> cOrigin, Vector_3D<F> cNormal)
{
	Vector_3D<F> cOriginVector = cOrigin;
	// Origin Point를 ZAxis 방향으로 투영한 내적값, 물론 ZAxis는 단위 벡터라야함.
	double dOriginPointProjectionDistance = cNormal.Dot(cOriginVector);

	// Target Point를 ZAxis 방향으로 투영한 내적값.
	double dInputPointProjectionDistance = cNormal.Dot(*this);

	// 두 내적값의 차이
	double dParameter = dOriginPointProjectionDistance - dInputPointProjectionDistance;

	return (*this + cNormal * dParameter);
}

template <typename F>
H3DF_INLINE Point_3D<F> Interpolate(Point_3D<F> const & a, Point_3D<F> const & b, float t) {
	return a + (b - a) * t;
}

template <typename F>
H3DF_INLINE Vector_3D<F> Interpolate(Vector_3D<F> const & a, Vector_3D<F> const & b, float t) {
	return Vector_3D<F>(a + (b - a) * t).Normalize();
}


template <typename F>
H3DF_INLINE	double PointToPointDistance(Point_3D<F> const & p1, Point_3D<F> const & p2) {
	return (p2 - p1).Length();
}

template <typename F>
H3DF_INLINE	double PointToPointDistanceSquared(Point_3D<F> const & p1, Point_3D<F> const & p2) {
	return (p2 - p1).LengthSquared();
}

template <typename F>
H3DF_INLINE Point_3D<F> Circumcenter(Point_3D<F> const & a, Point_3D<F> const & b, Point_3D<F> const & c) {
	F p = static_cast<F>((c - b).LengthSquared());
	F q = static_cast<F>((c - a).LengthSquared());
	F r = static_cast<F>((b - a).LengthSquared());

	return Point_3D<F>((a * (p * (q + r - p)) + (Vector_3D<F>)b * (q * (r + p - q)) + (Vector_3D<F>)c * (r * (p + q - r)))
		/ (2 * (p * q + p * r + q * r) - (p * p + q * q + r * r)));
}

template <typename F>
class TEMPLATE_API_H3DF Point_2D {
public:
	F	x;
	F	y;

	Point_2D() {}
	Point_2D(F v1, F v2) : x(v1), y(v2) {}

	template <typename D>
	explicit Point_2D(Point_2D<D> const & that) : x((F)that.x), y((F)that.y) {}

	explicit Point_2D(Point_3D<F> const & that) : x((F)that.x), y((F)that.y) {}
	explicit Point_2D(Vector_2D<F> const & that) : x((F)that.x), y((F)that.y) {}

	void Set(F X, F Y) { x = X; y = Y; };

	Point_2D const operator + (const Point_2D & p) const { return Point_2D(x + p.x, y + p.y); }
	Point_2D const	operator - () const { return Point_2D(-x, -y); }

	bool operator== (Point_2D const & p) const { return  x == p.x && y == p.y; }
	bool operator!= (Point_2D const & p) const { return  !(*this == p); }

	bool Equals(Point_2D const & p, int in_tolerance = 32) const {
		return Float::Equals(x, p.x, in_tolerance) && Float::Equals(y, p.y, in_tolerance);
	}

	Point_2D & operator *= (F s) { x *= s; y *= s; return *this; }
	Point_2D & operator /= (F s) { return operator*= ((F)1 / s); }
	Point_2D const operator * (F s) const { return Point_2D(x * s, y * s); }
	Point_2D const operator / (F s) const { return operator* ((F)1 / s); }

	F & operator[] (size_t i) { return (&x)[i]; }
	F const & operator[] (size_t i) const { return (&x)[i]; }

	Point_2D & operator += (Vector_2D<F> const & v);
	Point_2D & operator -= (Vector_2D<F> const & v);
	Point_2D & operator *= (Vector_2D<F> const & v);
	Point_2D & operator /= (Vector_2D<F> const & v);
	Point_2D const operator * (Vector_2D<F> const & v) const;
	Point_2D const operator / (Vector_2D<F> const & v) const;

	Vector_2D<F> const operator - (Point_2D const & p) const;

	Point_2D const operator + (Vector_2D<F> const & v) const;
	Point_2D const operator - (Vector_2D<F> const & v) const;

	static H3DF_INLINE Point_2D	Origin() { return Point_2D(0, 0); };
	static H3DF_INLINE Point_2D	Zero() { return Point_2D(0, 0); }; //-V524

	double DistanceWith(Point_2D const & p) const;

	Point_3D<F> LiftPoint(Point_3D<F> cOrigin, Vector_3D<F> cXAxis, Vector_3D<F> cYAxis);

	void Rotate(double dAngle, Point_2D cPivot) const;
};

// template <typename F>
// H3DF_INLINE Point_3D<F>::Point_3D(Point_2D<F> const & that) : x(that.x), y(that.y), z(0) {}

template <typename F, typename S>
H3DF_INLINE Point_2D<F>	operator* (S s, Point_2D<F> const & a) { return Point_2D<F>(F(s * a.x), F(s * a.y)); }

template <typename F>
H3DF_INLINE Point_2D<F> Midpoint(Point_2D<F> const & a, Point_2D<F> const & b) {
	return Point_2D<F>(a.x + b.x, a.y + b.y) * 0.5f;
}

template <typename F>
H3DF_INLINE Point_2D<F> Midpoint(Point_2D<F> const & a, Point_2D<F> const & b, Point_2D<F> const & c) {
	return Point_2D<F>(a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z) * (F)(1.0 / 3.0);
}

template <typename F>
H3DF_INLINE bool Is_Abnormal(Point_2D<F> const & p) {
	return Is_Abnormal(p.x) || Is_Abnormal(p.y);
}

template <typename F>
double Point_2D<F>::DistanceWith(Point_2D const & p) const {
	return (*this - p).Length();
}

template <typename F>
Point_3D<F> Point_2D<F>::LiftPoint(Point_3D<F> cOrigin, Vector_3D<F> cXAxis, Vector_3D<F> cYAxis)
{
	Point_3D<F> cLiftPoint;

	cLiftPoint.x = x * cXAxis.x + y * cYAxis.x + cOrigin.x;
	cLiftPoint.y = x * cXAxis.y + y * cYAxis.y + cOrigin.y;
	cLiftPoint.z = x * cXAxis.z + y * cYAxis.z + cOrigin.z;

	return cLiftPoint;
}

template <typename F>
void Point_2D<F>::Rotate(double dAngle, Point_2D<F> cPivot) const {
	x -= cPivot.x, y -= cPivot.y;
	double ox = x, oy = y;
	x = ox * cos(dAngle) - oy * sin(dAngle);
	y = ox * sin(dAngle) + oy * cos(dAngle);
	x += cPivot.x, y += cPivot.y;
}

template <typename F>
class TEMPLATE_API_H3DF Vector_3D
{
public:
	F x;
	F y;
	F z;

	Vector_3D () {}
	Vector_3D (F v1, F v2, F v3) : x (v1), y (v2), z (v3) {}
	
	template <typename D>
	Vector_3D (Vector_3D<D> const & that) : x ((F)that.x), y ((F)that.y), z ((F)that.z) {}

	template <typename D>
	Vector_3D (Point_3D<D> const & p) : x(p.x), y(p.y), z(p.z) {}
	Vector_3D(Plane_3D<F> const & p);
	Vector_3D (Vector_2D<F> const & that);

	void Set(F X, F Y, F Z) { x = X; y = Y; z = Z; };

	Vector_3D const	operator- () const	{ return Vector_3D (-x, -y, -z); }

	H3DF_INLINE bool operator== (Vector_3D const & v) const { return  Float::match(x, v.x) && Float::match(y, v.y) && Float::match(z, v.z); }
	H3DF_INLINE bool operator!= (Vector_3D const & v) const { return  !(*this == v); }

	H3DF_INLINE bool Equals(Vector_3D const & v, int in_tolerance = 32) const {
		return	Float::Equals(x, v.x, in_tolerance) && Float::Equals(y, v.y, in_tolerance) && Float::Equals(z, v.z, in_tolerance);
	}

	template<typename D>
	Vector_3D &			operator = (Point_3D<D> const & v)		{ x = v.x; y = v.y; z = v.z;  return *this; }

	template<typename D>
	Point_3D<D> & operator = (Vector_3D<D> const & v) { return Point_3D<D>(v.x, v.y, v.z); }

	Vector_3D &			operator+= (Vector_3D const & v)		{ x += v.x; y += v.y; z += v.z;  return *this; }
	Vector_3D &			operator-= (Vector_3D const & v)		{ x -= v.x; y -= v.y; z -= v.z;  return *this; }
	Vector_3D const		operator+ (Vector_3D const & v) const	{ return Vector_3D (x + v.x, y + v.y, z + v.z); }
	Vector_3D const		operator- (Vector_3D const & v) const	{ return Vector_3D (x - v.x, y - v.y, z - v.z); }

	Vector_3D &			operator*= (F s)				{ x *= s; y *= s; z *= s;  return *this; }
	Vector_3D &			operator/= (F s)				{ return operator*= (1.0f / s); }

	template<typename D>
	Vector_3D const		operator* (D s) const			{ return Vector_3D (F(x * s), F(y * s), F(z * s)); }
	//Vector_3D const		operator* (double s) const		{ return Vector_3D(x * s, y * s, z * s); }
	template<typename D>
	Vector_3D const		operator/ (D s) const			{ return operator* ((D)1.0 / s); }
	//Vector_3D const		operator/ (double s) const		{ return operator* (1.0f / s); }

	F &					operator[] (size_t i)		{ return (&x)[i]; }
	F const &			operator[] (size_t i) const	{ return (&x)[i]; }

	H3DF_INLINE double	Length () const { return sqrt (LengthSquared()); }

	H3DF_INLINE double	LengthSquared () const { return (double)x*(double)x + (double)y*(double)y + (double)z*(double)z; }

	H3DF_INLINE double	Length2D () const { return sqrt (LengthSquared2D()); }

	H3DF_INLINE double	LengthSquared2D () const { return (double)x*(double)x + (double)y*(double)y;}

	H3DF_INLINE Vector_3D &	Normalize (bool check_range = false, F epsilon = Float_Traits<F>::Epsilon()) {// not const &; allow V.normalize() *= S;
		if (check_range) {
			F	range = Max (Abs (x), Abs (y), Abs (z));
			if (range > F(1.0e10))
				operator/= (range);
		}

		F	len = (F)Length();
		if (len > epsilon)
			operator/= (len);
		else
			*this = Zero();
		return *this;
	}
	H3DF_INLINE Vector_3D &	Normalize (F epsilon) { return Normalize (false, epsilon); }

	H3DF_INLINE Vector_3D &	Clean_Zeroes (F epsilon = F(1.0e-4)) {
		if (Abs(x) < epsilon) x = 0;
		if (Abs(y) < epsilon) y = 0;
		if (Abs(z) < epsilon) z = 0;
		return *this;
	}

	H3DF_INLINE F Magnitude () const { return Max (Abs(x), Abs(y), Abs(z)); }
	H3DF_INLINE F Manhattan () const { return Abs(x)+Abs(y)+Abs(z); }

	H3DF_INLINE F Dot (Vector_3D const & v) const { return x * v.x  +  y * v.y  +  z * v.z; }

	H3DF_INLINE Vector_3D Cross (Vector_3D const & v) const { 
		return Vector_3D (y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); 
	}

	H3DF_INLINE Vector_3D Scale(Vector_3D const & v) const {
		return Vector_3D(x * v.x, y * v.y, z * v.z);
	}

	Vector_3D<F> different_vector () const {
		if (Abs (x) < Abs (y))
			return Vector_3D<F>::XAxis();
		else
			return Vector_3D<F>::YAxis();
	}

	H3DF_INLINE F AngleWith(Vector_3D const & v) const {
		F dot = Dot(v);
		F len = Length() * v.Length();
		F cos_angle = dot / len;
		return acos(cos_angle) * (F) 180 / PI;
	}

	H3DF_INLINE F CCWAngleWith(Vector_3D const & v1, Vector_3D const & v2) const {
		F dot = v1.Dot(v2);
		F len = v1.Length() * v2.Length();
		F cos_angle = dot / len;

		//Vector_3D<F> cross = v1.Cross(*this);
		Vector_3D<F> cross = this->Cross(v1);
		F dot2 = cross.Dot(v2);
		F angle = acos(cos_angle) * (F) 180 / PI;

		if (dot2 < 0) {
			angle = 360.0 - angle;
		}

		return angle;
	}

	void Rotate(Point_3D<F> cOrigin, Vector_3D cXAxis, Vector_3D cYAxis, double Angle) const;

	static H3DF_INLINE Vector_3D XAxis() { return Vector_3D(1, 0, 0); };
	static H3DF_INLINE Vector_3D YAxis() { return Vector_3D(0, 1, 0); };
	static H3DF_INLINE Vector_3D ZAxis() { return Vector_3D(0, 0, 1); };
	static H3DF_INLINE Vector_3D Zero() { return Vector_3D(0, 0, 0); };
	static H3DF_INLINE Vector_3D Unit() { return Vector_3D(1, 1, 1); };
};

template <typename F, typename S>
H3DF_INLINE	Vector_3D<F>	operator* (S s, Vector_3D<F> const & v) { return Vector_3D<F>(F(s * v.x), F(s * v.y), F(s * v.z)); }

template <typename F>
H3DF_INLINE bool Is_Abnormal(Vector_3D<F> const & v) {
	return Is_Abnormal(v.x) || Is_Abnormal(v.y) || Is_Abnormal(v.z);
}

template <typename F>
H3DF_INLINE bool Normalize(size_t count, Vector_3D<F> * vectors) {
	bool success = true;
	for (size_t i = 0; i < count; ++i) {
		if (vectors->Normalize() == Vector_3D<F>::Zero())
			success = false;
		vectors++;
	}
	return success;
}

template <typename F>
H3DF_INLINE	Vector_3D<F>::Vector_3D(Vector_2D<F> const & that) : x(that.x), y(that.y), z(0) {}

// template <typename F>
// H3DF_INLINE	Point_3D<F>::Point_3D(Vector_2D<F> const & v) : x(v.x), y(v.y), z(0) {}

template <typename F>
H3DF_INLINE	Point_3D<F> & Point_3D<F>::operator+= (Vector_2D<F> const & v) { x += v.x; y += v.y; return *this; }
template <typename F>
H3DF_INLINE	Point_3D<F> & Point_3D<F>::operator-= (Vector_2D<F> const & v) { x -= v.x; y -= v.y; return *this; }

template <typename F>
H3DF_INLINE	Point_3D<F> const	Point_3D<F>::operator+ (Vector_2D<F> const & v) const { return Point_3D<F>(x + v.x, y + v.y, z); }
template <typename F>
H3DF_INLINE	Point_3D<F> const	Point_3D<F>::operator- (Vector_2D<F> const & v) const { return Point_3D<F>(x - v.x, y - v.y, z); }

template <typename F>
H3DF_INLINE	Point_3D<F> & Point_3D<F>::operator*= (Vector_2D<F> const & v) { x *= v.x; y *= v.y; return *this; }
template <typename F>
H3DF_INLINE	Point_3D<F> & Point_3D<F>::operator/= (Vector_2D<F> const & v) { x /= v.x; y /= v.y; return *this; }

template <typename F>
H3DF_INLINE	Point_3D<F> const	Point_3D<F>::operator* (Vector_2D<F> const & v) const { return Point_3D<F>(x * v.x, y * v.y, z); }
template <typename F>
H3DF_INLINE	Point_3D<F> const	Point_3D<F>::operator/ (Vector_2D<F> const & v) const { return Point_3D<F>(x / v.x, y / v.y, z); }


template <typename F>
H3DF_INLINE	Point_2D<F> & Point_2D<F>::operator+= (Vector_2D<F> const & v) { x += v.x; y += v.y; return *this; }
template <typename F>
H3DF_INLINE	Point_2D<F> & Point_2D<F>::operator-= (Vector_2D<F> const & v) { x -= v.x; y -= v.y; return *this; }

template <typename F>
H3DF_INLINE	Vector_2D<F> const	Point_2D<F>::operator- (Point_2D<F> const & p) const { return Vector_2D<F>(x - p.x, y - p.y); }

template <typename F>
H3DF_INLINE	Point_2D<F> const	Point_2D<F>::operator+ (Vector_2D<F> const & v) const { return Point_2D<F>(x + v.x, y + v.y); }
template <typename F>
H3DF_INLINE	Point_2D<F> const	Point_2D<F>::operator- (Vector_2D<F> const & v) const { return Point_2D<F>(x - v.x, y - v.y); }

template <typename F>
H3DF_INLINE	Point_2D<F> & Point_2D<F>::operator*= (Vector_2D<F> const & v) { x *= v.x; y *= v.y; return *this; }
template <typename F>
H3DF_INLINE	Point_2D<F> & Point_2D<F>::operator/= (Vector_2D<F> const & v) { x /= v.x; y /= v.y; return *this; }
template <typename F>
H3DF_INLINE	Point_2D<F> const	Point_2D<F>::operator* (Vector_2D<F> const & v) const { return Point_2D<F>(x * v.x, y * v.y); }
template <typename F>
H3DF_INLINE	Point_2D<F> const	Point_2D<F>::operator/ (Vector_2D<F> const & v) const { return Point_2D<F>(x / v.x, y / v.y); }

template <typename F>
void Vector_3D<F>::Rotate(Point_3D<F> cOrigin, Vector_3D<F> cXAxis, Vector_3D<F> cYAxis, double Angle) const
{
	Point_2D<F> cPoint = this->DropPoint(cOrigin, cXAxis, cYAxis);
	Point_2D<F> cOrigin = cOrigin.DropPoint(cOrigin, cXAxis, cYAxis);

	cPoint.Rotate(Angle, cOrigin);

	*this = cPoint.LiftPoint(cOrigin, cXAxis, cYAxis);
}

template <typename F>
class TEMPLATE_API_H3DF Vector_2D {
public:
	F	x;
	F	y;

	Vector_2D() {}
	Vector_2D(F v1, F v2) : x(v1), y(v2) {}
	template <typename D>
	explicit Vector_2D(Vector_2D<D> const & that) : x((F)that.x), y((F)that.y) {}

	explicit Vector_2D(Vector_3D<F> const & that) : x(that.x), y(that.y) {}
	explicit Vector_2D(Point_2D<F> const & p) : x(p.x), y(p.y) {}
	//explicit Vector_2D(Plane_2D<F> const & p);

	Vector_2D const	operator- () const { return Vector_2D(-x, -y); }

	bool operator== (Vector_2D const & v) const {
		return  Float::match(x, v.x) && Float::match(y, v.y);
	}
	bool operator!= (Vector_2D const & v) const { return  !(*this == v); }

	bool Equals(Vector_2D const & v, int in_tolerance = 32) const {
		return	Float::Equals(x, v.x, in_tolerance) && Float::Equals(y, v.y, in_tolerance);
	}

	Vector_2D & operator+= (Vector_2D const & v) { x += v.x; y += v.y; return *this; }
	Vector_2D & operator-= (Vector_2D const & v) { x -= v.x; y -= v.y; return *this; }
	Vector_2D const		operator+ (Vector_2D const & v) const { return Vector_2D(x + v.x, y + v.y); }
	Vector_2D const		operator- (Vector_2D const & v) const { return Vector_2D(x - v.x, y - v.y); }

	Vector_2D & operator*= (F s) { x *= s; y *= s; return *this; }
	Vector_2D & operator/= (F s) { return operator*= (1.0f / s); }
	Vector_2D const		operator* (F s) const { return Vector_2D(x * s, y * s); }
	Vector_2D const		operator/ (F s) const { return operator* (1.0f / s); }

	F & operator[] (size_t i) { return (&x)[i]; }
	F const & operator[] (size_t i) const { return (&x)[i]; }

	H3DF_INLINE double	Length() const { return sqrt(LengthSquared()); }

	H3DF_INLINE double	LengthSquared() const { return (double)x * (double)x + (double)y * (double)y; }

	H3DF_INLINE Vector_2D & Normalize(bool check_range = false, F epsilon = Float_Traits<F>::Epsilon()) {// not const &; allow V.normalize() *= S;
		if (check_range) {
			F	range = Max(Abs(x), Abs(y));
			if (range > F(1.0e10))
				operator/= (range);
		}

		F	len = (F)Length();
		if (len > epsilon)
			operator/= (len);
		else
			*this = Zero();
		return *this;
	}
	H3DF_INLINE Vector_2D & Normalize(F epsilon) { return Normalize(false, epsilon); }

	H3DF_INLINE F		Magnitude() const { return Max(Abs(x), Abs(y)); }
	H3DF_INLINE F		Manhattan() const { return Abs(x) + Abs(y); }

	H3DF_INLINE F AngleWith(Vector_2D const & v) const {
		F dot = Dot(v);
		F len = Length() * v.Length();
		F cos_angle = dot / len;
		return acos(cos_angle) * (F)180 / PI;
	}

	H3DF_INLINE F CCWAngleWith(Vector_2D const & v) const {
		F dot = this->Dot(v);
		F len = this->Length() * v.Length();
		F cos_angle = dot / len;

		//Vector_3D<F> cross = v1.Cross(*this);
		Vector_3D<F> cross = Vector_3D<F>::ZAxis().Cross(Vector_3D<F>(*this));
		F dot2 = cross.Dot(Vector_3D<F>(v));
		F angle = acos(cos_angle) * (F)180 / PI;

		if (dot2 < 0) {
			angle = 360.0 - angle;
		}

		return angle;
	}

	H3DF_INLINE F Dot(Vector_2D const & v) const { return x * v.x + y * v.y; }
	H3DF_INLINE F Cross(Vector_2D const & v) const { return x * v.y - y * v.x; }

	H3DF_INLINE Vector_2D Scale(Vector_2D const & v) const { return Vector_2D(x * v.x, y * v.y); }

	static H3DF_INLINE Vector_2D	XAxis() { return Vector_2D(1, 0); };
	static H3DF_INLINE Vector_2D	YAxis() { return Vector_2D(0, 1); };

	static H3DF_INLINE Vector_2D	Zero() { return Vector_2D(0, 0); };
	static H3DF_INLINE Vector_2D	Unit() { return Vector_2D(1, 1); };
};

template <typename F, typename S>
H3DF_INLINE	Vector_2D<F>	operator* (S s, Vector_2D<F> const & v) { return Vector_2D<F>(F(s * v.x), F(s * v.y)); }

template <typename F>
H3DF_INLINE bool Is_Abnormal(Vector_2D<F> const & v) {
	return Is_Abnormal(v.x) || Is_Abnormal(v.y);
}

template <typename F>
class TEMPLATE_API_H3DF Plane_3D {
public:
	F	a;
	F	b;
	F	c;
	F	d;

	Plane_3D() {}
	Plane_3D(F v1, F v2, F v3, F v4) : a(v1), b(v2), c(v3), d(v4) {}
	Plane_3D(Vector_3D<F> const & v, F f = 0) : a(v.x), b(v.y), c(v.z), d(f) {}
	Plane_3D(Vector_3D<F> const & v, Point_3D<F> const & p) : a(v.x), b(v.y), c(v.z), d(-(p.x * v.x + p.y * v.y + p.z * v.z)) {}
	Plane_3D(Point_3D<F> const & p, Vector_3D<F> const & v) : a(v.x), b(v.y), c(v.z), d(-(p.x * v.x + p.y * v.y + p.z * v.z)) {}
	template <typename D>
	explicit Plane_3D(Plane_3D<D> const & that) : a((F)that.a), b((F)that.b), c((F)that.c), d((F)that.d) {}

	Plane_3D(size_t count, Point_3D<F> const * points) {
		if (count >= 3) {
			// The 3 coefficients A, B, and C are proportional to the areas of the
			// projections of the polygon onto the yz, zx, and xy planes, respectively.

			// run around the polygon, collecting trapezoidal areas
			// a "center" point is also collected, to make the plane 'd' slightly more "valid" when the polygon is non-planar.

			// take care of the swing point first
			Point_3D<F> const * p0 = &points[count - 1];

			Point_3D<double>	ctr = Point_3D<double>::Origin();
			Vector_3D<F>		normal = Vector_3D<F>::Zero();

			for (size_t i = 0; i < count; ++i) {
				Point_3D<F> const * p1 = &points[i];

				normal.x += (p0->y + p1->y) * (p1->z - p0->z);
				normal.y += (p0->z + p1->z) * (p1->x - p0->x);
				normal.z += (p0->x + p1->x) * (p1->y - p0->y);

				ctr += Vector_3D<double>(Vector_3D<F>(points[i]));

				p0 = p1;
			}

			// ("should" always be != 0)
			if (normal.Normalize() != Vector_3D<F>::Zero()) {
				/* finish finding the average */
				double	inv_count = 1.0 / (double)count;
				ctr *= inv_count;

				*this = Plane_3D(normal, Point_3D<F>(ctr));
				return;
			}
		}

		*this = Plane_3D::Zero();
	}


	Plane_3D const		operator- () const { return Plane_3D(-a, -b, -c, -d); }

	bool				operator== (Plane_3D const & p) const { return  a == p.a && b == p.b && c == p.c && d == p.d; }
	bool				operator!= (Plane_3D const & p) const { return  !(*this == p); }

	F & operator[] (size_t i) { return (&a)[i]; }
	F const & operator[] (size_t i) const { return (&a)[i]; }

	H3DF_INLINE bool	Equals(Plane_3D const & p, int in_tolerance = 32) const {
		return  Float::Equals(a, p.a, in_tolerance) && Float::Equals(b, p.b, in_tolerance) &&
			Float::Equals(c, p.c, in_tolerance) && Float::Equals(d, p.d, in_tolerance);
	}

	Plane_3D & Normalize(F epsilon = Float_Traits<F>::Epsilon()) {	// not const &; allow V.normalize() *= S;
		F len = (F)Vector_3D<F>(*this).Length();
		if (len > epsilon)
			operator/= (len);
		else
			*this = Zero();
		return *this;
	}

	bool IntersectLineSegment(Point_3D<F> const & p1, Point_3D<F> const & p2, Point_3D<F> & cIntersectPoint, float eps = 1e-5f) const {
//	Point_3D<F> IntersectLineSegment(Point_3D<F> const & p1, Point_3D<F> const & p2, float eps = 1e-5f) const {
		F val1 = Abs(a * p1.x + b * p1.y + c * p1.z + d);
		F val2 = Abs(a * p2.x + b * p2.y + c * p2.z + d);

		if (val1 >= eps) {
			cIntersectPoint = Point_3D<F>(((val1 * p2.x) + (val2 * p1.x)) / (val1 + val2),
				((val1 * p2.y) + (val2 * p1.y)) / (val1 + val2),
				((val1 * p2.z) + (val2 * p1.z)) / (val1 + val2));
			return true;
		}

		return false;
	}

	Point_3D<F> IntersectLineSegment2(Point_3D<F> const & p1, Point_3D<F> const & p2) const {
		F		 u = (a * p1.x + b * p1.y + c * p1.z + d) /
			(a * (p1.x - p2.x) + b * (p1.y - p2.y) + c * (p1.z - p2.z));

		return Point_3D<F>(p1.x + u * (p2.x - p1.x), p1.y + u * (p2.y - p1.y), p1.z + u * (p2.z - p1.z));
	}


	bool parallel(Plane_3D const & p) const {
		return  equivalent(a, p.a) &&
			equivalent(b, p.b) &&
			equivalent(c, p.c);
	}

	bool equivalent(Plane_3D const & p) const {
		return  parallel(p) &&
			equivalent(d, p.d, (F)1.0e-6);
	}


	static H3DF_INLINE Plane_3D Zero() { return Plane_3D(0.0f, 0.0f, 0.0f, 0.0f); };


private:
	Plane_3D & operator*= (F s) { a *= s; b *= s; c *= s; d *= s; return *this; }
	Plane_3D & operator/= (F s) { return operator*= ((F)1.0 / s); }
	Plane_3D const	operator* (F s) const { return Plane_3D(a * s, b * s, c * s, d * s); }
	Plane_3D const	operator/ (F s) const { return operator* ((F)1.0 / s); }

	static bool equivalent(float a, float b) {					// for vector components
		if (Abs(a) < 1.0e-4f && Abs(b) < 1.0e-4f)
			return true;
		return Float::Equals(a, b);
	}

	static bool equivalent(float a, float b, float cutoff) {	// for distance
		if (Abs(a) < cutoff && Abs(b) < cutoff)
			return a == b;
		return Float::Equals(a, b);
	}
};

template <typename F>
H3DF_INLINE bool Is_Abnormal(Plane_3D<F> const & p) {
	return Is_Abnormal(p.a) || Is_Abnormal(p.b) || Is_Abnormal(p.c) || Is_Abnormal(p.d);
}


template <typename F>
H3DF_INLINE	F operator* (Plane_3D<F> const & plane, Point_3D<F> const & point) {
	return plane.a * point.x + plane.b * point.y + plane.c * point.z + plane.d;
}
template <typename F>
H3DF_INLINE	F operator* (Point_3D<F> const & point, Plane_3D<F> const & plane) {
	return plane * point;
}

template <typename F>
H3DF_INLINE Plane_3D<F> Interpolate(Plane_3D<F> const & a, Plane_3D<F> const & b, float t) {
	return Plane_3D<F>(a.a + (b.a - a.a) * t, a.b + (b.b - a.b) * t, a.c + (b.c - a.c) * t, a.d + (b.d - a.d) * t);
}

template <typename F>
Vector_3D<F>::Vector_3D(Plane_3D<F> const & p) : x(p.a), y(p.b), z(p.c) {}

//== Cuboid_3D Class ===============================================================================

template<typename F>
struct Sphere_3D;

template <typename F>
struct Cuboid_3D {
	Point_3D<F>		cMin;
	Point_3D<F>		cMax;

	// Creates an invalid cuboid.
	Cuboid_3D() : cMin(Limit_Point()), cMax(-Limit_Point()) {}


	// Creates a cuboid equal to another cuboid.
	template <typename D>
	explicit Cuboid_3D(Cuboid_3D<D> const & that) : cMin(Point_3D<F>(that.cMin)), cMax(Point_3D<F>(that.cMax)) {}

	// Creates a cuboid that will fit tightly around a sphere.
	Cuboid_3D(Sphere_3D<F> const & that);

	// Creates a cuboid based on two points, which become opposite corners of the cuboid.
	Cuboid_3D(Point_3D<F> const & in_min, Point_3D<F> const & in_max) : cMin(in_min), cMax(in_max) {}

	// Creates a cuboid based on an array of points. Only the minimum and maximum points are used, which become opposite corners of the cuboid. If count is 0, then an invalid cuboid is returned.
	Cuboid_3D(size_t count, Point_3D<F> const * points) {
		if (count == 0) {
			cMin = Limit_Point();
			cMax = -Limit_Point();
			return;
		}
		cMin = cMax = *points++;
		if (--count > 0)
			Merge(count, points);
	}

	// Creates a cuboid based on an array of points. Only the minimum and maximum points are used, which become opposite corners of the cuboid. If count is 0, then an invalid cuboid is returned.
	template <typename T>
	Cuboid_3D(size_t count, T const * indices, Point_3D<F> const * points) {
		if (count == 0) {
			cMin = Limit_Point();
			cMax = -Limit_Point();
			return;
		}
		cMin = cMax = points[*indices++];
		if (--count > 0) {
			Merge(count, indices, points);
		}
	}

	// Creates a cuboid with the same dimensions as a rectangle.
	//Cuboid_3D(Rectangle const & that) : cMin(Point_3D<F>(that.left, that.bottom, 0)), cMax(Point_3D<F>(that.right, that.top, 0)) {}

	// Tests whether this cuboid is valid. The cuboid is considered valid if the minimum point is less than or equal to the maximum point.
	H3DF_INLINE bool		IsValid() const {
		return cMin.x <= cMax.x && cMin.y <= cMax.y && cMin.z <= cMax.z;
	}

	// Creates an invalid cuboid.
	static H3DF_INLINE Cuboid_3D Invalid() { return Cuboid_3D(); };

	// Invalidates this cuboid.
	void Invalidate() { cMin = Limit_Point(); cMax = -Limit_Point(); }

	// Determines if the maximum and minimum points of this cuboid are equal to the maximum and minimum points of another cuboid.
	H3DF_INLINE bool		operator== (Cuboid_3D const & cuboid) const { return  (cMin == cuboid.cMin && cMax == cuboid.cMax); }

	// Determines if the maximum and minimum points of this cuboid are not equal to the maximum and minimum points of another cuboid.
	H3DF_INLINE bool		operator!= (Cuboid_3D const & cuboid) const { return  !(*this == cuboid); }

	bool Equals(Cuboid_3D const & p, int in_tolerance = 32) const {
		if (false == cMin.Equals(p.cMin, in_tolerance)) {
			return false;
		}

		if (false == cMax.Equals(p.cMax, in_tolerance)) {
			return false;
		}

		return true;
	}

	// Assigns the eight corners of the cuboid based on the points in the parameter array. The array must contain at least eight points.
	H3DF_INLINE void Generate_Cuboid_Points(Point_3D<F> * points) const {
		points[0] = Point_3D<F>(cMin.x, cMin.y, cMin.z);
		points[1] = Point_3D<F>(cMin.x, cMin.y, cMax.z);
		points[2] = Point_3D<F>(cMin.x, cMax.y, cMin.z);
		points[3] = Point_3D<F>(cMin.x, cMax.y, cMax.z);
		points[4] = Point_3D<F>(cMax.x, cMin.y, cMin.z);
		points[5] = Point_3D<F>(cMax.x, cMin.y, cMax.z);
		points[6] = Point_3D<F>(cMax.x, cMax.y, cMin.z);
		points[7] = Point_3D<F>(cMax.x, cMax.y, cMax.z);
	}

	// Returns the diagonal vector of the cuboid.
	H3DF_INLINE Vector_3D<F> Diagonal() const { return cMax - cMin; }

	// Returns the volume of this cuboid.
	H3DF_INLINE F Volume() const { return (cMax.x - cMin.x) * (cMax.y - cMin.y) * (cMax.z - cMin.z); }

	// Determines whether this cuboid intersects with another cuboid.
	H3DF_INLINE bool Intersecting(Cuboid_3D const & cuboid) const {
		return	cMax.x >= cuboid.cMin.x && cMin.x <= cuboid.cMax.x &&
			cMax.y >= cuboid.cMin.y && cMin.y <= cuboid.cMax.y &&
			cMax.z >= cuboid.cMin.z && cMin.z <= cuboid.cMax.z;
	}

	// Determines whether this cuboid intersects with another cuboid. Allows for a tolerance value to be considered.
	H3DF_INLINE bool Intersecting(Cuboid_3D const & cuboid, F const allowance) const {
		return	cMax.x + allowance >= cuboid.cMin.x && cMin.x - allowance <= cuboid.cMax.x &&
			cMax.y + allowance >= cuboid.cMin.y && cMin.y - allowance <= cuboid.cMax.y &&
			cMax.z + allowance >= cuboid.cMin.z && cMin.z - allowance <= cuboid.cMax.z;
	}

	// Retuns true if this cuboid intersects with the parameter cuboid along a certain axis.
	H3DF_INLINE bool Intersecting(int dimension, Cuboid_3D const & cuboid) const {
		//ASSERT (0 <= dimension && dimension <= 2);
		return	cMax[dimension] >= cuboid.cMin[dimension] && cMin[dimension] <= cuboid.cMax[dimension];
	}

	// Returns true if this cuboid intersects with the parameter cuboid along a certain axis, and considers a tolerance value.
	H3DF_INLINE bool Intersecting(int dimension, Cuboid_3D const & cuboid, F const allowance) const {
		//ASSERT (0 <= dimension && dimension <= 2);
		return	cMax[dimension] + allowance >= cuboid.cMin[dimension] && cMin[dimension] - allowance <= cuboid.cMax[dimension];
	}

	// Determines whether this cuboid intersects with a ray.
	H3DF_INLINE bool Intersecting(Point_3D<F> const & start, Vector_3D<F> const & direction) const {
		return LineIntersecting(start, direction, true);
	}

	// Determines whether this cuboid intersects with an infinite line.
	H3DF_INLINE bool Intersecting(Point_3D<F> const & point1, Point_3D<F> const & point2) const {
		Vector_3D<F> const direction = point2 - point1;
		return LineIntersecting(point1, direction, false);
	}

	// Merges this cuboid with another cuboid. Only the smallest and largest values of the cuboids are retained.
	H3DF_INLINE void Merge(Cuboid_3D const & cuboid) {
		Float::replace_if_smaller(cMin.x, cuboid.cMin.x);
		Float::replace_if_smaller(cMin.y, cuboid.cMin.y);
		Float::replace_if_smaller(cMin.z, cuboid.cMin.z);
		Float::replace_if_larger(cMax.x, cuboid.cMax.x);
		Float::replace_if_larger(cMax.y, cuboid.cMax.y);
		Float::replace_if_larger(cMax.z, cuboid.cMax.z);
	}

	// Merges this cuboid with another cuboid created from a sphere. Only the smallest and largest values of the cuboids are retained.
	//TDF_INLINE void Merge(Sphere_3D<F> const & sphere) { Merge(Cuboid_3D(sphere)); }

	// Expands the cuboid to include a point.
	H3DF_INLINE void Merge(Point_3D<F> const & point) {
		Float::replace_if_smaller(cMin.x, point.x);
		Float::replace_if_smaller(cMin.y, point.y);
		Float::replace_if_smaller(cMin.z, point.z);
		Float::replace_if_larger(cMax.x, point.x);
		Float::replace_if_larger(cMax.y, point.y);
		Float::replace_if_larger(cMax.z, point.z);
	}

	// Expands this cuboid to include a set of points.
	void Merge(size_t count, Point_3D<F> const * points) {
		while (count > 1) {
			merge2(points[0], points[1]);
			points += 2;
			count -= 2;
		}

		if (count > 0)
			Merge(*points);
	}

	// Expands this cuboid to include a set of indexed points.
	template <typename T>
	void Merge(size_t count, T const * indices, Point_3D<F> const * points) {
		while (count > 1) {
			T		i1 = *indices++;
			T		i2 = *indices++;
			merge2(points[i1], points[i2]);
			count -= 2;
		}

		if (count > 0)
			Merge(points[*indices]);
	}

	// Returns true if the cuboid completely contains the parameter cuboid.
	H3DF_INLINE bool Contains(Cuboid_3D const & contained) const {
		return (contained.cMin.x >= cMin.x &&
			contained.cMin.y >= cMin.y &&
			contained.cMin.z >= cMin.z &&
			contained.cMax.x <= cMax.x &&
			contained.cMax.y <= cMax.y &&
			contained.cMax.z <= cMax.z);
	}

	// Returns true if the cuboid completely contains a cuboid based on the  parameter points.
	H3DF_INLINE bool Contains(Point_3D<F> const & contained) const {
		return (contained.x >= cMin.x &&
			contained.y >= cMin.y &&
			contained.z >= cMin.z &&
			contained.x <= cMax.x &&
			contained.y <= cMax.y &&
			contained.z <= cMax.z);
	}

	// Returns true if the cuboid completely contains a cuboid based on the  parameter points, with a tolerance value.
	H3DF_INLINE bool Contains(Point_3D<F> const & contained, F epsilon) const {
		return (contained.x >= cMin.x - epsilon &&
			contained.y >= cMin.y - epsilon &&
			contained.z >= cMin.z - epsilon &&
			contained.x <= cMax.x + epsilon &&
			contained.y <= cMax.y + epsilon &&
			contained.z <= cMax.z + epsilon);
	}

	// Replaces this cuboid with the intersection of this cuboid and the parameter cuboid.
	H3DF_INLINE Cuboid_3D & Intersect(Cuboid_3D const & cuboid) {
		Float::replace_if_larger(cMin.x, cuboid.cMin.x);
		Float::replace_if_larger(cMin.y, cuboid.cMin.y);
		Float::replace_if_larger(cMin.z, cuboid.cMin.z);
		Float::replace_if_smaller(cMax.x, cuboid.cMax.x);
		Float::replace_if_smaller(cMax.y, cuboid.cMax.y);
		Float::replace_if_smaller(cMax.z, cuboid.cMax.z);
		return *this;
	}

	// Replaces this cuboid with the union of this cuboid and the parameter cuboid.
	H3DF_INLINE Cuboid_3D & Union(Cuboid_3D const & cuboid) {
		Float::replace_if_smaller(cMin.x, cuboid.cMin.x);
		Float::replace_if_smaller(cMin.y, cuboid.cMin.y);
		Float::replace_if_smaller(cMin.z, cuboid.cMin.z);
		Float::replace_if_larger(cMax.x, cuboid.cMax.x);
		Float::replace_if_larger(cMax.y, cuboid.cMax.y);
		Float::replace_if_larger(cMax.z, cuboid.cMax.z);
		return *this;
	}

	// Expands both corners of this cuboid.
	H3DF_INLINE Cuboid_3D & Expand(F border) {
		Vector_3D<F>		delta(border, border, border);
		cMin -= delta;
		cMax += delta;
		return *this;
	}

	// Contracts both corners of this cuboid.
	H3DF_INLINE Cuboid_3D & Contract(F border) {
		Vector_3D<F>		delta(border, border, border);
		cMin += delta;
		cMax -= delta;
		return *this;
	}

private:
	void merge2(Point_3D<F> const & p1, Point_3D<F> const & p2) {
		if (p1.x > p2.x) {
			Float::replace_if_smaller(cMin.x, p2.x);
			Float::replace_if_larger(cMax.x, p1.x);
		}
		else {
			Float::replace_if_smaller(cMin.x, p1.x);
			Float::replace_if_larger(cMax.x, p2.x);
		}

		if (p1.y > p2.y) {
			Float::replace_if_smaller(cMin.y, p2.y);
			Float::replace_if_larger(cMax.y, p1.y);
		}
		else {
			Float::replace_if_smaller(cMin.y, p1.y);
			Float::replace_if_larger(cMax.y, p2.y);
		}

		if (p1.z > p2.z) {
			Float::replace_if_smaller(cMin.z, p2.z);
			Float::replace_if_larger(cMax.z, p1.z);
		}
		else {
			Float::replace_if_smaller(cMin.z, p1.z);
			Float::replace_if_larger(cMax.z, p2.z);
		}
	}

	H3DF_INLINE static Point_3D<F> Limit_Point() {
		F const	x = (std::numeric_limits<F>::max)();
		return Point_3D<F>(x, x, x);
	}

	bool LineIntersecting(Point_3D<F> const & cInStart, Vector_3D<F> const & cInDirection, bool bIsRay) const {
		// convert the line segment/ray to a line

		Point_3D<F> cStart, cEnd;

		if (true == bIsRay) {
			cStart = cInStart + cInDirection * (std::numeric_limits<F>::max)();
			cEnd = cInStart - cInDirection * (std::numeric_limits<F>::max)();
		}
		else  {
			cStart = cInStart;
			cEnd = cInStart + cInDirection;
		}

		// check if the line intersects any of the six faces of the cuboid
		const Plane_3D<F> planes[6] = {
			Plane_3D<F>(Point_3D<F>(cMin.x, cMin.y, cMin.z), Vector_3D<F>(1, 0, 0)), // x=cMin.x
			Plane_3D<F>(Point_3D<F>(cMin.x, cMin.y, cMin.z), Vector_3D<F>(0, 1, 0)), // y=cMin.y
			Plane_3D<F>(Point_3D<F>(cMin.x, cMin.y, cMin.z), Vector_3D<F>(0, 0, 1)), // z=cMin.z
			Plane_3D<F>(Point_3D<F>(cMax.x, cMax.y, cMax.z), Vector_3D<F>(-1, 0, 0)), // x=cMax.x
			Plane_3D<F>(Point_3D<F>(cMax.x, cMax.y, cMax.z), Vector_3D<F>(0, -1, 0)), // y=cMax.y
			Plane_3D<F>(Point_3D<F>(cMax.x, cMax.y, cMax.z), Vector_3D<F>(0, 0, -1)), // z=cMax.z
		};
		for (const auto & plane : planes) {
			if (true == plane.IntersectLineSegment(cStart, cEnd)) {
				return true;
			}
		}

		return false;
	};
};

template <typename F>
H3DF_INLINE Cuboid_3D<F> Intersect(Cuboid_3D<F> const & a, Cuboid_3D<F> const & b) {
	Cuboid_3D<F> temp = a;
	return temp.Intersect(b);
}

template <typename F>
H3DF_INLINE Cuboid_3D<F> Union(Cuboid_3D<F> const & a, Cuboid_3D<F> const & b) {
	Cuboid_3D<F> temp = a;
	return temp.Union(b);
}

template <typename F>
H3DF_INLINE Cuboid_3D<F> Expand(Cuboid_3D<F> const & a, F border) {
	Cuboid_3D<F> temp = a;
	return temp.Expand(border);
}

template <typename F>
H3DF_INLINE Cuboid_3D<F> Contract(Cuboid_3D<F> const & a, F border) {
	Cuboid_3D<F> temp = a;
	return temp.Contract(border);
}

template<typename F>
struct Sphere_3D {
	Point_3D<F> center;
	F radius;

	Sphere_3D() : center(Point_3D<F>(0, 0, 0)), radius(-1) {}

	template<typename D>
	explicit Sphere_3D(Sphere_3D<D> const & that) : center(Point_3D<F>(that.center)), radius(F(that.radius))
	{
	}

	Sphere_3D(Cuboid_3D<F> const & cuboid)
	{
		if (cuboid.cMax.x < cuboid.cMin.x || cuboid.cMax.y < cuboid.cMin.y || cuboid.cMax.z < cuboid.cMin.z)
			*this = Invalid();
		else {
			center = Midpoint(cuboid.cMin, cuboid.cMax);
			radius = F(0.5 * cuboid.Diagonal().Length());
		}
	}

	Sphere_3D(Point_3D<F> const & starting_center, F in_radius = 0) : center(starting_center), radius(in_radius) {}

	Sphere_3D(size_t count, Point_3D<F> const * points) : radius(0.0f)
	{
		Cuboid_3D<F> cuboid(count, points);
		center = Midpoint(cuboid.cMin, cuboid.cMax);
		Engulf(count, points);
	}

	template<typename T>
	Sphere_3D(size_t count, T const * indices, Point_3D<F> const * points) : radius(0.0f)
	{
		Cuboid_3D<F> cuboid(count, indices, points);
		center = Midpoint(cuboid.cMin, cuboid.cMax);
		Engulf(count, indices, points);
	}

	Sphere_3D(size_t count, Point_3D<F> const * points, Point_3D<F> const & starting_center) : center(starting_center), radius(0)
	{
		Engulf(count, points);
	}

	template<typename T>
	Sphere_3D(size_t count, T const * indices, Point_3D<F> const * points, Point_3D<F> const & starting_center) :
		center(starting_center), radius(0)
	{
		Engulf(count, indices, points);
	}

	H3DF_INLINE bool IsValid() const { return radius >= 0; }

	static H3DF_INLINE Sphere_3D Invalid() { return Sphere_3D(Point_3D<F>(0, 0, 0), -1); };

	void Invalidate() { radius = -1; }

	H3DF_INLINE bool operator==(Sphere_3D const & sphere) const { return (center == sphere.center && radius == sphere.radius); }

	H3DF_INLINE bool operator!=(Sphere_3D const & sphere) const { return !(*this == sphere); }

	H3DF_INLINE F Volume() const { return F((4.0 / 3.0 * PI) * radius * radius * radius); }

	H3DF_INLINE void Merge(Point_3D<F> const & point)
	{
		Vector_3D<F> dir = point - center;
		F distance = (F)dir.Length();

		if (distance > radius) {
			F t = F(0.5) * (distance - radius);
			center += t * dir.Normalize();
			radius += t;
		}
	}

	H3DF_INLINE void Merge(size_t count, Point_3D<F> const * points)
	{
		F radius_squared = radius * radius;
		for (size_t i = 0; i < count; ++i) {
			Vector_3D<F> dir = *points - center;
			F distance_squared = (F)dir.LengthSquared();

			if (distance_squared > radius_squared) {
				F distance = sqrt(distance_squared);
				F t = F(0.5) * (distance - radius);
				center += t * (distance > Float_Traits<F>::Epsilon() ? dir / distance : Vector_3D<F>::Zero());
				radius += t;
				radius_squared = radius * radius;
			}

			++points;
		}
	}

	H3DF_INLINE void Merge(Sphere_3D const & sphere)
	{
		Vector_3D<F> dir = sphere.center - center;
		F distance = (F)dir.Length();

		if (distance + sphere.radius > radius) {
			if (distance + radius > sphere.radius) {
				F t = F(0.5 * (sphere.radius + distance - radius));
				center += t * dir.Normalize();
				radius += t;
			}
			else {
				center = sphere.center;
				radius = sphere.radius;
			}
		}
	}

	H3DF_INLINE void Merge(Cuboid_3D<F> const & cuboid) { Merge(Sphere_3D(cuboid)); }

private:
	// Engulf expands the sphere to include the points, but does not change the center as Merge does
	H3DF_INLINE void Engulf(size_t count, Point_3D<F> const * points)
	{
		double rsq = radius * radius;
		for (size_t i = 0; i < count; ++i) {
			double dsq = (*points++ - center).LengthSquared();
			if (dsq > rsq)
				rsq = dsq;
		}
		radius = (F)sqrt(rsq);
	}

	template<typename T>
	H3DF_INLINE void Engulf(size_t count, T const * indices, Point_3D<F> const * points)
	{
		double rsq = radius * radius;
		for (size_t i = 0; i < count; ++i) {
			double dsq = (points[*indices++] - center).LengthSquared();
			if (dsq > rsq)
				rsq = dsq;
		}
		radius = (F)sqrt(rsq);
	}
};

template<typename F>
H3DF_INLINE Cuboid_3D<F>::Cuboid_3D(Sphere_3D<F> const & sphere)
{
	if (sphere.radius < 0) {
		*this = Invalid();
	}
	else {
		cMin = Point_3D<F>(sphere.center.x - sphere.radius, sphere.center.y - sphere.radius, sphere.center.z - sphere.radius);
		cMax = Point_3D<F>(sphere.center.x + sphere.radius, sphere.center.y + sphere.radius, sphere.center.z + sphere.radius);
	}
}

using Point = Point_3D<float>;
using DPoint = Point_3D<double>;

using Point2D = Point_2D<float>;
using DPoint2D = Point_2D<double>;

using Vector = Vector_3D<float>;
using DVector = Vector_3D<double>;

using Vector2D = Vector_2D<float>;
using DVector2D = Vector_2D<double>;

using SimpleSphere = Sphere_3D<float>;
using DSimpleSphere = Sphere_3D<double>;

using Plane = Plane_3D<float>;
using DPlane = Plane_3D<double>;

using SimpleCuboid = Cuboid_3D<float>;
using DSimpleCuboid = Cuboid_3D<double>;

using IntArray = std::vector<int>;
using Int2DArray = std::vector<std::vector<int>>;
using FloatArray = std::vector<float>;
using PointArray = std::vector<H3DF::Point>;
using Point2DArray = std::vector<H3DF::Point2D>;
using DPoint2DArray = std::vector<H3DF::DPoint2D>;
using VectorArray = std::vector<H3DF::Vector>;
using Vector2DArray = std::vector<H3DF::Vector2D>;
using PlaneArray = std::vector<Plane>;

namespace Math
{
	//----- Circle 관련 함수 -----
	bool API_3DF GetCircle(WorldPointArray & cPoints, CircleKit & cCircle);
	bool API_3DF CircleFitByHyper(DPoint2DArray & cPoints, double & dCX, double & dCY, double & dRadius, double & dSigma);

	//----- Line 관련 함수 -----
	bool API_3DF NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint);
	bool API_3DF NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint, double & dPrameter);

	bool API_3DF IntersectionPoint(Point cSP1, Point cEP1, Point cSP2, Point cEP2, Point & cIntersectionPoint);
	bool API_3DF IntersectionPointInRange(Point cSP1, Point cEP1, Point cSP2, Point cEP2, Point & cIntersectionPoint);

	bool API_3DF GetPoint(WindowKey const & cInWindow, WorldPointArray const & aInPoints, PixelPointArray & aOutPoints);
	bool API_3DF GetPoint(WindowKey const & cInWindow, WorldPointArray const & aInPoints, WindowPointArray & aOutPoints);
};

CLOSE_3DF_NAMESPACE

#pragma warning(pop)
