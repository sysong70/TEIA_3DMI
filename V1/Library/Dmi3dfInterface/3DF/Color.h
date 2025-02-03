#pragma once

#include "3DF.h"
#include "Math.h"

OPEN_3DF_NAMESPACE

class RGB24Color;
class RGBA32Color;
class RGBAS32Color;
class RGBAColor;

class API_3DF RGBColor
{
public:
	float	red = -1.f;
	float	green = -1.f;
	float	blue = -1.f;

	H3DF_INLINE RGBColor() {}
	H3DF_INLINE RGBColor(float r, float g, float b) : red(r), green(g), blue(b) {}
	explicit H3DF_INLINE RGBColor(RGB24Color const & c24);
	explicit H3DF_INLINE RGBColor(float gray) : red(gray), green(gray), blue(gray) {}
	explicit H3DF_INLINE RGBColor(RGBAS32Color const & c32);
	explicit H3DF_INLINE RGBColor(RGBA32Color const & c32);
	explicit H3DF_INLINE RGBColor(RGBAColor const & c);

	H3DF_INLINE void Set(float r, float g, float b) { red = r, green = g, blue = b; }
	H3DF_INLINE void SetRed(float r) { red = r; }
	H3DF_INLINE void SetGreen(float g) { green = g; }
	H3DF_INLINE void SetBlue(float b) { blue = b; }

	H3DF_INLINE bool IsGray() const { return (red == green && green == blue); }
	H3DF_INLINE float Gray() const { return  0.3125f * red + 0.5000f * green + 0.1875f * blue; }
	float Distance(RGBColor const & other_color) const;
	H3DF_INLINE bool		IsValid() const {
		if (red < 0.f && green < 0.f && blue < 0.f) { return false; }
		return (Float::extract_sign_bit(red) | Float::extract_sign_bit(green) | Float::extract_sign_bit(blue)) == 0;
	}

	H3DF_INLINE bool	operator== (RGBColor const & c) const { return  red == c.red && green == c.green && blue == c.blue; }
	H3DF_INLINE bool	operator!= (RGBColor const & c) const { return  !(*this == c); }

	H3DF_INLINE bool	Equals(RGBColor const & c, int in_tolerance = 32) const
	{
		return  Float::Equals(red, c.red, in_tolerance) && Float::Equals(green, c.green, in_tolerance) && Float::Equals(blue, c.blue, in_tolerance);
	}

	H3DF_INLINE RGBColor & operator*= (RGBColor const & c) { red *= c.red; green *= c.green; blue *= c.blue;  return *this; }
	H3DF_INLINE RGBColor & operator+= (RGBColor const & c) { red += c.red; green += c.green; blue += c.blue;  return *this; }
	H3DF_INLINE RGBColor & operator-= (RGBColor const & c) { red -= c.red; green -= c.green; blue -= c.blue;  return *this; }
	H3DF_INLINE RGBColor const	operator* (RGBColor const & c) const { return RGBColor(red * c.red, green * c.green, blue * c.blue); }
	H3DF_INLINE RGBColor const	operator+ (RGBColor const & c) const { return RGBColor(red + c.red, green + c.green, blue + c.blue); }
	H3DF_INLINE RGBColor const	operator- (RGBColor const & c) const { return RGBColor(red - c.red, green - c.green, blue - c.blue); }

	H3DF_INLINE RGBColor & operator*= (float s) { red *= s; green *= s; blue *= s;  return *this; }
	H3DF_INLINE RGBColor & operator/= (float s) { return operator*= (1.0f / s); }
	H3DF_INLINE RGBColor & operator+= (float s) { red += s; green += s; blue += s;  return *this; }
	H3DF_INLINE RGBColor & operator-= (float s) { red -= s; green -= s; blue -= s;  return *this; }
	H3DF_INLINE RGBColor const	operator* (float s) const { return RGBColor(red * s, green * s, blue * s); }
	H3DF_INLINE RGBColor const	operator/ (float s) const { return operator* (1.0f / s); }
	H3DF_INLINE RGBColor const	operator+ (float s) const { return RGBColor(red + s, green + s, blue + s); }
	H3DF_INLINE RGBColor const	operator- (float s) const { return RGBColor(red - s, green - s, blue - s); }

