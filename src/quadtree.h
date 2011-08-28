/*
 * quadtree.h
 *
 *  Created on: Aug 27, 2011
 *      Author: wavenger
 */

#ifndef QUADTREE_H_
#define QUADTREE_H_

#include "constants.h"
#include "geometry.h"


void geometry_quadtree_init(float x1, float y1, float x2, float y2);

void geometry_quadtree_addLine(struct geometry_LineSegment * newLine);

void geometry_quadtree_destroy();

void geometry_quadtree_CheckCollision(struct geometry_LineSegment * line, float * resultX, float * resultY);

void geometry_quadtree_ResetLineIterator();

struct geometry_LineSegment * geometry_quadtree_GetNextLine();

#endif /* QUADTREE_H_ */
