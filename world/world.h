/*
 * world.h
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#ifndef WORLD_WORLD_H_
#define WORLD_WORLD_H_

#include <vector>
#include "../object/object2d.h"

#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

using namespace cv;


typedef struct {
	float r1;
	float r2;
	float speed;
} collision_info_t;


/* World manages all Object2d in it
 * It is based on the first quadrant of a cartesian coordinate system
 */
class World
{
public:
	/* @param float     width of the world
	 * @param float     height of the world
	 */
	World(float w, float h);
	~World();
	void update(int dt);

	/* @param Object2d*    The object will be placed in the world,
	 *                     which will manage the memory of the object
	 */
	void spawn(Object2d* object);

	void setBorder(std::vector<vector2f> & points);

	void drawBorder(Canvas* canvas) const;

	vector< collision_info_t >& getCollisionInfo();

private:
	float width, height;
	vector< collision_info_t > collision_info;
public:
	std::vector<Object2d*> objects;
	std::vector<vector2f> border;
};


class Canvas
{
public:
	Canvas(int w, int h, World* wd, float wx, float wy, float ww, float wh);
	void clear();

	/* not yet implemented */
	void drawTriangle(vector2f v1, vector2f v2, vector2f v3, int color);

	/* @param vector2f    center of the circle in world coordinates
	 * @param float       radius of the circle
	 * @param int         color
	 */
	void drawCircle(vector2f center, float r, int color);

	/* @param std::vector<vector2f>    The vertices of the convex polygon
	 * @param int                      color
	 */
	void drawPoly(const std::vector<vector2f>& vs, int color);

	/* Draw the world */
	void drawWorld();

	int width, height;
private:
	World* world;
	float viewWidth, viewHeight;
	float startX, startY;

	std::pair<int, int> world2canvas(float x, float y);
	std::pair<int, int> canvas2img(int x, int y);
	vector2f canvas2world(int x, int y);
public:
	/* opencv object used to store image data */
	Mat img;
	void getImgVec(vector<uchar>& array);
};

#endif /* WORLD_WORLD_H_ */