	static H3DF_INLINE RGBColor Black() { return RGBColor(0, 0, 0); };
	static H3DF_INLINE RGBColor White() { return RGBColor(1, 1, 1); };
	static H3DF_INLINE RGBColor Invalid() { return RGBColor(-1, -1, -1); };

// 	void ShowHLS(float & out_hue, float & out_lightness, float & out_saturation) const;
// 	void ShowHSV(float & out_hue, float & out_saturation, float & out_value) const;
// 	void ShowHIC(float & out_hue, float & out_intensity, float & out_chromaticity) const;
// 
// 	static RGBColor HLS(float in_hue, float in_lightness, float in_saturation);
// 	static RGBColor HSV(float in_hue, float in_saturation, float in_value);
// 	static RGBColor HIC(float in_hue, float in_intensity, float in_chromaticity);
};

H3DF_INLINE	RGBColor const	operator* (float s, RGBColor const & v) { return RGBColor(s * v.red, s * v.green, s * v.blue); }
H3DF_INLINE	RGBColor const	operator+ (float s, RGBColor const & v) { return RGBColor(s + v.red, s + v.green, s + v.blue); }
H3DF_INLINE	RGBColor const	operator- (float s, RGBColor const & v) { return RGBColor(s - v.red, s - v.green, s - v.blue); }

class API_3DF RGBAColor
{
public:
	float	red = -1.0f;	// Red component of this color object, from 0.0 to 1.0
	float	green = -1.0f;	// Green component of this color object, from 0.0 to 1.0
	float	blue = -1.0f;	// Blue component of this color object, from 0.0 to 1.0
	float	alpha = -1.0f;	// Alpha component of this color object, from 0.0 to 1.0

	H3DF_INLINE RGBAColor() {}
	explicit H3DF_INLINE RGBAColor(float gray, float a = 1) : red(gray), green(gray), blue(gray), alpha(a) {}
	H3DF_INLINE RGBAColor(float r, float g, float b, float a = 1) : red(r), green(g), blue(b), alpha(a) {}
	H3DF_INLINE RGBAColor(COLORREF cColref) : red(GetRValue(cColref) / 255.f), green(GetGValue(cColref) / 255.f), blue(GetBValue(cColref) / 255.f), alpha(1.f) {}

	H3DF_INLINE void Set(float r, float g, float b, float a = 1) { red = r, green = g, blue = b, alpha = a; }
	H3DF_INLINE void SetRed(float r) { red = r; }
	H3DF_INLINE void SetGreen(float g) { green = g; }
	H3DF_INLINE void SetBlue(float b) { blue = b; }
	H3DF_INLINE void SetAlpha(float a) { alpha = a; }

	H3DF_INLINE RGBAColor(RGBColor const & c) {
		memcpy(this, &c, sizeof(RGBColor)); //-V512
		alpha = 1.0f;
		Float::apply_sign_bit(alpha, Float::extract_sign_bit(c.red) | Float::extract_sign_bit(c.green) | Float::extract_sign_bit(c.blue));
	}
	H3DF_INLINE RGBAColor(RGBColor const & c, float a) {
		memcpy(this, &c, sizeof(RGBColor));
		memcpy(&alpha, &a, sizeof(float));
		Float::apply_sign_bit(alpha, Float::extract_sign_bit(c.red) | Float::extract_sign_bit(c.green) | Float::extract_sign_bit(c.blue));
	}
	explicit H3DF_INLINE RGBAColor(RGBA32Color const & c32);
	explicit H3DF_INLINE RGBAColor(RGBAS32Color const & c32);

	H3DF_INLINE bool		IsGray() const { return (red == green && green == blue); }
	H3DF_INLINE float	Gray() const { return  0.3125f * red + 0.5000f * green + 0.1875f * blue; }
	H3DF_INLINE bool		IsValid() const {
		if (red < 0.f && green < 0.f && blue < 0.f && alpha < 0.f) { return false; }
		return (Float::extract_sign_bit(red) | Float::extract_sign_bit(green) | Float::extract_sign_bit(blue) | Float::extract_sign_bit(alpha)) == 0;
	}

	// Determines RGBA equivalency between this object and argument.
	// param: c The color object to test against
	// return: true if the objects are equivalent, false otherwise
	H3DF_INLINE bool	operator== (RGBAColor const & c) const { return  red == c.red && green == c.green && blue == c.blue && alpha == c.alpha; }

	// Determines RGBA non-equivalency between this object and argument.
	// param: c The color object to test against
	// return: true if the objects are not equivalent, false otherwise
	H3DF_INLINE bool	operator!= (RGBAColor const & c) const { return  !(*this == c); }

	H3DF_INLINE bool	Equals(RGBAColor const & c, int in_tolerance = 32) const {
		return Float::Equals(red, c.red, in_tolerance) && Float::Equals(green, c.green, in_tolerance) &&
			Float::Equals(blue, c.blue, in_tolerance) && Float::Equals(alpha, c.alpha, in_tolerance);
	}

