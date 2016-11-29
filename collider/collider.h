/*
 * collider.h
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#ifndef COLLIDER_COLLIDER_H_
#define COLLIDER_COLLIDER_H_

#include "../math/math_helper.h"

#define COLLIDER 0
#define AABB_COLLIDER 1
#define CIRCLE_COLLIDER 2
#define LINE_COLLIDER 3

/* Manifold struct is used in collision detection */
struct Manifold
{
	class Collider* A;
	class Collider* B;
	float penetration;
	vector2f normal;
	Manifold(Collider* a, Collider* b)
	{
		A = a; B = b; penetration = 0;
	}
};

/* Collider attached object will be automatically checked for collision*/
class Collider
{
public:
	/* Constructor
	 * @param Object2d* parent object
	 * @param float     restriction
	 */
	Collider(class Object2d* p, float res=1.0f);

	virtual ~Collider();

	/* @return vector2f   position of the collider in world coordinates
	 */
	vector2f getPosition() const;

	/* @return vector2f   actual scale of the collider
	 */
	vector2f getScale() const;

	void setRistriction(float res);

	/* @return int   the type of the current collider, only intended for subclasses
	 */
	virtual int getType() const;

	/* vector2f relative position with respect to parent object */
	vector2f relPos;

	/* vector2f relative scale with respect to parent object */
	vector2f relScale = vector2f(1.0f, 1.0f);

	/* Object2d* pointer to parent */
	class Object2d* parent;

	/* float restriction used when calculating colliding */
	float restriction = 1.0f;
};

class AABBCollider: public Collider
{
public:
	AABBCollider(class Object2d* p, float res=1.0f);
	vector2f getLowerLeft() const;
	vector2f getUpperRight() const;
	virtual int getType() const;
};

class CircleCollider: public Collider
{
public:
	CircleCollider(class Object2d* p, float res=1.0f);
	float getRadius() const;
	virtual int getType() const;
};

class LineCollider: public Collider
{
public:
	LineCollider(class Object2d* p, float res=1.0f, vector2f s=vector2f(), vector2f e=vector2f());
	virtual int getType() const;
	vector2f start, end;
};

/* This function is only intended for collision detection managed by the world object*/
/* The logic of collision handling is from
 * http://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331
 * credit to the author
 */
bool detectCollision(Manifold* m);

#endif /* COLLIDER_COLLIDER_H_ */
