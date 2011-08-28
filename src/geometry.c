//#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "header.h"
#include "geometry.h"
#include "util.h"


int geometry_LineCount;

int geometry_LineIntersect(struct geometry_LineSegment * line1, struct geometry_LineSegment * line2, float * resultX, float *resultY)
{
	//y = mx + c
	float m1, m2, c1, c2;
	int nonVertical1 = slope(line1, &m1);
	int nonVertical2 = slope(line2, &m2);

	if (nonVertical1 && nonVertical2)
	{
		if (m1 == m2)
		{
			// Lines are parallel, they never intersect
			return 0;
		}
		else
		{
			displacement(line1, &c1);
			displacement(line2, &c2);

			float xIntersect = (c2 - c1) / (m1 - m2);

			if (inRange(line1, xIntersect) && inRange(line2, xIntersect))
			{
				*resultX = xIntersect;
				*resultY = xIntersect * m1 + c1;
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		if (!nonVertical1 && !nonVertical2)
		{
			//Both lines are vertical, they never intersect
			return 0;
		}
		else if (! nonVertical1)
		{
			//line 1 is vertical
			if (inRange(line2, line1->x1))
			{
				displacement(line2, &c2);
				* resultX = line1->x1;
				* resultY = line1->x1 * m2 + c2;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			//line 2 is vertical
			if (inRange(line1, line2->x1))
			{
				displacement(line1, &c1);
				* resultX = line2->x1;
				* resultY = line2->x1 * m1 + c1;
				return 1;
			}
			else
			{
				return 0;
			}

		}
	}
}

struct geometry_LineSegment * geometry_LineSegment_construct(float x1, float y1, float x2, float y2, float height)
{
	struct geometry_LineSegment * line;
	line = (struct geometry_LineSegment *) malloc(sizeof (struct geometry_LineSegment));
	line->x1 = x1;
	line->y1 = y1;
	line->x2 = x2;
	line->y2 = y2;
	line->height = height;
	line->id = geometry_LineCount++;


	line->angle = randRange(0.0, 360.0);
	line->rx = randRange(-5.0, 5.0);
	line->ry = randRange(-5.0, 5.0);
	line->rz = randRange(-5.0, 5.0);
	line->dx = randRange(-5.0, 5.0);
	line->dy = randRange(-5.0, 5.0);
	line->dz = 5.0;
	line->ticks = (int)randRange(0.0, 300);

	if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
	{
		puts("Infinite subdivision");
	}
	return line;
}
struct geometry_LineSegment * geometry_LineSegment_constructDummy(float x1, float y1, float x2, float y2, float height)
{
	struct geometry_LineSegment * line;
	line = (struct geometry_LineSegment *) malloc(sizeof (struct geometry_LineSegment));
	line->x1 = x1;
	line->y1 = y1;
	line->x2 = x2;
	line->y2 = y2;
	line->height = height;

	if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
	{
		puts("Infinite subdivision");
	}
	return line;
}
struct geometry_LineSegment * geometry_LineSegment_constructSplit(float x1, float y1, float x2, float y2, struct geometry_LineSegment * init)
{
	struct geometry_LineSegment * line;
	line = (struct geometry_LineSegment *) malloc(sizeof (struct geometry_LineSegment));
	line->x1 = x1;
	line->y1 = y1;
	line->x2 = x2;
	line->y2 = y2;
	line->id = init->id;
	line->height = init->height;

	line->angle = init->angle;
	line->rx = init->rx;
	line->ry = init->ry;
	line->rz = init->rz;
	line->dx = init->dx;
	line->dy = init->dy;
	line->dz = init->dz;
	line->ticks = init->ticks;

	if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
	{
		puts("Infinite subdivision");
	}
	return line;
}

void geometry_PrintLine(struct geometry_LineSegment * line)
{
	printf("%f, %f x %f, %f", line->x1, line->y1, line->x2, line->y2);
}
