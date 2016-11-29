/*
 * object2d.h
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#ifndef OBJECT_OBJECT2D_H_
#define OBJECT_OBJECT2D_H_

#include "../math/math_helper.h"
#include "../collider/collider.h"

/* Transform struct contains position and scale info of Object2d object
 * Transform object is always attached to Object2d object
 */
struct Transform {
	vector2f position;
	vector2f scale = vector2f(1.0f, 1.0f);
};

/* Rigidbody controls the physics behaviors of Object2d
 * Object2d objects may or may not have a rigidbody object attached */
class Rigidbody {
public:
	/* @param Object2d*      parent Object2d
	 * @param float          mass
	 * @param bool           determines if this object is affected by physics system. e.g. force, impulse
	 * @param bool           determines if this object is affected by gravity (by defulat 9.8m/s^2)
	 */
	Rigidbody(class Object2d* parent, float mass = 1.0f, bool dynamic = true,
			bool gravity = true);

	/* get velocity of the object */
	vector2f getVelocity();

	/* set velocity of the object */
	void setVelocity(vector2f v);

	/* called every frame to update the positoin, velocity, etc. of the object*/
	void update(int dt);

	/* @param vector2f   force (Newton) is added to the object (this frame)*/
	void addForce(vector2f force);
	/* @param vector2f   Impulse (Newton second) is added */
	void addImpulse(vector2f impulse);
	float getMass() const;
	class Object2d* parent;
private:
	float mass;
	vector2f velocity;
	vector2f acceleration;
public:
	bool useGravity = true; // determine if the object will be affected by gravity
	bool isDynamic = true; // determine if the object is affected by forces
};

/* used to store the drawing features of the object, currently only color */
class Appearance {
public:
	Appearance(int color);
	int color;
	//TODO: Add more features.
};

/* The class for any object in the world */
class Object2d {
public:
	Object2d();
	virtual ~Object2d();

	/* @param Rigidbody*   attach a rigidbody to the object, which will manage the memory of the body*/
	void attachRigidbody(Rigidbody* body);

	/* @param Rigidbody*   attach a collider to the object, which will manage the memory of the body*/
	void attachCollider(Collider* c);

	/* Called every frame to update the object */
	void update(int dt);
	vector2f getPosition() const;
	vector2f getScale() const;
	void setPosition(vector2f pos);
	void move(vector2f d);
	void setScale(vector2f scale);
	int getColor() const;
	void setColor(int c);

	Rigidbody* getRigidBody();
	Collider* getCollider();
	virtual void draw(class Canvas* canvas) = 0;

protected:
	Transform* transform;
	Rigidbody* rigidbody;
	Appearance* appearance;
	Collider* collider;
};

class ObjectCircle: public Object2d {
	void draw(class Canvas* canvas);
};

class ObjectRect: public Object2d {
	void draw(class Canvas* canvas);
};

#endif /* OBJECT_OBJECT2D_H_ */
