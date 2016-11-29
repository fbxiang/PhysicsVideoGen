/*
 * object2d.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#include "object2d.h"
#include "../math/math_helper.h"
#include "../world/world.h"
#include <cstdlib>
#include <vector>
#include <cstdio>

#define GRAVITY vector2f(0, -9.8)

Rigidbody::Rigidbody(Object2d* p, float m, bool dynamic, bool gravity) {
	parent = p;
	mass = m;
	useGravity = gravity;
	isDynamic = dynamic;
}

vector2f Rigidbody::getVelocity() {
	return velocity;
}
void Rigidbody::setVelocity(vector2f v) {
	velocity = v;
}
void Rigidbody::update(int dt) {
	if (isDynamic && useGravity)
		acceleration = acceleration + GRAVITY;
	velocity = velocity + acceleration * (float) dt / 1000.0f;
	acceleration = vector2f();
	parent->setPosition(
			parent->getPosition() + velocity * (float) dt / 1000.0f);
}
void Rigidbody::addForce(vector2f force) {
	if (!isDynamic)
		return;
	acceleration = acceleration + force / mass;
}

void Rigidbody::addImpulse(vector2f impulse) {
	if (!isDynamic)
		return;
	velocity = velocity + impulse / mass;
}
float Rigidbody::getMass() const {
	return mass;
}

Appearance::Appearance(int c) {
	color = c;
}

Object2d::Object2d() {
	transform = new Transform();
	rigidbody = NULL;
	appearance = new Appearance(0xffffff);
	collider = NULL;
}
Object2d::~Object2d() {
	delete transform;
	delete rigidbody;
	delete appearance;
	delete collider;
}
/* Life of the rigidbody passed in is controlled by this object. */
void Object2d::attachRigidbody(Rigidbody* body) {
	if (rigidbody)
		delete rigidbody;
	rigidbody = body;
}
void Object2d::attachCollider(Collider* c) {
	collider = c;
}
void Object2d::update(int dt) {
	if (rigidbody)
		rigidbody->update(dt);
}
vector2f Object2d::getPosition() const {
	return transform->position;
}
void Object2d::setPosition(vector2f pos) {
	transform->position = pos;
}
void Object2d::move(vector2f d) {
	transform->position = transform->position + d;
}
vector2f Object2d::getScale() const {
	return transform->scale;
}
void Object2d::setScale(vector2f scale) {
	transform->scale = scale;
}
int Object2d::getColor() const {
	return appearance->color;
}
void Object2d::setColor(int c) {
	appearance->color = c;
}
Rigidbody* Object2d::getRigidBody() {
	return rigidbody;
}
Collider* Object2d::getCollider() {
	return collider;
}

void ObjectCircle::draw(Canvas* canvas) {
	canvas->drawCircle(getPosition(), getScale()[0], appearance->color);
}

void ObjectRect::draw(Canvas* canvas) {
	std::vector<vector2f> vs;
	vector2f shape = getScale();
	vector2f pos = getPosition();
	vs.push_back(pos + vector2f(-shape[0], -shape[1]));
	vs.push_back(pos + vector2f(shape[0], -shape[1]));
	vs.push_back(pos + vector2f(shape[0], shape[1]));
	vs.push_back(pos + vector2f(-shape[0], shape[1]));
	canvas->drawPoly(vs, appearance->color);
}
