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



#include <atlcoll.h>

#ifdef max
#	undef max
#	include <boost/pool/pool_alloc.hpp>
#	define max(a,b) (((a) > (b)) ? (a) : (b))
#else
#	include <boost/pool/pool_alloc.hpp>
#endif

#ifndef M_PI
#	define M_PI 3.14159265358979323846
#endif

OPEN_3DF_NAMESPACE

template <typename T>	TDF_INLINE	T		Abs(T const & a) { return  a < 0 ? -a : a; }
template <typename T>	TDF_INLINE	int		Compare(T const & a, T const & b) { return a == b ? 0 : a < b ? -1 : 1; }
template <typename T>	TDF_INLINE	int		Sign(T const & a) { return Compare(a, (T) 0); }
template <typename T>	TDF_INLINE	void	Swap(T & a, T & b) { T temp = a; a = b; b = temp; }
template <typename T>	TDF_INLINE	int		Floor(T const & a) { return ((a > 0 || (T) (int) a == a) ? (int) a : ((int) a - 1)); }
template <typename T>	TDF_INLINE	int		Ceiling(T const & a) { return ((a < 0 || (T) (int) a == a) ? (int) a : ((int) a + 1)); }

template <typename T>	TDF_INLINE	T const & Min(T const & a, T const & b) { return  a < b ? a : b; }
template <typename T>	TDF_INLINE	T const & Min(T const & a, T const & b, T const & c) { return  Min(Min(a, b), c); }
template <typename T>	TDF_INLINE	T const & Min(T const & a, T const & b, T const & c, T const & d) { return Min(Min(a, b, c), d); }
template <typename T>	TDF_INLINE	T const & Min(T const & a, T const & b, T const & c, T const & d, T const & e) { return Min(Min(a, b, c, d), e); }
template <typename T>	TDF_INLINE	T const & Min(T const & a, T const & b, T const & c, T const & d, T const & e, T const & f) { return Min(Min(a, b, c, d, e), f); }

template <typename T>	TDF_INLINE	T const & Max(T const & a, T const & b) { return  a > b ? a : b; }
template <typename T>	TDF_INLINE	T const & Max(T const & a, T const & b, T const & c) { return  Max(Max(a, b), c); }
template <typename T>	TDF_INLINE	T const & Max(T const & a, T const & b, T const & c, T const & d) { return Max(Max(a, b, c), d); }
template <typename T>	TDF_INLINE	T const & Max(T const & a, T const & b, T const & c, T const & d, T const & e) { return Max(Max(a, b, c, d), e); }
template <typename T>	TDF_INLINE	T const & Max(T const & a, T const & b, T const & c, T const & d, T const & e, T const & f) { return Max(Max(a, b, c, d, e), f); }

template <typename T>	TDF_INLINE	T const & Clamp(T const & x, T const & min, T const & max) { return x < min ? min : x > max ? max : x; }


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
	static TDF_INLINE bool is_infinite(int32_t const & v) { return (v & 0x7FFFFFFF) == 0x7F800000; }
	static TDF_INLINE bool is_infinite(uint32_t const & v) { return (v & 0x7FFFFFFF) == 0x7F800000; }
	static TDF_INLINE bool is_infinite(int32_t const * v) { return (v[High] & 0x7FFFFFFF) == 0x7FF00000 && v[Low] == 0; }
	static TDF_INLINE bool is_infinite(uint32_t const * v) { return (v[High] & 0x7FFFFFFF) == 0x7FF00000 && v[Low] == 0; }

	static TDF_INLINE bool is_nan(int32_t const & v) {
		uint32_t exp = v & 0x7F800000, mantissa = v & 0x007FFFFF;
		return exp == 0x7F800000 && mantissa != 0;
	}
	static TDF_INLINE bool is_nan(uint32_t const & v) {
		uint32_t exp = v & 0x7F800000, mantissa = v & 0x007FFFFF;
		return exp == 0x7F800000 && mantissa != 0;
	}
	static TDF_INLINE bool is_nan(int32_t const * v) {
		uint32_t exp = v[High] & 0x7FF00000, mantissa_high = v[High] & 0x000FFFFF;
		return exp == 0x7FF00000 && (mantissa_high | v[Low]) != 0;
	}
	static TDF_INLINE bool is_nan(uint32_t const * v) {
		uint32_t exp = v[High] & 0x7FF00000, mantissa_high = v[High] & 0x000FFFFF;
		return exp == 0x7FF00000 && (mantissa_high | v[Low]) != 0;
	}

	static TDF_INLINE bool is_special(int32_t const & v) { return (v & 0x7F800000) == 0x7F800000; }
	static TDF_INLINE bool is_special(uint32_t const & v) { return (v & 0x7F800000) == 0x7F800000; }
	static TDF_INLINE bool is_special(int32_t const * v) { return (v[High] & 0x7FF00000) == 0x7FF00000; }
	static TDF_INLINE bool is_special(uint32_t const * v) { return (v[High] & 0x7FF00000) == 0x7FF00000; }
