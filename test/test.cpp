/*
 * test.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#include "../object/object2d.h"
#include "../world/world.h"
#include <iostream>
#include <fstream>
#include "../collider/collider.h"
#include <cstring>
#include <cstdio>
#include "../synthesizer/synthesizer.h"

#include <sys/stat.h>

#include "mpeg_helper.h"
using namespace cv;

using std::cout;
using std::endl;

enum ObjectType {Circle, Rectangle};



class WorldManager
{
public:
	~WorldManager()
	{
		delete world;
		delete canvas;
	}


	void CreateWorld(float x, float y)
	{
		world = new World(x, y);
	}

	/* Called after CreateWorld, otherwise may result in undefined behavior */
	void CreateCanvas(float xstart, float ystart, float w, float h, int canvas_width)
	{
		assert(world);
		canvas = new Canvas(canvas_width, (int)(canvas_width/w*h),world, xstart, ystart, w, h);
	}

	void InitVideo(const char* filename, int fr)
	{
		framerate = fr;
		writer.open(filename, CV_FOURCC('X', 'V', 'I', 'D'), framerate, Size(canvas->width, canvas->height), true);
		if (!writer.isOpened() )
			cout << "ERROR: Failed to write the video" << endl;
	}

	void PutDynamicObject(ObjectType otype, float x, float y, float sx, float sy, float vx, float vy, float mass = 1.0f, float res = 0.8f, int color = 0)
	{
		if (!world) return;
		Object2d* obj;
		switch (otype)
		{
			case Circle:
			obj = new ObjectCircle();
			obj->attachCollider(new CircleCollider(obj, res));
			break;
			case Rectangle:
			obj = new ObjectRect();
			obj->attachCollider(new AABBCollider(obj, res));
			break;
			default:
			break;
		}
		obj->attachRigidbody(new Rigidbody(obj, mass));
		obj->setColor(color);
		obj->setScale(vector2f(sx, sy));
		obj->setPosition(vector2f(x, y));
		obj->getRigidBody()->setVelocity(vector2f(vx, vy));
		world->spawn(obj);
	}

	void PutStaticObject(ObjectType otype, float x, float y, float sx, float sy, float res = 0.8f, int color = 0)
	{
		if (!world) return;
		Object2d* obj;
		switch (otype)
		{
			case Circle:
			obj = new ObjectCircle();
			obj->attachCollider(new CircleCollider(obj, 1.0f));
			break;
			case Rectangle:
			obj = new ObjectRect();
			obj->attachCollider(new AABBCollider(obj, 1.0f));
			break;
			default:
			break;
		}
		obj->attachRigidbody(new Rigidbody(obj, 1.0f, false, false));
		obj->setColor(color);
		obj->setScale(vector2f(sx, sy));
		obj->setPosition(vector2f(x, y));
		world->spawn(obj);
	}

	void setBorder(std::vector<vector2f>& points)
	{
		world->setBorder(points);
	}

	void save_audio(const char* filename)
	{
		syn.normalize(30000);
		syn.write_to_file(filename);
	}

	void AdvanceTime(int dt)
	{
		int update_interval = 1000 / updaterate;
		int frame_interval = 1000 / framerate;
		int target_time = time + dt;
		while (time < target_time)
		{
			if (time%update_interval == 0)
			{
				world->update(update_interval);
				for (collision_info_t& col_info : world->getCollisionInfo())
				{
					if (col_info.speed > 1) // This value
					{
						printf("Collison occured at %d with impulse %f\n", time, col_info.speed);

						vector<double> info(3);
						if (col_info.r1>0 && col_info.r2>0)
							info[0] = 500.0 / std::min(col_info.r1, col_info.r2);
						else if (col_info.r1>0)
							info[0] = 500.0 / col_info.r1;
						else if (col_info.r2>0)
							info[0] = 500.0 / col_info.r2;
						else
							info[0] = 500.0;

						info[1] = 1000 * col_info.speed;
						info[2] = 5;
						syn.write(time/1000.0, 10, exp_decay_sin, info);

						just_colliding = true;

						printf("%f\n", col_info.speed);
					}
				}
			}
			if (time%frame_interval == 0)
			{
				canvas->drawWorld();
				writer.write(canvas->img);
				if (just_colliding) {
					just_colliding = false;
					labels.push_back(1);
				}
				else {
					labels.push_back(0);
				}
			}
			time++;
		}
	}
	void write_labels(char* filename) {
		std::fstream fs(filename, std::fstream::out);
		for (int i : labels) {
			fs << i << std::endl;
		}
		fs.close();
	}
