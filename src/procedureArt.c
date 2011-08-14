/*
 ============================================================================
 Name        : procedure2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <SDL.h>
#include <gl.h>
#include <glu.h>
#include <stdio.h>
#include <stdlib.h>
#include "geometry.h"
#include <limits.h>
#include <math.h>

#define ROTATE_FACTOR 0.01
#define BRANCHES 10
#define MAX_LINES 500
int numLines = 0;
float rotate1 = 0.0;
float rotate2 = 0.0;

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 640.0/480.0, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

void procedureArt_ShowLine(struct geometry_TreeNode * node)
{
	int i;
	if (node->children == 0)
	{
		for (i = 0; i < node->numLines; i++)
		{
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(node->lines[i]->x1, node->lines[i]->y1, -5.0);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(node->lines[i]->x2, node->lines[i]->y2, -5.0);
			glEnd();

		}
	}
	else
	{
		for (i = 0; i < TREE_ORDER; i++)
		{
			procedureArt_ShowLine(node->children[i]);
		}
	}

}
void display(Uint32 catchUp)
{
	float adjustedRotate;
	adjustedRotate = catchUp * ROTATE_FACTOR;
	rotate1 += adjustedRotate;
	rotate2 += adjustedRotate;
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(0.0, 0.0, -5.0);
	glRotatef(adjustedRotate, 5.0, 5.0, 0.0);
	glRotatef(adjustedRotate, 0.0, 5.0, 5.0);
	glRotatef(adjustedRotate, 5.0, 0.0, 5.0);
	glTranslatef(0.0, 0.0, 5.0);
	procedureArt_ShowLine(geometry_GetRootNode());
}

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
struct geometry_LineSegment * addLine(float x1, float y1, float x2, float y2)
{
	struct geometry_LineSegment * line = geometry_LineSegment_construct(x1, y1, x2, y2);
	geometry_CheckCollision(line, geometry_GetRootNode(), &x2, &y2);

	if (x2 != line->x2)
	{
		line->x2 = x2;
		line->y2 = y2;
	}
	geometry_addLine(line);
	return line;
}
struct geometry_LineSegment * addLineRandom()
{
	float x1 = randRange(-5.0, 5.0);
	float y1 = randRange(-5.0, 5.0);
	float x2 = randRange(-5.0, 5.0);
	float y2 = randRange(-5.0, 5.0);
	return addLine(x1, y1, x2, y2);
}
struct geometry_LineSegment * addLinePerpendicular(struct geometry_LineSegment * oldLine)
{
	float m, c;
	if (slope(oldLine, &m))
	{
		displacement(oldLine, m, &c);

		float newX1 = randRange(oldLine->x1, oldLine->x2);
		float newY1 = m * newX1 + c;

		float newM = m * -1;
		printf("Slope: %f (%f, %f x %f, %f\n", newM, oldLine->x1, oldLine->y1, oldLine->x2, oldLine->y2);
		float newC = newY1 - newX1 * newM;

		float newX2 = randRange(-5.0, 5.0);
		float newY2 = newM * newX2 + newC;

		return addLine(newX1, newY1, newX2, newY2);
	}
	else
	{
		float newX1 = randRange(oldLine->x1, oldLine->x2);
		float newY1 = randRange(oldLine->y1, oldLine->y2);

		float newC = newY1;

		float newX2 = randRange(-5.0, 5.0);
		float newY2 = newC;
		return addLine(newX1, newY1, newX2, newY2);
	}
}

void addLinesPerpendicularRecurse(struct geometry_LineSegment * oldLine, int branches)
{
	float m, c;
	if (branches > 0)
	{
		if (numLines > 100)
		{
			printf("Lines: %d\n", numLines);
		}
		if (numLines < MAX_LINES && fabs(oldLine->x1 - oldLine->x2) > 0.1 && slope(oldLine, &m))
		{
			displacement(oldLine, m, &c);
			int i;
			for (i = 0; i < branches; i++)
			{
				float newX1 = randRange(oldLine->x1, oldLine->x2);
				float newY1 = m * newX1 + c;

				float newM = m * -1;
				float newC = newY1 - newX1 * newM;

				float newX2 = randRange(-5.0, 5.0);
				float newY2 = newM * newX2 + newC;

				struct geometry_LineSegment * newLine = addLine(newX1, newY1, newX2, newY2);

				numLines++;
					addLinesPerpendicularRecurse(newLine, (branches / 2) - 1);//MAMAMAMA

			}
		}
	}
}
int SDL_main(int argc, char *argv[])
{
	geometry_init(5.0, 5.0, -5.0, -5.0);

	struct geometry_LineSegment * lastLine = addLineRandom();
	addLinesPerpendicularRecurse(lastLine, BRANCHES);
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen;

	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE | SDL_OPENGL);
	int running = 1;
	Uint32 start;
	Uint32 remainder;
	init();
	while (running)
	{
		SDL_Event event;

		start = SDL_GetTicks();

		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				running = 0;
				break;
			}
		}
		//printf("%d\n", remainder);
		display(remainder);
		SDL_GL_SwapBuffers();
		remainder = SDL_GetTicks() - start;
		if (1000/5 > remainder)
		{
				SDL_Delay(remainder);

		}
	}
	SDL_Quit();

	//geometry_WalkTree();
	geometry_destroy();
	return 0;
}