public:

	/*! The 32-bit float representation of infinity. */
	static const float Infinity;
	/*! The 32-bit float representation of negative infinity. */
	static const float NegativeInfinity;

	/*! See if the value is either infinity */
	static TDF_INLINE bool IsInfinite(float const & a) { return is_infinite(extract_uint32_t(a)); }
	static TDF_INLINE bool IsInfinite(double const & a) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		return is_infinite(v);
	}

	/*! See if the value is Not-A-Number */
	static TDF_INLINE bool IsNAN(float const & a) { return is_nan(extract_uint32_t(a)); }
	static TDF_INLINE bool IsNAN(double const & a) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		return is_nan(v);
	}

	/*! See if the value is not "normal" (infinite or NaN) */
	static TDF_INLINE bool IsAbnormal(float const & a) { return is_special(extract_uint32_t(a)); }
	static TDF_INLINE bool IsAbnormal(double const & a) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		return is_special(v);
	}

	// Checks two floats for equality within a specified tolerance.
	// The tolerance is specified in float increments that scale with the floats themselves.
	static TDF_INLINE bool Equals(float const & a, float const & b, int tolerance = 32);
	static TDF_INLINE bool Equals(double const & a, double const & b, int tolerance = 32);

	template <typename Alloc>
	static TDF_INLINE bool Equals(std::vector<float, Alloc> const & a, std::vector<float, Alloc> const & b, int tolerance = 32)
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

	static TDF_INLINE uint32_t extract_sign_bit(float const & a) {
		return extract_uint32_t(a) & 0x80000000;
	}
	static TDF_INLINE uint32_t extract_sign_bit(double const & a) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		return v[High] & 0x80000000;
	}

	static TDF_INLINE void apply_sign_bit(float & a, uint32_t const & sign_bit) {
		uint32_t v = extract_uint32_t(a);
		v &= 0x7FFFFFFF;
		v |= sign_bit;
		inject_uint32_t(a, v);
	}
	static TDF_INLINE void apply_sign_bit(double & a, uint32_t const & sign_bit) {
		uint32_t v[2];
		memcpy(v, &a, sizeof(double));
		v[High] &= 0x7FFFFFFF;
		v[High] |= sign_bit;
		memcpy(&a, v, sizeof(double));
	}


	static TDF_INLINE unsigned char unit_to_byte(float const & a) {
		uint32_t v = extract_uint32_t(a);

		v &= 0x7FFFFFFF;
		if(v < 0x3B800000)
			return 0;

		v--;

		uint32_t exp = v >> 23;
		uint32_t man = (v & 0x007FFFFF) | 0x00800000;

		return (unsigned char) (man >> (16 + 126 - exp));
	}

	static TDF_INLINE unsigned char unit_to_byte_scaled(float const & a, unsigned char mix) {
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


	static TDF_INLINE bool match(float const & a, float const & b) {
		uint32_t va = extract_uint32_t(a);
		uint32_t vb = extract_uint32_t(b);

		if(((va | vb) & 0x7FFFFFFF) == 0)
			return true;

		return va == vb;
	}
	static TDF_INLINE bool match(double const & a, double const & b) {
		return a == b;
	}


	static TDF_INLINE void replace_if_smaller(float & a, float const & b) {
		if(b < a)
			a = b;
	}
	static TDF_INLINE void replace_if_smaller(double & a, double const & b) {
		if(b < a)
			a = b;
	}

	static TDF_INLINE void replace_if_larger(float & a, float const & b) {
		if(b > a)
			a = b;
	}
	static TDF_INLINE void replace_if_larger(double & a, double const & b) {
		if(b > a)
			a = b;
	}


	static TDF_INLINE uint32_t extract_uint32_t(float const & a) {
		uint32_t i;
		memcpy(&i, &a, sizeof(float));
		return i;
	}

	static TDF_INLINE void inject_uint32_t(float & a, uint32_t const & i) {
		memcpy(&a, &i, sizeof(float));
	}

 	static TDF_INLINE float C2F(unsigned char x) {
 		return (float)x * (1.0f/255.0f);
 	}

	// SSE convenience functions
	static TDF_INLINE void pack_4(float const & f, float * m) {
		memcpy(&m[0], &f, sizeof(float));
		memcpy(&m[1], &f, sizeof(float));
		memcpy(&m[2], &f, sizeof(float));
		memcpy(&m[3], &f, sizeof(float));
	}

	static TDF_INLINE void pack_4(float const & f0, float const & f1, float const & f2, float const & f3, float * m) {
		memcpy(&m[0], &f0, sizeof(float));
		memcpy(&m[1], &f1, sizeof(float));
		memcpy(&m[2], &f2, sizeof(float));
		memcpy(&m[3], &f3, sizeof(float));
	}

	static TDF_INLINE void unpack_4(float * f0, float const * const m) {
		memcpy(f0, m, sizeof(float) * 4);
	}

	static TDF_INLINE void unpack_4(float & f0, float & f1, float & f2, float & f3, float const * const m) {
		memcpy(&f0, &m[0], sizeof(float));
		memcpy(&f1, &m[1], sizeof(float));
		memcpy(&f2, &m[2], sizeof(float));
		memcpy(&f3, &m[3], sizeof(float));
	}

private:

	Float();
};

