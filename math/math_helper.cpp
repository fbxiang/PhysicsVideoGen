/*
 * math_helper.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#include "math_helper.h"
#include <algorithm>

vector2f::vector2f() {
	x = 0.0f;
	y = 0.0f;
}
vector2f::vector2f(float inx, float iny) {
	x = inx;
	y = iny;
}

vector2f vector2f::operator+(const vector2f& other) const {
	return vector2f(x + other.x, y + other.y);
}

vector2f vector2f::operator-(const vector2f& other) const {
	return vector2f(x - other.x, y - other.y);
}

vector2f vector2f::operator*(float m) const {
	return vector2f(x * m, y * m);
}

vector2f vector2f::operator*(const vector2f& other) const {
	return vector2f(x * other.x, y * other.y);
}

vector2f vector2f::operator/(float m) const {
	return vector2f(x / m, y / m);
}

float vector2f::dot(const vector2f& other) const {
	return x * other.x + y * other.y;
}

float vector2f::operator[](int i) const {
	if (i == 0)
		return x;
	if (i == 1)
		return y;
	return 0;
}
float vector2f::normSquared() const {
	return x * x + y * y;
}
float vector2f::norm() const {
	return sqrt(normSquared());
}
float vector2f::norm(int p) const {
	if (p < 0)
		return std::max(x, y);
	return std::pow(std::pow(x, p) + std::pow(y, p), 1.0 / p);
}

bool vector2f::operator==(const vector2f& other) const {
	return x == other.x && y == other.y;
}

vector3f::vector3f() {
	x = y = z = 0;
}

vector3f::vector3f(float inx, float iny, float inz) {
	x = inx;
	y = iny;
	z = inz;
}

vector3f vector3f::operator+(const vector3f& other) const {
	return vector3f(x + other.x, y + other.y, z + other.z);
}

vector3f vector3f::operator-(const vector3f& other) const {
	return vector3f(x - other.x, y - other.y, z - other.z);
}

vector3f vector3f::operator*(float m) const {
	return vector3f(x * m, y * m, z * m);
}

float vector3f::dot(const vector3f& other) const {
	return x * other.x + y * other.y + z * other.z;
}

vector3f vector3f::cross(const vector3f& other) const {
	return vector3f(y * other.z - z * other.y, z * other.x - x * other.z,
			x * other.y - y * other.x);
}

vector3f vector3f::normalize() const {
	float norm = sqrt(x * x + y * y + z * z);
	return vector3f(x / norm, y / norm, z / norm);
}

float vector3f::operator[](int i) const {
	if (i == 0)
		return x;
	if (i == 1)
		return y;
	if (i == 2)
		return z;
	return 0;
}

float reflect(float n, float lower, float upper) {
	float d_l_u = upper - lower;
	float d_l_n = n - lower;
	float n_d = d_l_n / d_l_u;
	n_d += 2; // handle negative

	if ((int) n_d % 2 == 0)
		return lower + (n_d - (int) n_d) * d_l_u;
	else
		return upper - (n_d - (int) n_d) * d_l_u;
}
#include <iostream>
float rand_float(float start, float end) {
	float r = (float) std::rand() / 2147483647;
	return r * (end - start) + start;
}
