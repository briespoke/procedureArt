#include <stdlib.h>
#include "geometry.h"
#include <math.h>

float randRange(float lower, float upper)
{
	if (lower > upper)
	{
		float tmp;

		tmp = upper;
		upper = lower;
		lower = tmp;
	}
	float value = rand() * 1.0;
	float valueFloat = value / RAND_MAX;
	return (upper - lower) * valueFloat + lower;
}

int inRange(struct geometry_LineSegment * line, float test)
{
	return  (test > line->x1 && test < line->x2) ||
			(test > line->x2 && test < line->x1);
}

int slope(struct geometry_LineSegment * line, float * result)
{
	float rise = (line->y1 - line->y2);
	float run = (line->x1 - line->x2);
	if (run == 0)
	{
		return 0;
	}
	else
	{
		*result = rise / run;
		return 1;
	}
}

int length(struct geometry_LineSegment * line, float * result)
{
	float rise = line->y2 - line->y1;
	float run = line->x2 - line->x1;

	*result = sqrt(fabs((rise*rise)/(run*run)));

	return 1;
}

int displacement(struct geometry_LineSegment * line, float * result)
{
	float m;
	slope(line, &m);
	//c = y - xm
	* result = line->y1 - line->x1 * m;
	return 1;
}
