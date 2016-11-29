/*
 * world.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#include "world.h"
#include <algorithm>
#include <vector>
#include "../collider/collider.h"
#include <cmath>

World::World(float w, float h)
{
	width = w; height = h;
}
World::~World()
{
	for (auto o : objects)
		delete o;
}
void World::update(int dt)
{
	collision_info = std::vector< collision_info_t >();
	for (int i = 0; i < objects.size(); i++)
	{
		if (!objects[i]->getRigidBody() || !objects[i]->getCollider()) continue;
		for (int j = i+1; j < objects.size(); j++)
		{
			if (!objects[j]->getRigidBody() || !objects[j]->getCollider()) continue;
			Manifold m(objects[i]->getCollider(), objects[j]->getCollider());
			if (detectCollision(&m))
			{
				vector2f rv = objects[j]->getRigidBody()->getVelocity() - objects[i]->getRigidBody()->getVelocity();
				float velocityNormal = rv.dot(m.normal);
				if (velocityNormal > 0) continue; // This case objects are actually separating
				float e = 1.0f * std::min(objects[i]->getCollider()->restriction, objects[j]->getCollider()->restriction);
				float J = -(1.0f + e) * velocityNormal;
				float f = (objects[i]->getRigidBody()->isDynamic ? 1.0f / objects[i]->getRigidBody()->getMass() : 0)
				        + (objects[j]->getRigidBody()->isDynamic ? 1.0f / objects[j]->getRigidBody()->getMass() : 0);
				if (f==0) continue; // This case both objects are static, we really should not have this hanppening
				J/=f;
				vector2f impulse = m.normal * J;
				objects[i]->getRigidBody()->addImpulse(impulse * (-1.0f));
				objects[j]->getRigidBody()->addImpulse(impulse);

				// Retrive the information of this collision
				collision_info_t new_collision;
				new_collision.speed = -velocityNormal;
				new_collision.r1 = objects[i]->getRigidBody()->isDynamic ? objects[i]->getScale().norm(-1) : -1;
				new_collision.r2 = objects[j]->getRigidBody()->isDynamic ? objects[j]->getScale().norm(-1) : -1;
				collision_info.push_back(new_collision);

				// Code below are used to prevent object sinking caused by gravity.
				const float percent = 0.5;
				const float slop = 0.001;

				if (objects[i]->getRigidBody()->isDynamic && objects[j]->getRigidBody()->isDynamic)
				{
					vector2f correction = m.normal * (std::max(m.penetration - slop, 0.0f) /
													  (1/objects[i]->getRigidBody()->getMass() + 1/objects[j]->getRigidBody()->getMass())
													  * percent);
					objects[i]->move(correction * (-1/objects[i]->getRigidBody()->getMass()));
					objects[j]->move(correction * (1/objects[j]->getRigidBody()->getMass()));
				}
				else if (objects[i]->getRigidBody()->isDynamic)
				{
					vector2f correction = m.normal * (std::max(m.penetration - slop, 0.0f)  * percent);
					objects[i]->move(correction * (-1));
				}
				else if (objects[j]->getRigidBody()->isDynamic)
				{
					vector2f correction = m.normal * (std::max(m.penetration - slop, 0.0f)  * percent);
					objects[j]->move(correction);
				}
			}
		}
	}
	for (auto o:objects)
		o->update(dt);
}

void World::setBorder(std::vector<vector2f> & points)
{
	border = points;
	for (int i = 0; i < points.size(); i++)
	{
		ObjectRect* obj = new ObjectRect();
		obj->attachRigidbody(new Rigidbody(obj, 1.0f, false, false));
		obj->attachCollider(new LineCollider(obj, 1.0f, points[i], points[(i+1)%points.size()]));
		spawn(obj);
	}
}

void World::drawBorder(Canvas* canvas) const
{
	if (border.size()>2)
		canvas->drawPoly(border, 0xffffff);
}


std::vector< collision_info_t >& World::getCollisionInfo() // Make it int for now
{
	return collision_info;
}

void World::spawn(Object2d* object)
{
	objects.push_back(object);
}

Canvas::Canvas(int w, int h, World* wd, float wx, float wy, float ww, float wh)
{
	width = w; height = h;
	world = wd;
	startX = wx; startY = wy;
	viewWidth = ww;
	viewHeight = wh;
	img = Mat(h, w, CV_8UC3);
}
void Canvas::clear()
{
	img = Mat(height, width, CV_8UC3, Scalar(255,0,0));
}

void Canvas::drawTriangle(vector2f v1, vector2f v2, vector2f v3, int color)
{
	std::vector<vector2f> vs;
	vs.push_back(v1); vs.push_back(v2); vs.push_back(v3);
	drawPoly(vs, color);
}


Scalar int2scalar(int c)
{
	return Scalar(c&0xff, (c>>8)&0xff, c>>16);
}

void Canvas::drawCircle(vector2f center, float r, int color)
{
	int radius = (int)(r/viewWidth*width);
	auto p = world2canvas(center[0],center[1]);
	p = canvas2img(p.first,p.second);
	circle(img, Point(p.first, p.second), radius, int2scalar(color), -1);
}

void Canvas::drawPoly(const std::vector<vector2f>& vs, int color)
{
	int n = vs.size();
	Point* points = new Point[n];
	for (int i = 0; i < n; i++)
	{
		auto p = world2canvas(vs[i][0], vs[i][1]);
		p = canvas2img(p.first, p.second);
		points[i] = Point(p.first, p.second);
	}
	fillConvexPoly(img, points, n, int2scalar(color));
	delete [] points;
}

void Canvas::drawWorld()
{
	clear();
	world->drawBorder(this);
	for (Object2d* o : world->objects)
	{
		o->draw(this);
	}
}

std::pair<int, int> Canvas::world2canvas(float x, float y)
{
	return std::pair<int, int>(
		(int)((x - startX) / viewWidth * width),
		(int)((y - startY) / viewHeight * height));
}

vector2f Canvas::canvas2world(int x, int y)
{
	return vector2f(
		x / (float)width * viewWidth + startX,
		y / (float)height * viewHeight + startY);
}

std::pair<int, int> Canvas::canvas2img(int x, int y)
{
	return std::pair<int, int>(x, height-1-y);
}

void Canvas::getImgVec(vector<uchar>& array)
{
	if (img.isContinuous()) {
		array.assign(img.datastart, img.dataend);
	}
	else {
		for (int i = 0; i < img.rows; ++i) {
    		array.insert(array.end(), img.ptr<uchar>(i), img.ptr<uchar>(i)+img.cols);
		}
	}
}