TDF_INLINE bool Float::Equals(float const & a, float const & b, int tolerance) {
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

TDF_INLINE bool Float::Equals(double const & a, double const & b, int tolerance) {
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

template <typename F> class Vector_3D;

template<class T>
class CBoostPool
{
public:
	void * operator new(size_t _size)
	{
		return m_bpool.malloc();
	}
	void operator delete(void * _p)
	{
		m_bpool.free(_p);
	}
protected:
	static boost::pool<> m_bpool;
};
template<class T>
boost::pool<> CBoostPool<T>::m_bpool(sizeof(T));

/*
class Point3D // : public CBoostPool<Point3D>
{
public:
	float x;
	float y;
	float z;

	void Set(float X, float Y, float Z) { x = X; y = Y; z = Z; };
};

class Vector3D : public CBoostPool<Vector3D>
{
public:
	float x;
	float y;
	float z;

	void Set(float X, float Y, float Z) { x = X; y = Y; z = Z; };
};
*/

template <typename F> class Vector_3D;
template <typename F> class Vector_2D;
template <typename F> class Point_2D;

template <typename F>
class Point_3D
{
public:
	F x;
	F y;
	F z;

	Point_3D() { x = 0, y = 0, z = 0; }
	Point_3D(F v1, F v2, F v3) : x(v1), y(v2), z(v3) {}

	void Set(F X, F Y, F Z) { x = X; y = Y; z = Z; };

	template <typename D>
	Point_3D(Point_3D<D> const & that) : x((F) that.x), y((F) that.y), z((F) that.z) {}
	Point_3D(Vector_3D<F> const & v);
	explicit Point_3D(Vector_2D<F> const & v);
	explicit Point_3D(Point_2D<F> const & that);

	Point_3D const	operator- () const	{ return Point_3D (-x, -y, -z); }

	bool operator== (Point_3D const & p) const { return  x == p.x && y == p.y && z == p.z; }
	bool operator!= (Point_3D const & p) const { return  !(*this == p); }

	bool Equals(Point_3D const & p, int in_tolerance = 32) const {
		return	Float::Equals(x, p.x, in_tolerance) &&
			Float::Equals(y, p.y, in_tolerance) &&
			Float::Equals(z, p.z, in_tolerance);
	}

	Point_3D const operator+(const Point_3D & p) const { return Point_3D(x + p.x, y + p.y, z + p.z); }
	//Point_3D const operator-(const Point_3D & p) const { return Point_3D(x - p.x, y - p.y, z - p.z); }
	Vector_3D<F> const operator - (Point_3D const & p) const;

	template <typename D>
	TDF_INLINE Point_3D const operator + (Vector_3D<D> const & v) const { return Point_3D((F)(x + v.x), (F)(y + v.y), (F)(z + v.z)); }

	template <typename D>
	TDF_INLINE Point_3D const operator - (Vector_3D<D> const & v) const {return Point_3D(x - v.x, y - v.y, z - v.z); }

	Point_3D & operator*= (F s) { x *= s; y *= s; z *= s;  return *this; }
	Point_3D & operator/= (F s) { return operator*= ((F)1 / s); }
	Point_3D const operator* (F s) const { return Point_3D(x * s, y * s, z * s); }
	Point_3D const operator/ (F s) const { return operator* ((F)1 / s); }

	F & operator[] (size_t i) { return (&x)[i]; }
	F const & operator[] (size_t i) const { return (&x)[i]; }

	Point_3D & operator+= (Vector_3D<F> const & v);
	Point_3D & operator-= (Vector_3D<F> const & v);
	Point_3D & operator*= (Vector_3D<F> const & v);
	Point_3D & operator/= (Vector_3D<F> const & v);
	Point_3D const operator* (Vector_3D<F> const & v) const;
	Point_3D const operator/ (Vector_3D<F> const & v) const;


	Point_3D & operator+= (Vector_2D<F> const & v);
	Point_3D & operator-= (Vector_2D<F> const & v);
	Point_3D & operator*= (Vector_2D<F> const & v);
	Point_3D & operator/= (Vector_2D<F> const & v);
	Point_3D const operator* (Vector_2D<F> const & v) const;
	Point_3D const operator/ (Vector_2D<F> const & v) const;

	Point_3D const operator+ (Vector_2D<F> const & v) const;
	Point_3D const operator- (Vector_2D<F> const & v) const;

	static TDF_INLINE Point_3D	Origin() {return Point_3D (0, 0, 0);};
	static TDF_INLINE Point_3D	Zero() {return Point_3D (0, 0, 0);}; //-V524

	double	DistanceWith(Point_3D const & p) const;

	Point_2D<F> DropPoint(Point_3D cOrigin, Vector_3D<F> cXAxis, Vector_3D<F> cYAxis);
	Point_3D ProjectionPoint(Point_3D cOrigin, Vector_3D<F> cNormal);
};

using Point = Point_3D<float>;
using DPoint = Point_3D<double>;

template <typename F, typename S>
TDF_INLINE Point_3D<F>	operator* (S s, Point_3D<F> const & a) { return Point_3D<F>(F(s * a.x), F(s * a.y), F(s * a.z)); }

template <typename F>
TDF_INLINE Point_3D<F> Midpoint(Point_3D<F> const & a, Point_3D<F> const & b) {
	return Point_3D<F>(a.x + b.x, a.y + b.y, a.z + b.z) * 0.5f;
}

template <typename F>
TDF_INLINE Point_3D<F> Midpoint(Point_3D<F> const & a, Point_3D<F> const & b, Point_3D<F> const & c) {
	return Point_3D<F>(a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z) * (F)(1.0 / 3.0);
}

template <typename F>
TDF_INLINE bool Is_Abnormal(Point_3D<F> const & p) {
	return Is_Abnormal(p.x) || Is_Abnormal(p.y) || Is_Abnormal(p.z);
}

template <typename F>
TDF_INLINE	Point_3D<F>::Point_3D(Vector_3D<F> const & v) : x(v.x), y(v.y), z(v.z) {}

template <typename F>
TDF_INLINE	Point_3D<F> & Point_3D<F>::operator+= (Vector_3D<F> const & v) { x += v.x; y += v.y; z += v.z;  return *this; }
template <typename F>
TDF_INLINE	Point_3D<F> & Point_3D<F>::operator-= (Vector_3D<F> const & v) { x -= v.x; y -= v.y; z -= v.z;  return *this; }

template <typename F>
TDF_INLINE	Vector_3D<F> const	Point_3D<F>::operator- (Point_3D<F> const & p) const { return Vector_3D<F>(x - p.x, y - p.y, z - p.z); }

// template <typename F>
// TDF_INLINE	Point_3D<F> const	Point_3D<F>::operator- (Vector_3D<F> const & v) const { return Point_3D<F>(x - v.x, y - v.y, z - v.z); }

template <typename F>
TDF_INLINE	Point_3D<F> & Point_3D<F>::operator*= (Vector_3D<F> const & v) { x *= v.x; y *= v.y; z *= v.z;  return *this; }
template <typename F>
TDF_INLINE	Point_3D<F> & Point_3D<F>::operator/= (Vector_3D<F> const & v) { x /= v.x; y /= v.y; z /= v.z;  return *this; }
template <typename F>
TDF_INLINE	Point_3D<F> const	Point_3D<F>::operator* (Vector_3D<F> const & v) const { return Point_3D<F>(x * v.x, y * v.y, z * v.z); }
template <typename F>
TDF_INLINE	Point_3D<F> const	Point_3D<F>::operator/ (Vector_3D<F> const & v) const { return Point_3D<F>(x / v.x, y / v.y, z / v.z); }

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
// Origin Point를 ZAxis 방향으로 투영한 내적값, 물론 ZAxis는 단위 벡터라야함.
	double dOriginPointProjectionDistance = cNormal.Dot(cOrigin);

	// Target Point를 ZAxis 방향으로 투영한 내적값.
	double dInputPointProjectionDistance = cNormal .Dot(*this);

	// 두 내적값의 차이
	double dParameter = dOriginPointProjectionDistance - dInputPointProjectionDistance;

	return (*this + cNormal * dParameter);
}

template <typename F>
TDF_INLINE Point_3D<F> Interpolate(Point_3D<F> const & a, Point_3D<F> const & b, float t) {
	return a + (b - a) * t;
}

template <typename F>
TDF_INLINE Vector_3D<F> Interpolate(Vector_3D<F> const & a, Vector_3D<F> const & b, float t) {
	return Vector_3D<F>(a + (b - a) * t).Normalize();
}


template <typename F>
TDF_INLINE	double PointToPointDistance(Point_3D<F> const & p1, Point_3D<F> const & p2) {
	return (p2 - p1).Length();
}

template <typename F>
TDF_INLINE	double PointToPointDistanceSquared(Point_3D<F> const & p1, Point_3D<F> const & p2) {
	return (p2 - p1).LengthSquared();
}

template <typename F>
TDF_INLINE Point_3D<F> Circumcenter(Point_3D<F> const & a, Point_3D<F> const & b, Point_3D<F> const & c) {
	F p = static_cast<F>((c - b).LengthSquared());
	F q = static_cast<F>((c - a).LengthSquared());
	F r = static_cast<F>((b - a).LengthSquared());

	return Point_3D<F>((a * (p * (q + r - p)) + (Vector_3D<F>)b * (q * (r + p - q)) + (Vector_3D<F>)c * (r * (p + q - r)))
		/ (2 * (p * q + p * r + q * r) - (p * p + q * q + r * r)));
}

template <typename F>
class Point_2D {
public:
	F	x;
	F	y;

	Point_2D() {}
	Point_2D(F v1, F v2) : x(v1), y(v2) {}

	template <typename D>
	explicit Point_2D(Point_2D<D> const & that) : x((F)that.x), y((F)that.y) {}

	explicit Point_2D(Point_3D<F> const & that) : x((F)that.x), y((F)that.y) {}
	explicit Point_2D(Vector_2D<F> const & v);

	Point_2D const	operator- () const { return Point_2D(-x, -y); }

	bool operator== (Point_2D const & p) const { return  x == p.x && y == p.y; }
	bool operator!= (Point_2D const & p) const { return  !(*this == p); }

	bool Equals(Point_2D const & p, int in_tolerance = 32) const {
		return Float::Equals(x, p.x, in_tolerance) && Float::Equals(y, p.y, in_tolerance);
	}


	Point_2D & operator*= (F s) { x *= s; y *= s; return *this; }
	Point_2D & operator/= (F s) { return operator*= ((F)1 / s); }
	Point_2D const		operator* (F s) const { return Point_2D(x * s, y * s); }
	Point_2D const		operator/ (F s) const { return operator* ((F)1 / s); }

	F & operator[] (size_t i) { return (&x)[i]; }
	F const & operator[] (size_t i) const { return (&x)[i]; }

	Point_2D & operator+= (Vector_2D<F> const & v);
	Point_2D & operator-= (Vector_2D<F> const & v);
	Point_2D & operator*= (Vector_2D<F> const & v);
	Point_2D & operator/= (Vector_2D<F> const & v);
	Point_2D const		operator* (Vector_2D<F> const & v) const;
	Point_2D const		operator/ (Vector_2D<F> const & v) const;

	Vector_2D<F> const	operator- (Point_2D const & p) const;

	Point_2D const		operator+ (Vector_2D<F> const & v) const;
	Point_2D const		operator- (Vector_2D<F> const & v) const;

	static TDF_INLINE Point_2D	Origin() { return Point_2D(0, 0); };
	static TDF_INLINE Point_2D	Zero() { return Point_2D(0, 0); }; //-V524

	Point_3D<F> LiftPoint(Point_3D<F> cOrigin, Vector_3D<F> cXAxis, Vector_3D<F> cYAxis);
};

using Point2D = Point_2D<float>		;
using DPoint2D = Point_2D<double>	;

template <typename F>
TDF_INLINE Point_3D<F>::Point_3D(Point_2D<F> const & that) : x(that.x), y(that.y), z(0) {}

template <typename F, typename S>
TDF_INLINE Point_2D<F>	operator* (S s, Point_2D<F> const & a) { return Point_2D<F>(F(s * a.x), F(s * a.y)); }

template <typename F>
TDF_INLINE Point_2D<F> Midpoint(Point_2D<F> const & a, Point_2D<F> const & b) {
	return Point_2D<F>(a.x + b.x, a.y + b.y) * 0.5f;
}

template <typename F>
TDF_INLINE Point_2D<F> Midpoint(Point_2D<F> const & a, Point_2D<F> const & b, Point_2D<F> const & c) {
	return Point_2D<F>(a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z) * (F)(1.0 / 3.0);
}

template <typename F>
TDF_INLINE bool Is_Abnormal(Point_2D<F> const & p) {
	return Is_Abnormal(p.x) || Is_Abnormal(p.y);
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
class Vector_3D
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
	// explicit Vector_3D(Point_3D<F> const & p) : x(p.x), y(p.y), z(p.z) {}
	// explicit Vector_3D(Plane_3D<F> const & p);
	explicit Vector_3D (Vector_2D<F> const & that);

	void Set(F X, F Y, F Z) { x = X; y = Y; z = Z; };

	Vector_3D const	operator- () const	{ return Vector_3D (-x, -y, -z); }

	TDF_INLINE bool operator== (Vector_3D const & v) const { return  Float::match(x, v.x) && Float::match(y, v.y) && Float::match(z, v.z); }
	TDF_INLINE bool operator!= (Vector_3D const & v) const { return  !(*this == v); }

	TDF_INLINE bool Equals(Vector_3D const & v, int in_tolerance = 32) const {
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
	Vector_3D const		operator* (D s) const			{ return Vector_3D (x * s, y * s, z * s); }
	//Vector_3D const		operator* (double s) const		{ return Vector_3D(x * s, y * s, z * s); }
	template<typename D>
	Vector_3D const		operator/ (D s) const			{ return operator* ((D)1.0 / s); }
	//Vector_3D const		operator/ (double s) const		{ return operator* (1.0f / s); }

	F &					operator[] (size_t i)		{ return (&x)[i]; }
	F const &			operator[] (size_t i) const	{ return (&x)[i]; }

	TDF_INLINE double	Length () const { return sqrt (LengthSquared()); }

	TDF_INLINE double	LengthSquared () const { return (double)x*(double)x + (double)y*(double)y + (double)z*(double)z; }

	TDF_INLINE double	Length2D () const { return sqrt (LengthSquared2D()); }

	TDF_INLINE double	LengthSquared2D () const { return (double)x*(double)x + (double)y*(double)y;}

	TDF_INLINE Vector_3D &	Normalize (bool check_range = false, F epsilon = Float_Traits<F>::Epsilon()) {// not const &; allow V.normalize() *= S;
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
	TDF_INLINE Vector_3D &	Normalize (F epsilon) { return Normalize (false, epsilon); }

	TDF_INLINE Vector_3D &	Clean_Zeroes (F epsilon = F(1.0e-4)) {
		if (Abs(x) < epsilon) x = 0;
		if (Abs(y) < epsilon) y = 0;
		if (Abs(z) < epsilon) z = 0;
		return *this;
	}

	TDF_INLINE F Magnitude () const { return Max (Abs(x), Abs(y), Abs(z)); }
	TDF_INLINE F Manhattan () const { return Abs(x)+Abs(y)+Abs(z); }

	TDF_INLINE F AngleWith(Vector_3D const & v) const { 
		F dot = Dot(v);
		F len = Length() * v.Length();
		F cos_angle = dot / len;
		return acos(cos_angle) * (F)180 / M_PI;
	}

	TDF_INLINE F CCWAngleWith(Vector_3D const & v) const {
		F dot = Dot(v);
		F len = Length() * v.Length();
		F cos_angle = dot / len;

		Vector_3D<F> cross = Cross(v);
		F dot2 = cross.Dot(v);
		F angle = acos(cos_angle) * (F)180 / M_PI;

		if (dot2 < 0) {
			angle = 360.0 - angle;
		}

		return angle;
	}

	TDF_INLINE F CCWAngleWith(Vector_3D const & v1, Vector_3D const & v2) const {
		F dot = v1.Dot(v2);
		F len = v1.Length() * v2.Length();
		F cos_angle = dot / len;

		//Vector_3D<F> cross = v1.Cross(*this);
		Vector_3D<F> cross = this->Cross(v1);
		F dot2 = cross.Dot(v2);
		F angle = acos(cos_angle) * (F)180 / M_PI;

		if (dot2 < 0) {
			angle = 360.0 - angle;
		}

		return angle;
	}

	TDF_INLINE F Dot (Vector_3D const & v) const { return x * v.x  +  y * v.y  +  z * v.z; }

	TDF_INLINE Vector_3D Cross (Vector_3D const & v) const { 
		return Vector_3D (y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); 
	}

	TDF_INLINE Vector_3D Scale(Vector_3D const & v) const {
		return Vector_3D(x * v.x, y * v.y, z * v.z);
	}

	Vector_3D<F> different_vector () const {
		if (Abs (x) < Abs (y))
			return Vector_3D<F>::XAxis();
		else
			return Vector_3D<F>::YAxis();
	}


	static TDF_INLINE Vector_3D XAxis() { return Vector_3D(1, 0, 0); };
	static TDF_INLINE Vector_3D YAxis() { return Vector_3D(0, 1, 0); };
	static TDF_INLINE Vector_3D ZAxis() { return Vector_3D(0, 0, 1); };
	static TDF_INLINE Vector_3D Zero() { return Vector_3D(0, 0, 0); };
	static TDF_INLINE Vector_3D Unit() { return Vector_3D(1, 1, 1); };

};

template <typename F, typename S>
TDF_INLINE	Vector_3D<F>	operator* (S s, Vector_3D<F> const & v) { return Vector_3D<F>(F(s * v.x), F(s * v.y), F(s * v.z)); }

template <typename F>
TDF_INLINE bool Is_Abnormal(Vector_3D<F> const & v) {
	return Is_Abnormal(v.x) || Is_Abnormal(v.y) || Is_Abnormal(v.z);
}

template <typename F>
TDF_INLINE bool Normalize(size_t count, Vector_3D<F> * vectors) {
	bool success = true;
	for (size_t i = 0; i < count; ++i) {
		if (vectors->Normalize() == Vector_3D<F>::Zero())
			success = false;
		vectors++;
	}
	return success;
}

template <typename F>
TDF_INLINE	Vector_3D<F>::Vector_3D(Vector_2D<F> const & that) : x(that.x), y(that.y), z(0) {}

template <typename F>
TDF_INLINE	Point_3D<F>::Point_3D(Vector_2D<F> const & v) : x(v.x), y(v.y), z(0) {}

template <typename F>
TDF_INLINE	Point_3D<F> & Point_3D<F>::operator+= (Vector_2D<F> const & v) { x += v.x; y += v.y; return *this; }
template <typename F>
TDF_INLINE	Point_3D<F> & Point_3D<F>::operator-= (Vector_2D<F> const & v) { x -= v.x; y -= v.y; return *this; }

template <typename F>
TDF_INLINE	Point_3D<F> const	Point_3D<F>::operator+ (Vector_2D<F> const & v) const { return Point_3D<F>(x + v.x, y + v.y, z); }
template <typename F>
TDF_INLINE	Point_3D<F> const	Point_3D<F>::operator- (Vector_2D<F> const & v) const { return Point_3D<F>(x - v.x, y - v.y, z); }

template <typename F>
TDF_INLINE	Point_3D<F> & Point_3D<F>::operator*= (Vector_2D<F> const & v) { x *= v.x; y *= v.y; return *this; }
template <typename F>
TDF_INLINE	Point_3D<F> & Point_3D<F>::operator/= (Vector_2D<F> const & v) { x /= v.x; y /= v.y; return *this; }

template <typename F>
TDF_INLINE	Point_3D<F> const	Point_3D<F>::operator* (Vector_2D<F> const & v) const { return Point_3D<F>(x * v.x, y * v.y, z); }
template <typename F>
TDF_INLINE	Point_3D<F> const	Point_3D<F>::operator/ (Vector_2D<F> const & v) const { return Point_3D<F>(x / v.x, y / v.y, z); }


template <typename F>
TDF_INLINE	Point_2D<F> & Point_2D<F>::operator+= (Vector_2D<F> const & v) { x += v.x; y += v.y; return *this; }
template <typename F>
TDF_INLINE	Point_2D<F> & Point_2D<F>::operator-= (Vector_2D<F> const & v) { x -= v.x; y -= v.y; return *this; }

template <typename F>
TDF_INLINE	Vector_2D<F> const	Point_2D<F>::operator- (Point_2D<F> const & p) const { return Vector_2D<F>(x - p.x, y - p.y); }

template <typename F>
TDF_INLINE	Point_2D<F> const	Point_2D<F>::operator+ (Vector_2D<F> const & v) const { return Point_2D<F>(x + v.x, y + v.y); }
template <typename F>
TDF_INLINE	Point_2D<F> const	Point_2D<F>::operator- (Vector_2D<F> const & v) const { return Point_2D<F>(x - v.x, y - v.y); }

template <typename F>
TDF_INLINE	Point_2D<F> & Point_2D<F>::operator*= (Vector_2D<F> const & v) { x *= v.x; y *= v.y; return *this; }
template <typename F>
TDF_INLINE	Point_2D<F> & Point_2D<F>::operator/= (Vector_2D<F> const & v) { x /= v.x; y /= v.y; return *this; }
template <typename F>
TDF_INLINE	Point_2D<F> const	Point_2D<F>::operator* (Vector_2D<F> const & v) const { return Point_2D<F>(x * v.x, y * v.y); }
template <typename F>
TDF_INLINE	Point_2D<F> const	Point_2D<F>::operator/ (Vector_2D<F> const & v) const { return Point_2D<F>(x / v.x, y / v.y); }

using Vector = Vector_3D<float>;
using DVector = Vector_3D<double>;

template <typename F>
class Vector_2D {
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

	TDF_INLINE double	Length() const { return sqrt(LengthSquared()); }

	TDF_INLINE double	LengthSquared() const { return (double)x * (double)x + (double)y * (double)y; }

	TDF_INLINE Vector_2D & Normalize(bool check_range = false, F epsilon = Float_Traits<F>::Epsilon()) {// not const &; allow V.normalize() *= S;
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
	TDF_INLINE Vector_2D & Normalize(F epsilon) { return Normalize(false, epsilon); }

	TDF_INLINE F		Magnitude() const { return Max(Abs(x), Abs(y)); }
	TDF_INLINE F		Manhattan() const { return Abs(x) + Abs(y); }

	TDF_INLINE F		Dot(Vector_2D const & v) const { return x * v.x + y * v.y; }


	TDF_INLINE F		Cross(Vector_2D const & v) const {
		return x * v.y - y * v.x;
	}

	TDF_INLINE Vector_2D	Scale(Vector_2D const & v) const {
		return Vector_2D(x * v.x, y * v.y);
	}

	static TDF_INLINE Vector_2D	XAxis() { return Vector_2D(1, 0); };
	static TDF_INLINE Vector_2D	YAxis() { return Vector_2D(0, 1); };

	static TDF_INLINE Vector_2D	Zero() { return Vector_2D(0, 0); };
	static TDF_INLINE Vector_2D	Unit() { return Vector_2D(1, 1); };
};

using Vector2D = Vector_2D<float>;
using DVector2D = Vector_2D<double>;

template <typename F, typename S>
TDF_INLINE	Vector_2D<F>	operator* (S s, Vector_2D<F> const & v) { return Vector_2D<F>(F(s * v.x), F(s * v.y)); }

template <typename F>
TDF_INLINE bool Is_Abnormal(Vector_2D<F> const & v) {
	return Is_Abnormal(v.x) || Is_Abnormal(v.y);
}


// using IntArray = std::vector<int>;
// using PointArray = std::vector<HPoint>;
// using VectorArray = std::vector<Vector>;

// using IntArray = std::vector<int, boost::pool_allocator<int>>;
// using FloatArray = std::vector<float, boost::pool_allocator<float>>;
// using PointArray = std::vector<TDF::Point, boost::pool_allocator<Point>>;
// using VectorArray = std::vector<TDF::Vector, boost::pool_allocator<Vector>>;

using ByteArray = CAtlArray<byte>;
using IntArray = CAtlArray<int>;
using FloatArray = CAtlArray<float>;
using PointArray = CAtlArray<TDF::Point>;
using Point2DArray = CAtlArray<TDF::Point2D>;
using DPoint2DArray = CAtlArray<TDF::DPoint2D>;
using VectorArray = CAtlArray<TDF::Vector>;
using Vector2DArray = CAtlArray<TDF::Vector2D>;

// template <typename F>
// _3DF_INLINE	Point_3D<F>::Point_3D(Vector_3D<F> const & v) : x(v.x), y(v.y), z(v.z) {}

/*

template <typename F>
class MatrixKit
{
public:
	MatrixKit();
	MatrixKit(F const fInMatrixSource[]);

	TDF_INLINE const F * operator [] (int nIndex) const { return m[nIndex]; }
	TDF_INLINE F * operator [] (int nIndex) { return m[nIndex]; }

	void SetIdentity();
	bool IsIdentity();

	TDF::Point Transform(TDF::Point const & cInSource) const;

	F * GetData() const { return (F *)r; }

	union
	{
		F m[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
		F r[16];
	};
};

template <typename F>
MatrixKit<F>::MatrixKit()
{
	//SetIdentity();
}

template <typename F>
MatrixKit<F>::MatrixKit(F const fInMatrixSource[])
{
	memcpy(r, fInMatrixSource, 16 * sizeof(float));
}

template <typename F>
void MatrixKit<F>::SetIdentity()
{
	// set to zero all the elements except the diagonal
	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 4; j++) {
			m[i][j] = m[j][i] = 0.0;
		}
	}
	
	// set to 1 the diagonal
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
}

template <typename F>
bool MatrixKit<F>::IsIdentity()
{
	F fIdMatrix[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};

	// can't use memcmp because of -0.0f and +0.0f
	for (int i = 0; i < 16; ++i) {
		if (r[i] != fIdMatrix[i]) {
			return false;
		}
	}

	return true;
}

template <typename F>
Point MatrixKit<F>::Transform(TDF::Point const & cInSource) const
{
	TDF::Point cPoint;

	cPoint.x = m[0][0] * cInSource.x + m[1][0] * cInSource.y + m[2][0] * cInSource.z + m[3][0];
	cPoint.y = m[0][1] * cInSource.x + m[1][1] * cInSource.y + m[2][1] * cInSource.z + m[3][1];
	cPoint.z = m[0][2] * cInSource.x + m[1][2] * cInSource.y + m[2][2] * cInSource.z + m[3][2];

	return cPoint;
}

template <typename F>
TDF_INLINE MatrixKit<F> operator * (const MatrixKit<F> & M1, const MatrixKit<F> & M2)
{
	MatrixKit<F> cMatrix;

	for (int c = 0; c < 4; c++) {
		for (int r = 0; r < 4; r++) {
			cMatrix[r][c] = M1[0][c] * M2[r][0];

			for (int p = 1; p < 4; p++) {
				cMatrix[r][c] += M1[p][c] * M2[r][p];
			}
		}
	}

	return cMatrix;
}
*/

//using Matrix = MatrixKit<float>;
//using DMatrix = MatrixKit<double>;

// namespace  MatrixCal {
// 	API_3DF void InverseMatrix(const float * matrix, float * out_matrix);
// 	API_3DF void ComputeMatrixProduct(const float * matrix1, const float * matrix2, float * out_matrix);
// 	API_3DF void ComputeIdentityMatrix(float * out_matrix);
// };

namespace Math
{
	//----- Circle 관련 함수 -----
	bool GetCircle(WorldPointArray & cPoints, CircleKit & cCircle);
	bool CircleFitByHyper(DPoint2DArray & cPoints, double & dCX, double & dCY, double & dRadius, double & dSigma);

	//----- Line 관련 함수 -----
	bool NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint);
	bool NormalPointWithInRange(Point cSP, Point cEP, Point cOtherPoint, Point & cNormalPoint, double & dPrameter);

	bool IntersectionPoint(Point cSP1, Point cEP1, Point cSP2, Point cEP2, Point & cIntersectionPoint);
	bool IntersectionPointInRange(Point cSP1, Point cEP1, Point cSP2, Point cEP2, Point & cIntersectionPoint);

	bool GetPoint(WindowKey const & cInWindow, WorldPointArray const & aInPoints, PixelPointArray & aOutPoints);
	bool GetPoint(WindowKey const & cInWindow, WorldPointArray const & aInPoints, WindowPointArray & aOutPoints);

};

CLOSE_3DF_NAMESPACE