	H3DF_INLINE RGBAColor & operator*= (RGBAColor const & c) { red *= c.red; green *= c.green; blue *= c.blue; alpha *= c.alpha; return *this; }
	H3DF_INLINE RGBAColor & operator+= (RGBAColor const & c) { red += c.red; green += c.green; blue += c.blue; alpha += c.alpha; return *this; }
	H3DF_INLINE RGBAColor & operator-= (RGBAColor const & c) { red -= c.red; green -= c.green; blue -= c.blue; alpha -= c.alpha; return *this; }
	H3DF_INLINE RGBAColor const	operator* (RGBAColor const & c) const { return RGBAColor(red * c.red, green * c.green, blue * c.blue, alpha * c.alpha); }
	H3DF_INLINE RGBAColor const	operator+ (RGBAColor const & c) const { return RGBAColor(red + c.red, green + c.green, blue + c.blue, alpha + c.alpha); }
	H3DF_INLINE RGBAColor const	operator- (RGBAColor const & c) const { return RGBAColor(red - c.red, green - c.green, blue - c.blue, alpha - c.alpha); }

	H3DF_INLINE RGBAColor & operator*= (float s) { red *= s; green *= s; blue *= s; alpha *= s; return *this; }
	H3DF_INLINE RGBAColor & operator/= (float s) { return operator*= (1.0f / s); }
	H3DF_INLINE RGBAColor & operator+= (float s) { red += s; green += s; blue += s; alpha += s; return *this; }
	H3DF_INLINE RGBAColor & operator-= (float s) { red -= s; green -= s; blue -= s; alpha -= s; return *this; }
	H3DF_INLINE RGBAColor const	operator* (float s) const { return RGBAColor(red * s, green * s, blue * s, alpha * s); }
	H3DF_INLINE RGBAColor const	operator/ (float s) const { return operator* (1.0f / s); }
	H3DF_INLINE RGBAColor const	operator+ (float s) const { return RGBAColor(red + s, green + s, blue + s, alpha + s); }
	H3DF_INLINE RGBAColor const	operator- (float s) const { return RGBAColor(red - s, green - s, blue - s, alpha - s); }

	H3DF_INLINE RGBAColor & operator*= (RGBColor const & c) { red *= c.red; green *= c.green; blue *= c.blue; return *this; }
	H3DF_INLINE RGBAColor & operator+= (RGBColor const & c) { red += c.red; green += c.green; blue += c.blue; return *this; }
	H3DF_INLINE RGBAColor & operator-= (RGBColor const & c) { red -= c.red; green -= c.green; blue -= c.blue; return *this; }
	H3DF_INLINE RGBAColor const	operator* (RGBColor const & c) const { return RGBAColor(red * c.red, green * c.green, blue * c.blue, alpha); }
	H3DF_INLINE RGBAColor const	operator+ (RGBColor const & c) const { return RGBAColor(red + c.red, green + c.green, blue + c.blue, alpha); }
	H3DF_INLINE RGBAColor const	operator- (RGBColor const & c) const { return RGBAColor(red - c.red, green - c.green, blue - c.blue, alpha); }

	static H3DF_INLINE RGBAColor Black() { return RGBAColor(0, 0, 0, 1); };
	static H3DF_INLINE RGBAColor White() { return RGBAColor(1, 1, 1, 1); };
	static H3DF_INLINE RGBAColor Nothing() { return RGBAColor(0, 0, 0, 0); };
	static H3DF_INLINE RGBAColor Invalid() { return RGBAColor(-1, -1, -1, -1); };
};

H3DF_INLINE	RGBAColor const	operator* (float s, RGBAColor const & v) { return RGBAColor(s * v.red, s * v.green, s * v.blue, s * v.alpha); }
H3DF_INLINE	RGBAColor const	operator+ (float s, RGBAColor const & v) { return RGBAColor(s + v.red, s + v.green, s + v.blue, s + v.alpha); }
H3DF_INLINE	RGBAColor const	operator- (float s, RGBAColor const & v) { return RGBAColor(s - v.red, s - v.green, s - v.blue, s - v.alpha); }

struct DirectRGBColor
{
public:
	enum Order
	{
		Order_ABGR,
		Order_RGBA,
		Order_BGRA
	};

	H3DF_INLINE static Order Preferred_Order() { return Order_BGRA; }
	unsigned char	b, g, r, a;
};

class RGBAS32Color : public DirectRGBColor
{
public:

	H3DF_INLINE RGBAS32Color() /* : DirectRGBColor() */ {}
	explicit H3DF_INLINE RGBAS32Color(unsigned char gray, unsigned char aa = 255) {
		r = gray;
		g = gray;
		b = gray;
		a = aa;
	}
	H3DF_INLINE RGBAS32Color(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa = 255) {
		r = rr;
		g = gg;
		b = bb;
		a = aa;
	}
	H3DF_INLINE RGBAS32Color(DirectRGBColor const & c) : DirectRGBColor(c) {};

	explicit H3DF_INLINE RGBAS32Color(RGBColor const & c)
	{
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = 255;
	}

	H3DF_INLINE RGBAS32Color(RGB24Color const & c);

	H3DF_INLINE RGBAS32Color(RGBA32Color const & c);

	H3DF_INLINE RGBAS32Color(RGBColor const & c, float alpha)
	{
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = Float::unit_to_byte(alpha);
	}
	H3DF_INLINE RGBAS32Color(RGBColor const & c, unsigned char aa)
	{
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = aa;
	}
	explicit H3DF_INLINE RGBAS32Color(RGBAColor const & c)
	{
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = Float::unit_to_byte(c.alpha);
	}
	H3DF_INLINE RGBAS32Color(RGBAColor const & c, unsigned char mix)
	{
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = Float::unit_to_byte_scaled(c.alpha, mix);
	}

	H3DF_INLINE bool			IsGray() const { return (r == g && g == b); }
	H3DF_INLINE unsigned char	Gray() const { return (unsigned char) (0.3125f * (float) r + 0.5000f * (float) g + 0.1875f * (float) b); }
	H3DF_INLINE bool			IsValid() const { return ((r | g | b | a) != 0); }

	H3DF_INLINE bool	operator== (RGBAS32Color const & c) const { return (r == c.r && g == c.g && b == c.b && a == c.a); }
	H3DF_INLINE bool	operator!= (RGBAS32Color const & c) const { return !(*this == c); }

	static H3DF_INLINE RGBAS32Color Black() { return RGBAS32Color(0, 0, 0, 255); };
	static H3DF_INLINE RGBAS32Color White() { return RGBAS32Color(255, 255, 255, 255); };
	static H3DF_INLINE RGBAS32Color Invalid() { return RGBAS32Color(0, 0, 0, 0); };

	static	H3DF_INLINE unsigned char Opaque_Alpha() { return 0xFF; }
};

class RGBA32Color
{
public:
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	a;

	H3DF_INLINE RGBA32Color() {}
	explicit H3DF_INLINE RGBA32Color(unsigned char gray, unsigned char aa = 255)
		: r(gray), g(gray), b(gray), a(aa) {}
	H3DF_INLINE RGBA32Color(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa = 255)
		: r(rr), g(gg), b(bb), a(aa) {}
	H3DF_INLINE RGBA32Color(RGBAS32Color const & c32)
		: r(c32.r), g(c32.g), b(c32.b), a(c32.a) {}
	explicit H3DF_INLINE	RGBA32Color(RGBColor const & c) {
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = 255;
	}
	H3DF_INLINE RGBA32Color(RGBColor const & c, float alpha) {
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = Float::unit_to_byte(alpha);
	}
	H3DF_INLINE RGBA32Color(RGBColor const & c, unsigned char aa) {
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = aa;
	}
	explicit H3DF_INLINE RGBA32Color(RGBAColor const & c) {
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = Float::unit_to_byte(c.alpha);
	}
	H3DF_INLINE RGBA32Color(RGBAColor const & c, unsigned char mix) {
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
		a = Float::unit_to_byte_scaled(c.alpha, mix);
	}

	H3DF_INLINE bool IsGray() const { return (r == g && g == b); }
	H3DF_INLINE unsigned char	Gray() const { return (unsigned char) (0.3125f * (float) r + 0.5000f * (float) g + 0.1875f * (float) b); }

	H3DF_INLINE bool operator== (RGBA32Color const & c) const { return (r == c.r && g == c.g && b == c.b && a == c.a); }
	H3DF_INLINE bool operator!= (RGBA32Color const & c) const { return  !(*this == c); }

	static H3DF_INLINE RGBA32Color Black() { return RGBA32Color(0, 0, 0, 255); };
	static H3DF_INLINE RGBA32Color White() { return RGBA32Color(255, 255, 255, 255); };

	static	H3DF_INLINE unsigned char Opaque_Alpha() { return 0xFF; }
};


