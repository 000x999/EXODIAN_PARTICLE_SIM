#pragma once
#include <immintrin.h>

class Vec2
{
public:
	Vec2() = default;
	Vec2(float x_in, float y_in);
	Vec2 operator+(const Vec2& rhs) const;
	Vec2& operator+=(const Vec2& rhs);
	Vec2 operator*(float rhs) const;
	Vec2& operator*=(float rhs);
	Vec2 operator-(const Vec2& rhs) const;
	Vec2& operator-=(const Vec2& rhs);
	float fast_rsqrt(float x) { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
	float fast_sqrt(float x) { return x * fast_rsqrt(x); }
	float GetLength();
	float GetLengthSq() const;
	Vec2& Normalize();
	Vec2 GetNormalized();
public:
	float x;
	float y;
};