/*
 * geometry.h
 *
 *  Created on: Aug 27, 2011
 *      Author: wavenger
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "constants.h"

struct geometry_LineSegment
{
	int id;
	float x1, y1, x2, y2;

	float height;

	float angle, rx, ry, rz;
	float dx, dy, dz;
	int ticks;
};

struct geometry_LineSegment * geometry_LineSegment_construct(float x1, float y1, float x2, float y2, float height);
struct geometry_LineSegment * geometry_LineSegment_constructSplit(float x1, float y1, float x2, float y2, struct geometry_LineSegment * init);
struct geometry_LineSegment * geometry_LineSegment_constructDummy(float x1, float y1, float x2, float y2, float height);

int geometry_LineIntersect(struct geometry_LineSegment * line1, struct geometry_LineSegment * line2, float * resultX, float *resultY);

void geometry_PrintLine(struct geometry_LineSegment * line);
#endif