class RGB24Color
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;

	H3DF_INLINE RGB24Color() {}
	explicit H3DF_INLINE RGB24Color(unsigned char gray)
		: r(gray), g(gray), b(gray) {}
	H3DF_INLINE RGB24Color(unsigned char rr, unsigned char gg, unsigned char bb)
		: r(rr), g(gg), b(bb) {}
	explicit H3DF_INLINE RGB24Color(RGBColor const & c) {
		r = Float::unit_to_byte(c.red);
		g = Float::unit_to_byte(c.green);
		b = Float::unit_to_byte(c.blue);
	}

	H3DF_INLINE bool IsGray() const { return (r == g && g == b); }
	H3DF_INLINE unsigned char	Gray() const { return (unsigned char) (0.3125f * (float) r + 0.5000f * (float) g + 0.1875f * (float) b); }

	H3DF_INLINE bool	operator== (RGB24Color const & c) const { return (r == c.r && g == c.g && b == c.b); }
	H3DF_INLINE bool	operator!= (RGB24Color const & c) const { return  !(*this == c); }
};

H3DF_INLINE RGBColor::RGBColor(RGBAS32Color const & c32) {
	red = Float::C2F(c32.r);
	green = Float::C2F(c32.g);
	blue = Float::C2F(c32.b);
}

H3DF_INLINE RGBColor::RGBColor(RGBA32Color const & c32) {
	red = Float::C2F(c32.r);
	green = Float::C2F(c32.g);
	blue = Float::C2F(c32.b);
}

H3DF_INLINE RGBColor::RGBColor(RGBAColor const & c) {
	red = c.red;
	green = c.green;
	blue = c.blue;
}

H3DF_INLINE RGBColor::RGBColor(RGB24Color const & c24) {
	red = Float::C2F(c24.r);
	green = Float::C2F(c24.g);
	blue = Float::C2F(c24.b);
}

H3DF_INLINE RGBAS32Color::RGBAS32Color(RGB24Color const & c)
{
	r = c.r;
	g = c.g;
	b = c.b;
	a = 255;
}

H3DF_INLINE RGBAS32Color::RGBAS32Color(RGBA32Color const & c)
{
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}

H3DF_INLINE RGBAColor::RGBAColor(RGBAS32Color const & c32) {
	red = Float::C2F(c32.r);
	green = Float::C2F(c32.g);
	blue = Float::C2F(c32.b);
	alpha = Float::C2F(c32.a);
}

H3DF_INLINE RGBAColor::RGBAColor(RGBA32Color const & c32) {
	red = Float::C2F(c32.r);
	green = Float::C2F(c32.g);
	blue = Float::C2F(c32.b);
	alpha = Float::C2F(c32.a);
}

H3DF_INLINE RGBColor Modulate(RGBColor const & a, RGBColor const & b) {
	return RGBColor(a.red * b.red, a.green * b.green, a.blue * b.blue);
}



H3DF_INLINE RGBColor Interpolate(RGBColor const & a, RGBColor const & b, float t) {
	return RGBColor(a.red + (b.red - a.red) * t, a.green + (b.green - a.green) * t, a.blue + (b.blue - a.blue) * t);
}

H3DF_INLINE RGBAColor Interpolate(RGBAColor const & a, RGBAColor const & b, float t) {
	return RGBAColor(a.red + (b.red - a.red) * t, a.green + (b.green - a.green) * t, a.blue + (b.blue - a.blue) * t, a.alpha + (b.alpha - a.alpha) * t);
}

H3DF_INLINE RGBAS32Color Interpolate(RGBAS32Color const & a, RGBAS32Color const & b, float t) {
	return RGBAS32Color(
		(unsigned char) (a.r + ((float) b.r - (float) a.r) * t),
		(unsigned char) (a.g + ((float) b.g - (float) a.g) * t),
		(unsigned char) (a.b + ((float) b.b - (float) a.b) * t),
		(unsigned char) (a.a + ((float) b.a - (float) a.a) * t));
}

H3DF_INLINE RGBA32Color Interpolate(RGBA32Color const & a, RGBA32Color const & b, float t) {
	return RGBA32Color(
		(unsigned char) (a.r + ((float) b.r - (float) a.r) * t),
		(unsigned char) (a.g + ((float) b.g - (float) a.g) * t),
		(unsigned char) (a.b + ((float) b.b - (float) a.b) * t),
		(unsigned char) (a.a + ((float) b.a - (float) a.a) * t));
}

H3DF_INLINE RGB24Color Interpolate(RGB24Color const & a, RGB24Color const & b, float t) {
	return RGB24Color(
		(unsigned char) (a.r + ((float) b.r - (float) a.r) * t),
		(unsigned char) (a.g + ((float) b.g - (float) a.g) * t),
		(unsigned char) (a.b + ((float) b.b - (float) a.b) * t));
}

using RGBAColorArray = std::vector<H3DF::RGBAColor>;

CLOSE_3DF_NAMESPACE