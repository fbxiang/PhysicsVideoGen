/*
 * collider.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#include "collider.h"

#include "../object/object2d.h"
#include <cstdio>
#include "../math/math_helper.h"

bool CircleVsCircle(Manifold* m)
{
	CircleCollider *A = (CircleCollider*)m->A;
	CircleCollider *B = (CircleCollider*)m->B;

	vector2f n = B->getPosition() - A->getPosition();
	float r = A->getRadius() + B->getRadius();
	if(n.normSquared() > r*r)
    	return false;

	float d = n.norm();

	if(d != 0)
	{
		m->penetration = r - d;
		m->normal = n / d;
		return true;
	}
	else
	{
		m->penetration = A->getRadius();
    	m->normal = vector2f(1.0f, 0.0f);
    	return true;
	}
}

bool AABBVsAABB(Manifold* m)
{
	AABBCollider* A = (AABBCollider*)m->A;
	AABBCollider* B = (AABBCollider*)m->B;
	vector2f n = B->getPosition() - A->getPosition();
	vector2f aMin = A->getLowerLeft(); vector2f aMax = A->getUpperRight();
	vector2f bMin = B->getLowerLeft(); vector2f bMax = B->getUpperRight();
	if (aMax[0]<bMin[0] || aMin[0] > bMax[0]) return false;
	if (aMax[1]<bMin[1] || aMin[1] > bMax[1]) return false;

	vector2f a_extent = (aMax-aMin)/2; vector2f b_extent = (bMax-bMin)/2;
	vector2f xy_overlap = a_extent + b_extent - vector2f(std::abs(n[0]), std::abs(n[1]));
	if (xy_overlap[0]>0 && xy_overlap[1]>0)
	{
		if (xy_overlap[0] < xy_overlap[1])
		{
			if (n[0]<0)
				m->normal = vector2f(-1.0f, 0.0f);
			else
				m->normal = vector2f(1.0f, 0.0f);
			m->penetration = xy_overlap[0];
		}
		else
		{
			if (n[1]<0)
				m->normal = vector2f(0.0f, -1.0f);
			else
				m->normal = vector2f(0.0f, 1.0f);
			m->penetration = xy_overlap[1];
		}
		return true;
	}
	return false;
}

bool AABBVsCircle(Manifold* m)
{
	AABBCollider* A = (AABBCollider*)m->A;
	CircleCollider* B = (CircleCollider*)m->B;
	vector2f n = B->getPosition() - A->getPosition();

	vector2f aMin = A->getLowerLeft(); vector2f aMax = A->getUpperRight();
	vector2f xy_extent = (aMax - aMin) / 2;

	vector2f closest(clamp(n[0], -xy_extent[0], xy_extent[0]),
		             clamp(n[1], -xy_extent[1], xy_extent[1]));
	bool inside = false;
	if(n == closest)
	{
		inside = true;
    	if(std::abs(n[0]) > std::abs(n[1]))
    	{
			if(closest.x > 0)
				closest.x = xy_extent.x;
			else
				closest.x = -xy_extent.x;
    	}
    	else
    	{
			if(closest.y > 0)
				closest.y = xy_extent.y;
			else
				closest.y = -xy_extent.y;
		}
	}
	vector2f normal = n - closest;
	float d = normal.normSquared();
	float r = B->getRadius();
	if (d > r*r && !inside) return false;
	d = std::sqrt(d);

	if (inside)
	{
		m->normal = normal / normal.norm() * (-1.0f);
		m->penetration = r - d;
	}
	else
	{
		m->normal = normal / normal.norm() * (1.0f);
		m->penetration = r - d;
	}
	return true;
}
bool CircleVsLine(Manifold* m)
{
	CircleCollider* A = (CircleCollider*)m->A;
	LineCollider* B = (LineCollider*)m->B;
	vector2f c = A->getPosition();
	float radius = A->getRadius();
	vector2f a = B->start;
	vector2f b = B->end;

	vector2f ab = b - a;
	vector2f ac = c - a;
	float adl = ab.dot(ac)/ab.norm();

	if (adl<0 || adl*adl>ab.normSquared()) return false;
	vector2f d = ab*adl/ab.norm() + a;
	vector2f n = d - c;
	if (n.normSquared()>radius*radius) return false;

	m->penetration = (radius - n.norm());
	m->normal = n / n.norm();
	return true;
}

Collider::Collider(class Object2d* p, float res)
{
	parent = p;
	restriction = res;
}

Collider::~Collider() {}

vector2f Collider::getPosition() const
{
	return parent->getPosition()+relPos;
}

vector2f Collider::getScale() const
{
	return parent->getScale() * relScale;
}

void Collider::setRistriction(float res)
{
	restriction = res;
}

int Collider::getType() const
{
	return COLLIDER;
}


AABBCollider::AABBCollider(class Object2d* p, float res):Collider(p, res)
{
}

vector2f AABBCollider::getLowerLeft() const
{
	return getPosition() - getScale();
}
vector2f AABBCollider::getUpperRight() const
{
	return getPosition() + getScale();
}


int AABBCollider::getType() const
{
	return AABB_COLLIDER;
}

CircleCollider::CircleCollider(class Object2d* p, float res):Collider(p, res)
{

}

int CircleCollider::getType() const
{
	return CIRCLE_COLLIDER;
}

float CircleCollider::getRadius() const
{
	return getScale()[0];
}

LineCollider::LineCollider(class Object2d* p, float res, vector2f s, vector2f e):Collider(p, res)
{
	start = s; end = e;
}
int LineCollider::getType() const
{
	return LINE_COLLIDER;
}

bool detectCollision(Manifold* m)
{
	if (m->A->getType()==CIRCLE_COLLIDER && m->B->getType()==CIRCLE_COLLIDER)
	{
		return CircleVsCircle(m);
	}
	else if (m->A->getType()==AABB_COLLIDER && m->B->getType()==AABB_COLLIDER)
	{
		return AABBVsAABB(m);
	}
	else if (m->A->getType()==AABB_COLLIDER && m->B->getType()==CIRCLE_COLLIDER)
	{
		return AABBVsCircle(m);
	}
	else if (m->A->getType()==CIRCLE_COLLIDER && m->B->getType()==AABB_COLLIDER)
	{
		Manifold m2(m->B, m->A);
		if (!AABBVsCircle(&m2)) return false;
		m->normal = m2.normal * (-1.0f);
		m->penetration = m2.penetration;
		return true;
	}
	else if (m->A->getType()==CIRCLE_COLLIDER && m->B->getType()==LINE_COLLIDER)
	{
		return CircleVsLine(m);
	}
	else if (m->A->getType()==LINE_COLLIDER && m->B->getType()==CIRCLE_COLLIDER)
	{
		Manifold m2(m->B, m->A);
		if (!CircleVsLine(&m2)) return false;
		m->normal = m2.normal * (-1.0f);
		m->penetration = m2.penetration;
		return true;
	}

	return false;
}
