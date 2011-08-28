/*
 * util.h
 *
 *  Created on: Aug 27, 2011
 *      Author: wavenger
 */

#ifndef UTIL_H_
#define UTIL_H_

float randRange(float lower, float upper);

int inRange(struct geometry_LineSegment * line, float test);

int slope(struct geometry_LineSegment * line, float * result);
int displacement(struct geometry_LineSegment * line, float * result);

int length(struct geometry_LineSegment * line, float * result);

void read_png_file(char* file_name);
#endif
