/*
 * geometry_array.h
 *
 *  Created on: Aug 27, 2011
 *      Author: wavenger
 */

#ifndef GEOMETRY_ARRAY_H_
#define GEOMETRY_ARRAY_H_

void geometry_array_init(float x1, float y1, float x2, float y2);

void geometry_array_addLine(struct geometry_LineSegment * newLine);

void geometry_array_destroy();

void geometry_array_CheckCollision(struct geometry_LineSegment * line, float * resultX, float * resultY);

void geometry_array_ResetLineIterator();

struct geometry_LineSegment * geometry_array_GetNextLine();

#endif /* GEOMETRY_ARRAY_H_ */
