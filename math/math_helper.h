/*
 * math_helper.h
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#ifndef MATH_MATH_HELPER_H_
#define MATH_MATH_HELPER_H_

#include <cmath>

#define PI 3.14159265f

class vector2f {
public:
	float x;
	float y;
public:
	vector2f();
	vector2f(float inx, float iny);

	vector2f operator+(const vector2f& other) const;
	vector2f operator-(const vector2f& other) const;
	vector2f operator*(float m) const;
	vector2f operator*(const vector2f& other) const;
	vector2f operator/(float m) const;
	float dot(const vector2f& other) const;
	float operator[](int i) const;
	float normSquared() const;
	float norm() const;
	float norm(int p) const;
	bool operator==(const vector2f& other) const;
};

class vector3f {
private:
	float x;
	float y;
	float z;
public:
	vector3f();
	vector3f(float inx, float iny, float inz);

	vector3f operator+(const vector3f& other) const;
	vector3f operator-(const vector3f& other) const;
	vector3f operator*(float m) const;
	float dot(const vector3f& other) const;
	vector3f cross(const vector3f& other) const;
	vector3f normalize() const;
	float operator[](int i) const;
};

template<class T>
T interp(const T& begin, const T& end, float a) {
	return begin + (end - begin) * a;
}

template<class T>
T clamp(T v, T min, T max) {
	if (v > max)
		return max;
	if (v < min)
		return min;
	return v;
}

float reflect(float n, float lower, float upper);

float rand_float(float start, float end);

#endif /* MATH_MATH_HELPER_H_ */
