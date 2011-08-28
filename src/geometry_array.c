/*
 * geometry_array.c
 *
 *  Created on: Aug 27, 2011
 *      Author: wavenger
 */
#import "geometry.h"
#import "constants.h"
#import <stdlib.h>
#import <math.h>

#define NUM_geometry_array_bounds 4

struct geometry_LineSegment * geometry_array_bounds[NUM_geometry_array_bounds];
struct geometry_LineSegment ** geometry_array_lines;
int geometry_array_currentLine;
int geometry_array_numLines;

void geometry_array_CheckCollisionTest(struct geometry_LineSegment * line, struct geometry_LineSegment * testLine, float * resultX, float * resultY);

void geometry_array_init(float x1, float y1, float x2, float y2)
{
	geometry_array_bounds[0] = geometry_LineSegment_construct(x1, y1, x1, y2, 0);
	geometry_array_bounds[1] = geometry_LineSegment_construct(x1, y2, x2, y2, 0);
	geometry_array_bounds[2] = geometry_LineSegment_construct(x2, y2, x1, y2, 0);
	geometry_array_bounds[3] = geometry_LineSegment_construct(x1, y2, x1, y1, 0);

	geometry_array_lines = (struct geometry_LineSegment **) malloc(sizeof (struct geometry_LineSegment *) * BIG_BUFFER_SIZE);
	geometry_array_numLines = 0;
}

void geometry_array_addLine(struct geometry_LineSegment * newLine)
{
	//geometry_PrintLine(newLine);
	geometry_array_lines[geometry_array_numLines] = newLine;
	geometry_array_numLines++;
}

void geometry_array_destroy()
{
	int i;
	for (i = 0; i < NUM_geometry_array_bounds; i++)
	{
		free(geometry_array_bounds[i]);
	}
	for (i = 0; i < geometry_array_numLines; i++)
	{
		free(geometry_array_lines[i]);
	}
	free(geometry_array_lines);

}

void geometry_array_CheckCollision(struct geometry_LineSegment * line, float * resultX, float * resultY)
{
	int i;

	for (i = 0; i < NUM_geometry_array_bounds; i++)
	{
		struct geometry_LineSegment * testLine = geometry_array_bounds[i];
		geometry_array_CheckCollisionTest(line, testLine, resultX, resultY);
	}
	for (i = 0; i < geometry_array_numLines; i++)
	{
		struct geometry_LineSegment * testLine = geometry_array_lines[i];
		geometry_array_CheckCollisionTest(line, testLine, resultX, resultY);
	}
}
void geometry_array_CheckCollisionTest(struct geometry_LineSegment * line, struct geometry_LineSegment * testLine, float * resultX, float * resultY)
{
	float tmpX, tmpY;

	if(geometry_LineIntersect(line, testLine, &tmpX, &tmpY))
	{
		if (fabs(tmpX - line->x1) < fabs((* resultX) - line->x1))
		{
			* resultX = tmpX;
			* resultY = tmpY;
		}
	}
}

void geometry_array_ResetLineIterator()
{
	geometry_array_currentLine = -1;
}

struct geometry_LineSegment * geometry_array_GetNextLine()
{
	geometry_array_currentLine++;
	if (geometry_array_currentLine < geometry_array_numLines)
	{
		return geometry_array_lines[geometry_array_currentLine];
	}
	else
	{
		return 0;
	}
}