private:
	World* world;
	Canvas* canvas;

	VideoWriter writer;
	int framerate;
	const int updaterate = 100;
	int time = 0;

	Synthesizer syn;

	bool just_colliding;
	std::vector<int> labels;
};

int main()
{
	std::srand(time(NULL));
	std::vector<vector2f> border;
	border.push_back(vector2f(2,38));
	border.push_back(vector2f(38,38));
	border.push_back(vector2f(38,2));
	border.push_back(vector2f(2,2));
	mkdir("output", 0777);
	for (int i = 0; i < 10; i++)
	{
		WorldManager* wm = new WorldManager();
		wm->CreateWorld(40.0f,40.0f);
		wm->CreateCanvas(0.0f, 0.0f, 40.0f, 40.0f, 800);
		wm->InitVideo("tmp.mkv", 30);
		//wm->PutStaticObject(Rectangle, 20.0f, 0.0f, 100.0f, 0.5f, 10000.0f,0x66ffff);
		wm->setBorder(border);
		float scale = rand_float(2,4);
		wm->PutDynamicObject(Circle,
			rand_float(10,30), rand_float(10,30),
			scale, scale,
			rand_float(-30,30), rand_float(-30,30),
			rand_float(1,5), rand_float(0.2,0.8), 0x000000);
		wm->AdvanceTime(10000);
		wm->save_audio("tmp.wav");

		char file_name [20];
		char label_file_name[20];
		sprintf(file_name, "output/data%d.mkv", i);
		sprintf(label_file_name, "output/label%d.txt", i);
		combine_video_audio("tmp.mkv", "tmp.wav", file_name);
		wm->write_labels(label_file_name);
		delete(wm);
	}
	return 0;


	// World* world = new World(80.0f, 40.0f);
	// ObjectCircle* circle = new ObjectCircle();
	// circle->setColor(0x66ffff);
	// circle->setScale(vector2f(10.0f, 5.0f));
	// circle->setPosition(vector2f(0.0f, 30.0f));
	// circle->attachRigidbody(new Rigidbody(circle, 2.0f));
	// circle->attachCollider(new CircleCollider(circle, 0.5f));
	// circle->getRigidBody()->setVelocity(vector2f(25.0f, 0.0f));
	// world->spawn(circle);

	// ObjectRect* square = new ObjectRect();
	// square->setColor(0xff5500);
	// square->setPosition(vector2f(80.0f, 40.0f));
	// square->setScale(vector2f(3.0f, 2.0f));
	// square->attachRigidbody(new Rigidbody(square, 1.0f));
	// square->attachCollider(new AABBCollider(square, 0.5f));
	// square->getRigidBody()->setVelocity(vector2f(-35.0f, 0.0f));
	// world->spawn(square);

	// ObjectRect* whatever = new ObjectRect();
	// whatever->setColor(0xff0000);
	// whatever->setPosition(vector2f(40.0f, 5.0f));
	// whatever->setScale(vector2f(40.0f, 5.0f));
	// whatever->attachRigidbody(new Rigidbody(whatever, 1.0f, false));
	// whatever->attachCollider(new AABBCollider(whatever));
	// world->spawn(whatever);

	// Canvas* canvas = new Canvas(1200,600,world,0.0f,0.0f,80.0f,40.0f);


	// VideoWriter writer;
	// writer.open("test.mkv", CV_FOURCC('D', 'I', 'V', 'X'), 40, Size(1200, 600), true);

	// for (int i = 0; i < 20; i++)
	// {
	// 	canvas->drawWorld();
	// 	writer.write(canvas->img);
	// 	world->update(25);
	// }

	// delete world; delete canvas;
}
